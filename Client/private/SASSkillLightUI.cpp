#include "stdafx.h"
#include "..\public\SASSkillLightUI.h"
#include "GameInstance.h"
\
//m_tParams.Float4s[0] = { 226.0f, 158.0f, 1.0f, 0.7f };	// 색상 조정 a: 0.0f 미사용 0.7f 사용

CSASSkillLightUI::CSASSkillLightUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSASSkillLightUI::CSASSkillLightUI(const CSASSkillLightUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSASSkillLightUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASSkillLightUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	SkilInfo_Initialize();

	return S_OK;
}

void CSASSkillLightUI::BeginTick()
{
	m_pCanvas = dynamic_cast<CCanvas_SASSkill*>(CGameInstance::GetInstance()->Find_Prototype(LEVEL_NOW, TEXT("Canvas_SASSkill")));

}

void CSASSkillLightUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	//  TODO : 키 입력 삭제해야 한다. 플레이어 에서 Set 해야한다.
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	if (CCanvas_SASSkill::ONE == m_eObjectCount)
	{
		static _bool bOnSkil;
		if (pGameInstance->KeyDown(DIK_0))
		{
			bOnSkil = !bOnSkil;
		}

		m_pCanvas->Set_InputSkill(CCanvas_SASSkill::ONE0, bOnSkil);

		if (m_pCanvas->Get_InputSkill(CCanvas_SASSkill::ONE0))
		{
			m_dLight_TimeAcc += TimeDelta;

			if (0.2 < m_dLight_TimeAcc)
			{
				m_tParams.Float4s[0].w = { 0.0f };
				m_dLight_TimeAcc = 0.0;
			}
			else
			{
				m_tParams.Float4s[0].w = { 0.7f };
			}
		
		}
		else
		{
			m_dLight_TimeAcc = 0.0;
			m_tParams.Float4s[0].w = { 0.0f };
		}
	}
}

void CSASSkillLightUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CSASSkillLightUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASSkillLightUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CSASSkillLightUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CSASSkillLightUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CSASSkillLightUI::SkilInfo_Initialize()
{
	static _uint eObjectCount;
	m_eObjectCount = CCanvas_SASSkill::OBJECTCOUNT(eObjectCount);
	++eObjectCount;

	switch (m_eObjectCount)
	{
	case Client::CCanvas_SASSkill::ONE:
	{

	}
	break;
	case Client::CCanvas_SASSkill::TWO:
	{

	}
	break;
	case Client::CCanvas_SASSkill::THREE:
	{

	}
	break;
	case Client::CCanvas_SASSkill::FOUR:
	{

	}
	break;
	}
}

void CSASSkillLightUI::ChangeSkill()
{
	switch (m_pCanvas->Get_SuperPowers())
	{
	case CCanvas_SASSkill::PSYCHOKINESIS0:	//0
	{
		m_tParams.Float4s[0] = { 0.816f, 0.275f, 0.973f, 0.7f };
	}
		break;
	case CCanvas_SASSkill::PSYCHOKINESIS1:	//1
	{
		m_tParams.Float4s[0] = { 0.816f, 0.275f, 0.973f, 0.7f };
	}
		break;
	case CCanvas_SASSkill::IGNITION:	//2
	{
		m_tParams.Float4s[0] = { 0.970f, 0.309f, 0.418f, 0.7f };
	}
		break;
	case CCanvas_SASSkill::RESHUFFLE:	//3
	{
		m_tParams.Float4s[0] = { 1.0f, 0.824f, 0.427f, 1.0f };
	}
		break;
	case CCanvas_SASSkill::CLAIRVOYANCE:	//4
	{
		m_tParams.Float4s[0] = { 0.388f, 0.992f, 0.455f, 1.0f };

	}
		break;
	case CCanvas_SASSkill::TELEPORTATION:	//5
	{
		m_tParams.Float4s[0] = { 0.071f, 0.439f, 0.753f, 0.7f };

	}
		break;
	case CCanvas_SASSkill::TRANSPARENCY:	//6
	{
		m_tParams.Float4s[0] = { 0.071f, 0.604f, 0.635f, 0.7f };

	}
		break;
	case CCanvas_SASSkill::DISCHARGE:	//7
	{
		m_tParams.Float4s[0] = { 1.0f, 0.713f, 0.212f, 0.7f };

	}
		break;
	case CCanvas_SASSkill::COPY:	//8
	{
		m_tParams.Float4s[0] = { 0.235f, 0.345f, 0.894f, 0.7f };

	}
		break;
	case CCanvas_SASSkill::HIGHSPEED:	//9
	{
		m_tParams.Float4s[0] = { 0.910f, 0.231f, 0.659f, 1.0f };

	}
		break;
	case CCanvas_SASSkill::NOT:	//10
	{
		m_tParams.Float4s[0] = { 0.0f, 0.0f, 0.0f, 0.0f };

	}
		break;
	}
}

void CSASSkillLightUI::ChangeSkill_TickShader(const _float & fTimeDelta)
{
	//if (CCanvas_SASSkill::Get_SASSkill() != CCanvas_SASSkill::Get_PreSASSkill())
	//{
	//	ChangeSkill(); // 스킬이 바뀌면 쉐이더 값도 변경해준다.

	//	if (true == CCanvas_SASSkill::Get_ChangeX())
	//	{
	//		switch (CCanvas_SASSkill::Get_SASSkill())
	//		{
	//		case CCanvas_SASSkill::ONE0:
	//			m_tParams.Floats[0] = m_fSkill_TimeAcc;
	//			break;
	//		case CCanvas_SASSkill::TWO0:
	//			m_tParams.Floats[0] = m_fSkill_TimeAcc;
	//			break;
	//		case CCanvas_SASSkill::THREE0:
	//			m_tParams.Floats[0] = m_fSkill_TimeAcc;
	//			break;
	//		case CCanvas_SASSkill::FOUR0:
	//			m_tParams.Floats[0] = m_fSkill_TimeAcc;
	//			break;
	//		}
	//	}
	//	else
	//	{
	//		switch (CCanvas_SASSkill::Get_SASSkill())
	//		{
	//		case CCanvas_SASSkill::ONE1:
	//			m_tParams.Floats[0] = m_fSkill_TimeAcc;
	//			break;
	//		case CCanvas_SASSkill::TWO1:
	//			m_tParams.Floats[0] = m_fSkill_TimeAcc;
	//			break;
	//		case CCanvas_SASSkill::THREE1:
	//			m_tParams.Floats[0] = m_fSkill_TimeAcc;
	//			break;
	//		case CCanvas_SASSkill::FOUR1:
	//			m_tParams.Floats[0] = m_fSkill_TimeAcc;
	//			break;
	//		}
	//	}

	//	CCanvas_SASSkill::Set_PreSASSkill(CCanvas_SASSkill::Get_SASSkill());
	//}
}

CSASSkillLightUI * CSASSkillLightUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASSkillLightUI*		pInstance = new CSASSkillLightUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASSkillLightUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSASSkillLightUI::Clone(void * pArg)
{
	CSASSkillLightUI*		pInstance = new CSASSkillLightUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASSkillLightUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASSkillLightUI::Free()
{
	CUI::Free();

}
