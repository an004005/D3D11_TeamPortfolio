#include "stdafx.h"
#include "..\public\EM0800_Controller.h"
#include <FSMComponent.h>
#include "EM0800.h"

CEM0800_Controller::CEM0800_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CEM0800_Controller::CEM0800_Controller(const CEM0800_Controller & rhs)
	: CAIController(rhs)
{
}

HRESULT CEM0800_Controller::Initialize(void * pArg)
{
	m_iNearOrder = CMathUtils::RandomUInt(2);
	m_iMidOrder = CMathUtils::RandomUInt(2);

	//near : 깨물기, (+추가로 물에 들어갔다 나오는거, 체력50퍼 아래에서 실행)
	//mid : 레이저


	return S_OK;
}

void CEM0800_Controller::BeginTick()
{
	CAIController::BeginTick();

	CGameObject* pOwner = TryGetOwner();
	Assert(pOwner != nullptr);

	m_pCastedOwner = dynamic_cast<CEM0800*>(pOwner);
}

void CEM0800_Controller::AI_Tick(_double TimeDelta)
{
	if (m_pTarget == nullptr)
		return;

	m_bTurn = false;

	if (IsCommandRunning() == false && m_pCastedOwner->IsPlayingSocket() == false)
	{
		DefineState(TimeDelta);
	}
}

void CEM0800_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;
	//AS_em0800_212_AL_atk_a6_circumference2
	switch (m_iNearOrder)
	{
	case 0:
		AddCommand("Turn", 3.f, &CEM0800_Controller::DefineTurnCommand);
		AddCommand("Bite", 0.f, &CAIController::Input, B);
		AddCommand("Wait", 1.f, &CAIController::Wait);
		break;
	case 1:
	case 2:
		AddCommand("Move", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::SOUTH, 1.f);
		AddCommand("Wait", 1.f, &CAIController::Wait);
		break;
	case 3:
		AddCommand("Submergence", 0.f, &CAIController::Input, S);
		AddCommand("Wait", 1.5f, &CAIController::Wait);
		break;
	}

	const _int iPattern = m_pCastedOwner->GetHpRatio() >= 0.5f ? 3 : 4;
	m_iNearOrder = (m_iNearOrder + 1) % iPattern;
}

void CEM0800_Controller::Tick_Mid(_double TimeDelta)
{ 
	m_eDistance = DIS_MIDDLE;
	EMoveAxis eMove = rand() % 2 ? EMoveAxis::WEST : EMoveAxis::EAST;

	switch (m_iMidOrder)
	{
	case 0:
	case 1:
		AddCommand("Move", 2.5f, &CAIController::Move_TurnToTarget, eMove, 1.f);
		AddCommand("Wait", 1.f, &CAIController::Wait);	
		break;
	case 2:
		AddCommand("Turn", 3.f, &CEM0800_Controller::DefineTurnCommand);
		AddCommand("Laser", 0.f, &CAIController::Input, L);
		AddCommand("Wait", 1.f, &CAIController::Wait);
		break;
	}

	m_iMidOrder = (m_iMidOrder + 1) % 3;
}

void CEM0800_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;
	AddCommand("Wait", 2.f, &CAIController::Wait);
}

void CEM0800_Controller::DefineState(_double TimeDelta)
{
	if (m_pCastedOwner->IsPlayingSocket() == true) return;

	if (m_fTtoM_Distance <= 7.f)
		Tick_Near(TimeDelta);
	else if (m_fTtoM_Distance <= 25.f)
		Tick_Mid(TimeDelta);
	else
		Tick_Far(TimeDelta);
}

void CEM0800_Controller::DefineTurnCommand()
{
	m_bTurn = true;
	m_eTurn =  m_pCastedOwner->FindTargetDirection();
	const _float fSpeedRatio = m_eTurn == EBaseTurn::TURN_END ? 1.f : 10.f;
	TurnToTargetStop(fSpeedRatio);
}



CEM0800_Controller * CEM0800_Controller::Create()
{
	CEM0800_Controller*		pInstance = new CEM0800_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0800_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CEM0800_Controller::Clone(void * pArg)
{
	CEM0800_Controller*		pInstance = new CEM0800_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0800_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEM0800_Controller::Free()
{
	CAIController::Free();
}
