#include "stdafx.h"
#include "..\public\LastCheckUI.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CLastCheckUI::CLastCheckUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CLastCheckUI::CLastCheckUI(const CLastCheckUI& rhs)
	: CUI(rhs)
{
}

HRESULT CLastCheckUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLastCheckUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_bVisible = true;
	CUI_Manager::GetInstance()->Set_TempOff(true);

	return S_OK;
}

void CLastCheckUI::BeginTick()
{


}

void CLastCheckUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	if (CGameInstance::GetInstance()->KeyDown(DIK_RETURN))
	{
		CGameObject::SetDelete();
	}
}

void CLastCheckUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CLastCheckUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CLastCheckUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

CLastCheckUI * CLastCheckUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CLastCheckUI*		pInstance = new CLastCheckUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLastCheckUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CLastCheckUI::Clone(void * pArg)
{
	CLastCheckUI*		pInstance = new CLastCheckUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLastCheckUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLastCheckUI::Free()
{
	CUI::Free();

}
