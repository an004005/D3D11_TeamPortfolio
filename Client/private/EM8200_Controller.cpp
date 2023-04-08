#include "stdafx.h"
#include "..\public\EM8200_Controller.h"

#include <GameUtils.h>
#include <PhysX_Manager.h>

#include "FSMComponent.h"
#include "EM8200.h"
#include "MapKinetic_Object.h"
#include "PlayerInfoManager.h"

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
					return m_fTtoM_Distance <= 5.f;
				})


		.AddState("Mid")
		.Tick(this, &CEM8200_Controller::Tick_Mid)
		.AddTransition("Mid to Near", "Near")
		.Predicator([this]
		{
			return m_fTtoM_Distance <= 5.f;
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
			return m_fTtoM_Distance > 5.f && m_fTtoM_Distance <= 12.f;
		})

		.AddTransition("Near to Far", "Far")
		.Predicator([this]
		{
			return m_fTtoM_Distance > 12.f;
		})

		.Build();

		m_fTurnSlowTime = 0.9f;
		m_fTurnSlowRatio = 0.4f;

		Initialize_CoolTimeHelper();

		return S_OK;
}

void CEM8200_Controller::Initialize_CoolTimeHelper()
{
	m_IceNeedle_CoolTimeHelper.Initialize(300000.f, true);
	m_KickAtk_CoolTimeHelper.Initialize(5.f, true);
	m_RushAtk_CoolTimeHelper.Initialize(350000.f, true);
	m_ChaseElec_CoolTimeHelper.Initialize(200000.f, true);
	m_AirElec_CoolTimeHelper.Initialize(200000.f, true);
}

void CEM8200_Controller::BeginTick()
{
	CAIController::BeginTick();
	m_pCastedOwner = dynamic_cast<CEM8200*>(TryGetOwner());
	Assert(m_pCastedOwner != nullptr);
}

void CEM8200_Controller::AI_Tick(_double TimeDelta)
{
	Tick_TP_Cooltime(TimeDelta);
	Tick_CoolTimeHelper(TimeDelta);

	m_bRun = false;

	if (m_pTarget == nullptr)
		return;

	if (IsCommandRunning() == false && m_pCastedOwner->IsPlayingSocket() == false)
	{
		// m_pFSM->Tick(TimeDelta);
		// AddCommand("Kick_A1", 0.f, &CAIController::Input, E);
		AddCommand("Air_Elec_Atk_Charge_Start", 0.f, &CAIController::Input, F);
	}

	Detected_Attack();
}

void CEM8200_Controller::Detected_Attack()
{
	if (m_pCastedOwner->IsPlayingSocket() == false)
	{
		physx::PxOverlapHit hitBuffer[5];
		physx::PxOverlapBuffer overlapOut(hitBuffer, 5);

		SphereOverlapParams param;
		param.fVisibleTime = 0.3f;
		param.iTargetType = CTB_PLAYER | CTB_PSYCHICK_OBJ;
		param.fRadius = 5.f;
		param.vPos = m_pCastedOwner->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		param.overlapOut = &overlapOut;

		if (CGameInstance::GetInstance()->OverlapSphere(param))
		{
			for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
			{
				auto pObj = CPhysXUtils::GetOnwer(overlapOut.getAnyHit(i).actor);
				if (auto pTarget = dynamic_cast<CScarletCharacter*>(pObj))
				{
					if(pObj == m_pCastedOwner)
						continue;

					if(CPlayerInfoManager::GetInstance()->Get_PlayerAttackEnable() == true)
					{
						if (rand() % 4 == 0)
							return;

						ClearCommands();

						const _uint iNum = CMathUtils::RandomInt(NUM_1, NUM_4);

						AddCommand("Teleport", 0.f, &CAIController::Input, (EHandleInput)iNum);
					}
				}
				else if (auto pKinetic = dynamic_cast<CMapKinetic_Object*>(pObj))
				{
					if (pKinetic->GetThrow() == true && pKinetic->IsUseBoss() == false)
					{
						if (rand() % 4 == 0)
							return;

						ClearCommands();

						const _uint iNum = CMathUtils::RandomInt(NUM_1, NUM_4);

						AddCommand("Teleport", 0.f, &CAIController::Input, (EHandleInput)iNum);
					}
				}
			}
		}
	}
	else
		return;
}

void CEM8200_Controller::Tick_CoolTimeHelper(_double TimeDelta)
{
	m_IceNeedle_CoolTimeHelper.Tick(TimeDelta);
	m_KickAtk_CoolTimeHelper.Tick(TimeDelta);
	m_RushAtk_CoolTimeHelper.Tick(TimeDelta);
	m_ChaseElec_CoolTimeHelper.Tick(TimeDelta);
	m_AirElec_CoolTimeHelper.Tick(TimeDelta);
}

void CEM8200_Controller::Tick_TP_Cooltime(_double TimeDelta)
{
	if (m_bCanUseTeleport == false)
	{
		m_fTP_CurCoolTime += (_float)TimeDelta;
	}

	if (m_fTP_CurCoolTime >= m_fTP_CoolTime)
	{
		m_bCanUseTeleport = true;
		m_fTP_CurCoolTime = 0.f;
		m_fTP_CoolTime = CGameUtils::GetRandFloat(m_fTP_RandCoolTime.x, m_fTP_RandCoolTime.y);
	}
}

void CEM8200_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;
	const _uint iNum = CMathUtils::RandomInt(NUM_1, NUM_3);

	switch (m_iNearOrder)
	{
	case 0:
		 AddCommand("Walk", 1.f, &CAIController::Wait);
		break;
	case 1:
		if(m_bCanUseTeleport == true)
		{
			AddCommand("Teleport", 0.f, &CAIController::Input, (EHandleInput)iNum);
			m_bCanUseTeleport = false;
		}
		break;
	case 2:
		if (m_KickAtk_CoolTimeHelper.Use())
		{
			AddCommand("Kick_A1", 0.f, &CAIController::Input, E);
		}
		break;
	case 3:
		if(m_RushAtk_CoolTimeHelper.Use())
		{
			AddCommand("Rush_Copy_Start", 0.f, &CAIController::Input, G);
		}
		break;
	case 4:
		if(m_ChaseElec_CoolTimeHelper.Use())
		{
			AddCommand("Chase_Elec_Start", 0.f, &CAIController::Input, R);
		}
		break;
	// case 5:
	// 	// AddCommand("WalkTurn", 1.2f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
	// 	break;
	// case 6:
	// 	// AddCommand("Dodge_R", 0.f, &CAIController::Input, NUM_3);
	// 	break;
	}

	m_iNearOrder = (m_iNearOrder + 1) % 5;
}

void CEM8200_Controller::Tick_Mid(_double TimeDelta)
{
	m_eDistance = DIS_MIDDLE;
	const _uint iNum = CMathUtils::RandomInt(NUM_1, NUM_3);

	switch (m_iMidOrder)
	{
	case 0:
		AddCommand("Walk", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	case 1:
		if (m_bCanUseTeleport == true)
		{
			AddCommand("Teleport", 0.f, &CAIController::Input, (EHandleInput)iNum);
			m_bCanUseTeleport = false;
		}
		break;
	case 2:
		if (m_AirElec_CoolTimeHelper.Use())
		{
			AddCommand("Air_Elec_Atk_Charge_Start", 0.f, &CAIController::Input, F);
		}
		break;

	// case 2:
	// 	// AddCommand("Turn", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
	// 	// AddCommand("Jump", 0.f, &CAIController::Input, MOUSE_RB);
	// 	break;
	// case 3:
	// 	// AddCommand("Threat", 0.f, &CAIController::Input, C);
	// 	break;
	}
	m_iMidOrder = (m_iMidOrder + 1) % 3;
}

void CEM8200_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;

	switch (m_iFarOrder)
	{
	case 0:
		AddCommand("Teleport_F", 0.f, &CEM8200_Controller::Input, NUM_1);
		break;
	case 1:
		AddCommand("Teleport_F", 0.f, &CEM8200_Controller::Input, NUM_1);
		break;
	case 2:
		if(m_IceNeedle_CoolTimeHelper.Use())
		{
			AddCommand("Ice_Needle_Attack", 0.f, &CEM8200_Controller::Input, Q);
		}
		break;
	}

	m_iFarOrder = (m_iFarOrder + 1) % 3;
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

	// m_iOutOrder = (m_iOutOrder + 1) % 2;
}

void CEM8200_Controller::Run_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio)
{
	m_bRun = true;
	Move(eAxis);
	TurnToTarget(fSpeedRatio);
}

void CEM8200_Controller::Teleport_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio)
{
	TurnToTarget(fSpeedRatio);
}

void CEM8200_Controller::LookAtNow()
{
	m_pCastedOwner->GetTransform()->LookAt_NonY(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
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
