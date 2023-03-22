#include "stdafx.h"
#include "..\public\EM0110_Controller.h"
#include <FSMComponent.h>
#include "EM0110.h"

CEM0110_Controller::CEM0110_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CEM0110_Controller::CEM0110_Controller(const CEM0110_Controller & rhs)
	: CAIController(rhs)
{
}

HRESULT CEM0110_Controller::Initialize(void * pArg)
{
	m_iNearOrder = CMathUtils::RandomUInt(1);
	m_iMidOrder = CMathUtils::RandomUInt(3);
	m_iFarOrder = CMathUtils::RandomUInt(2);


	// Walk
	// Walk_B
	// Run
	// Threat
	// Attack_turn (돌려차기, 뒷발차기중 플레이어 위치에 따라 애니메이션 결정)
	// Attack_c1 (장판)
	// Attack_b2 (돌진)


	
	return S_OK;
}

void CEM0110_Controller::BeginTick()
{
	CAIController::BeginTick();

	CGameObject* pOwner = TryGetOwner();
	Assert(pOwner != nullptr);

	m_pCastedOwner = dynamic_cast<CEM0110*>(pOwner);
}

void CEM0110_Controller::AI_Tick(_double TimeDelta)
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

void CEM0110_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;

	/*
	기본 걷기(그냥 look방향으로 걸음)
	AddCommand("Walk", 2.f, &CAIController::Move, EMoveAxis::NORTH);

	//플레이어 쳐다보면서 뒷걸음
	AddCommand("Walk_BTurn", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::SOUTH, 1.f);

	//플레이어 쪽으로 달려감
	AddCommand("RunTurn", 2.f, &CEM0110_Controller::Run_TurnToTarget, EMoveAxis::NORTH, 1.f);
	*/

	switch (m_iNearOrder)
	{
	case 0:
		AddCommand("Attack_turn", 0.f, &CAIController::Input, R);
		break;
	case 1:
		AddCommand("Walk_BTurn", 2.f, &CAIController::Move, EMoveAxis::SOUTH);
		break;
	//case 2:
	//	AddCommand("RunTurn", 2.f, &CEM0110_Controller::Run_TurnToTarget, EMoveAxis::NORTH, 1.f);
	//	break;
	//case 3:
	//	AddCommand("Attack_c1", 0.f, &CAIController::Input, G);
	//	break;
	//case 4:
	//	AddCommand("Walk_BTurn", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::SOUTH, 1.f);
	//	break;
	//case 5:
	//	AddCommand("RunTurn", 2.f, &CEM0110_Controller::Run_TurnToTarget, EMoveAxis::NORTH, 1.f);
	//	break;
	}
	
	m_iNearOrder = (m_iNearOrder + 1) % 2;
}

void CEM0110_Controller::Tick_Mid(_double TimeDelta)
{
	m_eDistance = DIS_MIDDLE;

	switch (m_iMidOrder)
	{
	case 0:
		AddCommand("Turn", 2.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_b2", 0.f, &CAIController::Input, C);
		break;
	case 1:
		AddCommand("Walk", 2.f, &CAIController::Move, EMoveAxis::NORTH);
		break;
	case 2:
		AddCommand("Walk_BTurn", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::SOUTH, 1.f);
		break;
	case 3:
		AddCommand("Run", 2.f, &CEM0110_Controller::Run_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	}
	m_iMidOrder = (m_iMidOrder + 1) % 4;
}


void CEM0110_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;

	switch (m_iFarOrder)
	{
	case 0:
		AddCommand("Walk", 2.f, &CAIController::Move, EMoveAxis::NORTH);
		break;
	case 1:
		AddCommand("Threat", 0.f, &CAIController::Input, CTRL);
		break;
	}

	m_iFarOrder = (m_iFarOrder + 1) % 2;

}

void CEM0110_Controller::Tick_Outside(_double TimeDelta)
{
	m_eDistance = DIS_OUTSIDE;
	AddCommand("Attack_c1", 0.f, &CAIController::Input, G);
}


void CEM0110_Controller::Run_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio)
{
	m_bRun = true;
	Move(eAxis);
	TurnToTarget(fSpeedRatio);
}

void CEM0110_Controller::DefineState(_double TimeDelta)
{
	if (m_pCastedOwner->IsPlayingSocket() == true) return;

	if (m_fToTargetDistance <= 8.f)
		Tick_Near(TimeDelta);
	else if (m_fToTargetDistance <= 15.f)
		Tick_Mid(TimeDelta);
	else if (m_fToTargetDistance <= 25.f)
		Tick_Far(TimeDelta);
	else
		Tick_Outside(TimeDelta);
}



CEM0110_Controller * CEM0110_Controller::Create()
{
	CEM0110_Controller*		pInstance = new CEM0110_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0110_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CEM0110_Controller::Clone(void * pArg)
{
	CEM0110_Controller*		pInstance = new CEM0110_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0110_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEM0110_Controller::Free()
{
	CAIController::Free();
}
