#include "stdafx.h"
#include "..\public\Canvas_SASSkillMove.h"
#include "GameInstance.h"
#include "GameUtils.h"

#include "FSMComponent.h"
#include "Canvas_SASSkill.h"

CCanvas_SASSkillMove::CCanvas_SASSkillMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SASSkillMove::CCanvas_SASSkillMove(const CCanvas_SASSkillMove& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SASSkillMove::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_SASSkillMove::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	// 처음에 보이지 않을 UI 들

	Find_ChildUI(L"SASSkill_IconNumber1")->SetVisible(false);
	Find_ChildUI(L"SASSkill_IconNumber2")->SetVisible(false);
	Find_ChildUI(L"SASSkill_IconNumber3")->SetVisible(false);
	Find_ChildUI(L"SASSkill_IconNumber4")->SetVisible(false);

	Find_ChildUI(L"SASSKill_ColleagueName1")->SetVisible(false);
	Find_ChildUI(L"SASSKill_ColleagueName2")->SetVisible(false);
	Find_ChildUI(L"SASSKill_ColleagueName3")->SetVisible(false);
	Find_ChildUI(L"SASSKill_ColleagueName4")->SetVisible(false);

	Find_ChildUI(L"SASSKill_SuperPower1")->SetVisible(false);
	Find_ChildUI(L"SASSKill_SuperPower2")->SetVisible(false);
	Find_ChildUI(L"SASSKill_SuperPower3")->SetVisible(false);
	Find_ChildUI(L"SASSKill_SuperPower4")->SetVisible(false);

	UIMove_FSM();

	return S_OK;
}

void CCanvas_SASSkillMove::BeginTick()
{
	m_pCanvas = dynamic_cast<CCanvas_SASSkill*>(CGameInstance::GetInstance()->Find_Prototype(LEVEL_NOW, TEXT("Canvas_SASSkill")));

	Temp = m_pCanvas->Get_Map();

	if (m_pCanvas == nullptr)
		assert(!"Canvas not be found.");
}

void CCanvas_SASSkillMove::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	if (CGameInstance::GetInstance()->KeyDown(DIK_0))
	{
		m_bUIMove = true;
	}

	m_pFSM->Tick(TimeDelta);

	InputCtrl_Tick();
	InputAlt_Tick();
}

void CCanvas_SASSkillMove::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_SASSkillMove::Render()
{
	if (FAILED(CCanvas::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_SASSkillMove::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();
}

void CCanvas_SASSkillMove::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);

}

void CCanvas_SASSkillMove::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);

}

void CCanvas_SASSkillMove::Info_Tick(const _bool bPush)
{
	if (true == bPush)
	{
		Find_ChildUI(L"SASSkill_Icon1")->SetVisible(false);
		Find_ChildUI(L"SASSkill_Icon2")->SetVisible(false);
		Find_ChildUI(L"SASSkill_Icon3")->SetVisible(false);
		Find_ChildUI(L"SASSkill_Icon4")->SetVisible(false);

		Find_ChildUI(L"SASSkill_IconNumber1")->SetVisible(true);
		Find_ChildUI(L"SASSkill_IconNumber2")->SetVisible(true);
		Find_ChildUI(L"SASSkill_IconNumber3")->SetVisible(true);
		Find_ChildUI(L"SASSkill_IconNumber4")->SetVisible(true);

	}
	else
	{
		Find_ChildUI(L"SASSkill_Icon1")->SetVisible(true);
		Find_ChildUI(L"SASSkill_Icon2")->SetVisible(true);
		Find_ChildUI(L"SASSkill_Icon3")->SetVisible(true);
		Find_ChildUI(L"SASSkill_Icon4")->SetVisible(true);

		Find_ChildUI(L"SASSkill_IconNumber1")->SetVisible(false);
		Find_ChildUI(L"SASSkill_IconNumber2")->SetVisible(false);
		Find_ChildUI(L"SASSkill_IconNumber3")->SetVisible(false);
		Find_ChildUI(L"SASSkill_IconNumber4")->SetVisible(false);

	}
}

void CCanvas_SASSkillMove::InputCtrl_Tick()
{
	// Ctrel 눌렀을 때 보여야 하는 UI
	if (CGameInstance::GetInstance()->KeyDown(DIK_LCONTROL))	
	{
		Info_Tick(true);

		//m_pCanvas->InputCtrl(true);

		Find_ChildUI(L"SASSKill_ColleagueName1")->SetVisible(true);
		Find_ChildUI(L"SASSKill_ColleagueName2")->SetVisible(true);
		Find_ChildUI(L"SASSKill_ColleagueName3")->SetVisible(true);
		Find_ChildUI(L"SASSKill_ColleagueName4")->SetVisible(true);
	}
	// Ctrel 누르지 않을 때 보여야 하는 UI
	if (CGameInstance::GetInstance()->KeyUp(DIK_LCONTROL))
	{
		Info_Tick(false);
		
		//m_pCanvas->InputCtrl(false);

		Find_ChildUI(L"SASSKill_ColleagueName1")->SetVisible(false);
		Find_ChildUI(L"SASSKill_ColleagueName2")->SetVisible(false);
		Find_ChildUI(L"SASSKill_ColleagueName3")->SetVisible(false);
		Find_ChildUI(L"SASSKill_ColleagueName4")->SetVisible(false);
	}
}

void CCanvas_SASSkillMove::InputAlt_Tick()
{
	// Alt 눌렀을 때 보여야 하는 UI
	if (CGameInstance::GetInstance()->KeyDown(DIK_LALT))
	{
		Info_Tick(true);

		//InputAlt(true);

		Find_ChildUI(L"SASSKill_SuperPower1")->SetVisible(true);
		Find_ChildUI(L"SASSKill_SuperPower2")->SetVisible(true);
		Find_ChildUI(L"SASSKill_SuperPower3")->SetVisible(true);
		Find_ChildUI(L"SASSKill_SuperPower4")->SetVisible(true);
	}
	// Alt 누르지 않을 때 보여야 하는 UI
	if (CGameInstance::GetInstance()->KeyUp(DIK_LALT))
	{
		Info_Tick(false);

		//__super::InputAlt(false);

		Find_ChildUI(L"SASSKill_SuperPower1")->SetVisible(false);
		Find_ChildUI(L"SASSKill_SuperPower2")->SetVisible(false);
		Find_ChildUI(L"SASSKill_SuperPower3")->SetVisible(false);
		Find_ChildUI(L"SASSKill_SuperPower4")->SetVisible(false);
	}
}

void CCanvas_SASSkillMove::UIMove_FSM()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
		.AddTransition("Idle to Move", "Move")
		.Predicator([this] {
		return m_bUIMove;
	})
		.AddState("Move")
		.OnStart([this]
	{
		_float2 vRandomPosition = { CGameUtils::GetRandFloat(3.0f, 10.0f), CGameUtils::GetRandFloat(0.0f, 10.0f) };
		m_vDestination = { vRandomPosition.x, vRandomPosition.y };
	})
		.Tick([this](_double TimeDelta) {

		_vector vPosition = XMVectorSet(m_fX, m_fY, 0.0f, 1.0f);
		_vector vDestination = { m_vDestination.x, m_vDestination.y, 0.0f, 1.0f };
		_vector vDistance = vDestination - vPosition;

		vPosition += XMVector2Normalize(vDistance) * _float(TimeDelta) * 15.0f;
		m_fX = XMVectorGetX(vPosition);
		m_fY = XMVectorGetY(vPosition);

		// 목표 지점과 현재 지점을 비교한다.
		_float fDistance = XMVectorGetX(XMVector4Length(vDestination - vPosition));

		IM_LOG("X %f", fDistance);
		if (0.2f > fDistance)
		{
			IM_LOG("X ---------------------");
			m_bIsDestination = true;
		}
	})
		.AddTransition("Move to Return", "Return")
		.Predicator([this] {
		return m_bIsDestination;
	})
		.AddState("Return")
		.Tick([this](_double TimeDelta) {

		_vector vPosition = XMVectorSet(m_fX, m_fY, 0.0f, 1.0f);
		_vector vDestination = { 0.0f, 0.0f, 0.0f, 1.0f };
		_vector vDistance = vDestination - vPosition;

		vPosition += XMVector2Normalize(vDistance) * _float(TimeDelta) * 15.0f;
		m_fX = XMVectorGetX(vPosition);
		m_fY = XMVectorGetY(vPosition);

		// 원래 지점과 현재 지점을 비교한다.
		_float fDistance = XMVectorGetX(XMVector2Length(vDestination - vPosition));

		IM_LOG("Y %f", fDistance);
		if (0.2f > fDistance)
		{
			IM_LOG("Y --------------------");
			m_bIsDestination = false;
			m_bUIMove = false;
			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
		}
	})
		.AddTransition("Return to Idle", "Idle")
		.Predicator([this] {
		return m_bUIMove == false;
	})
		.Build();

	//m_pFSM = CFSMComponentBuilder()
	//	.InitState("Idle")
	//	.AddState("Idle")
	//	.AddTransition("Idle to Move", "Move")
	//	.Predicator([this] {
	//	return m_bUIMove;
	//})
	//	.AddState("Move")
	//	.OnStart([this]
	//{
	//	_float2 vRandomPosition = { CGameUtils::GetRandFloat(3.0f, 10.0f), CGameUtils::GetRandFloat(0.0f, 10.0f) };
	//	m_vDestination = { m_vStartingPoint.x + vRandomPosition.x, m_vStartingPoint.y - vRandomPosition.y };
	//})
	//	.Tick([this](_double TimeDelta) {

	//	_vector vPosition = XMVectorSet(m_fX, m_fY, 0.0f, 1.0f);
	//	_vector vDestination = { m_vDestination.x, m_vDestination.y, 0.0f, 1.0f };
	//	_vector vDistance = vDestination - vPosition;
	//		
	//	vPosition += XMVector2Normalize(vDistance) * _float(TimeDelta) * 15.0f;
	//	m_fX = XMVectorGetX(vPosition);
	//	m_fY = XMVectorGetY(vPosition);

	//	// 목표 지점과 현재 지점을 비교한다.
	//	_float fDistance = XMVectorGetX(XMVector4Length(vDestination - vPosition));

	//	IM_LOG("X %f", fDistance);
	//	if (0.2f > fDistance)
	//	{
	//		IM_LOG("X ---------------------");
	//		m_bIsOriginGoal = true;
	//	}
	//})
	//	.AddTransition("Move to Return", "Return")
	//	.Predicator([this] {
	//	return m_bIsOriginGoal;
	//})
	//	.AddState("Return")
	//	.Tick([this](_double TimeDelta) {

	//	_vector vPosition = XMVectorSet(m_fX, m_fY, 0.0f, 1.0f);
	//	_vector vDestination = { m_vStartingPoint.x, m_vStartingPoint.y, 0.0f, 1.0f };
	//	_vector vDistance = vDestination - vPosition;

	//	vPosition += XMVector2Normalize(vDistance) * _float(TimeDelta) * 15.0f;
	//	m_fX = XMVectorGetX(vPosition);
	//	m_fY = XMVectorGetY(vPosition);

	//	// 원래 지점과 현재 지점을 비교한다.
	//	_float fDistance = XMVectorGetX(XMVector2Length(vDestination - vPosition));

	//	IM_LOG("Y %f", fDistance);
	//	if (0.2f > fDistance)
	//	{
	//		IM_LOG("Y --------------------");
	//		m_bIsOriginGoal = false;
	//		m_bUIMove = false;
	//		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
	//	}
	//})
	//	.AddTransition("Return to Idle", "Idle")
	//	.Predicator([this] {
	//	return m_bUIMove == false;
	//})
	//	.Build();
}

CCanvas_SASSkillMove * CCanvas_SASSkillMove::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_SASSkillMove*		pInstance = new CCanvas_SASSkillMove(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SASSkillMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_SASSkillMove::Clone(void * pArg)
{
	CCanvas_SASSkillMove*		pInstance = new CCanvas_SASSkillMove(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SASSkillMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SASSkillMove::Free()
{
	CCanvas::Free();
	Safe_Release(m_pFSM);
}
