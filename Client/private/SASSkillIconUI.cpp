#include "stdafx.h"
#include "..\public\SASSkillIconUI.h"
#include "GameInstance.h"

// [0] 초능력 사용 가능(이미지) / [1] 초능력 사용 불 가능(검정색) -> 그냥 Float4s[0] 로 변경해도 되는데 이게 직관적인 것 같아서요
//m_tParams.Ints[0] = { 1 };

// x: [0] 염력 [1] 염력 [2] 발화 [3] 경질 [4] 투시 [5] 순간이동 [6] 투명 [7] 방전 [8] 복제 [9] 초고속 [10] 사용 불가
// y: [0] 초능력 사용 불 가능 (그냥) / [1] 초능력 사용 가능(불들어오는거)
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

	static _uint eObjectCount;
	m_eObjectCount = CCanvas_SASSkill::OBJECTCOUNT(eObjectCount);
	++eObjectCount;

	if (3 == eObjectCount)
		eObjectCount = 0;

	const _float2 PivotPair = GetPivotXY(m_ePivot);
	m_vOriginPosition = { m_fX + PivotPair.x, m_fY + PivotPair.y };

	return S_OK;
}

void CSASSkillIconUI::BeginTick()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pCanvas = dynamic_cast<CCanvas_SASSkill*>(pGameInstance->Find_Prototype(LEVEL_NOW, TEXT("Canvas_SASSkill")));
}

void CSASSkillIconUI::Tick(_double TimeDelta)
{
	//__super::Tick(TimeDelta);

	SASSkill_UIMove(TimeDelta);
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

	//static array<const char*, SUPERPOWERS_END> arrSuperPowersName{
	//	"PSYCHOKINESIS0", "PSYCHOKINESIS1", "IGNITION", "RESHUFFLE", "CLAIRVOYANCE", "TELEPORTATION", "TRANSPARENCY", "DISCHARGE", "COPY", "NON"
	//};

	//if (ImGui::BeginCombo("SAS Skill", arrSuperPowersName[static_cast<_uint>(m_pCanvas->Get_SuperPowers())]))
	//{
	//	for (int i = PSYCHOKINESIS0; i < SUPERPOWERS_END; ++i)
	//	{
	//		const bool bSelected = false;
	//		if (ImGui::Selectable(arrSuperPowersName[i], bSelected))
	//			m_pCanvas->Set_SuperPowers(SUPERPOWERS(i));
	//	}

	//	ImGui::EndCombo();
	//}
}

void CSASSkillIconUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

	//json["SuperPowers"] = m_pCanvas->Get_SuperPowers();
}

void CSASSkillIconUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

	//m_pCanvas->Set_SuperPowers(SUPERPOWERS(json["SuperPowers"]));
}

void CSASSkillIconUI::SASSkillIcon_Tick()
{
	if (m_pCanvas->Get_SASSkill() != m_pCanvas->Get_PreSASSkill())
	{
		ChangeSkill_Shader(); // 스킬이 바뀌면 쉐이더 값도 변경해준다.

		if (true == m_pCanvas->Get_ChangeX())
		{
			switch (m_pCanvas->Get_SASSkill())
			{
			case CCanvas_SASSkill::ONE0:
				break;
			case CCanvas_SASSkill::TWO0:
				break;
			case CCanvas_SASSkill::THREE0:
				break;
			case CCanvas_SASSkill::FOUR0:
				break;
			default:
				assert(!"Wrong Skill Icon Number");
				break;
			}
		}
		else
		{
			switch (m_pCanvas->Get_SASSkill())
			{
			case CCanvas_SASSkill::ONE1:
				break;
			case CCanvas_SASSkill::TWO1:
				break;
			case CCanvas_SASSkill::THREE1:
				break;
			case CCanvas_SASSkill::FOUR1:
				break;
			default:
				assert(!"Wrong Skill Icon Number");
				break;
			}
		}

		m_pCanvas->Set_PreSASSkill(m_pCanvas->Get_SASSkill());
	}
}

void CSASSkillIconUI::ChangeSkill_Shader()
{
	m_tParams.Ints[0] = { !m_pCanvas->Get_OnSkill() };
	m_tParams.Float2s[0] = { _float(m_pCanvas->Get_SuperPowers()), _float(m_pCanvas->Get_OnSkill()) };
}

void CSASSkillIconUI::SASSkill_UIMove(const _double & dTimeDelta)
{
	if (false == m_bOneCheck)
	{
		m_bOneCheck = true;
		m_bIsDestination = true;
		m_vDestination = { m_vOriginPosition.x + 3.0f, m_vOriginPosition.y - 3.0f };
	}

	static _float fSpeed = 3.0f;

	if (true == m_bIsDestination)
	{
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		vPosition += XMVector2Normalize(XMLoadFloat2(&m_vDestination) - vPosition) * _float(dTimeDelta) * fSpeed;

		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(vPosition, 1.0f));

		// 목표 지점과 현재 지점을 비교한다.
		_vector vDestination = { m_vDestination.x, m_vDestination.y, 0.0f, 1.0f };
		_float fDistance = XMVectorGetX(XMVector3Length(vDestination - vPosition));

		if (0.1f > fDistance)
		{
			m_bIsDestination = false;
			m_bIsOriginGoal = true;
		}
	}

	if (true == m_bIsOriginGoal)
	{
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

		vPosition += XMVector2Normalize(XMLoadFloat2(&m_vOriginPosition) - vPosition) * _float(dTimeDelta) * fSpeed;
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(vPosition, 1.0f));

		// 원래 지점과 현재 지점을 비교한다.
		_vector vDestination = { m_vOriginPosition.x, m_vOriginPosition.y, 0.0f, 1.0f };
		_float fDistance = XMVectorGetX(XMVector3Length(vDestination - vPosition));

		if (0.0f > fDistance)
		{
			m_bIsOriginGoal = false;

			// 모든 값을 초기화 한다.
			m_bOneCheck = false;
			m_bIsDestination = false;
			m_vOriginPosition = { 0.0f, 0.0f };
			m_vDestination = { 0.0f, 0.0f };
		}
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
