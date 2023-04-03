#include "stdafx.h"
#include "..\public\EM0320_Controller.h"
#include "EM0320.h"
#include "FSMComponent.h"


CEM0320_Controller::CEM0320_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAIController(pDevice, pContext)
{
}

CEM0320_Controller::CEM0320_Controller(const CEM0320_Controller& rhs)
	: CAIController(rhs)
{
}

HRESULT CEM0320_Controller::Initialize(void* pArg)
{
	FAILED_CHECK(CAIController::Initialize(pArg));

	m_pFSM = CFSMComponentBuilder()
		.InitState("Start")
		.AddState("Start")
			.AddTransition("Start to Outside", "Outside")
				.Predicator([this]
				{
					return m_pCastedOwner->IsPlayingSocket() == false;
				})

		//		.AddTransition("Start to Near", "Near")
		//			.Predicator([this]
		//			{
		//				return m_pCastedOwner->IsPlayingSocket() == false;
		//			})

		.AddState("Outside")
			.Tick(this, &CEM0320_Controller::Tick_Outside)

			.AddTransition("Outside to Far", "Far")
				.Predicator([this]
				{
					return m_fTtoM_Distance <= 25.f;
				})


		.AddState("Near")
			.Tick(this, &CEM0320_Controller::Tick_Near)
			.AddTransition("Near to Far", "Mid")
				.Predicator([this]
				{
					return m_fTtoM_Distance > 8.f;
				})

		.AddState("Mid")
			.Tick(this, &CEM0320_Controller::Tick_Mid)
			.AddTransition("Mid to Near", "Near")
				.Predicator([this]
				{
					return m_fTtoM_Distance <= 8.f;
				})
			.AddTransition("Mid to Far", "Far")
				.Predicator([this]
				{
					return m_fTtoM_Distance > 14.f;
				})

		.AddState("Far")
			.Tick(this, &CEM0320_Controller::Tick_Far)
			.AddTransition("Far to Near", "Near")
				.Predicator([this]
				{
					return m_fTtoM_Distance <= 14.f;
				})
		.Build();

	m_fTurnSlowTime = 0.7f;
	m_fTurnSlowRatio = 0.5f;

	return S_OK;
}

void CEM0320_Controller::BeginTick()
{
	CAIController::BeginTick();
	m_pCastedOwner = dynamic_cast<CEM0320*>(TryGetOwner());
	Assert(m_pCastedOwner != nullptr);
	AddCommand("Wait", 1.f, &CAIController::Wait);
}

void CEM0320_Controller::AI_Tick(_double TimeDelta)
{
	if (m_pTarget == nullptr)
		return;

	if (IsCommandRunning() == false && m_pCastedOwner->IsPlayingSocket() == false)
	{
		AddCommand("Jump", 0.f, &CAIController::Input, E);
		//m_pFSM->Tick(TimeDelta);
	}
}

void CEM0320_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;
	// 1. 스핀 공격, 플레이어 보고 때리기, 플레이어보고 뒤가기,
	switch (m_iNearOrder)
	{
	case 0:
		AddCommand("Move", 2.f, &CAIController::Move, EMoveAxis::SOUTH);
		break;
	case 1:
		AddCommand("Turn", 3.f, &CEM0320_Controller::TurnToTargetStop, 1.f);
		AddCommand("Attack", 0.f, &CAIController::Input, MOUSE_LB);//attack
		break;
	case 2:
		AddCommand("MoveTurn", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::SOUTH, 1.f);
		break;
	case 3:
		AddCommand("Turn", 3.f, &CEM0320_Controller::TurnToTargetStop, 1.f);
		AddCommand("Attack", 0.f, &CAIController::Input, MOUSE_LB);//attack
		break;
	case 4:
		AddCommand("Turn", 3.f, &CEM0320_Controller::TurnToTargetStop, 1.f);
		AddCommand("Attack", 0.f, &CAIController::Input, MOUSE_LB);//attack
		break;
	case 5:
		AddCommand("Attack_Spin", 0.f, &CAIController::Input, G); // spin attack
		break;
	}

	m_iNearOrder = (m_iNearOrder + 1) % 6;
}

void CEM0320_Controller::Tick_Mid(_double TimeDelta)
{
	m_eDistance = DIS_MIDDLE;
	// 2. 게걸음, 물폭탄
	EMoveAxis eMove = rand() % 2 ? EMoveAxis::WEST : EMoveAxis::EAST;
	switch (m_iMidOrder)
	{
	case 0:
		AddCommand("Move", 3.f, &CAIController::Move_TurnToTarget, eMove, 1.f);
		break;
	case 1:
		AddCommand("Turn", 3.f, &CEM0320_Controller::TurnToTargetStop, 1.f);
		AddCommand("Attack_Water", 0.f, &CAIController::Input, MOUSE_RB); // Water attack
		AddCommand("TurnSlow", m_fTurnSlowTime, &CEM0320_Controller::TurnToTarget, m_fTurnSlowRatio);
		break;
	case 2:
		AddCommand("Move", 3.f, &CAIController::Move_TurnToTarget, eMove, 1.f);
		break;
	case 3:
		AddCommand("Turn", 3.f, &CEM0320_Controller::TurnToTargetStop, 1.f);
		AddCommand("Attack_Water", 0.f, &CAIController::Input, MOUSE_RB); // Water attack
		AddCommand("TurnSlow", m_fTurnSlowTime, &CEM0320_Controller::TurnToTarget, m_fTurnSlowRatio);
		break;
	case 4:
		AddCommand("Turn", 3.f, &CEM0320_Controller::TurnToTargetStop, 1.f);
		AddCommand("Attack_Water", 0.f, &CAIController::Input, MOUSE_RB); // Water attack
		AddCommand("TurnSlow", m_fTurnSlowTime, &CEM0320_Controller::TurnToTarget, m_fTurnSlowRatio);
		break;
	}

	m_iMidOrder = (m_iMidOrder + 1) % 5;
}

void CEM0320_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;
	// 3. 게걸음, 물폭탄, 점프
	EMoveAxis eMove = rand() % 2 ? EMoveAxis::WEST : EMoveAxis::EAST;
	switch (m_iFarOrder)
	{
	case 0:
		AddCommand("Move", 3.f, &CAIController::Move_TurnToTarget, eMove, 1.f);
		break;
	case 1:
		AddCommand("Turn", 3.f, &CEM0320_Controller::TurnToTargetStop, 1.f);
		AddCommand("Attack_Water", 0.f, &CAIController::Input, MOUSE_RB); // Water attack
		AddCommand("TurnSlow", m_fTurnSlowTime, &CEM0320_Controller::TurnToTarget, m_fTurnSlowRatio);
		break;
	case 2:
		AddCommand("Turn", 3.f, &CEM0320_Controller::TurnToTargetStop, 1.f);
		AddCommand("Attack_Water", 0.f, &CAIController::Input, MOUSE_RB); // Water attack
		AddCommand("TurnSlow", m_fTurnSlowTime, &CEM0320_Controller::TurnToTarget, m_fTurnSlowRatio);
		break;
	case 3:
		AddCommand("Move", 3.f, &CAIController::Move_TurnToTarget, eMove, 1.f);
		break;
	case 4:
		AddCommand("Turn", 3.f, &CEM0320_Controller::TurnToTargetStop, 1.f);
		AddCommand("Jump", 0.f, &CAIController::Input, E);
		break;
	case 5:
		AddCommand("Turn", 3.f, &CEM0320_Controller::TurnToTargetStop, 1.f);
		AddCommand("Attack_Water", 0.f, &CAIController::Input, MOUSE_RB); // Water attack
		AddCommand("TurnSlow", m_fTurnSlowTime, &CEM0320_Controller::TurnToTarget, m_fTurnSlowRatio);
		break;
	case 6:
		AddCommand("Turn", 3.f, &CEM0320_Controller::TurnToTargetStop, 1.f);
		AddCommand("Attack_Water", 0.f, &CAIController::Input, MOUSE_RB); // Water attack
		AddCommand("TurnSlow", m_fTurnSlowTime, &CEM0320_Controller::TurnToTarget, m_fTurnSlowRatio);
		break;
	}
	m_iFarOrder = (m_iFarOrder + 1) % 7;
}

void CEM0320_Controller::Tick_Outside(_double TimeDelta)
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

//void CEM0320_Controller::TurnToTargetStop(_float fSpeedRatio)
//{
//	TurnToTarget(fSpeedRatio);
//	if (abs(m_fTurnRemain) < 0.1f)
//	{
//		m_Commands.front().SetFinish();
//	}
//}

void CEM0320_Controller::Free()
{
	CAIController::Free();
	Safe_Release(m_pFSM);
}

CComponent* CEM0320_Controller::Clone(void* pArg)
{
	CEM0320_Controller*		pInstance = new CEM0320_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0320_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEM0320_Controller* CEM0320_Controller::Create()
{
	CEM0320_Controller*		pInstance = new CEM0320_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0320_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}
