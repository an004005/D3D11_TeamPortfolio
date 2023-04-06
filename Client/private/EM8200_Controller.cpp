#include "stdafx.h"
#include "..\public\EM8200_Controller.h"
#include "FSMComponent.h"
#include "EM8200.h"

CEM8200_Controller::CEM8200_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAIController(pDevice, pContext)
{
}

CEM8200_Controller::CEM8200_Controller(const CEM8200_Controller& rhs)
	: CAIController(rhs)
{
}

HRESULT CEM8200_Controller::Initialize(void* pArg)
{
	FAILED_CHECK(CAIController::Initialize(pArg));

	m_pFSM = CFSMComponentBuilder()
		.InitState("Operate")
		.AddState("Operate")
		.AddTransition("Operate to Far", "Far")
		.Predicator([this]
			{
				return m_pCastedOwner->IsPlayingSocket() == false;
			})

		.AddState("Far")
		.Tick(this, &CEM8200_Controller::Tick_Far)
			.AddTransition("Far to Mid", "Mid")
			.Predicator([this]
				{
					return m_fTtoM_Distance <= 12.f;
				})
		.AddTransition("Mid to Near", "Near")
			.Predicator([this]
				{
					return m_fTtoM_Distance <= 4.f;
				})


		.AddState("Mid")
		.Tick(this, &CEM8200_Controller::Tick_Mid)
		.AddTransition("Mid to Near", "Near")
		.Predicator([this]
		{
			return m_fTtoM_Distance <= 4.f;
		})

		.AddTransition("Mid to Far", "Far")
		.Predicator([this]
		{
			return m_fTtoM_Distance > 12.f;
		})


		.AddState("Near")
		.Tick(this, &CEM8200_Controller::Tick_Near)

		.AddTransition("Near to Mid", "Mid")
		.Predicator([this]
		{
			return m_fTtoM_Distance > 4.f && m_fTtoM_Distance <= 12.f;
		})

		.AddTransition("Near to Far", "Far")
		.Predicator([this]
		{
			return m_fTtoM_Distance > 12.f;
		})

		
		// .AddState("Far")
				// .Tick(this, &CEM8200_Controller::Tick_Far)
				// .AddTransition("Far to Near", "Near")
				// .Predicator([this]
					// {
						// return m_fTtoM_Distance <= 4.f;
					// })

		// .AddTransition("Far to Mid", "Mid")
				// .Predicator([this]
					// {
						// return m_fTtoM_Distance > 4.f && m_fTtoM_Distance <= 12.f;
					// })

		.Build();

		m_fTurnSlowTime = 0.9f;
		m_fTurnSlowRatio = 0.4f;

		// m_iNearOrder = CMathUtils::RandomUInt(6);
		// m_iMidOrder = CMathUtils::RandomUInt(3);
		// m_iFarOrder = CMathUtils::RandomUInt(1);

		return S_OK;
}

void CEM8200_Controller::BeginTick()
{
	CAIController::BeginTick();
	m_pCastedOwner = dynamic_cast<CEM8200*>(TryGetOwner());
	Assert(m_pCastedOwner != nullptr);
}

void CEM8200_Controller::AI_Tick(_double TimeDelta)
{
	m_bRun = false;

	if (m_pTarget == nullptr)
		return;

	if (IsCommandRunning() == false && m_pCastedOwner->IsPlayingSocket() == false)
	{
		m_pFSM->Tick(TimeDelta);
	}
}

void CEM8200_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;

	switch (m_iNearOrder)
	{
	case 0:
		 AddCommand("Idle", 2.f, &CAIController::Wait);
		break;
	// case 1:
		// AddCommand("Run", 2.f, &CEM8200_Controller::Run_TurnToTarget, EMoveAxis::SOUTH, 1.f);
		// break;
	// case 1:
	// 	// AddCommand("WalkTurn", 1.2f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
	// 	break;
	// case 2:
	// 	// AddCommand("Dodge_L", 0.f, &CAIController::Input, NUM_1);
	// 	break;
	// case 3:
	// 	// AddCommand("Attack_Screw", 0.f, &CAIController::Input, G);
	// 	break;
	// case 4:
	// 	// AddCommand("Dodge_B", 0.f, &CAIController::Input, NUM_2);
	// 	break;
	// case 5:
	// 	// AddCommand("WalkTurn", 1.2f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
	// 	break;
	// case 6:
	// 	// AddCommand("Dodge_R", 0.f, &CAIController::Input, NUM_3);
	// 	break;
	}

	// m_iNearOrder = (m_iNearOrder + 1) % 1;
}

void CEM8200_Controller::Tick_Mid(_double TimeDelta)
{
	m_eDistance = DIS_MIDDLE;

	switch (m_iMidOrder)
	{
	case 0:
		AddCommand("Walk", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	// case 1:
	// 	// AddCommand("Run", 2.f, &CEM0200_Controller::Run_TurnToTarget, EMoveAxis::NORTH, 1.f);
	// 	break;
	// case 2:
	// 	// AddCommand("Turn", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
	// 	// AddCommand("Jump", 0.f, &CAIController::Input, MOUSE_RB);
	// 	break;
	// case 3:
	// 	// AddCommand("Threat", 0.f, &CAIController::Input, C);
	// 	break;
	}
	// m_iMidOrder = (m_iMidOrder + 1) % 4;
}

void CEM8200_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;

	switch (m_iFarOrder)
	{
	case 0:
		AddCommand("Run", 2.f, &CEM8200_Controller::Run_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	// case 1:
		// AddCommand("Threat", 0.f, &CAIController::Input, C);
		// break;
	}

	// m_iFarOrder = (m_iFarOrder + 1) % 2;
}

void CEM8200_Controller::Tick_Outside(_double TimeDelta)
{
	m_eDistance = DIS_OUTSIDE;

	switch (m_iOutOrder)
	{
	case 0:
		// AddCommand("Wait", 2.f, &CAIController::Wait);
		break;

	case 1:
		// AddCommand("Wait", 2.f, &CAIController::Wait);
		break;
	}

	m_iOutOrder = (m_iOutOrder + 1) % 2;
}

void CEM8200_Controller::Run_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio)
{
	m_bRun = true;
	Move(eAxis);
	TurnToTarget(fSpeedRatio);
}

void CEM8200_Controller::Free()
{
	CAIController::Free();
	Safe_Release(m_pFSM);
}

CComponent* CEM8200_Controller::Clone(void* pArg)
{
	CEM8200_Controller* pInstance = new CEM8200_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM8200_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEM8200_Controller* CEM8200_Controller::Create()
{
	CEM8200_Controller* pInstance = new CEM8200_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM8200_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}
