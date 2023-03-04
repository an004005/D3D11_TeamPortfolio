#include "stdafx.h"
#include "..\public\Boss1_AIController.h"
#include "Boss1.h"
#include "FSMComponent.h"


CBoss1_AIController::CBoss1_AIController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAIController(pDevice, pContext)
{
}

CBoss1_AIController::CBoss1_AIController(const CBoss1_AIController& rhs)
	: CAIController(rhs)
{
}

HRESULT CBoss1_AIController::Initialize(void* pArg)
{
	FAILED_CHECK(CAIController::Initialize(pArg));

	m_pFSM = CFSMComponentBuilder()
		.InitState("Start")
		.AddState("Start")
			.AddTransition("Start to Near", "Near")
				.Predicator([this]
				{
					return m_pCastedOwner->IsPlayingSocket() == false;
				})

		.AddState("Near")
			.Tick(this, &CBoss1_AIController::Tick_Near)
			.AddTransition("Near to Far", "Mid")
				.Predicator([this]
				{
					return m_fToTargetDistance > 8.f;
				})

		.AddState("Mid")
			.Tick(this, &CBoss1_AIController::Tick_Mid)
			.AddTransition("Mid to Near", "Near")
				.Predicator([this]
				{
					return m_fToTargetDistance <= 8.f;
				})
			.AddTransition("Mid to Far", "Far")
				.Predicator([this]
				{
					return m_fToTargetDistance > 14.f;
				})

		.AddState("Far")
			.Tick(this, &CBoss1_AIController::Tick_Far)
			.AddTransition("Far to Near", "Near")
				.Predicator([this]
				{
					return m_fToTargetDistance <= 14.f;
				})
		.Build();

	m_fTurnSlowTime = 0.7f;
	m_fTurnSlowRatio = 0.5f;

	return S_OK;
}

void CBoss1_AIController::BeginTick()
{
	CAIController::BeginTick();
	m_pCastedOwner = dynamic_cast<CBoss1*>(TryGetOwner());
	Assert(m_pCastedOwner != nullptr);
}

void CBoss1_AIController::AI_Tick(_double TimeDelta)
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

void CBoss1_AIController::Tick_Near(_double TimeDelta)
{
	// 1. 스핀 공격, 플레이어 보고 때리기, 플레이어보고 뒤가기,
	switch (m_iNearOrder)
	{
	case 0:
		AddCommand("Move", 2.f, &CAIController::Move, EMoveAxis::SOUTH);
		break;
	case 1:
		AddCommand("Turn", 3.f, &CBoss1_AIController::TurnToTargetStop, 1.f);
		AddCommand("Attack", 0.f, &CAIController::Input, MOUSE_LB);//attack
		break;
	case 2:
		AddCommand("MoveTurn", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::SOUTH, 1.f);
		break;
	case 3:
		AddCommand("Turn", 3.f, &CBoss1_AIController::TurnToTargetStop, 1.f);
		AddCommand("Attack", 0.f, &CAIController::Input, MOUSE_LB);//attack
		break;
	case 4:
		AddCommand("Turn", 3.f, &CBoss1_AIController::TurnToTargetStop, 1.f);
		AddCommand("Attack", 0.f, &CAIController::Input, MOUSE_LB);//attack
		break;
	case 5:
		AddCommand("Attack_Spin", 0.f, &CAIController::Input, G); // spin attack
		break;
	}

	m_iNearOrder = (m_iNearOrder + 1) % 6;
}

void CBoss1_AIController::Tick_Mid(_double TimeDelta)
{
	// 2. 게걸음, 물폭탄
	EMoveAxis eMove = rand() % 2 ? EMoveAxis::WEST : EMoveAxis::EAST;
	switch (m_iMidOrder)
	{
	case 0:
		AddCommand("Move", 3.f, &CAIController::Move_TurnToTarget, eMove, 1.f);
		break;
	case 1:
		AddCommand("Turn", 3.f, &CBoss1_AIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_Water", 0.f, &CAIController::Input, MOUSE_RB); // Water attack
		AddCommand("TurnSlow", m_fTurnSlowTime, &CBoss1_AIController::TurnToTarget, m_fTurnSlowRatio);
		break;
	case 2:
		AddCommand("Move", 3.f, &CAIController::Move_TurnToTarget, eMove, 1.f);
		break;
	case 3:
		AddCommand("Turn", 3.f, &CBoss1_AIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_Water", 0.f, &CAIController::Input, MOUSE_RB); // Water attack
		AddCommand("TurnSlow", m_fTurnSlowTime, &CBoss1_AIController::TurnToTarget, m_fTurnSlowRatio);
		break;
	case 4:
		AddCommand("Attack_Water", 0.f, &CAIController::Input, MOUSE_RB); // Water attack
		AddCommand("TurnSlow", m_fTurnSlowTime, &CBoss1_AIController::TurnToTarget, m_fTurnSlowRatio);
		break;
	}

	m_iMidOrder = (m_iMidOrder + 1) % 5;
}

void CBoss1_AIController::Tick_Far(_double TimeDelta)
{
	// 3. 게걸음, 물폭탄, 점프
	EMoveAxis eMove = rand() % 2 ? EMoveAxis::WEST : EMoveAxis::EAST;
	switch (m_iFarOrder)
	{
	case 0:
		AddCommand("Move", 3.f, &CAIController::Move_TurnToTarget, eMove, 1.f);
		break;
	case 1:
		AddCommand("Turn", 3.f, &CBoss1_AIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_Water", 0.f, &CAIController::Input, MOUSE_RB); // Water attack
		AddCommand("TurnSlow", m_fTurnSlowTime, &CBoss1_AIController::TurnToTarget, m_fTurnSlowRatio);
		break;
	case 2:
		AddCommand("Turn", 3.f, &CBoss1_AIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_Water", 0.f, &CAIController::Input, MOUSE_RB); // Water attack
		AddCommand("TurnSlow", m_fTurnSlowTime, &CBoss1_AIController::TurnToTarget, m_fTurnSlowRatio);
		break;
	case 3:
		AddCommand("Move", 3.f, &CAIController::Move_TurnToTarget, eMove, 1.f);
		break;
	case 4:
		AddCommand("Turn", 3.f, &CBoss1_AIController::TurnToTargetStop, 1.f);
		AddCommand("Jump", 0.f, &CAIController::Input, E);
		break;
	case 5:
		AddCommand("Turn", 3.f, &CBoss1_AIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_Water", 0.f, &CAIController::Input, MOUSE_RB); // Water attack
		AddCommand("TurnSlow", m_fTurnSlowTime, &CBoss1_AIController::TurnToTarget, m_fTurnSlowRatio);
		break;
	case 6:
		AddCommand("Turn", 3.f, &CBoss1_AIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_Water", 0.f, &CAIController::Input, MOUSE_RB); // Water attack
		AddCommand("TurnSlow", m_fTurnSlowTime, &CBoss1_AIController::TurnToTarget, m_fTurnSlowRatio);
		break;
	}
	m_iFarOrder = (m_iFarOrder + 1) % 7;
}

void CBoss1_AIController::TurnToTargetStop(_float fSpeedRatio)
{
	TurnToTarget(fSpeedRatio);
	if (abs(m_fTurnRemain) < 0.1f)
	{
		m_Commands.front().SetFinish();
	}
}

void CBoss1_AIController::Free()
{
	CAIController::Free();
	Safe_Release(m_pFSM);
}

CComponent* CBoss1_AIController::Clone(void* pArg)
{
	CBoss1_AIController*		pInstance = new CBoss1_AIController(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss1_AIController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBoss1_AIController* CBoss1_AIController::Create()
{
	CBoss1_AIController*		pInstance = new CBoss1_AIController(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBoss1_AIController");
		Safe_Release(pInstance);
	}
	return pInstance;
}
