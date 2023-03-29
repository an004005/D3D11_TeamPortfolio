#include "stdafx.h"
#include "..\public\SSAOManager.h"

#include "Camera.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CSSAOManager)

CSSAOManager::CSSAOManager()
{
	m_iSSAOSampRadius = 10;//max 64
	m_fRadius = 13; // max 100
}

HRESULT CSSAOManager::Initialize(UINT width, UINT height, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	Deinit();
	if (m_pDevice == nullptr)
	{
		m_pDevice = pDevice;
		Safe_AddRef(m_pDevice);
	}
	if (m_pContext == nullptr)
	{
		m_pContext = pContext;
		Safe_AddRef(m_pContext);
	}

	HRESULT hr = S_OK;

	m_nWidth = width / 2;
	m_nHeight = height / 2;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate SSAO
	D3D11_TEXTURE2D_DESC t2dDesc = {
		m_nWidth, //UINT Width;
		m_nHeight, //UINT Height;
		1, //UINT MipLevels;
		1, //UINT ArraySize;
		DXGI_FORMAT_R32_TYPELESS,//DXGI_FORMAT_R8_TYPELESS, //DXGI_FORMAT Format;
		1, //DXGI_SAMPLE_DESC SampleDesc;
		0,
		D3D11_USAGE_DEFAULT,//D3D11_USAGE Usage;
		D3D11_BIND_UNORDERED_ACCESS|D3D11_BIND_SHADER_RESOURCE,//UINT BindFlags;
		0,//UINT CPUAccessFlags;
		0//UINT MiscFlags;    
        };
	FAILED_CHECK( m_pDevice->CreateTexture2D( &t2dDesc, NULL, &m_pSSAO_RT ) );
	SetDebugName( m_pSSAO_RT, "SSAO - Final AO values" );

	// Create the UAVs
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory( &UAVDesc, sizeof(UAVDesc) );
	UAVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	FAILED_CHECK( m_pDevice->CreateUnorderedAccessView( m_pSSAO_RT, &UAVDesc, &m_pSSAO_UAV ) );
	SetDebugName( m_pSSAO_UAV, "SSAO - Final AO values UAV" );

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	FAILED_CHECK( m_pDevice->CreateShaderResourceView( m_pSSAO_RT, &SRVDesc, &m_pSSAO_SRV ) );
	SetDebugName( m_pMiniDepthSRV, "SSAO - Final AO values SRV" );

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate down scaled depth buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory( &bufferDesc, sizeof(bufferDesc) );
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.StructureByteStride = 4 * sizeof(float);
	bufferDesc.ByteWidth = m_nWidth * m_nHeight * bufferDesc.StructureByteStride;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	FAILED_CHECK( m_pDevice->CreateBuffer( &bufferDesc, NULL, &m_pMiniDepthBuffer ) );
	SetDebugName( m_pMiniDepthBuffer, "SSAO - Downscaled Depth Buffer" );

	ZeroMemory( &UAVDesc, sizeof(UAVDesc) );
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Buffer.FirstElement = 0;
	UAVDesc.Buffer.NumElements = m_nWidth * m_nHeight;
	FAILED_CHECK( m_pDevice->CreateUnorderedAccessView( m_pMiniDepthBuffer, &UAVDesc, &m_pMiniDepthUAV ) );
	SetDebugName( m_pMiniDepthUAV, "SSAO - Downscaled Depth UAV" );

	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Buffer.FirstElement = 0;
	SRVDesc.Buffer.NumElements = m_nWidth * m_nHeight;
	FAILED_CHECK( m_pDevice->CreateShaderResourceView( m_pMiniDepthBuffer, &SRVDesc, &m_pMiniDepthSRV ) );
	SetDebugName( m_pMiniDepthSRV, "SSAO - Downscaled Depth SRV" );

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate down scale depth constant buffer
	D3D11_BUFFER_DESC CBDesc;
	ZeroMemory( &CBDesc, sizeof(CBDesc) );
	CBDesc.Usage = D3D11_USAGE_DYNAMIC;
	CBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CBDesc.ByteWidth = sizeof(TDownscaleCB);
	FAILED_CHECK( m_pDevice->CreateBuffer( &CBDesc, NULL, &m_pDownscaleCB ) );
	SetDebugName( m_pDownscaleCB, "SSAO - Downscale Depth CB" );

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Compile the shaders
    DWORD dwShaderFlags = 0;
#ifdef _DEBUG
    dwShaderFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	dwShaderFlags = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	ID3DBlob* pShaderBlob = NULL;
	WCHAR str[MAX_PATH] = L"../Bin/ShaderFiles/SSAO.hlsl";

	FAILED_CHECK( CompileShader( str, NULL, "DepthDownscale", "cs_5_0", dwShaderFlags, &pShaderBlob ) );
	FAILED_CHECK( m_pDevice->CreateComputeShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pDepthDownscaleCS ) );
	Safe_Release( pShaderBlob );

	FAILED_CHECK( CompileShader( str, NULL, "SSAOCompute", "cs_5_0", dwShaderFlags, &pShaderBlob ) );
	FAILED_CHECK( m_pDevice->CreateComputeShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pComputeCS ) );
	Safe_Release( pShaderBlob );

	return S_OK;
}

void CSSAOManager::Deinit()
{
	Safe_Release(m_pDownscaleCB);
	Safe_Release(m_pSSAO_RT);
	Safe_Release(m_pSSAO_SRV);
	Safe_Release(m_pSSAO_UAV);
	Safe_Release(m_pMiniDepthBuffer);
	Safe_Release(m_pMiniDepthUAV);
	Safe_Release(m_pMiniDepthSRV);
	Safe_Release(m_pDepthDownscaleCS);
	Safe_Release(m_pComputeCS);
}

void CSSAOManager::Compute(ID3D11ShaderResourceView* pDepthSRV, ID3D11ShaderResourceView* pNormalsSRV)
{
	ID3D11ShaderResourceView *arrSRV[1] = { NULL };
	m_pContext->PSSetShaderResources(5, 1, arrSRV);
	DownscaleDepth(pDepthSRV, pNormalsSRV);
	ComputeSSAO();
	Blur();
}

void CSSAOManager::DownscaleDepth(ID3D11ShaderResourceView* pDepthSRV, ID3D11ShaderResourceView* pNormalsSRV)
{
	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pContext->Map(m_pDownscaleCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	TDownscaleCB* pDownscale = (TDownscaleCB*)MappedResource.pData; 
	pDownscale->nWidth = m_nWidth;
	pDownscale->nHeight = m_nHeight;
	pDownscale->fHorResRcp = 1.0f / (float)pDownscale->nWidth;
	pDownscale->fVerResRcp = 1.0f / (float)pDownscale->nHeight;
	
	const _float4x4 pProj = CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	pDownscale->ProjParams.x = 1.0f / pProj.m[0][0];
	pDownscale->ProjParams.y = 1.0f / pProj.m[1][1];

	const _float fFar = CGameInstance::GetInstance()->GetMainCam()->GetFar();
	const _float fNear = CGameInstance::GetInstance()->GetMainCam()->GetNear();

	float fQ = fFar / (fFar - fNear);
	pDownscale->ProjParams.z = -fNear * fQ;
	pDownscale->ProjParams.w = -fQ;

	const _float4x4 viewMatrix = CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	pDownscale->ViewMatrix = XMMatrixTranspose(viewMatrix); // col major로 바꾸는듯?
	pDownscale->fOffsetRadius = (float)m_iSSAOSampRadius;
	pDownscale->fRadius = m_fRadius;
	pDownscale->fMaxDepth = fFar;
	m_pContext->Unmap(m_pDownscaleCB, 0);
	ID3D11Buffer* arrConstBuffers[1] = { m_pDownscaleCB };
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);

	// Output
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pMiniDepthUAV };
	m_pContext->CSSetUnorderedAccessViews( 0, 1, arrUAVs, NULL);

	// Input
	ID3D11ShaderResourceView* arrViews[2] = { pDepthSRV, pNormalsSRV };
	m_pContext->CSSetShaderResources(0, 2, arrViews);

	// Shader
	m_pContext->CSSetShader( m_pDepthDownscaleCS, NULL, 0 );

	// Execute the downscales first pass with enough groups to cover the entire full res HDR buffer
	m_pContext->Dispatch((UINT)ceil((float)(m_nWidth * m_nHeight) / 1024.0f), 1, 1);

	// Cleanup
	m_pContext->CSSetShader( NULL, NULL, 0 );
	ZeroMemory(arrViews, sizeof(arrViews));
	m_pContext->CSSetShaderResources(0, 2, arrViews);
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	m_pContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);
	ZeroMemory(arrConstBuffers, sizeof(arrConstBuffers));
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);
}

void CSSAOManager::ComputeSSAO()
{
	// Constants
	ID3D11Buffer* arrConstBuffers[1] = { m_pDownscaleCB };
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);

	// Output
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pSSAO_UAV };
	m_pContext->CSSetUnorderedAccessViews( 0, 1, arrUAVs, NULL);

	// Input
	ID3D11ShaderResourceView* arrViews[1] = { m_pMiniDepthSRV };
	m_pContext->CSSetShaderResources(0, 1, arrViews);

	// Shader
	m_pContext->CSSetShader( m_pComputeCS, NULL, 0 );

	// Execute the downscales first pass with enough groups to cover the entire full res HDR buffer
	m_pContext->Dispatch((UINT)ceil((float)(m_nWidth * m_nHeight) / 1024.0f), 1, 1);

	// Cleanup
	m_pContext->CSSetShader( NULL, NULL, 0 );
	ZeroMemory(arrViews, sizeof(arrViews));
	m_pContext->CSSetShaderResources(0, 1, arrViews);
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	m_pContext->CSSetUnorderedAccessViews(0, 1, arrUAVs, NULL);
	ZeroMemory(arrConstBuffers, sizeof(arrConstBuffers));
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);
}

void CSSAOManager::Blur()
{
}

void CSSAOManager::SetDebugName(ID3D11DeviceChild* pObj, const char* pName)
{
	if (pObj)
        pObj->SetPrivateData( WKPDID_D3DDebugObjectName, lstrlenA(pName), pName );
}

HRESULT CSSAOManager::CompileShader(PWCHAR strPath, D3D10_SHADER_MACRO* pMacros, char* strEntryPoint, char* strProfile,
	DWORD dwShaderFlags, ID3DBlob** ppVertexShaderBuffer)
{
	return D3DCompileFromFile( strPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, strEntryPoint, strProfile, dwShaderFlags, 0,
		ppVertexShaderBuffer, nullptr );
}

void CSSAOManager::Free()
{
	Deinit();
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
