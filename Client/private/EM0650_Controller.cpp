#include "stdafx.h"
#include "..\public\EM0650_Controller.h"
#include <FSMComponent.h>
#include "EM0650.h"

CEM0650_Controller::CEM0650_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CEM0650_Controller::CEM0650_Controller(const CEM0650_Controller & rhs)
	: CAIController(rhs)
{
}

HRESULT CEM0650_Controller::Initialize(void * pArg)
{
	m_iNearOrder = CMathUtils::RandomUInt(5);
	m_iFarOrder = CMathUtils::RandomUInt(7);

	m_fTurnSlowTime = 1.5f;
	m_fTurnSlowRatio = 0.6f;

	return S_OK;
}

void CEM0650_Controller::BeginTick()
{
	CAIController::BeginTick();

	CGameObject* pOwner = TryGetOwner();
	Assert(pOwner != nullptr);

	m_pCastedOwner = dynamic_cast<CEM0650*>(pOwner);
}

void CEM0650_Controller::AI_Tick(_double TimeDelta)
{
	if (m_pTarget == nullptr)
		return;

	// 대상과의 거리 
	m_fToTargetDistance = XMVectorGetX(XMVector3LengthEst(
		m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)
		- m_pCastedOwner->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)));

	if (IsCommandRunning() == false && m_pCastedOwner->IsPlayingSocket() == false)
	{
		DefineState(TimeDelta);
	}
}

void CEM0650_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;

	switch (m_iNearOrder)
	{
	case 0:
		AddCommand("ForwardMove", 1.f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	
	case 1:
		AddCommand("Turn", 3.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack", 0.f, &CAIController::Input, MOUSE_LB); // Shoot
																	//		AddCommand("TurnSlow", m_fTurnSlowTime, &CAIController::TurnToTarget, m_fTurnSlowRatio);
		break;
	case 2:
		AddCommand("LeftMove", 2.f, &CAIController::Move_TurnToTargetStop, EMoveAxis::WEST, 1.f);
		break;

	case 3:
		AddCommand("BackMove", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::SOUTH, 1.f);
		break;

	case 4:
		AddCommand("Turn", 3.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack", 0.f, &CAIController::Input, MOUSE_LB); // Shoot												//		AddCommand("TurnSlow", m_fTurnSlowTime, &CAIController::TurnToTarget, m_fTurnSlowRatio);
		break;

	case 5:
		AddCommand("RightMove", 2.f, &CAIController::Move_TurnToTargetStop, EMoveAxis::EAST, 1.f);
		break;
	}
	
	m_iNearOrder = (m_iNearOrder + 1) % 6;
}


void CEM0650_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;

	switch (m_iFarOrder)
	{
	case 0:
		AddCommand("ForwardMove", 1.f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;

	case 1:
		AddCommand("Turn", 3.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack", 0.f, &CAIController::Input, MOUSE_LB); // Shoot															//		AddCommand("TurnSlow", m_fTurnSlowTime, &CAIController::TurnToTarget, m_fTurnSlowRatio);
		break;

	case 2:
		AddCommand("LeftMove", 2.f, &CAIController::Move_TurnToTargetStop, EMoveAxis::WEST, 1.f);
		break;
	
	case 3:
		AddCommand("Wait", 2.f, &CAIController::Wait);
		break;

	case 4:
		AddCommand("BackMove", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::SOUTH, 1.f);
		break;

	case 5:
		AddCommand("Turn", 3.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack", 0.f, &CAIController::Input, MOUSE_LB); // Shoot														//		AddCommand("TurnSlow", m_fTurnSlowTime, &CAIController::TurnToTarget, m_fTurnSlowRatio);
		break;

	case 6:
		AddCommand("RightMove", 2.f, &CAIController::Move_TurnToTargetStop, EMoveAxis::EAST, 1.f);
		break;

	case 7:
		AddCommand("Wait", 2.f, &CAIController::Wait);
		break;
	}

	m_iFarOrder = (m_iFarOrder + 1) % 8;
}

void CEM0650_Controller::Tick_Outside(_double TimeDelta)
{
	m_eDistance = DIS_OUTSIDE;
	AddCommand("Wait", 2.f, &CAIController::Wait);
}

void CEM0650_Controller::DefineState(_double TimeDelta)
{
	if (m_pCastedOwner->IsPlayingSocket() == true) return;

	if (m_fToTargetDistance <= 8.f)
		Tick_Near(TimeDelta);
	else if (m_fToTargetDistance <= 20.f)
		Tick_Far(TimeDelta);
	else
		Tick_Outside(TimeDelta);
}



CEM0650_Controller * CEM0650_Controller::Create()
{
	CEM0650_Controller*		pInstance = new CEM0650_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0650_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CEM0650_Controller::Clone(void * pArg)
{
	CEM0650_Controller*		pInstance = new CEM0650_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0650_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEM0650_Controller::Free()
{
	CAIController::Free();
}
