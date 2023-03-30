#include "stdafx.h"
#include "..\public\Main_SkillNameUI.h"
#include "GameInstance.h"

CMain_SkillNameUI::CMain_SkillNameUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CMain_SkillNameUI::CMain_SkillNameUI(const CMain_SkillNameUI& rhs)
	: CUI(rhs)
{
}

HRESULT CMain_SkillNameUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMain_SkillNameUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMain_SkillNameUI::Tick(_double TimeDelta)
{
	if (false == m_bVisible) return;
	
	__super::Tick(TimeDelta);
}

void CMain_SkillNameUI::Late_Tick(_double TimeDelta)
{
	if (false == m_bVisible) return;
	
	__super::Late_Tick(TimeDelta);
}

HRESULT CMain_SkillNameUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CMain_SkillNameUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CMain_SkillNameUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CMain_SkillNameUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CMain_SkillNameUI::Set_Name(const _float & fIcon)
{
	//[0] 염력 [1] 염력 [2] 발화 [3] 경질화 [4] 투시 [5] 순간이동 [6] 투명화 [7] 방전 [8] 복제 [9] 초고속
	m_tParams.Float2s[0].y = fIcon;
}

CMain_SkillNameUI * CMain_SkillNameUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CMain_SkillNameUI*		pInstance = new CMain_SkillNameUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMain_SkillNameUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CMain_SkillNameUI::Clone(void * pArg)
{
	CMain_SkillNameUI*		pInstance = new CMain_SkillNameUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMain_SkillNameUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMain_SkillNameUI::Free()
{
	__super::Free();

}
