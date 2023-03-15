#include "stdafx.h"
#include "..\public\SkPd_Controller.h"
#include <random>

#include "SkummyPandou.h"
#include "FSMComponent.h"

CSkPd_Controller::CSkPd_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CSkPd_Controller::CSkPd_Controller(const CSkPd_Controller & rhs)
	: CAIController(rhs)
{
}

HRESULT CSkPd_Controller::Initialize(void * pArg)
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
			.Tick(this, &CSkPd_Controller::Tick_Outside)
			.OnExit(dynamic_cast<CMonster*>(m_pCastedOwner), &CMonster::TurnEyesOut)
			.AddTransition("Outside to Far", "Far")
				.Predicator([this]
				{
					return m_fToTargetDistance <= 33.f;
				})

		.AddState("Near")
			.Tick(this, &CSkPd_Controller::Tick_Near)

			.AddTransition("Near to Mid", "Mid")
				.Predicator([this]
				{
					return m_fToTargetDistance > 10.f && m_fToTargetDistance <= 22.f;
				})

			.AddTransition("Near to Far", "Far")
				.Predicator([this]
				{
					return m_fToTargetDistance > 22.f;
				})

		.AddState("Mid")
			.Tick(this, &CSkPd_Controller::Tick_Mid)
			.AddTransition("Mid to Near", "Near")
				.Predicator([this]
				{
					return m_fToTargetDistance <= 10.f;
				})

			.AddTransition("Mid to Far", "Far")
				.Predicator([this]
				{
					return m_fToTargetDistance > 22.f;
				})

		.AddState("Far")
			.Tick(this, &CSkPd_Controller::Tick_Far)
			.AddTransition("Far to Near", "Near")
				.Predicator([this]
				{
					return m_fToTargetDistance <= 10.f;
				})

			.AddTransition("Far to Mid", "Mid")
				.Predicator([this]
				{
					return m_fToTargetDistance > 10.f && m_fToTargetDistance <= 22.f;
				})

		.Build();

	m_fTurnSlowTime = 0.9f;
	m_fTurnSlowRatio = 0.4f;

	return S_OK;
}

void CSkPd_Controller::BeginTick()
{
	CAIController::BeginTick();
	m_pCastedOwner = dynamic_cast<CSkummyPandou*>(TryGetOwner());
	Assert(m_pCastedOwner != nullptr);
}

void CSkPd_Controller::AI_Tick(_double TimeDelta)
{
	m_bRush = false;

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

void CSkPd_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;
	// 1. 돌진 2. 도발 3. 좌로 이동 4. 우로 이동 5. Idle

	switch (m_iNearOrder)
	{
	case 0:
		AddCommand("Turn", 4.f, &CAIController::TurnToTarget, 1.f);
		break;
	case 1:
		AddCommand("Attack_Start", 0.f, &CAIController::Input, MOUSE_LB);
		break;
	case 2:
		AddCommand("Attack_Rush", 0.f, &CAIController::Input, R);
		AddCommand("Rush", 3.f, &CSkPd_Controller::Rush, EMoveAxis::NORTH);
		break;
		/*case 3:
		AddCommand("Attack_End", 0.f, &CAIController::Input, C);
		break;*/
	case 3:
		AddCommand("Turn", 4.f, &CAIController::TurnToTarget, 1.f);
		break;
	case 4:
		AddCommand("LeftMove", 2.5f, &CAIController::Move_TurnToTarget, EMoveAxis::WEST, 1.f);
		break;
	case 5:
		AddCommand("Turn", 3.5f, &CAIController::TurnToTarget, 1.f);
		AddCommand("Threat", 0.f, &CAIController::Input, G);
		break;
	case 6:
		AddCommand("Turn", 3.f, &CAIController::TurnToTarget, 1.f);
		break;
	case 7:
		AddCommand("Attack_Start", 0.f, &CAIController::Input, MOUSE_LB);
		break;
	case 8:
		AddCommand("Attack_Rush", 0.f, &CAIController::Input, R);
		AddCommand("Rush", 3.f, &CSkPd_Controller::Rush, EMoveAxis::NORTH);
		break;
		/*case 10:
		AddCommand("Attack_End", 0.f, &CAIController::Input, C);
		break;*/
	case 9:
		AddCommand("Turn", 4.f, &CAIController::TurnToTarget, 1.f);
		break;
	case 10:
		AddCommand("RightMove", 2.5f, &CAIController::Move_TurnToTarget, EMoveAxis::EAST, 1.f);
		break;
	case 11:
		AddCommand("ForwardMove", 1.5f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	}

	m_iNearOrder = (m_iNearOrder + 1) % 12;
}

void CSkPd_Controller::Tick_Mid(_double TimeDelta)
{
	m_eDistance = DIS_MIDDLE;
	
	switch (m_iMidOrder)
	{
	case 0:
		AddCommand("Turn", 4.f, &CAIController::TurnToTarget, 1.f);
		break;
	case 1:
		AddCommand("Attack_Start", 0.f, &CAIController::Input, MOUSE_LB);
		break;
	case 2:
		AddCommand("Attack_Rush", 0.f, &CAIController::Input, R);
		AddCommand("Rush", 3.f, &CSkPd_Controller::Rush, EMoveAxis::NORTH);
		break;
		/*case 3:
		AddCommand("Attack_End", 0.f, &CAIController::Input, C);
		break;*/
	case 3:
		AddCommand("Turn", 4.f, &CAIController::TurnToTarget, 1.f);
		break;
	case 4:
		AddCommand("LeftMove", 2.5f, &CAIController::Move_TurnToTarget, EMoveAxis::WEST, 1.f);
		break;
	case 5:
		AddCommand("Turn", 3.5f, &CAIController::TurnToTarget, 1.f);
		AddCommand("Threat", 0.f, &CAIController::Input, G);
		break;
	case 6:
		AddCommand("Turn", 3.f, &CAIController::TurnToTarget, 1.f);
		break;
	case 7:
		AddCommand("Attack_Start", 0.f, &CAIController::Input, MOUSE_LB);
		break;
	case 8:
		AddCommand("Attack_Rush", 0.f, &CAIController::Input, R);
		AddCommand("Rush", 3.f, &CSkPd_Controller::Rush, EMoveAxis::NORTH);
		break;
		/*case 10:
		AddCommand("Attack_End", 0.f, &CAIController::Input, C);
		break;*/
	case 9:
		AddCommand("Turn", 4.f, &CAIController::TurnToTarget, 1.f);
		break;
	case 10:
		AddCommand("RightMove", 2.5f, &CAIController::Move_TurnToTarget, EMoveAxis::EAST, 1.f);
		break;
	case 11:
		AddCommand("ForwardMove", 1.5f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	}
	
	m_iMidOrder = (m_iMidOrder + 1) % 12;
}

void CSkPd_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;

	switch (m_iFarOrder)
	{
	case 0: 
		AddCommand("ForwardMove", 4.f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
//	case 1:
////		AddCommand("Turn", 2.f, &CAIController::TurnToTarget, 1.f);
//		AddCommand("Wait", 2.f, &CAIController::Wait);
//		break;
	}

	m_iFarOrder = (m_iFarOrder + 1) % 1;
}

void CSkPd_Controller::Tick_Outside(_double TimeDelta)
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

void CSkPd_Controller::Rush(EMoveAxis eAxis)
{
	m_bRush = true;

	switch (eAxis)
	{
	case EMoveAxis::NORTH:
		m_vMoveAxis.z += 1.f;
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

void CSkPd_Controller::Free()
{
	CAIController::Free();
	Safe_Release(m_pFSM);
}

CComponent * CSkPd_Controller::Clone(void * pArg)
{
	CSkPd_Controller*		pInstance = new CSkPd_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSkPd_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSkPd_Controller * CSkPd_Controller::Create()
{
	CSkPd_Controller*		pInstance = new CSkPd_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSkPd_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}
