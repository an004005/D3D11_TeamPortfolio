#include "stdafx.h"
#include "..\public\Main_SkillIconUI.h"
#include "GameInstance.h"

CMain_SkillIconUI::CMain_SkillIconUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMain_SkillIconUI::CMain_SkillIconUI(const CMain_SkillIconUI& rhs)
	: CUI(rhs)
{
}

HRESULT CMain_SkillIconUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMain_SkillIconUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMain_SkillIconUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CMain_SkillIconUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CMain_SkillIconUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CMain_SkillIconUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CMain_SkillIconUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CMain_SkillIconUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CMain_SkillIconUI * CMain_SkillIconUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CMain_SkillIconUI*		pInstance = new CMain_SkillIconUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMain_SkillIconUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CMain_SkillIconUI::Clone(void * pArg)
{
	CMain_SkillIconUI*		pInstance = new CMain_SkillIconUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMain_SkillIconUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMain_SkillIconUI::Free()
{
	__super::Free();

}
