#include "stdafx.h"
#include "..\public\FL_Controller.h"
#include "FlowerLeg.h"
#include "FSMComponent.h"

CFL_Controller::CFL_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CFL_Controller::CFL_Controller(const CFL_Controller & rhs)
	: CAIController(rhs)
{
}

HRESULT CFL_Controller::Initialize(void * pArg)
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
				.Tick(this, &CFL_Controller::Tick_Outside)

				.AddTransition("Outside to Near", "Near")
					.Predicator([this] 
					{
						return m_fToTargetDistance <= 20.f;
					})

			.AddState("Near")
				.Tick(this, &CFL_Controller::Tick_Near)
				
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
				.Tick(this, &CFL_Controller::Tick_Mid)
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
				.Tick(this, &CFL_Controller::Tick_Far)
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

	return S_OK;
}

void CFL_Controller::BeginTick()
{
	CAIController::BeginTick();
	m_pCastedOwner = dynamic_cast<CFlowerLeg*>(TryGetOwner());
	Assert(m_pCastedOwner != nullptr);
}

void CFL_Controller::AI_Tick(_double TimeDelta)
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

void CFL_Controller::Tick_Near(_double TimeDelta)
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

void CFL_Controller::Tick_Mid(_double TimeDelta)
{
	m_eDistance = DIS_MIDDLE;

	switch (m_iMidOrder)
	{
	case 0:
		AddCommand("WalkTurn", 1.f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	case 1:
		AddCommand("Run", 4.f, &CFL_Controller::Run_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	}
	m_iMidOrder = (m_iMidOrder + 1) % 2;
}

void CFL_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;

	switch (m_iFarOrder)
	{
	case 0:
		AddCommand("Turn", 3.f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		AddCommand("Jump", 0.f, &CAIController::Input, MOUSE_RB);
		break;
	case 1:
		AddCommand("Threat", 0.f, &CAIController::Input, C);
		break;
	}

	m_iFarOrder = (m_iFarOrder + 1) % 2;
}

void CFL_Controller::Tick_Outside(_double TimeDelta)
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


void CFL_Controller::Run(EMoveAxis eAxis)
{
	m_bRun = true;

	switch (eAxis)
	{
	case EMoveAxis::NORTH:
		m_vMoveAxis.z += 1.f;
		if (abs(m_fTtoM_Distance) < 3.f)
		{
			m_Commands.front().SetFinish();
		}
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

void CFL_Controller::Run_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio)
{
	Run(eAxis);
	TurnToTarget(fSpeedRatio);
}

void CFL_Controller::Free()
{
	CAIController::Free();
	Safe_Release(m_pFSM);
}

CComponent * CFL_Controller::Clone(void * pArg)
{
	CFL_Controller*		pInstance = new CFL_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFL_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CFL_Controller * CFL_Controller::Create()
{
	CFL_Controller*		pInstance = new CFL_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFL_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}
