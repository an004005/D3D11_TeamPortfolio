#include "stdafx.h"
#include "..\public\SASSkillDefaultUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

// 

CSASSkillDefaultUI::CSASSkillDefaultUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas_SASSkill(pDevice, pContext)
{
}

CSASSkillDefaultUI::CSASSkillDefaultUI(const CSASSkillDefaultUI& rhs)
	: CCanvas_SASSkill(rhs)
{
}

HRESULT CSASSkillDefaultUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASSkillDefaultUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

void CSASSkillDefaultUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	CCanvas_SASSkill::SASSkill_UIMove(TimeDelta);
}

void CSASSkillDefaultUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CSASSkillDefaultUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASSkillDefaultUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CSASSkillDefaultUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CSASSkillDefaultUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CSASSkillDefaultUI * CSASSkillDefaultUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASSkillDefaultUI*		pInstance = new CSASSkillDefaultUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASSkillDefaultUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas_SASSkill * CSASSkillDefaultUI::Clone(void * pArg)
{
	CSASSkillDefaultUI*		pInstance = new CSASSkillDefaultUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASSkillDefaultUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASSkillDefaultUI::Free()
{
	CUI::Free();

}
