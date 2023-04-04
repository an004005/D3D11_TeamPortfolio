#include "stdafx.h"
#include "..\public\EM1200_Controller.h"
#include <FSMComponent.h>
#include "EM1200.h"

CEM1200_Controller::CEM1200_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CEM1200_Controller::CEM1200_Controller(const CEM1200_Controller & rhs)
	: CAIController(rhs)
{
}

HRESULT CEM1200_Controller::Initialize(void * pArg)
{

	m_fNearestTargetDist = 5.f;

	//1Phase

	//near :  Fall, shout
	//mid : walk

	//2Phase

	//near : Stamp(cool), shout(cool), swing (뒤쪽에 있어야 함), rush
	//mid : rush (mid 거리가 은근 가까움)
	//far : cable
	//out : run

	return S_OK;
}

void CEM1200_Controller::BeginTick()
{
	CAIController::BeginTick();

	CGameObject* pOwner = TryGetOwner();
	Assert(pOwner != nullptr);

	m_pCastedOwner = dynamic_cast<CEM1200*>(pOwner);
}

void CEM1200_Controller::AI_Tick(_double TimeDelta)
{
	if (m_pTarget == nullptr)
		return;

	//페이즈가 변하면 체력 회복
	if (m_bChangePhase == false && m_pCastedOwner->GetHpRatio() <= 0.7f)
	{
		m_bChangePhase = true;
		m_pCastedOwner->HealFullHp();
	}

	m_bRun = false;
	m_eTurn = EBaseTurn::TURN_END;

	m_dStampCoolTime[CURTIME] += TimeDelta;
	m_dShoutCoolTime[CURTIME] += TimeDelta;

	_bool tt = IsCommandRunning() == false;
	_bool ttty = m_pCastedOwner->IsPlayingSocket() == false;

	if (IsCommandRunning() == false && m_pCastedOwner->IsPlayingSocket() == false)
	{	
		AddCommand("Cable", 0.f, &CAIController::Input, C);
		//DefineState(TimeDelta);
	}
}

void CEM1200_Controller::Tick_Near_1Phase(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;
	
	switch (m_iNear1PhaseOrder)
	{
	case 0:
	case 1:
		AddCommand("Shout1", 0.f, &CAIController::Input, NUM_1);
		AddCommand("Wait", 1.f, &CAIController::Wait);
		break;
	case 2:
		AddCommand("Fall", 0.f, &CAIController::Input, F);
		AddCommand("Wait", 1.f, &CAIController::Wait);
		break;
	}

	m_iNear1PhaseOrder = (m_iNear1PhaseOrder + 1) % 3;

}

void CEM1200_Controller::Tick_Near_2Phase(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;

	ESimpleAxis eSimpeAxis = m_pCastedOwner->TargetSimpleAxis();

	if (eSimpeAxis == ESimpleAxis::NORTH)
	{

		if (m_dShoutCoolTime[CURTIME] >= m_dShoutCoolTime[MAXTIME])
		{
			AddCommand("Turn", 3.f, &CEM1200_Controller::Turn, 1.f);
			AddCommand("Shout2", 0.f, &CAIController::Input, NUM_2);
			AddCommand("Wait", 1.f, &CAIController::Wait);
			m_dShoutCoolTime[CURTIME] = 0.0;
		}
		else if (m_dStampCoolTime[CURTIME] >= m_dStampCoolTime[MAXTIME])
		{
			AddCommand("Turn", 3.f, &CEM1200_Controller::Turn, 1.f);
			AddCommand("Stamp", 0.f, &CAIController::Input, S);
			AddCommand("Wait", 1.f, &CAIController::Wait);
			m_dStampCoolTime[CURTIME] = 0.0;
		}
		else
		{
			AddCommand("Turn", 3.f, &CEM1200_Controller::Turn, 1.f);
			AddCommand("Rush", 0.f, &CAIController::Input, R);
			AddCommand("Wait", 1.f, &CAIController::Wait);
		}
	}
	else
	{
		EBaseTurn eTurn = m_pCastedOwner->TargetBaseTurn();

		if (eTurn == EBaseTurn::TURN_RIGHT)
			AddCommand("Swing_R", 0.f, &CAIController::Input, MOUSE_RB);
		else
			AddCommand("Swing_L", 0.f, &CAIController::Input, MOUSE_LB);
	}

	
}

void CEM1200_Controller::Tick_Mid(_double TimeDelta)
{
	m_eDistance = DIS_MIDDLE;

	if (m_bChangePhase == false)
	{
		AddCommand("Turn", 2.f, &CEM1200_Controller::Turn, 0.5f);
		AddCommand("Walk", 2.f, &CAIController::Move, EMoveAxis::NORTH);
	}
	else
	{
		AddCommand("Turn", 3.f, &CEM1200_Controller::Turn, 2.f);
		AddCommand("Rush", 0.f, &CAIController::Input, R);
		AddCommand("Wait", 1.f, &CAIController::Wait);
	}
}

void CEM1200_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;
	AddCommand("Cable", 0.f, &CAIController::Input, C);
}

void CEM1200_Controller::Tick_Outside(_double TimeDelta)
{
	m_eDistance = DIS_OUTSIDE;

	if (m_bChangePhase == false)
	{
		AddCommand("Wait", 2.f, &CAIController::Wait);
	}
	else
	{
		AddCommand("Turn", 3.f, &CEM1200_Controller::Turn, 2.f);
		AddCommand("Run", 3.f, &CEM1200_Controller::Run_TurnToTarget, EMoveAxis::NORTH, 2.f);
	}
}

void CEM1200_Controller::Run_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio)
{
	m_bRun = true;
	Move(eAxis);
	TurnToTarget(fSpeedRatio);
}
 
void CEM1200_Controller::Turn( _float fSpeedRatio)
{
	m_eTurn = m_pCastedOwner->TargetBaseTurn();
	TurnToTargetStop(fSpeedRatio);
}

void CEM1200_Controller::DefineState(_double TimeDelta)
{
	if (m_pCastedOwner->IsPlayingSocket() == true) return;

	if (m_bChangePhase == false)
	{
		if (m_fTtoM_Distance <= 7.f)
			Tick_Near_1Phase(TimeDelta);
		else if(m_fTtoM_Distance <= 15.f)
			Tick_Mid(TimeDelta);
		else
			Tick_Outside(TimeDelta);
	}

	else
	{
		if (m_fTtoM_Distance <= 7.f)
			Tick_Near_2Phase(TimeDelta);
		else if (m_fTtoM_Distance <= 12.f)
			Tick_Mid(TimeDelta);
		else if (m_fTtoM_Distance <= 16.f)
			Tick_Far(TimeDelta);
		else
			Tick_Outside(TimeDelta);
	}
}



CEM1200_Controller * CEM1200_Controller::Create()
{
	CEM1200_Controller*		pInstance = new CEM1200_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM1200_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CEM1200_Controller::Clone(void * pArg)
{
	CEM1200_Controller*		pInstance = new CEM1200_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM1200_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEM1200_Controller::Free()
{
	CAIController::Free();
}
