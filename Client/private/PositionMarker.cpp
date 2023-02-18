#include "stdafx.h"
#include "..\public\PositionMarker.h"
#include "GameInstance.h"

CPositionMarker::CPositionMarker(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CPositionMarker::CPositionMarker(const CPositionMarker& rhs)
	: CUI(rhs)
{
}

HRESULT CPositionMarker::Initialize(void* pArg)
{
	m_fSizeX = 100.f;
	m_fSizeY = 100.f;

	FAILED_CHECK(__super::Initialize(pArg));

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTexUI"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer_Rect"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_NOW, L"../Bin/Resources/Textures/UI/TobBarHUD/Symbol/%d.png", TEXT("TextureCom"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// 0: attack, 1 : defence

	return S_OK;
}

HRESULT CPositionMarker::Render()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_ORTHO))))
		return E_FAIL;

	_int iFlip = 0;

	m_pShaderCom->Set_Param_Texture(0, m_pTextureCom, m_bAttacker ? 0 : 1);
	m_pShaderCom->Set_Param_Int(0, &iFlip);
	m_pShaderCom->Set_Param_float4(0, &m_vColor);
	m_pShaderCom->Begin(5);
	m_pVIBufferCom->Render();

	return S_OK;
}


void CPositionMarker::Free()
{
	CUI::Free();
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
