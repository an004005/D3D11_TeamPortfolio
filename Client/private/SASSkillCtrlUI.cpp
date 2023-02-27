#include "stdafx.h"
#include "..\public\SASSkillCtrlUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

// 

CSASSkillCtrlUI::CSASSkillCtrlUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSASSkillCtrlUI::CSASSkillCtrlUI(const CSASSkillCtrlUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSASSkillCtrlUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASSkillCtrlUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	

	return S_OK;
}

void CSASSkillCtrlUI::BeginTick()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pCanvas = dynamic_cast<CCanvas_SASSkill*>(pGameInstance->Find_Prototype(LEVEL_NOW, TEXT("Canvas_SASSkill")));
}

void CSASSkillCtrlUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

}

void CSASSkillCtrlUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CSASSkillCtrlUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASSkillCtrlUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CSASSkillCtrlUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CSASSkillCtrlUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CSASSkillCtrlUI * CSASSkillCtrlUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASSkillCtrlUI*		pInstance = new CSASSkillCtrlUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASSkillCtrlUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSASSkillCtrlUI::Clone(void * pArg)
{
	CSASSkillCtrlUI*		pInstance = new CSASSkillCtrlUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASSkillCtrlUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASSkillCtrlUI::Free()
{
	__super::Free();

}
