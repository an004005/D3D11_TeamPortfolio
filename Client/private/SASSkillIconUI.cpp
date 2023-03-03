#include "stdafx.h"
#include "..\public\SASSkillIconUI.h"
#include "GameInstance.h"
#include "FSMComponent.h"

// [0] �ʴɷ� ��� ����(�̹���) / [1] �ʴɷ� ��� �� ����(������) -> �׳� Float4s[0] �� �����ص� �Ǵµ� �̰� �������� �� ���Ƽ���
//m_tParams.Ints[0] = { 1 };

// x: [0] ���� [1] ���� [2] ��ȭ [3] ���� [4] ���� [5] �����̵� [6] ���� [7] ���� [8] ���� [9] �ʰ�� [10] ��� �Ұ�
// y: [0] �ʴɷ� ��� �� ���� (�׳�) / [1] �ʴɷ� ��� ����(�ҵ����°�)
//m_tParams.Float2s[0] = _float2(1.0f, 0.0f);

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

	//static _uint eObjectCount;
	//m_eObjectCount = CUI::OBJECTCOUNT(eObjectCount);
	//++eObjectCount;

	//if (3 == eObjectCount)
	//	eObjectCount = 0;

	return S_OK;
}

void CSASSkillIconUI::BeginTick()
{
	//m_pCanvas = dynamic_cast<CCanvas_SASSkill*>(CGameInstance::GetInstance()->Find_Prototype(LEVEL_NOW, TEXT("Canvas_SASSkill")));

}

void CSASSkillIconUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	if (CGameInstance::GetInstance()->KeyDown(DIK_8))
	{
		asdfasdfasdfasdf = !asdfasdfasdfasdf;
	}

	//m_pFSM->Tick(TimeDelta);

}

void CSASSkillIconUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CSASSkillIconUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASSkillIconUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

	//static array<const char*, SUPERPOWERS_END> arrSuperPowersName{
	//	"PSYCHOKINESIS0", "PSYCHOKINESIS1", "IGNITION", "RESHUFFLE", "CLAIRVOYANCE", "TELEPORTATION", "TRANSPARENCY", "DISCHARGE", "COPY", "NON"
	//};

	//if (ImGui::BeginCombo("SAS Skill", arrSuperPowersName[static_cast<_uint>(CUI::Get_SuperPowers())]))
	//{
	//	for (int i = PSYCHOKINESIS0; i < SUPERPOWERS_END; ++i)
	//	{
	//		const bool bSelected = false;
	//		if (ImGui::Selectable(arrSuperPowersName[i], bSelected))
	//			CUI::Set_SuperPowers(SUPERPOWERS(i));
	//	}

	//	ImGui::EndCombo();
	//}
}

void CSASSkillIconUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

	//json["SuperPowers"] = CUI::Get_SuperPowers();
}

void CSASSkillIconUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

	//CUI::Set_SuperPowers(SUPERPOWERS(json["SuperPowers"]));
}

void CSASSkillIconUI::SASSkillIcon_Tick()
{
	//if (m_pCanvas->Get_SASSkill() != m_pCanvas->Get_PreSASSkill())
	//{
	//	ChangeSkill_Shader(); // ��ų�� �ٲ�� ���̴� ���� �������ش�.

	//	if (true == m_pCanvas->Get_ChangeX())
	//	{
	//		switch (m_pCanvas->Get_SASSkill())
	//		{
	//		case CCanvas_SASSkill::ONE0:
	//			break;
	//		case CCanvas_SASSkill::TWO0:
	//			break;
	//		case CCanvas_SASSkill::THREE0:
	//			break;
	//		case CCanvas_SASSkill::FOUR0:
	//			break;
	//		default:
	//			assert(!"Wrong Skill Icon Number");
	//			break;
	//		}
	//	}
	//	else
	//	{
	//		switch (m_pCanvas->Get_SASSkill())
	//		{
	//		case CCanvas_SASSkill::ONE1:
	//			break;
	//		case CCanvas_SASSkill::TWO1:
	//			break;
	//		case CCanvas_SASSkill::THREE1:
	//			break;
	//		case CCanvas_SASSkill::FOUR1:
	//			break;
	//		default:
	//			assert(!"Wrong Skill Icon Number");
	//			break;
	//		}
	//	}

	//	m_pCanvas->Set_PreSASSkill(m_pCanvas->Get_SASSkill());
	//}
}

void CSASSkillIconUI::ChangeSkill_Shader()
{
	//m_tParams.Ints[0] = { !m_pCanvas->Get_OnSkill() };
	//m_tParams.Float2s[0] = { _float(m_pCanvas->Get_SuperPowers()), _float(m_pCanvas->Get_OnSkill()) };
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
	CUI::Free();

}
