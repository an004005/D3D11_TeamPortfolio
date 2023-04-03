#include "stdafx.h"
#include "SSLRManager.h"

#include "Camera.h"
#include "GameInstance.h"

#pragma pack(push,1)
struct CB_OCCLUSSION
{
	UINT nWidth;
	UINT nHeight;
	UINT pad[2];
};

struct CB_LIGHT_RAYS
{
	_float2 vSunPos;
	float fInitDecay;
	float fDistDecay;
	_float3 vRayColor;
	float fMaxDeltaLen;
};
#pragma pack(pop)

bool g_bShowRayTraceRes = false;

IMPLEMENT_SINGLETON(CSSLRManager)

CSSLRManager::CSSLRManager()
{
	m_fInitDecay = 0.2f;
	m_fDistDecay = 0.8f;
	m_fMaxDeltaLen = 0.005f;
}

HRESULT CSSLRManager::Initialize(UINT width, UINT height, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
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
	// Allocate the occlusion resources
	D3D11_TEXTURE2D_DESC t2dDesc = {
		m_nWidth, //UINT Width;
		m_nHeight, //UINT Height;
		1, //UINT MipLevels;
		1, //UINT ArraySize;
		DXGI_FORMAT_R8_TYPELESS, //DXGI_FORMAT Format;
		1, //DXGI_SAMPLE_DESC SampleDesc;
		0,
		D3D11_USAGE_DEFAULT,//D3D11_USAGE Usage;
		D3D11_BIND_UNORDERED_ACCESS|D3D11_BIND_SHADER_RESOURCE,//UINT BindFlags;
		0,//UINT CPUAccessFlags;
		0//UINT MiscFlags;    
	};
	FAILED_CHECK( m_pDevice->CreateTexture2D( &t2dDesc, NULL, &m_pOcclusionTex ) );
	SetDebugName( m_pOcclusionTex, "SSLR Occlusion Texture" );

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	ZeroMemory( &UAVDesc, sizeof(UAVDesc) );
	UAVDesc.Format = DXGI_FORMAT_R8_UNORM;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	FAILED_CHECK( m_pDevice->CreateUnorderedAccessView( m_pOcclusionTex, &UAVDesc, &m_pOcclusionUAV ) );
	SetDebugName( m_pOcclusionUAV, "SSLR Occlusion UAV" );

	D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd = 
	{
		DXGI_FORMAT_R8_UNORM,
		D3D11_SRV_DIMENSION_TEXTURE2D,
		0,
		0
	};
	dsrvd.Texture2D.MipLevels = 1;
	FAILED_CHECK( m_pDevice->CreateShaderResourceView( m_pOcclusionTex, &dsrvd, &m_pOcclusionSRV ) );
	SetDebugName( m_pOcclusionSRV, "SSLR Occlusion SRV" );

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate the light rays resources
	t2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE;
	FAILED_CHECK( m_pDevice->CreateTexture2D( &t2dDesc, NULL, &m_pLightRaysTex ) );
	SetDebugName( m_pLightRaysTex, "SSLR Light Rays Texture" );

	D3D11_RENDER_TARGET_VIEW_DESC rtsvd = 
	{
		DXGI_FORMAT_R8_UNORM,
		D3D11_RTV_DIMENSION_TEXTURE2D
	};
	FAILED_CHECK( m_pDevice->CreateRenderTargetView( m_pLightRaysTex, &rtsvd, &m_pLightRaysRTV ) ); 
	SetDebugName( m_pLightRaysRTV, "SSLR Light Rays RTV" );

	FAILED_CHECK( m_pDevice->CreateShaderResourceView( m_pLightRaysTex, &dsrvd, &m_pLightRaysSRV ) );
	SetDebugName( m_pLightRaysSRV, "SSLR Light Rays SRV" );

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate the occlussion constant buffer
	D3D11_BUFFER_DESC CBDesc;
	ZeroMemory( &CBDesc, sizeof(CBDesc) );
	CBDesc.Usage = D3D11_USAGE_DYNAMIC;
	CBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CBDesc.ByteWidth = sizeof(CB_OCCLUSSION);
	FAILED_CHECK( m_pDevice->CreateBuffer( &CBDesc, NULL, &m_pOcclusionCB ) );
	SetDebugName( m_pOcclusionCB, "SSLR - Occlussion CB" );

	CBDesc.ByteWidth = sizeof(CB_LIGHT_RAYS);
	FAILED_CHECK( m_pDevice->CreateBuffer( &CBDesc, NULL, &m_pRayTraceCB ) );
	SetDebugName( m_pRayTraceCB, "SSLR - Ray Trace CB" );
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Compile the shaders
    DWORD dwShaderFlags = 0;
#ifdef _DEBUG
    dwShaderFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	dwShaderFlags = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	ID3DBlob* pShaderBlob = NULL;
	WCHAR str[MAX_PATH] = L"../Bin/ShaderFiles/SSLR.hlsl";

	FAILED_CHECK( CompileShader( str, NULL, "Occlussion", "cs_5_0", dwShaderFlags, &pShaderBlob ) );
	FAILED_CHECK( m_pDevice->CreateComputeShader( pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), NULL, &m_pOcclusionCS ) );
	SetDebugName( m_pOcclusionCS, "SSLR - Occlussion CS" );
	Safe_Release( pShaderBlob );

	FAILED_CHECK( CompileShader(str, NULL, "RayTraceVS", "vs_5_0", dwShaderFlags, &pShaderBlob) );
	FAILED_CHECK( m_pDevice->CreateVertexShader( pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pFullScreenVS ) );
	SetDebugName( m_pFullScreenVS, "SSLR - Full Screen VS" );
	Safe_Release( pShaderBlob );

	FAILED_CHECK( CompileShader(str, NULL, "RayTracePS", "ps_5_0", dwShaderFlags, &pShaderBlob) );
	FAILED_CHECK( m_pDevice->CreatePixelShader( pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pRayTracePS ) );
	SetDebugName( m_pRayTracePS, "SSLR - Ray Trace PS" );
	Safe_Release( pShaderBlob );

	FAILED_CHECK( CompileShader(str, NULL, "CombinePS", "ps_5_0", dwShaderFlags, &pShaderBlob) );
	FAILED_CHECK( m_pDevice->CreatePixelShader( pShaderBlob->GetBufferPointer(),
		pShaderBlob->GetBufferSize(), NULL, &m_pCombinePS ) );
	SetDebugName( m_pCombinePS, "SSLR - Combine PS" );
	Safe_Release( pShaderBlob );
	
	// Create the additive blend state
	D3D11_BLEND_DESC descBlend;
	descBlend.AlphaToCoverageEnable = FALSE;
	descBlend.IndependentBlendEnable = FALSE;
	const D3D11_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
	{
		TRUE,
		D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
		D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD,
		D3D11_COLOR_WRITE_ENABLE_ALL,
	};
	for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		descBlend.RenderTarget[ i ] = defaultRenderTargetBlendDesc;
	FAILED_CHECK( m_pDevice->CreateBlendState(&descBlend, &m_pAdditiveBlendState) );
	SetDebugName( m_pAdditiveBlendState, "SSLR - Additive Alpha BS" );

	return hr;
}

void CSSLRManager::Deinit()
{
	Safe_Release(m_pOcclusionTex);
	Safe_Release(m_pOcclusionUAV);
	Safe_Release(m_pOcclusionSRV);
	Safe_Release(m_pLightRaysTex);
	Safe_Release(m_pLightRaysRTV);
	Safe_Release(m_pLightRaysSRV);
	Safe_Release(m_pOcclusionCB);
	Safe_Release(m_pOcclusionCS);
	Safe_Release(m_pRayTraceCB);
	Safe_Release(m_pFullScreenVS);
	Safe_Release(m_pRayTracePS);
	Safe_Release(m_pCombinePS);
	Safe_Release(m_pAdditiveBlendState);
}

void CSSLRManager::Render(ID3D11RenderTargetView* pLightAccumRTV, ID3D11ShaderResourceView* pMiniDepthSRV,
	const _float4& vSunDir, const _float4& vSunColor)
{
	// No need to do anything if the camera is facing away from the sun
	// This will not work if the FOV is close to 180 or higher

	auto pCam = CGameInstance::GetInstance()->GetMainCam();
	if (pCam == nullptr)
		return;
	
	const float dotCamSun = -XMVectorGetX(XMVector3Dot(pCam->GetTransform()->Get_State(CTransform::STATE_LOOK), vSunDir));
	if(dotCamSun <= 0.0f)
	{
		return;
	}

	_float4 vSunPos = -200.0f * vSunDir;
	_float4 vEyePos = pCam->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	vSunPos.x += vEyePos.x;
	vSunPos.z += vEyePos.z;
	_matrix mView = pCam->GetXMViewMatrix();
	_matrix mProj = pCam->GetXMProjMatrix();
	_matrix mViewProjection = mView * mProj;
	_float3 vSunPosSS = XMVector3TransformCoord(vSunPos, mViewProjection);

	// If the sun is too far out of view we just want to turn off the effect
	static const float fMaxSunDist = 1.3f;
	if(fabs(vSunPosSS.x) >= fMaxSunDist || fabs(vSunPosSS.y) >= fMaxSunDist)
	{
		return;
	}

	// Attenuate the sun color based on how far the sun is from the view
	_float3 vSunColorAtt{vSunColor.x, vSunColor.y, vSunColor.z };
	float fMaxDist = max(fabs(vSunPosSS.x), fabs(vSunPosSS.y));
	if(fMaxDist >= 1.0f)
	{
		vSunColorAtt *= (fMaxSunDist - fMaxDist);
	}

	ID3D11RenderTargetView*	pBackBufferView = nullptr;
	ID3D11DepthStencilView*	pDepthStencilView = nullptr;
	m_pContext->OMGetRenderTargets(1, &pBackBufferView, &pDepthStencilView);

	FLOAT prevBlendFactor[ 4 ]{0.f, 0.f, 0.f, 0.f};
	UINT prevSampleMask = 0xffffffff;
	m_pContext->OMSetBlendState(nullptr, prevBlendFactor, prevSampleMask);

	PrepareOcclusion(pMiniDepthSRV);
	RayTrace(_float2(vSunPosSS.x, vSunPosSS.y), vSunColorAtt);
	if(!g_bShowRayTraceRes)
		Combine(pLightAccumRTV);

	m_pContext->OMSetRenderTargets(1, &pBackBufferView, pDepthStencilView);
	Safe_Release(pBackBufferView);
	Safe_Release(pDepthStencilView);
}

void CSSLRManager::Imgui_Render()
{
	ImGui::InputFloat("InitDecay", &m_fInitDecay);
	ImGui::InputFloat("DistDecay", &m_fDistDecay);
	ImGui::InputFloat("MaxDeltaLen", &m_fMaxDeltaLen);
}

void CSSLRManager::PrepareOcclusion(ID3D11ShaderResourceView* pMiniDepthSRV)
{
	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pContext->Map(m_pOcclusionCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	CB_OCCLUSSION* pOcclussion = (CB_OCCLUSSION*)MappedResource.pData; 
	pOcclussion->nWidth = m_nWidth;
	pOcclussion->nHeight = m_nHeight;
	m_pContext->Unmap(m_pOcclusionCB, 0);
	ID3D11Buffer* arrConstBuffers[1] = { m_pOcclusionCB };
	m_pContext->CSSetConstantBuffers(0, 1, arrConstBuffers);

	// Output
	ID3D11UnorderedAccessView* arrUAVs[1] = { m_pOcclusionUAV };
	m_pContext->CSSetUnorderedAccessViews( 0, 1, arrUAVs, NULL);

	// Input
	ID3D11ShaderResourceView* arrViews[1] = { pMiniDepthSRV };
	m_pContext->CSSetShaderResources(0, 1, arrViews);

	// Shader
	m_pContext->CSSetShader( m_pOcclusionCS, NULL, 0 );

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

void CSSLRManager::RayTrace(const _float2& vSunPosSS, const _float3& vSunColor)
{
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pContext->ClearRenderTargetView( m_pLightRaysRTV, ClearColor );

	D3D11_VIEWPORT oldvp;
	UINT num = 1;
	m_pContext->RSGetViewports(&num, &oldvp);
	if(!g_bShowRayTraceRes)
	{
		D3D11_VIEWPORT vp[1] = { { 0, 0, (float)m_nWidth, (float)m_nHeight, 0.0f, 1.0f } };
		m_pContext->RSSetViewports( 1, vp );

		m_pContext->OMSetRenderTargets( 1, &m_pLightRaysRTV, NULL );
	}

	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	m_pContext->Map(m_pRayTraceCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	CB_LIGHT_RAYS* pRayTrace = (CB_LIGHT_RAYS*)MappedResource.pData; 
	pRayTrace->vSunPos = _float2(0.5f * vSunPosSS.x + 0.5f, -0.5f * vSunPosSS.y + 0.5f);
	pRayTrace->fInitDecay = m_fInitDecay;
	pRayTrace->fDistDecay = m_fDistDecay;
	pRayTrace->vRayColor = vSunColor;
	pRayTrace->fMaxDeltaLen = m_fMaxDeltaLen;
	m_pContext->Unmap(m_pRayTraceCB, 0);
	ID3D11Buffer* arrConstBuffers[1] = { m_pRayTraceCB };
	m_pContext->PSSetConstantBuffers(0, 1, arrConstBuffers);

	// Input
	ID3D11ShaderResourceView* arrViews[1] = { m_pOcclusionSRV };
	m_pContext->PSSetShaderResources(0, 1, arrViews);

	// Primitive settings
	m_pContext->IASetInputLayout( NULL );
	m_pContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	m_pContext->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	// Set the shaders
	m_pContext->VSSetShader(m_pFullScreenVS, NULL, 0);
	m_pContext->GSSetShader(NULL, NULL, 0);
	m_pContext->PSSetShader(m_pRayTracePS, NULL, 0);

	m_pContext->Draw(4, 0);

	// Cleanup
	arrViews[0] = NULL;
	m_pContext->PSSetShaderResources(0, 1, arrViews);
	m_pContext->VSSetShader(NULL, NULL, 0);
	m_pContext->PSSetShader(NULL, NULL, 0);
	m_pContext->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_pContext->RSSetViewports( 1, &oldvp );
}

void CSSLRManager::Combine(ID3D11RenderTargetView* pLightAccumRTV)
{
	// ID3D11BlendState* pPrevBlendState;
	FLOAT prevBlendFactor[ 4 ]{0.f, 0.f, 0.f, 0.f};
	UINT prevSampleMask = 0xffffffff;
	// m_pContext->OMGetBlendState(&pPrevBlendState, prevBlendFactor, &prevSampleMask);
	m_pContext->OMSetBlendState(m_pAdditiveBlendState, prevBlendFactor, prevSampleMask);

	// Restore the light accumulation view
	m_pContext->OMSetRenderTargets( 1, &pLightAccumRTV, NULL );

	// Constants
	ID3D11Buffer* arrConstBuffers[1] = { m_pRayTraceCB };
	m_pContext->PSSetConstantBuffers(0, 1, arrConstBuffers);

	// Input
	ID3D11ShaderResourceView* arrViews[1] = { m_pLightRaysSRV };
	m_pContext->PSSetShaderResources(0, 1, arrViews);

	// Primitive settings
	m_pContext->IASetInputLayout( NULL );
	m_pContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	m_pContext->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	// Set the shaders
	m_pContext->VSSetShader(m_pFullScreenVS, NULL, 0);
	m_pContext->GSSetShader(NULL, NULL, 0);
	m_pContext->PSSetShader(m_pCombinePS, NULL, 0);

	m_pContext->Draw(4, 0);

	// Cleanup
	arrViews[0] = NULL;
	m_pContext->PSSetShaderResources(0, 1, arrViews);
	m_pContext->VSSetShader(NULL, NULL, 0);
	m_pContext->PSSetShader(NULL, NULL, 0);
	m_pContext->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	// m_pContext->OMSetBlendState(pPrevBlendState, prevBlendFactor, prevSampleMask);

	// Safe_Release(pPrevBlendState);


}


void CSSLRManager::SetDebugName(ID3D11DeviceChild* pObj, const char* pName)
{
	if (pObj)
        pObj->SetPrivateData( WKPDID_D3DDebugObjectName, lstrlenA(pName), pName );
}

HRESULT CSSLRManager::CompileShader(PWCHAR strPath, D3D10_SHADER_MACRO* pMacros, char* strEntryPoint, char* strProfile,
	DWORD dwShaderFlags, ID3DBlob** ppVertexShaderBuffer)
{
	return D3DCompileFromFile( strPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, strEntryPoint, strProfile, dwShaderFlags, 0,
		ppVertexShaderBuffer, nullptr );
}

void CSSLRManager::Free()
{
	Deinit();
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
