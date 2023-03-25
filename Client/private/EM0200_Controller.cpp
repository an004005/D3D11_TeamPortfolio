#include "stdafx.h"
#include "..\public\EM0200_Controller.h"
#include <FSMComponent.h>
#include "EM0200.h"

CEM0200_Controller::CEM0200_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAIController(pDevice, pContext)
{
}

CEM0200_Controller::CEM0200_Controller(const CEM0200_Controller& rhs)
	: CAIController(rhs)
{
}

HRESULT CEM0200_Controller::Initialize(void* pArg)
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
				.Tick(this, &CEM0200_Controller::Tick_Outside)
				.OnExit([this]
					{
						//m_pCastedOwner->TurnEyesOut();
					})
				.AddTransition("Outside to Far", "Far")
					.Predicator([this] 
					{
						return m_fToTargetDistance <= 20.f;
					})

			.AddState("Near")
				.Tick(this, &CEM0200_Controller::Tick_Near)
				
				.AddTransition("Near to Mid", "Mid")
					.Predicator([this] 
					{
						return m_fToTargetDistance > 4.f && m_fToTargetDistance <= 12.f;
					})

				.AddTransition("Near to Far", "Far")
					.Predicator([this] 
					{
						return m_fToTargetDistance > 12.f;
					})

			.AddState("Mid")
				.Tick(this, &CEM0200_Controller::Tick_Mid)
				.AddTransition("Mid to Near", "Near")
					.Predicator([this] 
					{
						return m_fToTargetDistance <= 4.f;
					})

				.AddTransition("Mid to Far", "Far")
					.Predicator([this]
					{
						return m_fToTargetDistance > 12.f;
					})

			.AddState("Far")
				.Tick(this, &CEM0200_Controller::Tick_Far)
				.AddTransition("Far to Near", "Near")
					.Predicator([this]
					{
						return m_fToTargetDistance <= 4.f;
					})
				
				.AddTransition("Far to Mid", "Mid")
					.Predicator([this] 
					{
						return m_fToTargetDistance > 4.f && m_fToTargetDistance <= 12.f;
					})

			.Build();
	
	m_fTurnSlowTime = 0.9f;
	m_fTurnSlowRatio = 0.4f;

	m_iNearOrder = CMathUtils::RandomUInt(6);
	m_iMidOrder = CMathUtils::RandomUInt(3);
	m_iFarOrder = CMathUtils::RandomUInt(1);

	return S_OK;
}

void CEM0200_Controller::BeginTick()
{
	CAIController::BeginTick();
	m_pCastedOwner = dynamic_cast<CEM0200*>(TryGetOwner());
	Assert(m_pCastedOwner != nullptr);
}

void CEM0200_Controller::AI_Tick(_double TimeDelta)
{
	m_bRun = false;

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

void CEM0200_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;
	// 1. 회전 공격 2. 흩뿌리기 공격 3. 회피(좌, 우, 뒤) 4. Walk
	switch (m_iNearOrder) 
	{
	case 0:
		AddCommand("Attack_Spin", 0.f, &CAIController::Input, R);
		break;	
	case 1:
		AddCommand("WalkTurn", 1.2f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	case 2:
		AddCommand("Dodge_L", 0.f, &CAIController::Input, NUM_1);
		break;
	case 3:
		AddCommand("Attack_Screw", 0.f, &CAIController::Input, G);
		break;
	case 4:
		AddCommand("Dodge_B", 0.f, &CAIController::Input, NUM_2);
		break;
	case 5:
		AddCommand("WalkTurn", 1.2f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	case 6:
		AddCommand("Dodge_R", 0.f, &CAIController::Input, NUM_3);
		break;
	}

	m_iNearOrder = (m_iNearOrder + 1) % 7;
}

void CEM0200_Controller::Tick_Mid(_double TimeDelta)
{
	m_eDistance = DIS_MIDDLE;

	switch (m_iMidOrder)
	{
	case 0:
		AddCommand("WalkTurn", 1.f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	case 1:
		AddCommand("Run", 2.f, &CEM0200_Controller::Run_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	case 2:
		AddCommand("Turn", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		AddCommand("Jump", 0.f, &CAIController::Input, MOUSE_RB);
		break;
	case 3:
		AddCommand("Threat", 0.f, &CAIController::Input, C);
		break;
	}
	m_iMidOrder = (m_iMidOrder + 1) % 4;
}

void CEM0200_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;

	switch (m_iFarOrder)
	{
	case 0:
		AddCommand("Turn", 2.5f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		AddCommand("Jump", 0.f, &CAIController::Input, MOUSE_RB);
		break;
	case 1:
		AddCommand("Threat", 0.f, &CAIController::Input, C);
		break;
	}

	m_iFarOrder = (m_iFarOrder + 1) % 2;
}

void CEM0200_Controller::Tick_Outside(_double TimeDelta)
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


void CEM0200_Controller::Run_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio)
{
	m_bRun = true;
	Move(eAxis);
	TurnToTarget(fSpeedRatio);
}

void CEM0200_Controller::Free()
{
	CAIController::Free();
	Safe_Release(m_pFSM);
}

CComponent* CEM0200_Controller::Clone(void* pArg)
{
	CEM0200_Controller*		pInstance = new CEM0200_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0200_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEM0200_Controller* CEM0200_Controller::Create()
{
	CEM0200_Controller*		pInstance = new CEM0200_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0200_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}
