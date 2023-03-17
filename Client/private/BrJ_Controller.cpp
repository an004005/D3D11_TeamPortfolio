#include "stdafx.h"
#include "..\public\BrJ_Controller.h"
#include <random>

#include "BronJon.h"
#include "FSMComponent.h"

CBrJ_Controller::CBrJ_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CBrJ_Controller::CBrJ_Controller(const CBrJ_Controller & rhs)
	: CAIController(rhs)
{
}

HRESULT CBrJ_Controller::Initialize(void * pArg)
{
	FAILED_CHECK(CAIController::Initialize(pArg));

	m_pFSM = CFSMComponentBuilder()
		.InitState("Operate")
		.AddState("Operate")
			.AddTransition("Operate to Near", "Near")
				.Predicator([this]
				{
					return m_pCastedOwner->IsPlayingSocket() == false;
				})

		.AddState("Near")
			.Tick(this, &CBrJ_Controller::Tick_Near)

			.AddTransition("Near to Mid", "Mid")
				.Predicator([this]
				{
					return m_fToTargetDistance > 8.f && m_fToTargetDistance <= 20.f;
				})

			.AddTransition("Near to Far", "Far")
				.Predicator([this]
				{
					return m_fToTargetDistance > 20.f;
				})

		.AddState("Mid")
			.Tick(this, &CBrJ_Controller::Tick_Mid)

			.AddTransition("Mid to Near", "Near")
				.Predicator([this]
				{
					return m_fToTargetDistance <= 8.f;
				})

			.AddTransition("Mid to Far", "Far")
				.Predicator([this]
				{
					return m_fToTargetDistance > 20.f;
				})

		.AddState("Far")
			.Tick(this, &CBrJ_Controller::Tick_Far)
			
			.AddTransition("Far to Near", "Near")
				.Predicator([this]
				{
					return m_fToTargetDistance <= 8.f;
				})

			.AddTransition("Far to Mid", "Mid")
				.Predicator([this]
				{
					return m_fToTargetDistance > 8.f && m_fToTargetDistance <= 20.f;
				})

		.Build();

	m_fTurnSlowTime = 0.7f;
	m_fTurnSlowRatio = 0.5f;

	return S_OK;
}

void CBrJ_Controller::BeginTick()
{
	CAIController::BeginTick();
	m_pCastedOwner = dynamic_cast<CBronJon*>(TryGetOwner());
	Assert(m_pCastedOwner != nullptr);
}

void CBrJ_Controller::AI_Tick(_double TimeDelta)
{
	if (m_pTarget == nullptr)
		return;
	// 대상과의 거리 
	m_fToTargetDistance = XMVectorGetX(XMVector3LengthEst(
		m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)
		- m_pCastedOwner->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)));

	if (IsCommandRunning() == false && m_pCastedOwner->IsPlayingSocket() == false)
	{
		m_pFSM->Tick(TimeDelta);
	}
}

void CBrJ_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;
	// 1. 좌 우 뒤 이동(플레이어 보면서) 2. 전방 물기 공격 3. 레이저 공격
	EMoveAxis eMove = rand() % 2 ? EMoveAxis::WEST : EMoveAxis::EAST;
	switch (m_iNearOrder)
	{
	case 0:
		AddCommand("MoveL_or_R", 2.7f, &CAIController::Move_TurnToTarget, eMove, 1.f);
		break;
	case 1:
		AddCommand("Wait", 1.3f, &CAIController::Wait);
		break;
	case 2:	// Bite_Atk
		AddCommand("Attack_Bite", 0.f, &CAIController::Input, MOUSE_RB); 
		break;
	case 3:
		AddCommand("MoveL_or_R", 2.7f, &CAIController::Move_TurnToTarget, eMove, 1.f);
		break;
	case 4:
		AddCommand("Wait", 1.3f, &CAIController::Wait);
		break;
	case 5:
		AddCommand("Turn", 3.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_Laser", 0.f, &CAIController::Input, MOUSE_LB);
		AddCommand("TurnSlow", m_fTurnSlowTime, &CAIController::TurnToTarget, m_fTurnSlowRatio);
		break;
	case 6:
		AddCommand("Wait", 1.3f, &CAIController::Wait);
		break;
	case 7:
		AddCommand("BackMove", 2.3f, &CAIController::Move, EMoveAxis::SOUTH);
		break;
	case 8:
		AddCommand("Attack_Bite", 0.f, &CAIController::Input, MOUSE_RB);
		break;
	case 9:
		AddCommand("Turn", 3.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_Laser", 0.f, &CAIController::Input, MOUSE_LB);
		AddCommand("TurnSlow", m_fTurnSlowTime, &CAIController::TurnToTarget, m_fTurnSlowRatio);
		break;
	}

//	_uint iRand = (rand() % 2) + 1;
	m_iNearOrder = (m_iNearOrder + 1) % 10;
}

void CBrJ_Controller::Tick_Mid(_double TimeDelta)
{
	m_eDistance = DIS_MIDDLE;
	EMoveAxis eMove = rand() % 2 ? EMoveAxis::WEST : EMoveAxis::EAST;
	// 1. 좌 우 앞 이동 2. 레이저 공격
	switch (m_iMidOrder)
	{
	case 0:
		AddCommand("MoveL_or_R", 2.7f, &CAIController::Move_TurnToTarget, eMove, 1.f);
		break;
	case 1:
		AddCommand("Wait", 1.3f, &CAIController::Wait);
		break;
	case 2:
		AddCommand("Turn", 3.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_Laser", 0.f, &CAIController::Input, MOUSE_LB);
		AddCommand("TurnSlow", m_fTurnSlowTime, &CAIController::TurnToTarget, m_fTurnSlowRatio);
		break;
	case 3:
		AddCommand("Wait", 2.f, &CAIController::Wait);
		break;
	case 4: 
		AddCommand("MoveL_or_R", 2.7f, &CAIController::Move_TurnToTarget, eMove, 1.f);
		break;
	case 5:
		AddCommand("Wait", 1.3f, &CAIController::Wait);
		break;
	case 6:
		AddCommand("ForwardMove", 2.3f, &CAIController::Move, EMoveAxis::NORTH);
		break;
	case 7:
		AddCommand("Turn", 3.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_Laser", 0.f, &CAIController::Input, MOUSE_LB);
		AddCommand("TurnSlow", m_fTurnSlowTime, &CAIController::TurnToTarget, m_fTurnSlowRatio);
		break;
	}

//	_uint iRand = (rand() % 2) + 1;
	m_iMidOrder = (m_iMidOrder + 1) % 8;
}

void CBrJ_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;

	switch (m_iFarOrder)
	{
	case 0:
		AddCommand("Wait", 4.f, &CAIController::Wait);
		break;
	case 1:
		AddCommand("Wait", 4.f, &CAIController::Wait);
		break;
	case 2:
		AddCommand("Threat", 0.f, &CAIController::Input, G);
		break;
	case 3:
		AddCommand("Wait", 4.f, &CAIController::Wait);
		break;
	}

	m_iFarOrder = (m_iFarOrder + 1) % 4;
}

void CBrJ_Controller::Free()
{
	CAIController::Free();
	Safe_Release(m_pFSM);
}

CComponent * CBrJ_Controller::Clone(void * pArg)
{
	CBrJ_Controller*		pInstance = new CBrJ_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBrJ_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBrJ_Controller * CBrJ_Controller::Create()
{
	CBrJ_Controller*		pInstance = new CBrJ_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBrJ_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}
