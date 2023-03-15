#include "stdafx.h"
#include "..\public\SkmP_Controller.h"
#include <random>

#include "SkummyPool.h"
#include "FSMComponent.h"


CSkmP_Controller::CSkmP_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CSkmP_Controller::CSkmP_Controller(const CSkmP_Controller & rhs)
	: CAIController(rhs)
{
}

HRESULT CSkmP_Controller::Initialize(void * pArg)
{
	FAILED_CHECK(CAIController::Initialize(pArg));

	m_pFSM = CFSMComponentBuilder()
		.InitState("Operate")
		.AddState("Operate")
			.AddTransition("Operate to Outside", "Outside")
				.Predicator([this]
				{
					return m_pCastedOwner->IsPlayingSocket() == false;
				})

			/*.AddTransition("Operate to Near", "Near")
				.Predicator([this] 
				{
					return m_pCastedOwner->IsPlayingSocket() == false;
				})*/

		.AddState("Outside")
			.Tick(this, &CSkmP_Controller::Tick_Outside)
			.OnExit(dynamic_cast<CMonster*>(m_pCastedOwner), &CMonster::TurnEyesOut)
			.AddTransition("Outside to Far", "Far")
				.Predicator([this]
				{
					return m_fToTargetDistance <= 20.f;
				})

		.AddState("Near")
			.Tick(this, &CSkmP_Controller::Tick_Near)

			.AddTransition("Near to Far", "Far")
				.Predicator([this] 
				{
					return m_fToTargetDistance >= 8.f;
				})

		.AddState("Far")
			.Tick(this, &CSkmP_Controller::Tick_Far)

			.AddTransition("Far to Near", "Near")
				.Predicator([this] 
				{
					return m_fToTargetDistance < 8.f;
				})

		.Build();

	m_fTurnSlowTime = 1.5f;
	m_fTurnSlowRatio = 0.6f;

	return S_OK;
}

void CSkmP_Controller::BeginTick()
{
	CAIController::BeginTick();
	m_pCastedOwner = dynamic_cast<CSkummyPool*>(TryGetOwner());
	Assert(m_pCastedOwner != nullptr);
}

void CSkmP_Controller::AI_Tick(_double TimeDelta)
{
	if (m_pTarget == nullptr)
		return;

	m_fToTargetDistance = XMVectorGetX(XMVector3LengthEst(
		m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)
		- m_pCastedOwner->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)));

	if (IsCommandRunning() == false && m_pCastedOwner->IsPlayingSocket() == false)
	{
		m_pFSM->Tick(TimeDelta);
	}
}

void CSkmP_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;

//	EMoveAxis eMove = rand() % 2 ? EMoveAxis::WEST : EMoveAxis::EAST;

	switch(m_iNearOrder)
	{
	case 0:
		AddCommand("ForwardMove", 1.f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	/*case 1:
		AddCommand("Threat", 0.f, &CAIController::Input, R);
		AddCommand("Wait", 2.f, &CAIController::Wait);
		break;*/
	case 1:
		AddCommand("Turn", 3.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack", 0.f, &CAIController::Input, MOUSE_LB); // Shoot
//		AddCommand("TurnSlow", m_fTurnSlowTime, &CAIController::TurnToTarget, m_fTurnSlowRatio);
		break;
	case 2:
		AddCommand("LeftMove", 2.f, &CAIController::Move_TurnToTargetStop, EMoveAxis::WEST, 1.f);
		break;
	/*case 4:
		AddCommand("Threat", 0.f, &CAIController::Input, R);
		break;*/
	case 3:
		AddCommand("Wait", 2.f, &CAIController::Wait);
		break;
	case 4:
		AddCommand("BackMove", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::SOUTH, 1.f);
		break;
	case 5:
		AddCommand("Turn", 3.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack", 0.f, &CAIController::Input, MOUSE_LB); // Shoot
//		AddCommand("TurnSlow", m_fTurnSlowTime, &CAIController::TurnToTarget, m_fTurnSlowRatio);
		break;
	case 6:
		AddCommand("RightMove", 2.f, &CAIController::Move_TurnToTargetStop, EMoveAxis::EAST, 1.f);
		break;
	case 7:
		AddCommand("Wait", 2.f, &CAIController::Wait);
		break;
	}
	_uint iRand = (rand() % 3) + 1;
	m_iNearOrder = (m_iNearOrder + iRand) % 8;
}

void CSkmP_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;

//	EMoveAxis eMove = rand() % 2 ? EMoveAxis::WEST : EMoveAxis::EAST;

	switch (m_iFarOrder)
	{
	case 0:
		AddCommand("ForwardMove", 1.f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	/*case 1:
		AddCommand("Threat", 0.f, &CAIController::Input, R);
		AddCommand("Wait", 2.f, &CAIController::Wait);
		break;*/
	case 1:
		AddCommand("Turn", 3.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack", 0.f, &CAIController::Input, MOUSE_LB); // Shoot
//		AddCommand("TurnSlow", m_fTurnSlowTime, &CAIController::TurnToTarget, m_fTurnSlowRatio);
		break;
	case 2:
		AddCommand("LeftMove", 2.f, &CAIController::Move_TurnToTargetStop, EMoveAxis::WEST, 1.f);
		break;
	/*case 4:
		AddCommand("Threat", 0.f, &CAIController::Input, R);
		break;*/
	case 3:
		AddCommand("Wait", 2.f, &CAIController::Wait);
		break;
	case 4:
		AddCommand("BackMove", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::SOUTH, 1.f);
		break;
	case 5:
		AddCommand("Turn", 3.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack", 0.f, &CAIController::Input, MOUSE_LB); // Shoot
//		AddCommand("TurnSlow", m_fTurnSlowTime, &CAIController::TurnToTarget, m_fTurnSlowRatio);
		break;
	case 6:
		AddCommand("RightMove", 2.f, &CAIController::Move_TurnToTargetStop, EMoveAxis::EAST, 1.f);
		break;
	case 7:
		AddCommand("Wait", 2.f, &CAIController::Wait);
		break;
	}
	_uint iRand = (rand() % 3) + 1;
	m_iFarOrder = (m_iFarOrder + iRand) % 8;
}

void CSkmP_Controller::Tick_Outside(_double TimeDelta)
{
	m_eDistance = DIS_OUTSIDE;

	switch (m_iOutOrder)
	{
	case 0:
		AddCommand("Wait", 2.f, &CAIController::Wait);
		break;

	case 1:
		AddCommand("Wait", 2.f, &CAIController::Wait);
		break;
	}

	m_iOutOrder = (m_iOutOrder + 1) % 2;
}

void CSkmP_Controller::Free()
{
	CAIController::Free();
	Safe_Release(m_pFSM);
}

CComponent * CSkmP_Controller::Clone(void * pArg)
{
	CSkmP_Controller*		pInstance = new CSkmP_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSkmP_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSkmP_Controller * CSkmP_Controller::Create()
{
	CSkmP_Controller*		pInstance = new CSkmP_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSkmP_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}
