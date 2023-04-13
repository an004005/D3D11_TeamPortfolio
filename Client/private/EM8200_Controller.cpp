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
	m_IceNeedle_CoolTimeHelper.Initialize(30.f, true);
	m_KickAtk_CoolTimeHelper.Initialize(15.f, false);
	m_RushAtk_CoolTimeHelper.Initialize(35.f, false);
	m_ChaseElec_CoolTimeHelper.Initialize(20.f, false);
	m_AirElec_CoolTimeHelper.Initialize(30.f, false);

	m_CaptureKinetic_CoolTimeHelper.Initialize(10.f, false);
	m_Seethrough_CoolTimeHelper.Initialize(30.f, false);
	m_Counter_CoolTimeHelper.Initialize(30.f, false);

	m_DetectedCoolTimeHelper.Initialize(3.f, true);
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

		// AddCommand("Rush_Copy_Start", 0.f, &CAIController::Input, G);
		// AddCommand("Air_Elec_Atk_Charge_Start", 0.f, &CAIController::Input, F);
		// AddCommand("Ice_Needle_Attack", 0.f, &CEM8200_Controller::Input, Q);
		// AddCommand("Kick_A1", 0.f, &CAIController::Input, E);
		// AddCommand("Chase_Elec_Start", 0.f, &CAIController::Input, R);

		// AddCommand("CaptureKinetic", 0.f, &CAIController::Input, B);

		// AddCommand("Seethrough", 0.f, &CEM8200_Controller::Input, L);
		AddCommand("Counter", 0.f, &CEM8200_Controller::Input, C);

	}

	if(m_Commands.empty() || m_Commands.front().GetName() == "Walk")
		Detected_Attack();

	if(m_pCastedOwner->Check_SecondPhase() == true)
		Check_PlayerUseSas();
}

void CEM8200_Controller::Detected_Attack()
{
	if (m_DetectedCoolTimeHelper.CanUse())
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

			if (CGameInstance::GetInstance()->OverlapSphere(param) )
			{
				for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
				{
					auto pObj = CPhysXUtils::GetOnwer(overlapOut.getAnyHit(i).actor);
					if (auto pTarget = dynamic_cast<CScarletCharacter*>(pObj))
					{
						if (pObj == m_pCastedOwner)
							continue;

						if (CPlayerInfoManager::GetInstance()->Get_PlayerAttackEnable() == true)
						{
							_uint iRand = CMathUtils::RandomUInt(3);

							if (iRand == 0 && m_DetectedCoolTimeHelper.Use())
							{
								ClearCommands();

								const _uint iNum = CMathUtils::RandomInt(NUM_2, NUM_4);

								AddCommand("Teleport", 0.f, &CAIController::Input, (EHandleInput)iNum);
							}
							else
								return;
						}
					}
					else if (auto pKinetic = dynamic_cast<CMapKinetic_Object*>(pObj))
					{
						if (pKinetic->GetThrow() == true && pKinetic->IsUseBoss() == false)
						{
							_uint iRand = CMathUtils::RandomUInt(3);

							if (iRand == 0 && m_DetectedCoolTimeHelper.Use())
							{
								ClearCommands();
								_uint iNum = CMathUtils::RandomInt(NUM_2, NUM_3);
								AddCommand("Teleport", 0.f, &CAIController::Input, (EHandleInput)iNum);
							}

							else if (iRand == 1 && m_KineticSet.IsNotDo() && m_DetectedCoolTimeHelper.Use())
							{
								// Capture
								ClearCommands();
								m_pCastedOwner->Set_KineticObject(pObj);
								AddCommand("CaptureKinetic", 0.f, &CAIController::Input, B);
							}
							else
								return;
						}
					}
				}
			}
		}
		else
			return;
	}
	else
		return;
}

void CEM8200_Controller::Check_PlayerUseSas()
{
	if (CPlayerInfoManager::GetInstance()->Get_PlayerSasList().empty() == false && m_Seethrough_CoolTimeHelper.CanUse())
	{
		if(m_Seethrough_CoolTimeHelper.Use())
			AddCommand("Seethrough", 0.f, &CEM8200_Controller::Input, L);
	}
}

void CEM8200_Controller::Tick_CoolTimeHelper(_double TimeDelta)
{
	m_IceNeedle_CoolTimeHelper.Tick(TimeDelta);
	m_KickAtk_CoolTimeHelper.Tick(TimeDelta);
	m_RushAtk_CoolTimeHelper.Tick(TimeDelta);
	m_ChaseElec_CoolTimeHelper.Tick(TimeDelta);
	m_AirElec_CoolTimeHelper.Tick(TimeDelta);

	m_CaptureKinetic_CoolTimeHelper.Tick(TimeDelta);
	m_Seethrough_CoolTimeHelper.Tick(TimeDelta);
	m_Counter_CoolTimeHelper.Tick(TimeDelta);

	m_DetectedCoolTimeHelper.Tick(TimeDelta);
}

void CEM8200_Controller::CalcTeleport_Dir()
{

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

	// if (m_RushAtk_CoolTimeHelper.CanUse() == false && m_ChaseElec_CoolTimeHelper.CanUse() == false && m_AirElec_CoolTimeHelper.CanUse() == false)
	// {
	// 	if (rand() % 1 == 0)
	// 		AddCommand("Teleport_B_Start", 0.f, &CEM8200_Controller::Input, NUM_4);
	//
	// }


	switch (m_iNearOrder)
	{
	case 0:
		 AddCommand("Idle", 1.f, &CAIController::Wait);
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
		if(m_ChaseElec_CoolTimeHelper.Use() && m_pCastedOwner->Check_SecondPhase() == true)
		{
			AddCommand("Chase_Elec_Start", 0.f, &CAIController::Input, R);
		}
		break;
	case 5:
		if (m_Counter_CoolTimeHelper.Use() && m_pCastedOwner->Check_SecondPhase() == true)
		{
			AddCommand("Counter", 0.f, &CEM8200_Controller::Input, C);
		}
		break;
	}

	m_iNearOrder = (m_iNearOrder + 1) % 6;
}

void CEM8200_Controller::Tick_Mid(_double TimeDelta)
{
	m_eDistance = DIS_MIDDLE;
	const _uint iNum = CMathUtils::RandomInt(NUM_1, NUM_3);

	switch (m_iMidOrder)
	{
	case 0:
		AddCommand("Walk", 2.f, &CAIController::Move, EMoveAxis::NORTH);
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
	case 3:
		if (m_Counter_CoolTimeHelper.Use() && m_pCastedOwner->Check_SecondPhase() == true)
		{
			AddCommand("Counter", 0.f, &CEM8200_Controller::Input, C);
		}
		break;
	}
	m_iMidOrder = (m_iMidOrder + 1) % 4;
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
		if(m_IceNeedle_CoolTimeHelper.Use())
		{
			AddCommand("Ice_Needle_Attack", 0.f, &CEM8200_Controller::Input, Q);
		}
		break;
	}

	m_iFarOrder = (m_iFarOrder + 1) % 2;
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
