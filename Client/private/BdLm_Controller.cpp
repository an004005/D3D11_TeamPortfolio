#include "stdafx.h"
#include "..\public\BdLm_Controller.h"
#include "BuddyLumi.h"
#include "FSMComponent.h"

CBdLm_Controller::CBdLm_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CBdLm_Controller::CBdLm_Controller(const CBdLm_Controller & rhs)
	: CAIController(rhs)
{
}

HRESULT CBdLm_Controller::Initialize(void * pArg)
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
		
		.AddState("Outside")
			.Tick(this, &CBdLm_Controller::Tick_Outside)

			.AddTransition("Outside to Far", "Far")
				.Predicator([this]
				{
					return m_fToTargetDistance <= 20.f;
				})

		.AddState("Near")
			.Tick(this, &CBdLm_Controller::Tick_Near)

			.AddTransition("Near to Mid", "Mid")
				.Predicator([this]
				{
					return m_fToTargetDistance > 5.f && m_fToTargetDistance <= 14.f;
				})

			.AddTransition("Near to Far", "Far")
				.Predicator([this]
				{
					return m_fToTargetDistance > 14.f;
				})

		.AddState("Mid")
			.Tick(this, &CBdLm_Controller::Tick_Mid)
			
			.AddTransition("Mid to Near", "Near")
				.Predicator([this]
				{
					return m_fToTargetDistance <= 5.f;
				})

			.AddTransition("Mid to Far", "Far")
				.Predicator([this]
				{
					return m_fToTargetDistance > 14.f;
				})

		.AddState("Far")
			.Tick(this, &CBdLm_Controller::Tick_Far)

			.AddTransition("Far to Near", "Near")
				.Predicator([this]
				{
					return m_fToTargetDistance <= 5.f;
				})

			.AddTransition("Far to Mid", "Mid")
				.Predicator([this]
				{
					return m_fToTargetDistance > 5.f && m_fToTargetDistance <= 14.f;
				})

		.Build();

	m_fTurnSlowTime = 0.9f;
	m_fTurnSlowRatio = 0.4f;

	return S_OK;
}

void CBdLm_Controller::BeginTick()
{
	CAIController::BeginTick();
	m_pCastedOwner = dynamic_cast<CBuddyLumi*>(TryGetOwner());
	Assert(m_pCastedOwner != nullptr);
}

void CBdLm_Controller::AI_Tick(_double TimeDelta)
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

void CBdLm_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;

	switch (m_iNearOrder)
	{
	case 0:
		AddCommand("Dodge_L", 0.f, &CAIController::Input, NUM_1);
		AddCommand("Turn", 2.7f, &CAIController::Move_TurnToTarget, EMoveAxis::WEST, 1.f);
		break;
	case 1:
		AddCommand("Dodge_R", 0.f, &CAIController::Input, NUM_2);
		AddCommand("Turn", 2.7f, &CAIController::Move_TurnToTarget, EMoveAxis::EAST, 1.f);
		break;
	case 2:
		AddCommand("Attack_Swing", 0.f, &CAIController::Input, MOUSE_LB);
		break;
	case 3:
		AddCommand("Dodge_B", 0.f, &CAIController::Input, NUM_3);
		AddCommand("BackMove", 2.3f, &CAIController::Move_TurnToTarget, EMoveAxis::SOUTH, 1.f);
		break;
	case 4:
		AddCommand("Threat", 0.f, &CAIController::Input, MOUSE_RB);
		AddCommand("StateTurn", 3.f, &CAIController::TurnToTarget, 1.f);
		break;	
	}

	m_iNearOrder = (m_iNearOrder + 1) % 5;
}

void CBdLm_Controller::Tick_Mid(_double TimeDelta)
{
	m_eDistance = DIS_MIDDLE;

	switch (m_iMidOrder)
	{
	case 0:
		AddCommand("Walk", 1.5f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	case 1:
		AddCommand("Run", 5.f, &CBdLm_Controller::Run_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	}

	m_iMidOrder = (m_iMidOrder + 1) % 2;
}

void CBdLm_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;

	switch (m_iFarOrder)
	{
	case 0:
		AddCommand("Run", 3.5f, &CBdLm_Controller::Run_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	}
	
	m_iFarOrder = (m_iFarOrder + 1) % 1;
}

void CBdLm_Controller::Tick_Outside(_double TimeDelta)
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

void CBdLm_Controller::Run(EMoveAxis eAxis)
{
	m_bRun = true;

	switch (eAxis)
	{
	case EMoveAxis::NORTH:
		m_vMoveAxis.z += 1.f;
		if (abs(m_fTtoM_Distance) < 2.3f)
		{
			m_Commands.front().SetFinish();
		}
		break;
	case EMoveAxis::NORTH_EAST:
		m_vMoveAxis.z += 1.f;
		m_vMoveAxis.x += 1.f;
		break;
	case EMoveAxis::EAST:
		m_vMoveAxis.x += 1.f;
		break;
	case EMoveAxis::SOUTH_EAST:
		m_vMoveAxis.z -= 1.f;
		m_vMoveAxis.x += 1.f;
		break;
	case EMoveAxis::SOUTH:
		m_vMoveAxis.z -= 1.f;
		break;
	case EMoveAxis::SOUTH_WEST:
		m_vMoveAxis.z -= 1.f;
		m_vMoveAxis.x -= 1.f;
		break;
	case EMoveAxis::WEST:
		m_vMoveAxis.x -= 1.f;
		break;
	case EMoveAxis::NORTH_WEST:
		m_vMoveAxis.z += 1.f;
		m_vMoveAxis.x -= 1.f;
		break;
	case EMoveAxis::CENTER:
		FALLTHROUGH;
	case EMoveAxis::AXIS_END:
		break;
	default:
		NODEFAULT;
	}
}

void CBdLm_Controller::Run_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio)
{
	Run(eAxis);
	TurnToTarget(fSpeedRatio);
}

void CBdLm_Controller::Free()
{
	CAIController::Free();
	Safe_Release(m_pFSM);
}

CComponent * CBdLm_Controller::Clone(void * pArg)
{
	CBdLm_Controller*		pInstance = new CBdLm_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBdLm_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBdLm_Controller * CBdLm_Controller::Create()
{
	CBdLm_Controller*		pInstance = new CBdLm_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFL_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}
