#include "stdafx.h"
#include "..\public\Main_SkillIconUI.h"
#include "GameInstance.h"

// m_tParams.Float2s[0].x : [0] 염력 [1] 염력 [2] 발화 [3] 경질 [4] 투시 [5] 순간이동 [6] 투명 [7] 방전 [8] 복제 [9] 초고속

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
	if (false == m_bVisible) return;

	__super::Tick(TimeDelta);

}

void CMain_SkillIconUI::Late_Tick(_double TimeDelta)
{
	if (false == m_bVisible) return;

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

void CMain_SkillIconUI::Set_Icon(const _float & fIcon)
{
	// [0] 염력 [1] 염력 [2] 발화 [3] 경질 [4] 투시 [5] 순간이동 [6] 투명 [7] 방전 [8] 복제 [9] 초고속
	m_tParams.Float2s[0].x = fIcon;
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
