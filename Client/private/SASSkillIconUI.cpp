#include "stdafx.h"
#include "..\public\SASSkillIconUI.h"
#include "GameInstance.h"

CSASSkillIconUI::CSASSkillIconUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSASSkillIconUI::CSASSkillIconUI(const CSASSkillIconUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSASSkillIconUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASSkillIconUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSASSkillIconUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	// x: [0] 염력 [1] 염력 [2] 발화 [3] 경질 [4] 투시 [5] 순간이동 [6] 투명 [7] 방전 [8] 복제 [9] 초고속 [10] 사용 불가
	// y: [0] 스킬 사용 불 가능 (그냥) / [1] 스킬 사용 가능(불들어오는거)
	m_tParams.Float2s[0] = _float2(1, 0);
	// [0] 스킬 사용 가능(이미지) / [1] 스킬 사용 불 가능(검정색) -> 그냥 Float4s[0] 로 변경해도 되는데 이게 직관적인 것 같아서요
	m_tParams.Ints[0] = { 1 };
}

void CSASSkillIconUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CSASSkillIconUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASSkillIconUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

	static array<const char*, SUPERPOWERS_END> arrSuperPowersName{
		"PSYCHOKINESIS0", "PSYCHOKINESIS1", "IGNITION", "RESHUFFLE", "CLAIRVOYANCE", "TELEPORTATION", "TRANSPARENCY", "DISCHARGE", "COPY", 
	};

	if (ImGui::BeginCombo("SAS Skill", arrSuperPowersName[static_cast<_uint>(m_eSuperPowers)]))
	{
		for (int i = PSYCHOKINESIS0; i < SUPERPOWERS_END; ++i)
		{
			const bool bSelected = false;
			if (ImGui::Selectable(arrSuperPowersName[i], bSelected))
				m_eSuperPowers = SUPERPOWERS(i);
		}

		ImGui::EndCombo();
	}
}

void CSASSkillIconUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);



	//json["SuperPowers"] = m_eSuperPowers;
}

void CSASSkillIconUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

	//m_eSuperPowers = json["SuperPowers"];
}

void CSASSkillIconUI::Key_Input()
{
	//  TODO : 스킬 사용
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->KeyDown(DIK_1))
	{
		
	}
	if (pGameInstance->KeyDown(DIK_2))
	{

	}
	if (pGameInstance->KeyDown(DIK_3))
	{

	}
	if (pGameInstance->KeyDown(DIK_4))
	{

	}
}

void CSASSkillIconUI::SASSkillIcon_Tick()
{
	if (m_eSASSkill != m_ePreSASSkill)
	{
		switch (m_eSASSkill)
		{
		case ONE:
			break;
		case TWO:
			break;
		case THREE:
			break;
		case FOUR:
			break;
		}

		m_ePreSASSkill = m_eSASSkill;
	}
}

CSASSkillIconUI * CSASSkillIconUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASSkillIconUI*		pInstance = new CSASSkillIconUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASSkillIconUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSASSkillIconUI::Clone(void * pArg)
{
	CSASSkillIconUI*		pInstance = new CSASSkillIconUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASSkillIconUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASSkillIconUI::Free()
{
	__super::Free();

}
