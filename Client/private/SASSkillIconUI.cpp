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

	// x: [0] ���� [1] ���� [2] ��ȭ [3] ���� [4] ���� [5] �����̵� [6] ���� [7] ���� [8] ���� [9] �ʰ�� [10] ��� �Ұ�
	// y: [0] ��ų ��� �� ���� (�׳�) / [1] ��ų ��� ����(�ҵ����°�)
	m_tParams.Float2s[0] = _float2(1, 0);
	// [0] ��ų ��� ����(�̹���) / [1] ��ų ��� �� ����(������) -> �׳� Float4s[0] �� �����ص� �Ǵµ� �̰� �������� �� ���Ƽ���
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
	//  TODO : ��ų ���
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
