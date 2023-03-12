#include "stdafx.h"
#include "..\public\SASSkillGaugeUI.h"
#include "GameInstance.h"

//m_tParams.Floats[0] = _float(TimeDelta) * m_fGaugeSpeed;	// 게이지 조정
//m_tParams.Float4s[0] = { 226.0f, 158.0f, 1.0f, 1.0f };	// 색상 조정

CSASSkillGaugeUI::CSASSkillGaugeUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSASSkillGaugeUI::CSASSkillGaugeUI(const CSASSkillGaugeUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSASSkillGaugeUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASSkillGaugeUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	SkilInfo_Initialize();

	return S_OK;
}

void CSASSkillGaugeUI::BeginTick()
{

}

void CSASSkillGaugeUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	//  TODO : 키 입력 삭제해야 한다. 플레이어 에서 Set 해야한다.
	//CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	//if (CCanvas_SASSkill::ONE == m_eObjectCount)
	//{
	//	static _bool bOnSkil;
	//	if (pGameInstance->KeyDown(DIK_0))
	//	{
	//		bOnSkil = !bOnSkil;
	//	}

	//	m_pCanvas->Set_InputSkill(CCanvas_SASSkill::ONE0, bOnSkil);

	//	if (m_pCanvas->Get_InputSkill(CCanvas_SASSkill::ONE0))
	//	{
	//		if (0.0f < m_tParams.Floats[0])
	//			m_tParams.Floats[0] -= _float(TimeDelta) /** m_fGaugeSpeed*/;
	//	}
	//	else
	//	{
	//		if(1.0f > m_tParams.Floats[0])
	//			m_tParams.Floats[0] += _float(TimeDelta) /** m_fGaugeSpeed*/;
	//	}

	//	
	//	m_pCanvas->Set_SuperPowers(CCanvas_SASSkill::SUPERPOWERS(1));
	//}
}

void CSASSkillGaugeUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CSASSkillGaugeUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASSkillGaugeUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CSASSkillGaugeUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CSASSkillGaugeUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CSASSkillGaugeUI::SkilInfo_Initialize()
{
	//static _uint eObjectCount;
	//m_eObjectCount = CCanvas_SASSkill::OBJECTCOUNT(eObjectCount);
	//++eObjectCount;

	//switch (m_eObjectCount)
	//{
	//case Client::CCanvas_SASSkill::ONE:
	//{

	//}
	//	break;
	//case Client::CCanvas_SASSkill::TWO:
	//{

	//}
	//	break;
	//case Client::CCanvas_SASSkill::THREE:
	//{

	//}
	//	break;
	//case Client::CCanvas_SASSkill::FOUR:
	//{

	//}
	//	break;
	//default:
	//	assert(!"Wrong Skillgauge Number");
	//	break;
	//}
}

void CSASSkillGaugeUI::ChangeSkill()
{
	//switch (m_pCanvas->Get_SuperPowers())
	//{
	//case CCanvas_SASSkill::PSYCHOKINESIS0:	// 0
	//{
	//	m_tParams.Floats[0] = 1.0f;
	//	m_tParams.Floats[1] = 1.0f;
	//	m_tParams.Float4s[0] = { 0.928f, 0.803f, 0.981f, 1.0f };
	//	m_fMinMaxGauge = { 0.0f, 1.0f };
	//	m_fGaugeSpeed = { 0.5f };

	//	m_tParams.Float4s[0] = { 0.826f, 0.420f, 1.0f, 1.0f };
	//}
	//	break;
	//case CCanvas_SASSkill::PSYCHOKINESIS1:	// 1
	//{
	//	m_tParams.Floats[0] = 1.0f;
	//	m_tParams.Floats[1] = 1.0f;
	//	m_tParams.Float4s[0] = { 0.928f, 0.803f, 0.981f, 1.0f };
	//	m_fMinMaxGauge = { 0.0f, 1.0f };
	//	m_fGaugeSpeed = { 0.5f };

	//	m_tParams.Float4s[0] = { 0.826f, 0.420f, 1.0f, 1.0f };
	//}
	//	break;
	//case CCanvas_SASSkill::IGNITION:	// 2
	//{
	//	m_tParams.Floats[0] = 1.0f;
	//	m_tParams.Floats[1] = 7.0f; 삭제예정
	//	m_tParams.Float4s[0] = { 1.0f, 0.857f, 0.841f, 1.0f };
	//	m_fMinMaxGauge = { 0.0f, 1.0f };
	//	m_fGaugeSpeed = { 0.2f };

	//	m_tParams.Float4s[0] = { 0.992f, 0.449f, 0.387f, 1.0f };
	//}
	//	break;
	//case CCanvas_SASSkill::RESHUFFLE:	// 3
	//{
	//	m_tParams.Floats[0] = 1.0f;
	//	m_tParams.Float4s[0] = { 1.0f, 0.968f, 0.871f, 1.0f };
	//	m_fMinMaxGauge = { 0.0f, 1.0f };
	//	m_fGaugeSpeed = { 0.8f };
	
	//	m_tParams.Float4s[0] = { 0.989f, 0.761f, 0.401f, 1.0f };
	//}
	//	break;
	//case CCanvas_SASSkill::CLAIRVOYANCE:	// 4
	//{
	//	m_tParams.Floats[0] = 1.0f;
	//	m_tParams.Float4s[0] = { 0.814f, 1.0f, 0.886f, 1.0f };
	//	m_fMinMaxGauge = { 0.0f, 1.0f };
	//	m_fGaugeSpeed = { 0.8f };

	//	m_tParams.Float4s[0] = { 0.378f, 0.754f, 0.217f, 1.0f };

	//}
	//	break;
	//case CCanvas_SASSkill::TELEPORTATION:	// 5
	//{
	//	m_tParams.Floats[0] = 1.0f;
	//	m_tParams.Float4s[0] = { 0.803f, 0.977f, 0.981f, 1.0f };
	//	m_fMinMaxGauge = { 0.0f, 1.0f };
	//	m_fGaugeSpeed = { 0.8f };

	//	m_tParams.Float4s[0] = { 0.420f, 0.605f, 1.0f, 1.0f };

	//}
	//	break;
	//case CCanvas_SASSkill::TRANSPARENCY:	// 6
	//{
	//	m_tParams.Floats[0] = 1.0f;
	//	m_tParams.Float4s[0] = { 0.280f, 0.754f, 0.657f, 1.0f };
	//	m_fMinMaxGauge = { 0.0f, 1.0f };
	//	m_fGaugeSpeed = { 0.8f };

	//	m_tParams.Float4s[0] = { 1.0f, 1.0f, 1.0f, 1.0f };

	//}
	//	break;
	//case CCanvas_SASSkill::DISCHARGE:	// 7
	//{
	//	m_tParams.Floats[0] = 1.0f;
	//	m_tParams.Float4s[0] = { 0.833f, 0.798f, 0.319f, 1.0f };
	//	m_fMinMaxGauge = { 0.0f, 1.0f };
	//	m_fGaugeSpeed = { 0.8f };

	//	m_tParams.Float4s[0] = { 1.0f, 1.0f, 1.0f, 1.0f };

	//}
	//	break;
	//case CCanvas_SASSkill::COPY:	// 8
	//{
	//	m_tParams.Floats[0] = 1.0f;
	//	m_tParams.Float4s[0] = { 0.598f, 0.781f, 0.981f, 1.0f };
	//	m_fMinMaxGauge = { 0.0f, 1.0f };
	//	m_fGaugeSpeed = { 0.8f };

	//	m_tParams.Float4s[0] = { 0.379f, 0.392f, 0.943f, 1.0f };

	//}
	//	break;
	//case CCanvas_SASSkill::HIGHSPEED:	// 9
	//{
	//	m_tParams.Float4s[0] = { 1.0f, 0.553f, 0.878f, 1.0f };

	//}
	//	break;
	//case CCanvas_SASSkill::NOT:	// 10
	//{
	//	m_tParams.Float4s[0] = { 0.0f, 0.0f, 0.0f, 0.0f };

	//}
	//	break;
	//default:
	//	assert(!"Wrong Skillgauge Number");
	//	break;
	//}
}

void CSASSkillGaugeUI::ChangeSkill_TickShader(const _float & fTimeDelta)
{
	//m_fSkill_TimeAcc += fTimeDelta;

	//if (m_pCanvas->Get_SASSkill() != m_pCanvas->Get_PreSASSkill())
	//{
	//	ChangeSkill(); // 스킬이 바뀌면 쉐이더 값도 변경해준다.

	//	if (true == m_pCanvas->Get_ChangeX())
	//	{
	//		switch (m_pCanvas->Get_SASSkill())
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
	//		default:
	//			assert(!"Wrong Skillgauge Number");
	//			break;
	//		}
	//	}
	//	else
	//	{
	//		switch (m_pCanvas->Get_SASSkill())
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
	//		default:
	//			assert(!"Wrong Skillgauge Number");
	//			break;
	//		}
	//	}

	//	m_pCanvas->Set_PreSASSkill(m_pCanvas->Get_SASSkill());
	//}
}

CSASSkillGaugeUI * CSASSkillGaugeUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASSkillGaugeUI*		pInstance = new CSASSkillGaugeUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASSkillGaugeUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSASSkillGaugeUI::Clone(void * pArg)
{
	CSASSkillGaugeUI*		pInstance = new CSASSkillGaugeUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASSkillGaugeUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASSkillGaugeUI::Free()
{
	CUI::Free();

}
