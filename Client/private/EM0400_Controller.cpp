#include "stdafx.h"
#include "..\public\EM0400_Controller.h"
#include <FSMComponent.h>
#include "EM0400.h"

CEM0400_Controller::CEM0400_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CEM0400_Controller::CEM0400_Controller(const CEM0400_Controller & rhs)
	: CAIController(rhs)
{
}

HRESULT CEM0400_Controller::Initialize(void * pArg)
{
	m_iNearOrder = CMathUtils::RandomUInt(6);
	m_iMidOrder = CMathUtils::RandomUInt(1);

	m_fTurnSlowTime = 0.7f;
	m_fTurnSlowRatio = 0.3f;

	return S_OK;
}

void CEM0400_Controller::BeginTick()
{
	CAIController::BeginTick();

	CGameObject* pOwner = TryGetOwner();
	Assert(pOwner != nullptr);

	m_pCastedOwner = dynamic_cast<CEM0400*>(pOwner);
}

void CEM0400_Controller::AI_Tick(_double TimeDelta)
{
	if (m_pTarget == nullptr)
		return;

	m_bRun = false;

	// 대상과의 거리 
	m_fToTargetDistance = XMVectorGetX(XMVector3LengthEst(
		m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)
		- m_pCastedOwner->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)));

	if (IsCommandRunning() == false && m_pCastedOwner->IsPlayingSocket() == false)
	{
		DefineState(TimeDelta);
	}
}

void CEM0400_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;

	switch (m_iNearOrder)
	{
	case 0:
		AddCommand("Dodge_L", 0.f, &CAIController::Input, NUM_1);
		AddCommand("Turn", 2.5f, &CAIController::Move_TurnToTarget, EMoveAxis::WEST, 1.f);
		break;
	case 1:
		AddCommand("Attack_Swing", 0.f, &CAIController::Input, MOUSE_LB);
		break;
	case 2:
		AddCommand("Walk", 3.f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	case 3:
		AddCommand("Attack_Swing", 0.f, &CAIController::Input, MOUSE_LB);
		break;
	case 4:
		AddCommand("Dodge_R", 0.f, &CAIController::Input, NUM_2);
		AddCommand("Turn", 2.5f, &CAIController::Move_TurnToTarget, EMoveAxis::EAST, 1.f);
		break;
	case 5:
		AddCommand("Walk", 3.f, &CAIController::Move_TurnToTarget, EMoveAxis::EAST, 1.f);
		break;
	case 6:
		AddCommand("Attack_Swing", 0.f, &CAIController::Input, MOUSE_LB);
		break;
	case 7:
		AddCommand("Dodge_B", 0.f, &CAIController::Input, NUM_3);
		AddCommand("Turn", 2.5f, &CAIController::Move_TurnToTarget, EMoveAxis::SOUTH, 1.f);
		break;
	case 8:
		AddCommand("Walk", 3.f, &CAIController::Move_TurnToTarget, EMoveAxis::WEST, 1.f);
		break;
	case 9:
		AddCommand("Attack_Swing", 0.f, &CAIController::Input, MOUSE_LB);
		break;
	}
	
	m_iNearOrder = (m_iNearOrder + 1) % 10;
}

void CEM0400_Controller::Tick_Mid(_double TimeDelta)
{
	m_eDistance = DIS_MIDDLE;

	switch (m_iMidOrder)
	{
	case 0:
		AddCommand("Walk", 3.5f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	case 1:
		AddCommand("Threat", 0.f, &CAIController::Input, MOUSE_RB);
		break;
	case 2:
		AddCommand("Run", 2.5f, &CEM0400_Controller::Run_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	}

	m_iMidOrder = (m_iMidOrder + 1) % 3;
}


void CEM0400_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;
	AddCommand("Run", 3.5f, &CEM0400_Controller::Run_TurnToTarget, EMoveAxis::NORTH, 1.f);

}

void CEM0400_Controller::Tick_Outside(_double TimeDelta)
{
	m_eDistance = DIS_OUTSIDE;
	AddCommand("Wait", 2.f, &CAIController::Wait);
}


void CEM0400_Controller::Run_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio)
{
	m_bRun = true;
	Move(eAxis);
	TurnToTarget(fSpeedRatio);
}

void CEM0400_Controller::DefineState(_double TimeDelta)
{
	if (m_pCastedOwner->IsPlayingSocket() == true) return;

	if (m_fToTargetDistance <= 8.f)
		Tick_Near(TimeDelta);
	else if (m_fToTargetDistance <= 14.f)
		Tick_Mid(TimeDelta);
	else if (m_fToTargetDistance <= 20.f)
		Tick_Far(TimeDelta);
	else
		Tick_Outside(TimeDelta);
}



CEM0400_Controller * CEM0400_Controller::Create()
{
	CEM0400_Controller*		pInstance = new CEM0400_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0400_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CEM0400_Controller::Clone(void * pArg)
{
	CEM0400_Controller*		pInstance = new CEM0400_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0400_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEM0400_Controller::Free()
{
	CAIController::Free();
}
