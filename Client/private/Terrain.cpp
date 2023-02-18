#include "stdafx.h"
#include "Terrain.h"
#include "GameInstance.h"
#include "MathUtils.h"
#include "GameUtils.h"

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::Initialize(void * pArg)
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	m_vBrushPos = _float4::Zero;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Filter();

	if (FAILED(SetUp_Components()))
		return E_FAIL;	


	return S_OK;
}

void CTerrain::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CTerrain::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CTerrain::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CTerrain::Imgui_RenderProperty()
{
}

void CTerrain::Ready_Filter()
{
	const _tchar* pFilterPath = L"../Bin/Resources/Textures/Terrain/Filter.dds";

	if (CGameUtils::FileExist(pFilterPath))
	{
		ID3D11Texture2D* pFilterTex = nullptr;
		DirectX::CreateDDSTextureFromFileEx(m_pDevice, pFilterPath, 0, D3D11_USAGE_DYNAMIC, D3D11_BIND_SHADER_RESOURCE, D3D11_CPU_ACCESS_WRITE, 0, false, (ID3D11Resource**)&pFilterTex, &m_pFilterSRV);

		D3D11_TEXTURE2D_DESC	TextureDesc; // bgra
		ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
		pFilterTex->GetDesc(&TextureDesc);

		m_iFilterWidth = TextureDesc.Width;
		m_iFilterHeight = TextureDesc.Height;
		// m_pFilterPixels.resize(static_cast<size_t>(m_iFilterWidth) * m_iFilterHeight);

		// Safe_Release(pFilterTex);

		// 다운로드용 텍스쳐 생성
		ID3D11Texture2D* pFilterStageTex = nullptr;
		ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		TextureDesc.Width = m_iFilterWidth;
		TextureDesc.Height = m_iFilterHeight;
		TextureDesc.MipLevels = 1;
		TextureDesc.ArraySize = 1;
		TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.SampleDesc.Count = 1;

		TextureDesc.Usage = D3D11_USAGE_STAGING;
		TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		TextureDesc.MiscFlags = 0;

		FAILED_CHECK(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pFilterStageTex))
		// 필터 텍스쳐 생성


		m_pContext->CopyResource(pFilterStageTex, pFilterTex);

		D3D11_MAPPED_SUBRESOURCE		SubResource;
		ZeroMemory(&SubResource, sizeof SubResource);
		m_pContext->Map(pFilterStageTex, 0, D3D11_MAP_READ, 0, &SubResource);
		PIXEL* ptr = static_cast<PIXEL*>(SubResource.pData);
		m_pFilterPixels.assign(ptr, ptr + (m_iFilterWidth * m_iFilterHeight));
		m_pContext->Unmap(pFilterStageTex, 0);


		Safe_Release(pFilterStageTex);
		Safe_Release(pFilterTex);
	}
	else
	{
		// m_iFilterWidth = 256;
		// m_iFilterHeight = 256;
		// m_pFilterPixels.resize(static_cast<size_t>(m_iFilterWidth) * m_iFilterHeight, 0xffffffff);

	
	}
}

bool CTerrain::PickTerrain(_float4& vPickPos)
{
	return m_pVIBufferCom->PickTerrainVtx(vPickPos, m_pTransformCom->Get_WorldMatrix_Inverse());
}

void CTerrain::OverwriteFilterTex()
{
	D3D11_MAPPED_SUBRESOURCE		SubResource;
	ZeroMemory(&SubResource, sizeof SubResource);

	ID3D11Resource* pFilterTex = nullptr;
	m_pFilterSRV->GetResource(&pFilterTex);
	NULL_CHECK(pFilterTex)

	CONTEXT_LOCK
	m_pContext->Map(pFilterTex, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	memcpy(SubResource.pData, m_pFilterPixels.data(), sizeof(_ulong) * m_iFilterWidth * m_iFilterHeight);
	m_pContext->Unmap(pFilterTex, 0);

	Safe_Release(pFilterTex);
}

void CTerrain::EditFilter(_float fPower, _uint iEditFilter)// 0,1,2,3 : bgra
{
	if (iEditFilter > 3) return;

	// 지형은 사이즈 1 사각형을 한칸으로, 필터는 1 픽셀을 한칸으로 사용한다.
	// 이때 지형과 필터의 '한칸' 의 개수가 다르기 때문이 이를 보정하는 작업을 해야한다.
	const _float fXRatio = _float(m_iFilterWidth) / m_pVIBufferCom->GetNumVtxX();
	const _float fZRatio = _float(m_iFilterHeight) / m_pVIBufferCom->GetNumVtxZ();

	const Vector2 v2BrushPos{m_vBrushPos.x * fXRatio, m_vBrushPos.z * fZRatio};
	const _float fBrushRange = m_fBrushRange * fXRatio;

    const _uint iMinZ = static_cast<_uint>(CMathUtils::Clamp(v2BrushPos.y - fBrushRange, 0.f, static_cast<_float>(m_iFilterHeight)));
    const _uint iMaxZ = static_cast<_uint>(CMathUtils::Clamp(v2BrushPos.y + fBrushRange, 0.f, static_cast<_float>(m_iFilterHeight)));
    const _uint iMinX = static_cast<_uint>(CMathUtils::Clamp(v2BrushPos.x - fBrushRange, 0.f, static_cast<_float>(m_iFilterWidth)));
    const _uint iMaxX = static_cast<_uint>(CMathUtils::Clamp(v2BrushPos.x + fBrushRange, 0.f, static_cast<_float>(m_iFilterWidth)));

	for (_uint i = iMinZ; i < iMaxZ; ++i) // z
	{
		for (_uint j = iMinX; j < iMaxX; ++j) // x
		{
			Vector2 v2Cur{static_cast<_float>(j), static_cast<_float>(i)};
			const _float fLength = (v2BrushPos - v2Cur).Length();

			if (fLength > fBrushRange)
				continue;

			const _float fAlpha = (fBrushRange - fLength) / fBrushRange;
			const _uint	iIndex = i * m_iFilterWidth + j;

			const _float fPixelValue = CMathUtils::Clamp( static_cast<_float>(m_pFilterPixels[iIndex].m[iEditFilter]) + fPower * fAlpha, 0.f, 255.f);
			m_pFilterPixels[iIndex].m[iEditFilter] = static_cast<_ubyte>(fPixelValue);
		}
	}

	OverwriteFilterTex();
}

void CTerrain::SaveFilter()
{
	ID3D11Resource* pFilterTex = nullptr;
	m_pFilterSRV->GetResource(&pFilterTex);
	NULL_CHECK(pFilterTex)

	FAILED_CHECK(DirectX::SaveDDSTextureToFile(m_pContext, pFilterTex, TEXT("../Bin/Resources/Textures/Terrain/Filter.dds")));

	Safe_Release(pFilterTex);
}

void CTerrain::ResetFilter()
{
	for (auto& pixel : m_pFilterPixels)
		pixel.color = 0xfffffffful;
	OverwriteFilterTex();
}

HRESULT CTerrain::SetUp_Components()
{
	/* For.Com_Renderer */
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom))

	/* For.Com_Shader */
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom))


	/* For.Com_VIBuffer */
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom))

	/* For.Com_Texture */
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom[TYPE_DIFFUSE]))

	/* For.Com_Brush*/
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("../Bin/Resources/Textures/Terrain/Brush.png"), TEXT("Com_Brush"),
		(CComponent**)&m_pTextureCom[TYPE_BRUSH]))

	/* For.Com_Filter */
	// FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("../Bin/Resources/Textures/Terrain/Filter.bmp"), TEXT("Com_Filter"),
	// 	(CComponent**)&m_pTextureCom[TYPE_FILTER]))

	// m_pTextureCom[TYPE_FILTER] = CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/Terrain/Filter.bmp");



	

	return S_OK;
}

HRESULT CTerrain::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))

	FAILED_CHECK(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)))
	FAILED_CHECK(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)))


	/* For.Lights */
	const LIGHTDESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;


	FAILED_CHECK(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResources(m_pShaderCom, "g_DiffuseTexture"))
	FAILED_CHECK(m_pTextureCom[TYPE_BRUSH]->Bind_ShaderResource(m_pShaderCom, "g_BrushTexture", 0))
	// FAILED_CHECK(m_pTextureCom[TYPE_FILTER]->Bind_ShaderResource(m_pShaderCom, "g_FilterTexture", 0))
	FAILED_CHECK(m_pShaderCom->Set_ShaderResourceView("g_FilterTexture", m_pFilterSRV)) 

	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_bEditMode", &m_bEditMode, sizeof(_uint)))
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_fBrushRange", &m_fBrushRange, sizeof(_float)))
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vBrushPos", &m_vBrushPos, sizeof(_float4)))

	return S_OK;
}

CTerrain * CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CTerrain*		pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTerrain::Clone(void * pArg)
{
	CTerrain*		pInstance = new CTerrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	for (auto& pTextureCom : m_pTextureCom)	
		Safe_Release(pTextureCom);	

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pGameInstance);

	Safe_Release(m_pFilterSRV);
}
