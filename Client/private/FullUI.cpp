#include "stdafx.h"
#include "..\public\FullUI.h"
#include "GameInstance.h"

CFullUI::CFullUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CFullUI::CFullUI(const CFullUI& rhs)
	: CUI(rhs)
{
}

HRESULT CFullUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFullUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CFullUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_fSizeX = _float(g_iWinSizeX);
	m_fSizeY = _float(g_iWinSizeY);
}

void CFullUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CFullUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CFullUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CFullUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CFullUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CFullUI * CFullUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CFullUI*		pInstance = new CFullUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFullUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CFullUI::Clone(void * pArg)
{
	CFullUI*		pInstance = new CFullUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFullUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFullUI::Free()
{
	__super::Free();

}
