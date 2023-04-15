#include "stdafx.h"
#include "..\public\EM1200_Controller.h"
#include <FSMComponent.h>
#include "EM1200.h"
#include "EM1200_AnimInstance.h"

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

	m_dStampCoolTime[CURTIME] += TimeDelta;
	m_dShoutCoolTime[CURTIME] += TimeDelta;

	if (IsCommandRunning() == false && m_pCastedOwner->IsPlayingSocket() == false)
	{
		AddCommand("Rush", 0.f, &CAIController::Input, R);


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
		AddCommand("Wait", 2.0f, &CAIController::Wait);
		break;
	case 2:
		AddCommand("Fall", 0.f, &CAIController::Input, F);
		AddCommand("Wait", 2.0f, &CAIController::Wait);
		break;
	}

	m_iNear1PhaseOrder = (m_iNear1PhaseOrder + 1) % 3;

}

void CEM1200_Controller::Tick_Near_2Phase(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;

	ESimpleAxis eSimpeAxis = m_pCastedOwner->IsTargetFront(60.f) ? ESimpleAxis::NORTH : ESimpleAxis::SOUTH;

	if (eSimpeAxis == ESimpleAxis::NORTH)
	{
		if (m_dShoutCoolTime[CURTIME] >= m_dShoutCoolTime[MAXTIME])
		{
			AddCommand("Turn", 10.f, &CEM1200_Controller::Turn, 1.f);
			AddCommand("Wait", 0.5f, &CAIController::Wait);
			AddCommand("Shout2", 0.f, &CAIController::Input, NUM_2);
			AddCommand("Wait", 2.f, &CAIController::Wait);

			m_dShoutCoolTime[CURTIME] = 0.0;
		}
		else if (m_dStampCoolTime[CURTIME] >= m_dStampCoolTime[MAXTIME])
		{
			AddCommand("Turn", 10.f, &CEM1200_Controller::Turn, 1.f);
			AddCommand("Wait", 0.5f, &CAIController::Wait);
			AddCommand("Stamp", 0.f, &CAIController::Input, S);
			AddCommand("Wait", 2.f, &CAIController::Wait);

			m_dStampCoolTime[CURTIME] = 0.0;
		}
		else
		{
			AddCommand("Turn", 10.f, &CEM1200_Controller::Turn, 1.f);
			AddCommand("Wait", 0.5f, &CAIController::Wait);
			AddCommand("Rush", 0.f, &CAIController::Input, R);
			AddCommand("Wait", 3.f, &CAIController::Wait);

		}
	}
	else
	{
		EBaseTurn eTurn = m_pCastedOwner->IsTargetRight() ? EBaseTurn::TURN_RIGHT : EBaseTurn::TURN_LEFT;

		if (eTurn == EBaseTurn::TURN_RIGHT)
		{
			AddCommand("Swing_R", 0.f, &CAIController::Input, MOUSE_RB);
			AddCommand("Wait", 2.f, &CAIController::Wait);
		}
		else
		{
			AddCommand("Swing_L", 0.f, &CAIController::Input, MOUSE_LB);
			AddCommand("Wait", 2.f, &CAIController::Wait);
		}
	}


}

void CEM1200_Controller::Tick_Mid(_double TimeDelta)
{
	m_eDistance = DIS_MIDDLE;

	if (m_pCastedOwner->IsTargetFront())
	{
		AddCommand("Turn", 10.f, &CEM1200_Controller::Turn, 1.f);
		AddCommand("Wait", 0.5f, &CAIController::Wait);
		AddCommand("Rush", 0.f, &CAIController::Input, R);
		AddCommand("Wait", 3.f, &CAIController::Wait);

	}
	else
	{
		AddCommand("Cable", 0.f, &CAIController::Input, C);
		AddCommand("Wait", 3.f, &CAIController::Wait);
	}
}

void CEM1200_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;
	AddCommand("Cable", 0.f, &CAIController::Input, C);
	AddCommand("Wait", 3.f, &CAIController::Wait);

}

void CEM1200_Controller::Tick_Outside(_double TimeDelta)
{
	m_eDistance = DIS_OUTSIDE;
	AddCommand("Wait", 2.f, &CAIController::Wait);
}

void CEM1200_Controller::Run_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio)
{
	m_bRun = true;
	Move(eAxis);
	TurnToTarget(fSpeedRatio);
}
 
void CEM1200_Controller::Turn( _float fSpeedRatio)
{
	if (m_pTarget == nullptr || m_pCastedOwner == nullptr)
		return;

	if (m_bturn == false)
	{
		m_eTurn = m_pCastedOwner->IsTargetRight() ? EBaseTurn::TURN_RIGHT : EBaseTurn::TURN_LEFT;
		m_bturn = true;
	}

	const _vector vLookAt = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);

	CAnimation* pAnimation = m_pCastedOwner->Get_Model()->GetPlayAnimation();

	if (nullptr == pAnimation) return;

	_float fRatio = pAnimation->GetPlayRatio();

	if (pAnimation == m_pCastedOwner->Get_Model()->Find_Animation("AS_em1200_157_AL_turn_R_loop2") ||
		pAnimation == m_pCastedOwner->Get_Model()->Find_Animation("AS_em1200_155_AL_turn_L_loop2"))
	{
		if (m_pCastedOwner->GetTransform()->LookAt_Lerp_NonY(vLookAt, fRatio))
		{
			m_Commands.front().SetFinish();
			m_eTurn = EBaseTurn::TURN_END;
			m_bturn = false;
		}
	}
}

void CEM1200_Controller::DefineState(_double TimeDelta)
{
	if (m_pCastedOwner->IsPlayingSocket() == true) return;

	if (m_bChangePhase == false)
	{
		if (m_fTtoM_Distance <= 8.f)
			Tick_Near_1Phase(TimeDelta);
		else
			Tick_Outside(TimeDelta);
	}

	else
	{
		if (m_fTtoM_Distance <= 8.f)
			Tick_Near_2Phase(TimeDelta);
		else if (m_fTtoM_Distance <= 12.f)
			Tick_Mid(TimeDelta);
		else if (m_fTtoM_Distance <= 25.f)
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
