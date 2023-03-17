#include "stdafx.h"
#include "../public/BaseAnimInstance.h"
#include "Player.h"
#include "Model.h"
#include "GameInstance.h"
#include "HelperClasses.h"

HRESULT CBaseAnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	m_pModel->Find_Animation("AS_ch0100_021_AL_run_start_F")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_ch0100_022_AL_run_start_L")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_ch0100_023_AL_run_start_R")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_ch0100_024_AL_run_start_BL")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_ch0100_025_AL_run_start_BR")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_ch0100_121_AL_run_start_F")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_ch0100_122_AL_run_start_L")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_ch0100_123_AL_run_start_R")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_ch0100_124_AL_run_start_BL")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_ch0100_125_AL_run_start_BR")->SetLocalRotation(true);

	m_pASM_Base = CASMBuilder()
		.InitState("IDLE")

#pragma region IDLE
		.AddState("IDLE")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_001_AL_wait01"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, true, true, false })
					.Event_Trail(false);
			})

				.AddTransition("IDLE to JUMP_FALL", "JUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f)
				.Priority(0)

				.AddTransition("IDLE to WALK", "WALK")
				.Predicator([&]()->_bool { return m_bWalk; })
				.Duration(0.1f)
				.Priority(0)

				.AddTransition("IDLE to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("IDLE to ATTACK_UPPER_START", "ATTACK_UPPER_START")
				.Predicator([&]()->_bool {return m_bLeftClick && m_bUpper; })
				.Duration(0.1f)
				.Priority(0)

				.AddTransition("IDLE to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash; })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("IDLE to JUMP_START", "JUMP_START")
				.Predicator([&]()->_bool {return m_bJump; })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("IDLE to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("IDLE to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f)
				.Priority(1)

#pragma endregion IDLE

#pragma region Walk

		.AddState("WALK")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_021_AL_run_start_F"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->Event_Trail(false);
			})

				.AddTransition("WALK to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f)
				.Priority(0)

				.AddTransition("WALK to WALK_START_FRONT", "WALK_START_FRONT")
				.Predicator([&]()->_bool { return m_eMoveDir == CPlayer::DIR_F; })
				.Duration(0.1f)
				.Priority(0)

				.AddTransition("WALK to WALK_START_LEFT", "WALK_START_LEFT")
				.Predicator([&]()->_bool { return m_eMoveDir == CPlayer::DIR_L; })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("WALK to WALK_START_RIGHT", "WALK_START_RIGHT")
				.Predicator([&]()->_bool { return m_eMoveDir == CPlayer::DIR_R; })
				.Duration(0.1f)
				.Priority(2)

				.AddTransition("WALK to WALK_START_BACK_LEFT", "WALK_START_BACK_LEFT")
				.Predicator([&]()->_bool { return m_eMoveDir == CPlayer::DIR_B; })
				.Duration(0.1f)
				.Priority(3)

				.AddTransition("WALK to WALK_START_BACK_RIGHT", "WALK_START_BACK_RIGHT")
				.Predicator([&]()->_bool { return m_eMoveDir == CPlayer::DIR_B; })
				.Duration(0.1f)
				.Priority(4)

				.AddTransition("WALK to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("WALK to IDLE", "IDLE")
				.Predicator([&]()->_bool { return !m_bWalk; })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("WALK to ATTACK_UPPER_START", "ATTACK_UPPER_START")
				.Predicator([&]()->_bool {return m_bLeftClick && m_bUpper; })
				.Duration(0.1f)
				.Priority(0)

		.AddState("WALK_START_FRONT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_021_AL_run_start_F"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ true, false, false, false, false, true, true, true, true, false });
			})

				.AddTransition("WALK_START_FRONT to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f)
				.Priority(0)

				.AddTransition("WALK to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("WALK_START_FRONT to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash; })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("WALK_START_FRONT to RUNJUMP_START", "RUNJUMP_START")
				.Predicator([&]()->_bool {return m_bJump; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_FRONT to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("WALK_START_FRONT to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("WALK_START_FRONT to WALK_LOOP", "WALK_LOOP")
//				.Predicator([&]()->_bool { return 0.95f <= m_fPlayRatio; })
				.Priority(0)
				.Duration(0.1f)

				.AddTransition("WALK_START_FRONT to WALK_END", "WALK_END")
				.Predicator([&]()->_bool { return !m_bWalk && (m_eMoveDir != CPlayer::DIR_F); })
				.Priority(1)
				.Duration(0.1f)

				.AddTransition("WALK_START_FRONT to ATTACK_UPPER_START", "ATTACK_UPPER_START")
				.Predicator([&]()->_bool {return m_bLeftClick && m_bUpper; })
				.Duration(0.1f)
				.Priority(0)

		.AddState("WALK_START_LEFT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_022_AL_run_start_L"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ true, false, false, false, false, true, true, true, true, false });
			})
			.FinishEvent([&]()
			{
				//static_cast<CPlayer*>(m_pTargetObject)->LookAtDir(m_vLocalMove);
			})

				.AddTransition("WALK_START_LEFT to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("WALK to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_LEFT to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_LEFT to RUNJUMP_START", "RUNJUMP_START")
				.Predicator([&]()->_bool {return m_bJump; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_LEFT to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_LEFT to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_LEFT to WALK_LOOP", "WALK_LOOP")
//				.Predicator([&]()->_bool { return 0.95f <= m_fPlayRatio; })
				.Priority(0).Duration(0.1f)

				.AddTransition("WALK_START_LEFT to WALK_END", "WALK_END")
				.Predicator([&]()->_bool { return !m_bWalk/* && (0.4255f <= m_fPlayRatio)*/ && (m_eMoveDir != CPlayer::DIR_L); })
				.Priority(1)
				.Duration(0.1f)

				.AddTransition("WALK_START_LEFT to ATTACK_UPPER_START", "ATTACK_UPPER_START")
				.Predicator([&]()->_bool {return m_bLeftClick && m_bUpper; })
				.Duration(0.1f)
				.Priority(0)

		.AddState("WALK_START_RIGHT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_023_AL_run_start_R"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ true, false, false, false, false, true, true, true, true, false });
			})
			.FinishEvent([&]()
			{
				//static_cast<CPlayer*>(m_pTargetObject)->LookAtDir(m_vLocalMove);
			})

				.AddTransition("WALK_START_RIGHT to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("WALK to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_RIGHT to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_RIGHT to RUNJUMP_START", "RUNJUMP_START")
				.Predicator([&]()->_bool {return m_bJump; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_RIGHT to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_RIGHT to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_RIGHT to WALK_LOOP", "WALK_LOOP")
//				.Predicator([&]()->_bool { return 0.95f <= m_fPlayRatio; })
				.Duration(0.1f).Priority(0)

				.AddTransition("WALK_START_RIGHT to WALK_END", "WALK_END")
				.Predicator([&]()->_bool { return !m_bWalk/* && (0.4255f <= m_fPlayRatio)*/ && (m_eMoveDir != CPlayer::DIR_R); })
				.Priority(1)
				.Duration(0.1f)

				.AddTransition("WALK_START_RIGHT to ATTACK_UPPER_START", "ATTACK_UPPER_START")
				.Predicator([&]()->_bool {return m_bLeftClick && m_bUpper; })
				.Duration(0.1f)
				.Priority(0)

		.AddState("WALK_START_BACK_LEFT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_024_AL_run_start_BL"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ true, false, false, false, false, true, true, true, true, false });
			})
			.FinishEvent([&]()
			{
				//static_cast<CPlayer*>(m_pTargetObject)->LookAtDir(m_vLocalMove);
			})

				.AddTransition("WALK_START_BACK_LEFT to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("WALK to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_BACK_LEFT to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_BACK_LEFT to RUNJUMP_START", "RUNJUMP_START")
				.Predicator([&]()->_bool {return m_bJump; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_BACK_LEFT to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_BACK_LEFT to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_BACK_LEFT to WALK_LOOP", "WALK_LOOP")
	//			.Predicator([&]()->_bool { return 0.95f <= m_fPlayRatio; })
				.Duration(0.1f).Priority(0)

				.AddTransition("WALK_START_BACK_LEFT to WALK_END", "WALK_END")
				.Predicator([&]()->_bool { return !m_bWalk/* && (0.4255f <= m_fPlayRatio)*/ && (m_eMoveDir != CPlayer::DIR_B); })
				.Priority(1)
				.Duration(0.1f)

				.AddTransition("WALK_START_BACK_LEFT to ATTACK_UPPER_START", "ATTACK_UPPER_START")
				.Predicator([&]()->_bool {return m_bLeftClick && m_bUpper; })
				.Duration(0.1f)
				.Priority(0)

		.AddState("WALK_START_BACK_RIGHT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_025_AL_run_start_BR"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
				->SetAbleState({ true, false, false, false, false, true, true, true, true, false });
			})
			.FinishEvent([&]()
			{
				//static_cast<CPlayer*>(m_pTargetObject)->LookAtDir(m_vLocalMove);
			})

				.AddTransition("WALK_START_BACK_RIGHT to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("WALK to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_BACK_RIGHT to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_BACK_RIGHT to RUNJUMP_START", "RUNJUMP_START")
				.Predicator([&]()->_bool {return m_bJump; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_BACK_RIGHT to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_BACK_RIGHT to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(1)
			
				.AddTransition("WALK_START_BACK_RIGHT to WALK_LOOP", "WALK_LOOP")
			//	.Predicator([&]()->_bool { return 0.95f <= m_fPlayRatio; })
				.Duration(0.1f).Priority(0)

				.AddTransition("WALK_START_BACK_RIGHT to WALK_END", "WALK_END")
				.Predicator([&]()->_bool { return !m_bWalk/* && (0.4255f <= m_fPlayRatio)*/ && (m_eMoveDir != CPlayer::DIR_B); })
				.Priority(1)
				.Duration(0.1f)

				.AddTransition("WALK_START_BACK_RIGHT to ATTACK_UPPER_START", "ATTACK_UPPER_START")
				.Predicator([&]()->_bool {return m_bLeftClick && m_bUpper; })
				.Duration(0.1f)
				.Priority(0)

		.AddState("WALK_LOOP")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_026_AL_run"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)->LookAtDir(m_vMoveDir);	// 이동하고 있는 방향을 봐야됨
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ true, true, false, false, false, true, true, true, true, false });
			})
		
				.AddTransition("WALK_LOOP to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("WALK to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_LOOP to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_LOOP to RUNJUMP_START", "RUNJUMP_START")
				.Predicator([&]()->_bool {return m_bJump; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_LOOP to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_LOOP to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_LOOP to WALK_END", "WALK_END")
				.Predicator([&]()->_bool { return !m_bWalk; })
				.Duration(0.1f).Priority(0)

				.AddTransition("WALK_LOOP to ATTACK_UPPER_START", "ATTACK_UPPER_START")
				.Predicator([&]()->_bool {return m_bLeftClick && m_bUpper; })
				.Duration(0.1f)
				.Priority(0)

		.AddState("WALK_END")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_028_AL_run_end"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			})

				.AddTransition("WALK_END to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("WALK_END to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_END to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_END to JUMP_START", "JUMP_START")
				.Predicator([&]()->_bool {return m_bJump; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_END to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_END to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_END to IDLE", "IDLE")
				.Duration(0.1f).Priority(0)

				.AddTransition("WALK_END to WALK", "WALK")
				.Predicator([&]()->_bool { return m_bWalk; })
				.Duration(0.1f).Priority(0)

				.AddTransition("WALK_END to ATTACK_UPPER_START", "ATTACK_UPPER_START")
				.Predicator([&]()->_bool {return m_bLeftClick && m_bUpper; })
				.Duration(0.1f)
				.Priority(0)

#pragma endregion Walk

#pragma region Attack_A

		.AddState("ATK_A1")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_201_AL_atk_a1"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, true, false, true, true, false, true, false })
					.LookTarget();
			})

				.AddTransition("ATK_A1 to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("ATK_A1 to ATK_A2", "ATK_A2")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.15f <= m_fPlayRatio) && (0.3f > m_fPlayRatio) && (CheckAnim("AS_ch0100_201_AL_atk_a1")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("REPEAT", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.3f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_201_AL_atk_a1")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_A1 to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && (0.15f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_201_AL_atk_a1")) && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("ATK_A1 to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && (0.15f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_201_AL_atk_a1")) && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(2)

				.AddTransition("ATK_A1 to ATTACK_UPPER_START", "ATTACK_UPPER_START")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.15f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_201_AL_atk_a1")) && m_bUpper; })
				.Duration(0.1f)
				.Priority(0)

				.AddTransition("ATK_A1 to JUMP_START", "JUMP_START")
				.Predicator([&]()->_bool {return m_bJump && (0.3f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_201_AL_atk_a1")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_A1 to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash && (0.15f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_201_AL_atk_a1")); })
				.Duration(0.1f).Priority(0)

				.AddTransition("ATK_A1 to WALK", "WALK")
				.Predicator([&]()->_bool {return m_bWalk && (0.3f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_201_AL_atk_a1")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_A1 to IDLE", "IDLE")
				.Duration(0.1f)
				.Priority(100)

		.AddState("ATK_A2")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_202_AL_atk_a2"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, true, false, true, true, false, true, false })
					.LookTarget();
			})

				.AddTransition("ATK_A2 to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("ATK_A2 to ATK_A3", "ATK_A3")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.15f <= m_fPlayRatio) && (0.5f > m_fPlayRatio) && (CheckAnim("AS_ch0100_202_AL_atk_a2")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_A2 to ATTACK_UPPER_START", "ATTACK_UPPER_START")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.15f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_202_AL_atk_a2")) && m_bUpper; })
				.Duration(0.1f)
				.Priority(0)

				.AddTransition("ATK_A2 to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.5f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_202_AL_atk_a2")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_A2 to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && (0.15f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_202_AL_atk_a2")) && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("ATK_A2 to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && (0.15f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_202_AL_atk_a2")) && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(2)

				.AddTransition("ATK_A2 to JUMP_START", "JUMP_START")
				.Predicator([&]()->_bool {return m_bJump && (0.5f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_202_AL_atk_a2")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_A2 to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash && (0.15f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_202_AL_atk_a2")); })
				.Duration(0.1f).Priority(0)

				.AddTransition("ATK_A2 to WALK", "WALK")
				.Predicator([&]()->_bool {return m_bWalk && (0.5f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_202_AL_atk_a2")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_A2 to IDLE", "IDLE")
				.Duration(0.1f)
				.Priority(100)

		.AddState("ATK_A3")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_203_AL_atk_a3"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, true, false, true, true, false, true, false })
					.LookTarget();
			})

				.AddTransition("ATK_A3 to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("ATK_A3 to ATK_A4", "ATK_A4")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.25f <= m_fPlayRatio) && (0.65f > m_fPlayRatio) && (CheckAnim("AS_ch0100_203_AL_atk_a3")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_A3 to ATTACK_UPPER_START", "ATTACK_UPPER_START")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.25f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_203_AL_atk_a3")) && m_bUpper; })
				.Duration(0.1f)
				.Priority(0)

				.AddTransition("ATK_A3 to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.65f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_203_AL_atk_a3")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_A3 to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && (0.25f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_203_AL_atk_a3")) && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("ATK_A3 to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && (0.25f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_203_AL_atk_a3")) && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(2)

				.AddTransition("ATK_A3 to JUMP_START", "JUMP_START")
				.Predicator([&]()->_bool {return m_bJump && (0.65f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_203_AL_atk_a3")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_A3 to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash && (0.25f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_203_AL_atk_a3")); })
				.Duration(0.1f).Priority(0)

				.AddTransition("ATK_A3 to WALK", "WALK")
				.Predicator([&]()->_bool {return m_bWalk && (0.65f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_203_AL_atk_a3")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_A3 to IDLE", "IDLE")
				.Duration(0.1f)
				.Priority(100)

		.AddState("ATK_A4")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_204_AL_atk_a4"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, true, false, true, true, false, true, false })
					.LookTarget();
			})

				.AddTransition("ATK_A4 to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("ATK_A4 to ATK_A5", "ATK_A5")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.25f <= m_fPlayRatio) && (0.6f > m_fPlayRatio) && (CheckAnim("AS_ch0100_204_AL_atk_a4")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_A4 to ATTACK_UPPER_START", "ATTACK_UPPER_START")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.25f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_204_AL_atk_a4")) && m_bUpper; })
				.Duration(0.1f)
				.Priority(0)

				.AddTransition("ATK_A4 to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.6f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_204_AL_atk_a4")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_A4 to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && (0.25f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_204_AL_atk_a4")) && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("ATK_A4 to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && (0.25f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_204_AL_atk_a4")) && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(2)

				.AddTransition("ATK_A4 to JUMP_START", "JUMP_START")
				.Predicator([&]()->_bool {return m_bJump && (0.6f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_204_AL_atk_a4")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_A4 to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash && (0.25f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_204_AL_atk_a4")); })
				.Duration(0.1f).Priority(0)

				.AddTransition("ATK_A4 to WALK", "WALK")
				.Predicator([&]()->_bool {return m_bWalk && (0.6f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_204_AL_atk_a4")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_A4 to IDLE", "IDLE")
				.Duration(0.1f)
				.Priority(100)

		.AddState("ATK_A5")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_205_AL_atk_a5"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, true, false, true, true, false, true, false })
					.LookTarget();

				CGameInstance::GetInstance()->SetCameraFovCurve("Attack_A5_ActionCam");
			})
			.FinishEvent([&]()
			{
				CGameInstance::GetInstance()->ReleaseCameraFovCurve();
			})

				.AddTransition("ATK_A5 to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("ATK_A5 to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.4f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_205_AL_atk_a5")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_A5 to ATTACK_UPPER_START", "ATTACK_UPPER_START")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.4f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_205_AL_atk_a5")) && m_bUpper; })
				.Duration(0.1f)
				.Priority(0)

				.AddTransition("ATK_A5 to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && (0.4f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_205_AL_atk_a5")) && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("ATK_A5 to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && (0.4f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_205_AL_atk_a5")) && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(2)

				.AddTransition("ATK_A5 to JUMP_START", "JUMP_START")
				.Predicator([&]()->_bool {return m_bJump && (0.4f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_205_AL_atk_a5")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_A5 to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash && (0.4f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_205_AL_atk_a5")); })
				.Duration(0.1f).Priority(0)

				.AddTransition("ATK_A5 to WALK", "WALK")
				.Predicator([&]()->_bool {return m_bWalk && (0.4f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_205_AL_atk_a5")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_A5 to IDLE", "IDLE")
				.Duration(0.1f)
				.Priority(100)

#pragma endregion Attack_A

#pragma region Dash And Run
		// 대시 스타트 모션
		.AddState("DASH")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_051_AL_dodge_F_start"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, true, true, false })
					.Event_Trail(false);
			})

				.AddTransition("DASH to DASHJUMP_FALL", "DASHJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("DASH to ATTACK_DASH_START", "ATTACK_DASH_START")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f).Priority(0)

				.AddTransition("DASH to DASH_START_FRONT", "DASH_START_FRONT")
				.Predicator([&]()->_bool { return m_eMoveDir == CPlayer::DIR_F; })
				.Duration(0.f).Priority(0)

				.AddTransition("DASH to DASH_START_LEFT", "DASH_START_LEFT")
				.Predicator([&]()->_bool { return m_eMoveDir == CPlayer::DIR_L; })
				.Duration(0.f).Priority(0)

				.AddTransition("DASH to DASH_START_RIGHT", "DASH_START_RIGHT")
				.Predicator([&]()->_bool { return m_eMoveDir == CPlayer::DIR_R; })
				.Duration(0.f).Priority(0)

				.AddTransition("DASH to DASH_START_BACK", "DASH_START_BACK")
				.Predicator([&]()->_bool { return m_eMoveDir == CPlayer::DIR_B; })
				.Duration(0.f).Priority(0)

				.AddTransition("DASH to DASH_END_FRONT", "DASH_END_FRONT")
				.Duration(0.f).Priority(0)

		// 앞쪽 대시 관련
		.AddState("DASH_START_FRONT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_051_AL_dodge_F_start"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
				->SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			})

				.AddTransition("DASH_START_FRONT to DASHJUMP_FALL", "DASHJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("DASH_START_FRONT to ATTACK_DASH_START", "ATTACK_DASH_START")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f).Priority(0)

				.AddTransition("DASH_START_FRONT to DASHJUMP_START", "DASHJUMP_START")
				.Predicator([&]()->_bool {return m_bJump; })
				.Duration(0.1f).Priority(1)

				.AddTransition("DASH_START_FRONT to RUN_FRONT", "RUN_FRONT")
				.Predicator([&]()->_bool {return m_bWalk && (0.8f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_051_AL_dodge_F_start") || CheckAnim("AS_ch0100_151_AL_dodge_F_start")); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DASH_START_FRONT to DASH_END_FRONT", "DASH_END_FRONT")
				.Duration(0.f)
				.Priority(100)

		.AddState("DASH_END_FRONT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_051_AL_dodge_F_stop"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			})
		
				.AddTransition("DASH_END_FRONT to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("DASH_END_FRONT to RUN_FRONT", "RUN_FRONT")
				.Predicator([&]()->_bool {return m_bWalk && (0.1f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_051_AL_dodge_F_stop") || CheckAnim("AS_ch0100_151_AL_dodge_F_stop")); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DASH_END_FRONT to IDLE", "IDLE")
				.Duration(0.1f)
				.Priority(100)

		// 좌측 대시 관련
		.AddState("DASH_START_LEFT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_057_AL_dodge_L_start"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			})

				.AddTransition("DASH_START_LEFT to DASHJUMP_FALL", "DASHJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("DASH_START_LEFT to RUN_FRONT", "RUN_FRONT")
				.Predicator([&]()->_bool {return m_bWalk && (0.8f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_057_AL_dodge_L_start") || CheckAnim("AS_ch0100_157_AL_dodge_L_start")); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DASH_START_LEFT to DASHJUMP_START", "DASHJUMP_START")
				.Predicator([&]()->_bool {return m_bJump; })
				.Duration(0.1f).Priority(1)

				.AddTransition("DASH_START_LEFT to DASH_END_LEFT", "DASH_END_LEFT")
				.Duration(0.f)
				.Priority(100)

		.AddState("DASH_END_LEFT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_057_AL_dodge_L_stop"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			})
		
				.AddTransition("DASH_END_LEFT to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("DASH_END_LEFT to IDLE", "IDLE")
				.Duration(0.1f)
				.Priority(100)

		// 우측 대시 관련
		.AddState("DASH_START_RIGHT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_053_AL_dodge_R_start"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			})

				.AddTransition("DASH_START_RIGHT to DASHJUMP_FALL", "DASHJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("DASH_START_RIGHT to RUN_FRONT", "RUN_FRONT")
				.Predicator([&]()->_bool {return m_bWalk && (0.8f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_053_AL_dodge_R_start") || CheckAnim("AS_ch0100_153_AL_dodge_R_start")); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DASH_START_RIGHT to DASHJUMP_START", "DASHJUMP_START")
				.Predicator([&]()->_bool {return m_bJump; })
				.Duration(0.1f).Priority(1)

				.AddTransition("DASH_START_RIGHT to DASH_END_RIGHT", "DASH_END_RIGHT")
				.Duration(0.f)
				.Priority(100)

		.AddState("DASH_END_RIGHT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_053_AL_dodge_R_stop"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			})
		
				.AddTransition("DASH_END_RIGHT to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("DASH_END_RIGHT to RUNJUMP_START", "RUNJUMP_START")
				.Predicator([&]()->_bool {return m_bJump; })
				.Duration(0.1f).Priority(1)

				.AddTransition("DASH_END_RIGHT to IDLE", "IDLE")
				.Duration(0.1f)
				.Priority(100)

		// 후방 대시 관련
		.AddState("DASH_START_BACK")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_055_AL_dodge_B_start"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			})

				.AddTransition("DASH_START_BACK to DASHJUMP_FALL", "DASHJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("DASH_START_BACK to RUNJUMP_START", "RUNJUMP_START")
				.Predicator([&]()->_bool {return m_bJump; })
				.Duration(0.1f).Priority(1)

				.AddTransition("DASH_START_BACK to DASH_END_BACK", "DASH_END_BACK")
				.Duration(0.f)
				.Priority(100)

				.AddTransition("DASH_START_BACK to DASHJUMP_START", "DASHJUMP_START")
				.Predicator([&]()->_bool {return m_bJump; })
				.Duration(0.1f).Priority(1)

		.AddState("DASH_END_BACK")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_055_AL_dodge_B_stop"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			})

				.AddTransition("DASH_END_BACK to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("DASH_END_BACK to RUN_FRONT", "RUN_FRONT")
				.Predicator([&]()->_bool {return m_bWalk && (0.5f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_055_AL_dodge_B_stop") || CheckAnim("AS_ch0100_155_AL_dodge_B_stop")); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DASH_START_BACK to JUMP_START", "JUMP_START")
				.Predicator([&]()->_bool {return m_bJump; })
				.Duration(0.1f).Priority(1)

				.AddTransition("DASH_END_BACK to IDLE", "IDLE")
				.Duration(0.1f)
				.Priority(100)

		// 달리기
		.AddState("RUN_FRONT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_036_AL_dash"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ true, true, true, false, false, true, true, true, true, false });
			})

				.AddTransition("RUN_FRONT to DASHJUMP_FALL", "DASHJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("RUN_FRONT to ATTACK_DASH_START", "ATTACK_DASH_START")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f).Priority(0)

				.AddTransition("RUN_FRONT to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash && (CheckAnim("AS_ch0100_036_AL_dash") || CheckAnim("AS_ch0100_136_AL_dash")); })
				.Duration(0.1f).Priority(0)

				.AddTransition("RUN_FRONT to DASHJUMP_START", "DASHJUMP_START")
				.Predicator([&]()->_bool {return m_bJump; })
				.Duration(0.1f).Priority(1)

				.AddTransition("RUN_FRONT to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("RUN_FRONT to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(1)

				.AddTransition("RUN_FRONT to RUN_END", "RUN_END")
				.Predicator([&]()->_bool {return !m_bWalk; })
				.Duration(0.1f)
				.Priority(100)

		// 달리기 종료
		.AddState("RUN_END")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_038_AL_dash_stop"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			})

				.AddTransition("RUN_END to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("RUN_END to ATTACK_DASH_START", "ATTACK_DASH_START")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f).Priority(0)

				.AddTransition("RUN_END to WALK", "WALK")
				.Predicator([&]()->_bool {return m_bWalk; })
				.Duration(0.1f).Priority(0)

				.AddTransition("RUN_END to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash; })
				.Duration(0.1f).Priority(0)

				.AddTransition("RUN_END to JUMP_START", "JUMP_START")
				.Predicator([&]()->_bool {return m_bJump; })
				.Duration(0.1f).Priority(1)

				.AddTransition("RUN_END to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("RUN_END to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(1)

				.AddTransition("RUN_END to IDLE", "IDLE")
				.Duration(0.1f)
				.Priority(100)
#pragma endregion Dash And Run

#pragma region DashAttack

		.AddState("ATTACK_DASH_START")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_214_AL_atk_dash_start"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, true, false, true, true, false, true, false })
					.LookTarget();
			})

				.AddTransition("ATTACK_DASH_START to DASHJUMP_FALL", "DASHJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("ATTACK_DASH_START to ATTACK_DASH_END", "ATTACK_DASH_END")
				.Duration(0.1f).Priority(1)

		.AddState("ATTACK_DASH_END")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_214_AL_atk_dash_end"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, false, true, false });
			})

				.AddTransition("ATTACK_DASH_END to DASHJUMP_FALL", "DASHJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("ATTACK_DASH_END to ATK_A3", "ATK_A3")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.07f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_214_AL_atk_dash_end")); })
				.Duration(0.1f).Priority(1)

				.AddTransition("ATTACK_DASH_END to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash && (0.07f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_214_AL_atk_dash_end")); })
				.Duration(0.1f).Priority(0)

				.AddTransition("ATTACK_DASH_END to WALK", "WALK")
				.Predicator([&]()->_bool {return m_bWalk && (0.3f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_214_AL_atk_dash_end")); })
				.Duration(0.1f).Priority(0)

				.AddTransition("ATTACK_DASH_END to IDLE", "IDLE")
				.Duration(0.1f).Priority(1)

#pragma endregion DashAttack

#pragma region ChargeAttack

		.AddState("ATTACK_NONCHARGE")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_215_AL_atk_dash_hold"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, false, true, false });
			})


				.AddTransition("ATTACK_NONCHARGE to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("REPEAT", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && (0.15f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_215_AL_atk_dash_hold")) && (static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR)); })
				.Duration(0.1f).Priority(1)

				.AddTransition("ATTACK_NONCHARGE to WALK", "WALK")
				.Predicator([&]()->_bool {return m_bWalk && (0.3f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_215_AL_atk_dash_hold")); })
				.Duration(0.1f).Priority(1)

				.AddTransition("ATTACK_NONCHARGE to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash && (0.3f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_215_AL_atk_dash_hold")); })
				.Duration(0.1f).Priority(1)

				.AddTransition("ATTACK_NONCHARGE to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.15f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_215_AL_atk_dash_hold")); })
				.Duration(0.1f).Priority(1)

				.AddTransition("ATTACK_NONCHARGE to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && (0.15f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_215_AL_atk_dash_hold")) && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(2)

				.AddTransition("ATTACK_NONCHARGE to JUMP_START", "JUMP_START")
				.Predicator([&]()->_bool {return m_bJump && (0.3f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_215_AL_atk_dash_hold")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATTACK_NONCHARGE to IDLE", "IDLE")
				.Duration(0.1f).Priority(1)

		.AddState("ATTACK_CHARGE_START")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_210_AL_atk_charge"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, false, false, false });
			})

				.AddTransition("ATTACK_CHARGE_START to JUMP_FALL", "JUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("ATTACK_CHARGE_START to ATTACK_CHARGE_LOOP", "ATTACK_CHARGE_LOOP")
				.Duration(0.1f).Priority(1)

				.AddTransition("ATTACK_CHARGE_START to CHARGE_CANCEL", "CHARGE_CANCEL")
				.Predicator([&]() { return !m_bCharge; })
				.Duration(0.1f).Priority(100)


		.AddState("ATTACK_CHARGE_LOOP")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_210_AL_atk_charge_loop"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, false, false, false });
			})

				 .AddTransition("ATTACK_CHARGE_LOOP to JUMP_FALL", "JUMP_FALL")
				 .Predicator([&]()->_bool{return FloorCheck();})
				 .Duration(0.1f).Priority(0)
				
				 .AddTransition("ATTACK_CHARGE_LOOP to CHARGE_ATTACK_01", "CHARGE_ATTACK_01")
				 .Predicator([&]() { return (static_cast<CPlayer*>(m_pTargetObject)->Charge(0, 0.5f)) && (!m_bCharge); })
				 .Duration(0.1f).Priority(2)
				
				 .AddTransition("ATTACK_CHARGE_LOOP to CHARGE_ATTACK_02", "CHARGE_ATTACK_02")
				 .Predicator([&]() { return (static_cast<CPlayer*>(m_pTargetObject)->Charge(1, 1.f)) && (!m_bCharge); })
				 .Duration(0.1f).Priority(1)
				
				 .AddTransition("ATTACK_CHARGE_LOOP to CHARGE_ATTACK_03", "CHARGE_ATTACK_03")
				 .Predicator([&]() { return (static_cast<CPlayer*>(m_pTargetObject)->Charge(2, 1.5f)) && (!m_bCharge); })
				 .Duration(0.1f).Priority(0)
				
				 .AddTransition("ATTACK_CHARGE_LOOP to CHARGE_CANCEL", "CHARGE_CANCEL")
				 .Predicator([&]() { return !m_bCharge; })
				 .Duration(0.1f).Priority(100)

		.AddState("CHARGE_CANCEL")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_210_AL_atk_charge_cancel"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, false, true, false });
			})

				.AddTransition("CHARGE_CANCEL to JUMP_FALL", "JUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("CHARGE_CANCEL to WALK", "WALK")
				.Predicator([&]()->_bool {return m_bWalk && (0.6f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_210_AL_atk_charge_cancel")); })
				.Duration(0.1f).Priority(100)

				.AddTransition("CHARGE_CANCEL to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash && (0.2f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_210_AL_atk_charge_cancel")); })
				.Duration(0.1f).Priority(1)

				.AddTransition("CHARGE_CANCEL to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.2f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_210_AL_atk_charge_cancel")); })
				.Duration(0.1f).Priority(1)

				.AddTransition("CHARGE_CANCEL to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && (0.2f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_210_AL_atk_charge_cancel")) && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("CHARGE_CANCEL to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && (0.2f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_210_AL_atk_charge_cancel")) && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(2)

				.AddTransition("CHARGE_CANCEL to JUMP_START", "JUMP_START")
				.Predicator([&]()->_bool {return m_bJump && (0.4f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_210_AL_atk_charge_cancel")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)
			
				.AddTransition("CHARGE_CANCEL to IDLE", "IDLE")
				.Duration(0.1f).Priority(1)

		.AddState("CHARGE_ATTACK_01")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_211_AL_atk_charge01"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, false, true, false });

				CGameInstance::GetInstance()->SetCameraFovCurve("Charge01_ActionCam");
			})
			.FinishEvent([&]()
			{
				CGameInstance::GetInstance()->ReleaseCameraFovCurve();
			})

				.AddTransition("CHARGE_ATTACK_01 to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("CHARGE_ATTACK_01 to WALK", "WALK")
				.Predicator([&]()->_bool {return m_bWalk && (0.5f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_211_AL_atk_charge01")); })
				.Duration(0.1f).Priority(100)

				.AddTransition("CHARGE_ATTACK_01 to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash && (0.5f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_211_AL_atk_charge01")); })
				.Duration(0.1f).Priority(1)

				.AddTransition("CHARGE_ATTACK_01 to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.5f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_211_AL_atk_charge01")); })
				.Duration(0.1f).Priority(1)

				.AddTransition("CHARGE_ATTACK_01 to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && (0.5f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_211_AL_atk_charge01")) && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("CHARGE_ATTACK_01 to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && (0.5f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_211_AL_atk_charge01")) && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(2)

				.AddTransition("CHARGE_ATTACK_01 to JUMP_START", "JUMP_START")
				.Predicator([&]()->_bool {return m_bJump && (0.5f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_211_AL_atk_charge01")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("CHARGE_ATTACK_01 to IDLE", "IDLE")
				.Duration(0.1f).Priority(1)

		.AddState("CHARGE_ATTACK_02")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_212_AL_atk_charge02"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, false, true, false });
			})

				.AddTransition("CHARGE_ATTACK_02 to JUMP_FALL", "JUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("CHARGE_ATTACK_02 to WALK", "WALK")
				.Predicator([&]()->_bool {return m_bWalk && (0.55f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_212_AL_atk_charge02")); })
				.Duration(0.1f).Priority(100)

				.AddTransition("CHARGE_ATTACK_02 to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash && (0.55f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_212_AL_atk_charge02")); })
				.Duration(0.1f).Priority(1)

				.AddTransition("CHARGE_ATTACK_02 to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.55f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_212_AL_atk_charge02")); })
				.Duration(0.1f).Priority(1)

				.AddTransition("CHARGE_ATTACK_02 to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && (0.55f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_212_AL_atk_charge02")) && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("CHARGE_ATTACK_02 to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && (0.55f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_212_AL_atk_charge02")) && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(2)

				.AddTransition("CHARGE_ATTACK_02 to JUMP_START", "JUMP_START")
				.Predicator([&]()->_bool {return m_bJump && (0.55f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_212_AL_atk_charge02")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("CHARGE_ATTACK_02 to IDLE", "IDLE")
				.Duration(0.1f).Priority(1)

		.AddState("CHARGE_ATTACK_03")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_213_AL_atk_charge03"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, false, true, false });
			})

				.AddTransition("CHARGE_ATTACK_03 to JUMP_FALL", "JUMP_FALL")
				.Predicator([&]()->_bool{return FloorCheck();})
				.Duration(0.1f).Priority(0)

				.AddTransition("CHARGE_ATTACK_03 to WALK", "WALK")
				.Predicator([&]()->_bool {return m_bWalk && (0.6f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_213_AL_atk_charge03")); })
				.Duration(0.1f).Priority(100)

				.AddTransition("CHARGE_ATTACK_03 to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash && (0.6f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_213_AL_atk_charge03")); })
				.Duration(0.1f).Priority(1)

				.AddTransition("CHARGE_ATTACK_03 to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.6f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_213_AL_atk_charge03")); })
				.Duration(0.1f).Priority(1)

				.AddTransition("CHARGE_ATTACK_03 to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && (0.6f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_213_AL_atk_charge03")) && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("CHARGE_ATTACK_03 to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && (0.6f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_213_AL_atk_charge03")) && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(2)

				.AddTransition("CHARGE_ATTACK_03 to JUMP_START", "JUMP_START")
				.Predicator([&]()->_bool {return m_bJump && (0.6f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_213_AL_atk_charge03")) && (!m_bLerp); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("CHARGE_ATTACK_03 to IDLE", "IDLE")
				.Duration(0.1f).Priority(1)

		// 차지 취소, 차지 1단 공격, 차지 2단 공격


#pragma endregion ChargeAttack

#pragma region Jump

		.AddState("JUMP_START")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_041_AL_jump_start"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			})
			.FinishEvent([&]() 
			{
				static_cast<CPlayer*>(m_pTargetObject)->Jump(); 
			})

				.AddTransition("JUMP_START to JUMP_RISE", "JUMP_RISE")
				.Duration(0.1f)
				.Priority(100)

		.AddState("JUMP_RISE")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_041_AL_jump_rise"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
				->SetAbleState({ false, true, false, false, true, true, false, false, false, false });
			})

				.AddTransition("JUMP_RISE to JUMP_LANDING", "JUMP_LANDING")
				.Predicator([&]()->_bool {return m_bOnFloor && (CheckAnim("AS_ch0100_041_AL_jump_rise") || CheckAnim("AS_ch0100_141_AL_jump_rise")); })
				.Duration(0.f)
				.Priority(100)

				.AddTransition("JUMP_RISE to AIR_DODGE", "AIR_DODGE")
				.Predicator([&]()->_bool {return m_bDash && static_cast<CPlayer*>(m_pTargetObject)->UseMoveCnt(CPlayer::LIMIT_AIRDODGE); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("JUMP_RISE to ATK_AIR1", "ATK_AIR1")
				.Predicator([&]()->_bool {return m_bLeftClick && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_AIRATK01); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("JUMP_RISE to ATK_AIR_NONCHARGE", "ATK_AIR_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_AIR); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("JUMP_RISE to ATK_AIR_CHARGE_START", "ATK_AIR_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("JUMP_RISE to JUMP_FALL", "JUMP_FALL")
				.Duration(0.1f)
				.Priority(100)

		.AddState("JUMP_FALL")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_041_AL_jump_fall"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
				->SetAbleState({ false, true, false, false, true, true, false, false, false, false });
			})

				.AddTransition("JUMP_FALL to DOUBLE_JUMP_RISE", "DOUBLE_JUMP_RISE")
				.Predicator([&]()->_bool {return m_bJump && static_cast<CPlayer*>(m_pTargetObject)->UseMoveCnt(CPlayer::LIMIT_DOUBLEJUMP); })
				.Duration(0.0f)
				.Priority(100)

				.AddTransition("JUMP_FALL to AIR_DODGE", "AIR_DODGE")
				.Predicator([&]()->_bool {return m_bDash && static_cast<CPlayer*>(m_pTargetObject)->UseMoveCnt(CPlayer::LIMIT_AIRDODGE); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("JUMP_FALL to ATK_AIR1", "ATK_AIR1")
				.Predicator([&]()->_bool {return m_bLeftClick && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_AIRATK01); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("JUMP_FALL to ATK_AIR_NONCHARGE", "ATK_AIR_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_AIR); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("JUMP_FALL to ATK_AIR_CHARGE_START", "ATK_AIR_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("JUMP_FALL to JUMP_LANDING", "JUMP_LANDING")
				.Predicator([&]()->_bool {return m_bOnFloor; })
				.Duration(0.f)
				.Priority(100)

		.AddState("JUMP_LANDING")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_041_AL_jump_landing"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			})

				.AddTransition("JUMP_LANDING to WALK_LOOP", "WALK_LOOP")
				.Predicator([&]()->_bool {return m_bWalk && (CheckAnim("AS_ch0100_041_AL_jump_landing") || CheckAnim("AS_ch0100_141_AL_jump_landing")) && (0.1f <= m_fPlayRatio); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("JUMP_LANDING to JUMP_START", "JUMP_START")
				.Predicator([&]()->_bool {return m_bJump && (CheckAnim("AS_ch0100_041_AL_jump_landing") || CheckAnim("AS_ch0100_141_AL_jump_landing")) && (0.1f <= m_fPlayRatio); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("JUMP_LANDING to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick && (CheckAnim("AS_ch0100_041_AL_jump_landing") || CheckAnim("AS_ch0100_141_AL_jump_landing")) && (0.1f <= m_fPlayRatio); })
				.Duration(0.1f).Priority(1)

				.AddTransition("JUMP_LANDING to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && (CheckAnim("AS_ch0100_041_AL_jump_landing") || CheckAnim("AS_ch0100_141_AL_jump_landing")) && (0.1f <= m_fPlayRatio) && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("JUMP_LANDING to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && (CheckAnim("AS_ch0100_041_AL_jump_landing") || CheckAnim("AS_ch0100_141_AL_jump_landing")) && (0.1f <= m_fPlayRatio) && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(1)

				.AddTransition("JUMP_LANDING to IDLE", "IDLE")
				.Duration(0.1f)
				.Priority(100)

		.AddState("DOUBLE_JUMP_RISE")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_043_AL_doublejump_rise"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, true, false, false, true, true, false, true, true, false })
					.Jump();
			})

				.AddTransition("DOUBLE_JUMP_RISE to JUMP_LANDING", "JUMP_LANDING")
				.Predicator([&]()->_bool {return m_bOnFloor && (CheckAnim("AS_ch0100_043_AL_doublejump_rise") || CheckAnim("AS_ch0100_043_AL_doublejump_rise")); })
				.Duration(0.f)
				.Priority(100)

				.AddTransition("DOUBLE_JUMP_RISE to AIR_DODGE", "AIR_DODGE")
				.Predicator([&]()->_bool {return m_bDash && static_cast<CPlayer*>(m_pTargetObject)->UseMoveCnt(CPlayer::LIMIT_AIRDODGE); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DOUBLE_JUMP_RISE to ATK_AIR1", "ATK_AIR1")
				.Predicator([&]()->_bool {return m_bLeftClick && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_AIRATK01); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DOUBLE_JUMP_RISE to ATK_AIR_NONCHARGE", "ATK_AIR_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_AIR); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DOUBLE_JUMP_RISE to ATK_AIR_CHARGE_START", "ATK_AIR_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("DOUBLE_JUMP_RISE to JUMP_FALL", "JUMP_FALL")
				.Duration(0.1f)
				.Priority(100)

#pragma endregion Jump

#pragma region RunJump

		.AddState("RUNJUMP_START")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_044_AL_runjump_start"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, true, true, false })
					.LookAtDir(m_vMoveDir);
			})
			.FinishEvent([&]() 
			{
				static_cast<CPlayer*>(m_pTargetObject)->Jump(); 
			})

				.AddTransition("RUNJUMP_START to RUNJUMP_RISE", "RUNJUMP_RISE")
				.Duration(0.1f)
				.Priority(100)

		.AddState("RUNJUMP_RISE")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_044_AL_runjump_rise"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, true, false, false, true, true, false, false, false, false });
			})

				.AddTransition("RUNJUMP_RISE to RUNJUMP_LANDING", "RUNJUMP_LANDING")
				.Predicator([&]()->_bool {return m_bOnFloor && (CheckAnim("AS_ch0100_044_AL_runjump_rise") || CheckAnim("AS_ch0100_144_AL_runjump_rise")); })
				.Duration(0.f)
				.Priority(100)

				.AddTransition("RUNJUMP_RISE to AIR_DODGE", "AIR_DODGE")
				.Predicator([&]()->_bool {return m_bDash && static_cast<CPlayer*>(m_pTargetObject)->UseMoveCnt(CPlayer::LIMIT_AIRDODGE); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("RUNJUMP_RISE to ATK_AIR1", "ATK_AIR1")
				.Predicator([&]()->_bool {return m_bLeftClick && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_AIRATK01); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("RUNJUMP_RISE to ATK_AIR_NONCHARGE", "ATK_AIR_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_AIR); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("RUNJUMP_RISE to ATK_AIR_CHARGE_START", "ATK_AIR_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("RUNJUMP_RISE to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Duration(0.1f)
				.Priority(100)

		.AddState("RUNJUMP_FALL")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_044_AL_runjump_fall"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, true, false, false, true, true, false, false, false, false });
			})

				.AddTransition("RUNJUMP_FALL to DOUBLE_RUNJUMP_RISE", "DOUBLE_RUNJUMP_RISE")
				.Predicator([&]()->_bool {return m_bJump && static_cast<CPlayer*>(m_pTargetObject)->UseMoveCnt(CPlayer::LIMIT_DOUBLEJUMP); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("RUNJUMP_FALL to AIR_DODGE", "AIR_DODGE")
				.Predicator([&]()->_bool {return m_bDash && static_cast<CPlayer*>(m_pTargetObject)->UseMoveCnt(CPlayer::LIMIT_AIRDODGE); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("RUNJUMP_FALL to ATK_AIR1", "ATK_AIR1")
				.Predicator([&]()->_bool {return m_bLeftClick && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_AIRATK01); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("RUNJUMP_FALL to ATK_AIR_NONCHARGE", "ATK_AIR_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_AIR); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("RUNJUMP_FALL to ATK_AIR_CHARGE_START", "ATK_AIR_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("RUNJUMP_FALL to RUNJUMP_LANDING", "RUNJUMP_LANDING")
				.Predicator([&]()->_bool {return m_bOnFloor; })
				.Duration(0.f)
				.Priority(100)

		.AddState("RUNJUMP_LANDING")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_044_AL_runjump_landing"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			})

				.AddTransition("RUNJUMP_LANDING to WALK_LOOP", "WALK_LOOP")
				.Predicator([&]()->_bool {return m_bWalk && (CheckAnim("AS_ch0100_044_AL_runjump_landing") || CheckAnim("AS_ch0100_144_AL_runjump_landing")) && (0.1f <= m_fPlayRatio); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("RUNJUMP_LANDING to JUMP_START", "JUMP_START")
				.Predicator([&]()->_bool {return m_bJump && (CheckAnim("AS_ch0100_044_AL_runjump_landing") || CheckAnim("AS_ch0100_144_AL_runjump_landing")) && (0.1f <= m_fPlayRatio); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("RUNJUMP_LANDING to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick && (CheckAnim("AS_ch0100_044_AL_runjump_landing") || CheckAnim("AS_ch0100_144_AL_runjump_landing")) && (0.1f <= m_fPlayRatio); })
				.Duration(0.1f).Priority(1)

				.AddTransition("RUNJUMP_LANDING to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && (CheckAnim("AS_ch0100_044_AL_runjump_landing") || CheckAnim("AS_ch0100_144_AL_runjump_landing")) && (0.1f <= m_fPlayRatio) && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("RUNJUMP_LANDING to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && (CheckAnim("AS_ch0100_044_AL_runjump_landing") || CheckAnim("AS_ch0100_144_AL_runjump_landing")) && (0.1f <= m_fPlayRatio) && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(1)

				.AddTransition("RUNJUMP_LANDING to IDLE", "IDLE")
				.Duration(0.1f)
				.Priority(100)

		.AddState("DOUBLE_RUNJUMP_RISE")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_043_AL_doublejump_rise"))
			.StartEvent([&]()
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, true, false, false, true, true, false, true, true, false })
					.Jump();
			})

				.AddTransition("DOUBLE_RUNJUMP_RISE to RUNJUMP_LANDING", "RUNJUMP_LANDING")
				.Predicator([&]()->_bool {return m_bOnFloor && (CheckAnim("AS_ch0100_043_AL_doublejump_rise") || CheckAnim("AS_ch0100_143_AL_doublejump_rise")); })
				.Duration(0.f)
				.Priority(100)

				.AddTransition("DOUBLE_RUNJUMP_RISE to AIR_DODGE", "AIR_DODGE")
				.Predicator([&]()->_bool {return m_bDash && static_cast<CPlayer*>(m_pTargetObject)->UseMoveCnt(CPlayer::LIMIT_AIRDODGE); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DOUBLE_RUNJUMP_RISE to ATK_AIR1", "ATK_AIR1")
				.Predicator([&]()->_bool {return m_bLeftClick && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_AIRATK01); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DOUBLE_RUNJUMP_RISE to ATK_AIR_NONCHARGE", "ATK_AIR_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_AIR); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DOUBLE_RUNJUMP_RISE to ATK_AIR_CHARGE_START", "ATK_AIR_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("DOUBLE_RUNJUMP_RISE to RUNJUMP_FALL", "RUNJUMP_FALL")
				.Duration(0.1f)
				.Priority(100)

#pragma endregion RunJump

#pragma region DashJump

		.AddState("DASHJUMP_START")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_042_AL_dashjump_start"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, true, true, false })
					.LookAtDir(m_vMoveDir);
			})
			.FinishEvent([&]() 
			{
				static_cast<CPlayer*>(m_pTargetObject)->Jump(); 
			})

				.AddTransition("DASHJUMP_START to DASHJUMP_RISE", "DASHJUMP_RISE")
				.Duration(0.1f)
				.Priority(100)

		.AddState("DASHJUMP_RISE")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_042_AL_dashjump_rise"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, true, true, false, true, true, false, false, false, false });
			})

				.AddTransition("DASHJUMP_RISE to DASHJUMP_LANDING", "DASHJUMP_LANDING")
				.Predicator([&]()->_bool {return m_bOnFloor && (CheckAnim("AS_ch0100_042_AL_dashjump_rise") || CheckAnim("AS_ch0100_142_AL_dashjump_rise")); })
				.Duration(0.f)
				.Priority(100)

				.AddTransition("DASHJUMP_RISE to AIR_DODGE", "AIR_DODGE")
				.Predicator([&]()->_bool {return m_bDash && static_cast<CPlayer*>(m_pTargetObject)->UseMoveCnt(CPlayer::LIMIT_AIRDODGE); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DASHJUMP_RISE to ATK_AIR1", "ATK_AIR1")
				.Predicator([&]()->_bool {return m_bLeftClick && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_AIRATK01); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DASHJUMP_RISE to ATK_AIR_NONCHARGE", "ATK_AIR_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_AIR); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DASHJUMP_RISE to ATK_AIR_CHARGE_START", "ATK_AIR_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("DASHJUMP_RISE to DASHJUMP_FALL", "DASHJUMP_FALL")
				.Duration(0.1f)
				.Priority(100)

		.AddState("DASHJUMP_FALL")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_042_AL_dashjump_fall"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, true, true, false, true, true, false, false, false, false });
			})

				.AddTransition("DASHJUMP_FALL to DOUBLE_DASHJUMP_RISE", "DOUBLE_DASHJUMP_RISE")
				.Predicator([&]()->_bool {return m_bJump && static_cast<CPlayer*>(m_pTargetObject)->UseMoveCnt(CPlayer::LIMIT_DOUBLEJUMP); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DASHJUMP_FALL to AIR_DODGE", "AIR_DODGE")
				.Predicator([&]()->_bool {return m_bDash && static_cast<CPlayer*>(m_pTargetObject)->UseMoveCnt(CPlayer::LIMIT_AIRDODGE); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DASHJUMP_FALL to ATK_AIR1", "ATK_AIR1")
				.Predicator([&]()->_bool {return m_bLeftClick && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_AIRATK01); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DASHJUMP_FALL to ATK_AIR_NONCHARGE", "ATK_AIR_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_AIR); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DASHJUMP_FALL to ATK_AIR_CHARGE_START", "ATK_AIR_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("DASHJUMP_FALL to DASHJUMP_LANDING", "DASHJUMP_LANDING")
				.Predicator([&]()->_bool {return m_bOnFloor; })
				.Duration(0.f)
				.Priority(100)

		.AddState("DASHJUMP_LANDING")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_042_AL_dashjump_landing"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			})

				.AddTransition("DASHJUMP_LANDING to RUN_FRONT", "RUN_FRONT")
				.Predicator([&]()->_bool {return m_bWalk && (CheckAnim("AS_ch0100_042_AL_dashjump_landing") || CheckAnim("AS_ch0100_142_AL_dashjump_landing")) && (0.1f <= m_fPlayRatio); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("DASHJUMP_LANDING to JUMP_START", "JUMP_START")
				.Predicator([&]()->_bool {return m_bJump && (CheckAnim("AS_ch0100_042_AL_dashjump_landing") || CheckAnim("AS_ch0100_142_AL_dashjump_landing")) && (0.1f <= m_fPlayRatio); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("DASHJUMP_LANDING to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick && (CheckAnim("AS_ch0100_042_AL_dashjump_landing") || CheckAnim("AS_ch0100_142_AL_dashjump_landing")) && (0.1f <= m_fPlayRatio); })
				.Duration(0.1f).Priority(1)

				.AddTransition("DASHJUMP_LANDING to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && (CheckAnim("AS_ch0100_042_AL_dashjump_landing") || CheckAnim("AS_ch0100_142_AL_dashjump_landing")) && (0.1f <= m_fPlayRatio) && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("DASHJUMP_LANDING to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && (CheckAnim("AS_ch0100_042_AL_dashjump_landing") || CheckAnim("AS_ch0100_142_AL_dashjump_landing")) && (0.1f <= m_fPlayRatio) && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(1)

				.AddTransition("DASHJUMP_LANDING to IDLE", "IDLE")
				.Duration(0.1f)
				.Priority(100)

		.AddState("DOUBLE_DASHJUMP_RISE")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_043_AL_doublejump_rise"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, true, true, false, true, true, false, true, true, false })
					.Jump();
			})

				.AddTransition("DOUBLE_DASHJUMP_RISE to DASHJUMP_LANDING", "DASHJUMP_LANDING")
				.Predicator([&]()->_bool {return m_bOnFloor && (CheckAnim("AS_ch0100_043_AL_doublejump_rise") || CheckAnim("AS_ch0100_143_AL_doublejump_rise")); })
				.Duration(0.f)
				.Priority(100)

				.AddTransition("DOUBLE_DASHJUMP_RISE to AIR_DODGE", "AIR_DODGE")
				.Predicator([&]()->_bool {return m_bDash && static_cast<CPlayer*>(m_pTargetObject)->UseMoveCnt(CPlayer::LIMIT_AIRDODGE); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DOUBLE_DASHJUMP_RISE to ATK_AIR1", "ATK_AIR1")
				.Predicator([&]()->_bool {return m_bLeftClick && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_AIRATK01); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DOUBLE_DASHJUMP_RISE to ATK_AIR_NONCHARGE", "ATK_AIR_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_AIR); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("DOUBLE_DASHJUMP_RISE to ATK_AIR_CHARGE_START", "ATK_AIR_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("DOUBLE_DASHJUMP_RISE to DASHJUMP_FALL", "DASHJUMP_FALL")
				.Duration(0.1f)
				.Priority(100)

#pragma endregion DashJump 

#pragma region JumpAction

		.AddState("ATK_AIR1")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_221_AL_atk_air1"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, true, false, false, false, true, false })
					.LookTarget();
			})

				.AddTransition("ATK_AIR1 to ATK_AIR2", "ATK_AIR2")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.3f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_221_AL_atk_air1")) && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_AIRATK02); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_AIR1 to DOUBLE_JUMP_RISE", "DOUBLE_JUMP_RISE")
				.Predicator([&]()->_bool {return m_bJump && static_cast<CPlayer*>(m_pTargetObject)->UseMoveCnt(CPlayer::LIMIT_DOUBLEJUMP); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("ATK_AIR1 to AIR_DODGE", "AIR_DODGE")
				.Predicator([&]()->_bool {return m_bDash && static_cast<CPlayer*>(m_pTargetObject)->UseMoveCnt(CPlayer::LIMIT_AIRDODGE); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("ATK_AIR1 to JUMP_LANDING", "JUMP_LANDING")
				.Predicator([&]()->_bool {return m_bOnFloor; })
				.Duration(0.f)
				.Priority(100)

				.AddTransition("ATK_AIR1 to JUMP_FALL", "JUMP_FALL")
				.Predicator([&]()->_bool {return (0.7f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_221_AL_atk_air1")); })
				.Duration(0.1f)
				.Priority(100)

		.AddState("ATK_AIR2")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_222_AL_atk_air2"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, true, false, false, false, true, false })
					.LookTarget();
			})

				.AddTransition("ATK_AIR2 to JUMP_LANDING", "JUMP_LANDING")
				.Predicator([&]()->_bool {return m_bOnFloor; })
				.Duration(0.f)
				.Priority(100)

				.AddTransition("ATK_AIR2 to JUMP_FALL", "JUMP_FALL")
				.Predicator([&]()->_bool {return (0.7f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_222_AL_atk_air2")); })
				.Duration(0.1f)
				.Priority(100)

		.AddState("ATK_AIR_NONCHARGE")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_229_AL_atk_air_dash_hold"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, true, false, false, false, true, false });
			})

				.AddTransition("REPEAT", "ATK_AIR_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && (0.2f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_229_AL_atk_air_dash_hold")) && (static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_AIR)); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_AIR_NONCHARGE to JUMP_LANDING", "JUMP_LANDING")
				.Predicator([&]()->_bool {return m_bOnFloor; })
				.Duration(0.f)
				.Priority(100)

				.AddTransition("ATK_AIR_NONCHARGE to JUMP_FALL", "JUMP_FALL")
				.Predicator([&]()->_bool {return (0.5f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_229_AL_atk_air_dash_hold")); })
				.Duration(0.1f)
				.Priority(100)

		.AddState("ATK_AIR_CHARGE_START")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_227_AL_atk_air_hold_start"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, true, false, true, true, true, false });
			})

				.AddTransition("ATK_AIR_CHARGE_START to ATK_AIR_CHARGE_FALL", "ATK_AIR_CHARGE_FALL")
				.Duration(0.1f)
				.Priority(0)

		.AddState("ATK_AIR_CHARGE_FALL")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_227_AL_atk_air_hold_fall"))
			.StartEvent([&]()
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, true, true, true, true, true, false })
					.SetGravity_Optional(-20.f);
			})

				.AddTransition("ATK_AIR_CHARGE_FALL to ATK_AIR_CHARGE_LANDING", "ATK_AIR_CHARGE_LANDING")
				.Predicator([&]()->_bool {return m_bOnFloor; })
				.Duration(0.f)
				.Priority(0)

		.AddState("ATK_AIR_CHARGE_LANDING")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_227_AL_atk_air_hold_landing"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			})

				.AddTransition("ATK_AIR_CHARGE_LANDING to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick && (CheckAnim("AS_ch0100_227_AL_atk_air_hold_landing")) && (0.5f <= m_fPlayRatio); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_AIR_CHARGE_LANDING to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash && (CheckAnim("AS_ch0100_227_AL_atk_air_hold_landing")) && (0.5f <= m_fPlayRatio); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_AIR_CHARGE_LANDING to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && (0.5f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_227_AL_atk_air_hold_landing")) && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("ATK_AIR_CHARGE_LANDING to IDLE", "IDLE")
				.Duration(0.1f)
				.Priority(0)

		.AddState("ATK_AIR_DODGE_START")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_230_AL_atk_air_dash_start"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, true, false, false, false, true, false })
					.LookTarget();
			})

				.AddTransition("ATK_AIR_DODGE_START to ATK_AIR_DODGE_END", "ATK_AIR_DODGE_END")
				.Duration(0.1f)
				.Priority(100)

		.AddState("ATK_AIR_DODGE_END")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_230_AL_atk_air_dash_end"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, true, false, false, false, true, false });
			})

				.AddTransition("ATK_AIR_DODGE_END to JUMP_LANDING", "JUMP_LANDING")
				.Predicator([&]()->_bool {return m_bOnFloor; })
				.Duration(0.f)
				.Priority(100)

				.AddTransition("ATK_AIR_DODGE_END to ATK_AIR1", "ATK_AIR1")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.5f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_230_AL_atk_air_dash_end")) && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_AIRATK01); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATK_AIR_DODGE_END to JUMP_FALL", "JUMP_FALL")
				.Predicator([&]()->_bool {return (0.7f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_230_AL_atk_air_dash_end")); })
				.Duration(0.1f)
				.Priority(100)

#pragma endregion JumpAction

#pragma region AirDodge

			.AddState("AIR_DODGE")
				.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_061_AL_air_dodge_F"))
				.StartEvent([&]() 
				{
					static_cast<CPlayer*>(m_pTargetObject)
						->SetAbleState({ false, false, false, false, true, false, false, true, true, false }); 
				})

					.AddTransition("AIR_DODGE to AIR_DODGE_FRONT", "AIR_DODGE_FRONT")
					.Predicator([&]()->_bool {return m_eMoveDir == CPlayer::DIR_F || true; })
					.Duration(0.1f)
					.Priority(100)
				
					.AddTransition("AIR_DODGE to AIR_DODGE_BACK", "AIR_DODGE_BACK")
					.Predicator([&]()->_bool {return m_eMoveDir == CPlayer::DIR_B; })
					.Duration(0.1f)
					.Priority(0)

					.AddTransition("AIR_DODGE to AIR_DODGE_LEFT", "AIR_DODGE_LEFT")
					.Predicator([&]()->_bool {return m_eMoveDir == CPlayer::DIR_L; })
					.Duration(0.1f)
					.Priority(1)

					.AddTransition("AIR_DODGE to AIR_DODGE_RIGHT", "AIR_DODGE_RIGHT")
					.Predicator([&]()->_bool {return m_eMoveDir == CPlayer::DIR_R; })
					.Duration(0.1f)
					.Priority(2)

			.AddState("AIR_DODGE_FRONT")
				.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_061_AL_air_dodge_F"))
				.StartEvent([&]() 
				{
					static_cast<CPlayer*>(m_pTargetObject)
						->SetAbleState({ false, false, false, false, true, false, false, true, true, false }); 
				})
			
					.AddTransition("AIR_DODGE_FRONT to ATK_AIR_DODGE_START", "ATK_AIR_DODGE_START")
					.Predicator([&]()->_bool {return m_bLeftClick && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_AIRDODGEATK); })
					.Duration(0.1f)
					.Priority(100)

					.AddTransition("AIR_DODGE_FRONT to JUMP_FALL", "JUMP_FALL")
					.Duration(0.1f)
					.Priority(100)

			.AddState("AIR_DODGE_BACK")
				.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_065_AL_air_dodge_B"))
				.StartEvent([&]() 
				{
					static_cast<CPlayer*>(m_pTargetObject)
						->SetAbleState({ false, false, false, false, true, false, false, true, true, false }); 
				})

					.AddTransition("AIR_DODGE_BACK to ATK_AIR_DODGE_START", "ATK_AIR_DODGE_START")
					.Predicator([&]()->_bool {return m_bLeftClick && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_AIRDODGEATK); })
					.Duration(0.1f)
					.Priority(100)

					.AddTransition("AIR_DODGE_BACK to JUMP_FALL", "JUMP_FALL")
					.Duration(0.1f)
					.Priority(100)

			.AddState("AIR_DODGE_LEFT")
				.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_067_AL_air_dodge_L"))
				.StartEvent([&]() 
				{
					static_cast<CPlayer*>(m_pTargetObject)
						->SetAbleState({ false, false, false, false, true, false, false, true, true, false }); 
				})

					.AddTransition("AIR_DODGE_LEFT to ATK_AIR_DODGE_START", "ATK_AIR_DODGE_START")
					.Predicator([&]()->_bool {return m_bLeftClick && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_AIRDODGEATK); })
					.Duration(0.1f)
					.Priority(100)
	
					.AddTransition("AIR_DODGE_LEFT to JUMP_FALL", "JUMP_FALL")
					.Duration(0.1f)
					.Priority(100)

			.AddState("AIR_DODGE_RIGHT")
				.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_063_AL_air_dodge_R"))
				.StartEvent([&]() 
				{
					static_cast<CPlayer*>(m_pTargetObject)
						->SetAbleState({ false, false, false, false, true, false, false, true, true, false }); 
				})

					.AddTransition("AIR_DODGE_RIGHT to ATK_AIR_DODGE_START", "ATK_AIR_DODGE_START")
					.Predicator([&]()->_bool {return m_bLeftClick && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_AIRDODGEATK); })
					.Duration(0.1f)
					.Priority(100)

					.AddTransition("AIR_DODGE_RIGHT to JUMP_FALL", "JUMP_FALL")
					.Duration(0.1f)
					.Priority(100)

#pragma endregion AirDodge

#pragma region Upper

		.AddState("ATTACK_UPPER_START")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_206_AL_atk_upper"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, true, true, true, true, true, false })
					.LookTarget();
			})

				.AddTransition("ATTACK_UPPER_START to ATK_AIR1", "ATK_AIR1")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.3f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_206_AL_atk_upper")) && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_AIRATK01); })
				.Duration(0.1f).Priority(0)

				.AddTransition("ATTACK_UPPER_START to AIR_DODGE", "AIR_DODGE")
				.Predicator([&]()->_bool {return m_bDash && (0.3f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_206_AL_atk_upper")) && static_cast<CPlayer*>(m_pTargetObject)->UseMoveCnt(CPlayer::LIMIT_AIRDODGE); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("ATTACK_UPPER_START to ATK_AIR_NONCHARGE", "ATK_AIR_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && (0.3f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_206_AL_atk_upper")) && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_AIR); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("ATTACK_UPPER_START to ATK_AIR_CHARGE_START", "ATK_AIR_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("ATTACK_UPPER_START to DOUBLE_JUMP_RISE", "DOUBLE_JUMP_RISE")
				.Predicator([&]()->_bool {return m_bJump && (0.3f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_206_AL_atk_upper")) && static_cast<CPlayer*>(m_pTargetObject)->UseMoveCnt(CPlayer::LIMIT_DOUBLEJUMP); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("ATTACK_UPPER_START to ATTACK_UPPER_FALL", "ATTACK_UPPER_FALL")
				.Duration(0.1f).Priority(0)

		.AddState("ATTACK_UPPER_FALL")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_206_AL_atk_upper_fall"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, true, false, false, true, true, false, false, true, false });
			})

				.AddTransition("ATTACK_UPPER_FALL to ATK_AIR1", "ATK_AIR1")
				.Predicator([&]()->_bool {return m_bLeftClick && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_AIRATK01); })
				.Duration(0.1f).Priority(0)

				.AddTransition("ATTACK_UPPER_FALL to AIR_DODGE", "AIR_DODGE")
				.Predicator([&]()->_bool {return m_bDash && static_cast<CPlayer*>(m_pTargetObject)->UseMoveCnt(CPlayer::LIMIT_AIRDODGE); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("ATTACK_UPPER_FALL to ATK_AIR_NONCHARGE", "ATK_AIR_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_AIR); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("ATTACK_UPPER_FALL to ATK_AIR_CHARGE_START", "ATK_AIR_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f)
				.Priority(1)

				.AddTransition("ATTACK_UPPER_FALL to DOUBLE_JUMP_RISE", "DOUBLE_JUMP_RISE")
				.Predicator([&]()->_bool {return m_bJump && static_cast<CPlayer*>(m_pTargetObject)->UseMoveCnt(CPlayer::LIMIT_DOUBLEJUMP); })
				.Duration(0.05f)
				.Priority(100)

				.AddTransition("ATTACK_UPPER_FALL to ATTACK_UPPER_LANDING", "ATTACK_UPPER_LANDING")
				.Predicator([&]()->_bool {return m_bOnFloor; })
				.Duration(0.f)
				.Priority(100)

		.AddState("ATTACK_UPPER_LANDING")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_206_AL_atk_upper_landing"))
			.StartEvent([&]() 
			{ 
				static_cast<CPlayer*>(m_pTargetObject)
					->SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			})

				.AddTransition("ATTACK_UPPER_LANDING to WALK_LOOP", "WALK_LOOP")
				.Predicator([&]()->_bool {return m_bWalk && (CheckAnim("AS_ch0100_206_AL_atk_upper_landing")) && (0.1f <= m_fPlayRatio); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATTACK_UPPER_LANDING to JUMP_START", "JUMP_START")
				.Predicator([&]()->_bool {return m_bJump && (CheckAnim("AS_ch0100_206_AL_atk_upper_landing")) && (0.1f <= m_fPlayRatio); })
				.Duration(0.1f)
				.Priority(100)

				.AddTransition("ATTACK_UPPER_LANDING to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick && (CheckAnim("AS_ch0100_206_AL_atk_upper_landing")) && (0.1f <= m_fPlayRatio); })
				.Duration(0.1f).Priority(1)

				.AddTransition("ATTACK_UPPER_LANDING to ATTACK_NONCHARGE", "ATTACK_NONCHARGE")
				.Predicator([&]()->_bool {return m_bNonCharge && (CheckAnim("AS_ch0100_206_AL_atk_upper_landing")) && (0.1f <= m_fPlayRatio) && static_cast<CPlayer*>(m_pTargetObject)->UseAttackCnt(CPlayer::LIMIT_NONCHARGE_FLOOR); })
				.Duration(0.1f).Priority(1)

				.AddTransition("ATTACK_UPPER_LANDING to ATTACK_CHARGE_START", "ATTACK_CHARGE_START")
				.Predicator([&]()->_bool {return m_bCharge && (CheckAnim("AS_ch0100_206_AL_atk_upper_landing")) && (0.1f <= m_fPlayRatio) && static_cast<CPlayer*>(m_pTargetObject)->BeforeCharge(0.2f); })
				.Duration(0.1f).Priority(1)

				.AddTransition("ATTACK_UPPER_LANDING to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash && (CheckAnim("AS_ch0100_206_AL_atk_upper_landing")) && (0.1f <= m_fPlayRatio); })
				.Duration(0.1f).Priority(1)

				.AddTransition("ATTACK_UPPER_LANDING to IDLE", "IDLE")
				.Duration(0.1f)
				.Priority(100)


#pragma endregion Upper

		.Build();

	m_pASM_Base->SetCurState("IDLE");

	list<CAnimation*> SocketList;
	m_mapAnimSocket.emplace("Netual_Saperate_Animation", SocketList);
	m_mapAnimSocket.emplace("Upper_Saperate_Animation", SocketList);
	m_mapAnimSocket.emplace("Under_Saperate_Animation", SocketList);
	m_mapAnimSocket.emplace("Kinetic_AnimSocket", SocketList);
	m_mapAnimSocket.emplace("Kinetic_Air_AnimSocket", SocketList);
	m_mapAnimSocket.emplace("Hit_AnimSocket", SocketList);
	m_mapAnimSocket.emplace("Kinetic_Combo_AnimSocket", SocketList);
	m_mapAnimSocket.emplace("JustDodge_AnimSocket", SocketList);
	m_mapAnimSocket.emplace("Common_AnimSocket", SocketList);

	return S_OK;
}

void CBaseAnimInstance::Tick(_double TimeDelta)
{
	UpdateTargetState(TimeDelta);

	_bool bChange = CheckFinishedAnimSocket();	
	_bool bLocalMove = true;
	_bool vAirEnd = bChange && m_bAir;

	string szCurAnimName = "";

	// 소켓이 비어있는지 탐색
	list<CAnimation*> CurSocket;

	for (auto& iter : m_mapAnimSocket)
	{
		if (iter.second.empty())
			continue;
		else
		{
			CurSocket = iter.second;
			break;
		}
	}

	// 발견한 소켓이 있으면 해당 소켓을 실행
	if (!CurSocket.empty())
	{
		auto Socket = CurSocket.front();
		if (bChange)
		{
			Socket = CurSocket.front();
			m_pModel->SetPlayAnimation(Socket->GetName());
			m_pModel->Reset_LocalMove(true);
			m_fLerpTime = 0.f;
		}

		if (1.f > m_fLerpTime / m_fLerpDuration)
		{
			Socket->Update_Bones(TimeDelta, EAnimUpdateType::BLEND, m_fLerpTime / m_fLerpDuration);
			m_fLerpTime += (_float)TimeDelta;
		}
		else if (m_bAttach)
		{
			m_fLerpTime = 0.f;	// 어태치면 바로 보간
			m_bSeperateSwitch = false;
			m_bAttach = false;
		}
		else
		{
			szCurAnimName = Socket->GetName();
			Socket->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
		}

		if (m_bSeperateAnim)
		{
			m_pModel->SetBoneMask(EBoneMask::OFF_CHILD_EQ, "Spine1");

			m_pASM_Base->Tick(TimeDelta);

			m_pModel->SetBoneMask(EBoneMask::ON_ALL);

			m_fSeperateLerpTime = 0.f;
		}
		else
		{
			m_pModel->SetBoneMask(EBoneMask::ON_ALL);
		}
	}
	else if (bChange)
	{
		bLocalMove = false;
		
		if (m_bAir)
			m_pASM_Base->SetCurState("JUMP_FALL");
		else
		{
			m_pASM_Base->SetCurState("IDLE");
			//IM_LOG("Change State");
		}

		m_fLerpTime = 0.f;
	}
	else if (!bChange && m_bSeperateSwitch)
	{
		bLocalMove = false;
		m_bSeperateSwitch = false;
		m_fLerpTime = 0.f;
	}
	else if (m_fLerpTime < m_fLerpDuration)
	{
		if(m_pASM_Base->GetCurState()->m_SpairAnimation != nullptr)
			m_pASM_Base->GetCurState()->m_SpairAnimation->Update_Bones(TimeDelta, EAnimUpdateType::BLEND, m_fLerpTime / m_fLerpDuration);
		else
			m_pASM_Base->GetCurState()->m_Animation->Update_Bones(TimeDelta, EAnimUpdateType::BLEND, m_fLerpTime / m_fLerpDuration);
		
		m_fLerpTime += (_float)TimeDelta;
	}
	else
	{
		m_pASM_Base->Tick(TimeDelta);
	}

	if (m_pASM_Base->GetCurState()->m_SpairAnimation != nullptr)
		m_pModel->SetCurAnimName(m_pASM_Base->GetCurState()->m_SpairAnimation->GetName());
	else
		m_pModel->SetCurAnimName(m_pASM_Base->GetCurState()->m_Animation->GetName());

	m_pModel->Compute_CombindTransformationMatrix();

	// 로컬무브 연산은 계속 해주되, 적용은 적용을 할 타이밍에만 사용
	_matrix WorldMatrix = m_pTargetObject->GetTransform()->Get_WorldMatrix();
	_vector vLocalMove = m_pModel->GetLocalMove(WorldMatrix);
	if (0.f != XMVectorGetX(XMVector3Length(vLocalMove)))
		m_vLocalMove = vLocalMove;

	if (bLocalMove/* && ("" == szCurAnimName)*/)
	{	
		if (m_pASM_Base->GetCurState()->m_strName.find("WALK_START") != string::npos)
		{
			_float fLength = XMVectorGetX(XMVector3Length(vLocalMove));
			_vector vLook = XMVector3Normalize(m_pTargetObject->GetTransform()->Get_State(CTransform::STATE_LOOK));

			m_pTargetObject->GetTransform()->LocalMove(vLook * fLength);

		}
		else
		{
			m_pTargetObject->GetTransform()->LocalMove(vLocalMove);
		}
	}
	else
	{
		IM_LOG("%d", bLocalMove);
		IM_LOG(szCurAnimName.c_str());
	}

	//m_pTargetObject->GetTransform()->TurnByMatrix(m_pModel->GetLocalRotationMatrix(m_pTargetObject->GetTransform()->Get_WorldMatrix()));

	//m_pTargetObject->GetTransform()->AddQuaternion(m_pModel->GetLocalRotationDelta());
	
	//m_pTargetObject->GetTransform()->TurnByMatrix(m_pModel->GetLocalEularMatrix(m_pTargetObject->GetTransform()->Get_WorldMatrix()));
	
	//else if ("" != szCurAnimName)
	//{
	//	_matrix WorldMatrix = m_pTargetObject->GetTransform()->Get_WorldMatrix();
	//	_vector vLocalMove = m_pModel->GetLocalMove(WorldMatrix, szCurAnimName);

	//	if (!m_bSeperateAnim && !m_bSeperateSwitch)
	//		m_pTargetObject->GetTransform()->LocalMove(vLocalMove);
	//}

//	if (m_bOptionalMove)
//	{
		// 툴 작업 시에는 이거 끄고 하기!! 옵셔널 무브
		_vector vOpTest = m_pModel->GetOptionalMoveVector(m_pTargetObject->GetTransform()->Get_WorldMatrix());
		m_pTargetObject->GetTransform()->LocalMove(vOpTest);
//	}

	// 카메라
	CGameInstance::GetInstance()->ActionCamTickByPlayTime(m_pModel->GetPlayAnimation()->GetPlayRatio());
}

void CBaseAnimInstance::UpdateTargetState(_double TimeDelta)
{
	// 해당 애니메이션 스테이트 머신을 작동시키는 객체의 상태를 갱신받는 함수

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pTargetObject);

	m_bPreAir = m_bAir;

	m_bAir = pPlayer->isAir();
	m_bMove = pPlayer->isMove();
	m_bWalk = pPlayer->isWalk();
	m_bOptionalMove = pPlayer->isOptionalMove();

	m_bLeftClick = pPlayer->isLeftClick();
	m_bDash = pPlayer->isDash();

	m_bJump = pPlayer->isJump();
	m_bUpper = pPlayer->isUpper();

	m_bNonCharge = pPlayer->isNonCharge();
	m_bCharge = pPlayer->isCharge();

	m_bOnFloor = pPlayer->isOnFloor();

	m_bSeperateAnim = pPlayer->isSeperateAnim();

	m_fPlayRatio = 0.f;
	m_fPlayRatio = pPlayer->GetPlayRatio();

	m_fYSpeed = pPlayer->GetfYSpeed();

	Vector4 vCamLook = pGameInstance->Get_CamLook();
	m_vLookPoint = m_pTargetObject->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) + XMVectorSet(vCamLook.x, 0.f, vCamLook.z, 0.f);

	m_eMoveDir = pPlayer->GetMoveDir();
	m_vMoveDir = pPlayer->GetMoveVector();

	m_bLerp = m_pASM_Base->isLerping();


	_bool bBefOnBattle = m_bOnBattle;
	m_bOnBattle = pPlayer->isBattle();

	// 전투,비전투가 변경되면 스페어 애니메이션 관리
	if (bBefOnBattle != m_bOnBattle)
	{
		SpairAnimationChecker();
	}
}

void CBaseAnimInstance::Imgui_RenderState()
{
}

void CBaseAnimInstance::InputAnimSocket(const string& strSocName, list<CAnimation*> AnimList)
{
	// 기존 소켓을 싹 지우고 이걸로 덮어버림
	if (!m_bSeperateAnim)
		m_pModel->Reset_LocalMove(true);

//	static_cast<CPlayer*>(m_pTargetObject)->SetAbleState({ false, false, false, false, false, true, true, true, true, false });

//	m_pASM_Base->SetCurState("IDLE");

	for (auto& iter : m_mapAnimSocket)
	{
		if (!iter.second.empty())
			iter.second.front()->Reset();
	
		list<CAnimation*> SocketList;
		iter.second = SocketList;
	}

	m_bSeperateSwitch = false;
	m_mapAnimSocket[strSocName] = (AnimList);
}

void CBaseAnimInstance::AttachAnimSocket(const string & strSocName, list<CAnimation*> AnimList)
{
	// 소켓의 애니메이션을 교환하고 보간함, 아닐 경우 그냥 덮어버림

	if (!m_bSeperateAnim)
		m_pModel->Reset_LocalMove(true);

//	static_cast<CPlayer*>(m_pTargetObject)->SetAbleState({ false, false, false, false, false, true, true, true, true, false });

//	m_pASM_Base->SetCurState("IDLE");

	for (auto& iter : m_mapAnimSocket)
	{
		if (!iter.second.empty())
			iter.second.front()->Reset();

		list<CAnimation*> SocketList;
		iter.second = SocketList;
	}

	const auto List = m_mapAnimSocket.find(strSocName);

	if (List != m_mapAnimSocket.end())
	{
		if (!List->second.empty())
		{
			m_bAttach = true;
			List->second.front()->Reset();
		}
		m_mapAnimSocket[strSocName] = (AnimList);
	}

	m_bSeperateSwitch = false;
}

const string & CBaseAnimInstance::GetCurSocketAnimName()
{
	for (auto& iter : m_mapAnimSocket)
	{
		if (!iter.second.empty())
		{
			return iter.second.front()->GetName();
		}
	}

	return m_szNullString;
}

_bool CBaseAnimInstance::isSocketAlmostFinish(const string & strSocName)
{
	return (m_mapAnimSocket[strSocName].size() == 1) && (m_mapAnimSocket[strSocName].front()->GetPlayRatio() >= 0.95f);
}

_bool CBaseAnimInstance::isSocketPassby(const string & strSocName, _float fPlayRatio)
{
	return (m_mapAnimSocket[strSocName].size() == 1) && (m_mapAnimSocket[strSocName].front()->GetPlayRatio() >= fPlayRatio);
}

_bool CBaseAnimInstance::isSocketExactlyEmpty()
{
	for (auto& iter : m_mapAnimSocket)
	{
		if (iter.second.empty())
			continue;
		else
			return false;
	}

	return true;
}

void CBaseAnimInstance::ClearAnimSocket(const string & strSocName)
{
	if ("" == strSocName)
	{
		for (auto& iter : m_mapAnimSocket)
		{
			if (!iter.second.empty())
				iter.second.front()->Reset();

			list<CAnimation*> SocketList;
			iter.second = SocketList;
		}
	}
	else
	{
		if (!m_mapAnimSocket[strSocName].empty())
			m_mapAnimSocket[strSocName].front()->Reset();

		list<CAnimation*> SocketList;
		m_mapAnimSocket[strSocName] = SocketList;
	}

	m_bSeperateSwitch = false;
}

void CBaseAnimInstance::SpairAnimationChecker()
{
	// 전투 중 스페어 애니메이션 추가
	if (m_bOnBattle)
	{
		m_pASM_Base->SetSpairAnim("IDLE", m_pModel->Find_Animation("AS_ch0100_101_AL_wait01"));
		m_pASM_Base->SetSpairAnim("WALK", m_pModel->Find_Animation("AS_ch0100_121_AL_run_start_F"));
		m_pASM_Base->SetSpairAnim("WALK_START_FRONT", m_pModel->Find_Animation("AS_ch0100_121_AL_run_start_F"));
		m_pASM_Base->SetSpairAnim("WALK_START_LEFT", m_pModel->Find_Animation("AS_ch0100_122_AL_run_start_L"));
		m_pASM_Base->SetSpairAnim("WALK_START_RIGHT", m_pModel->Find_Animation("AS_ch0100_123_AL_run_start_R"));
		m_pASM_Base->SetSpairAnim("WALK_START_BACK_LEFT", m_pModel->Find_Animation("AS_ch0100_124_AL_run_start_BL"));
		m_pASM_Base->SetSpairAnim("WALK_START_BACK_RIGHT", m_pModel->Find_Animation("AS_ch0100_125_AL_run_start_BR"));
		m_pASM_Base->SetSpairAnim("WALK_LOOP", m_pModel->Find_Animation("AS_ch0100_126_AL_run"));
		m_pASM_Base->SetSpairAnim("WALK_END", m_pModel->Find_Animation("AS_ch0100_128_AL_run_end"));
		m_pASM_Base->SetSpairAnim("DASH", m_pModel->Find_Animation("AS_ch0100_151_AL_dodge_F_start"));
		m_pASM_Base->SetSpairAnim("DASH_START_FRONT", m_pModel->Find_Animation("AS_ch0100_151_AL_dodge_F_start"));
		m_pASM_Base->SetSpairAnim("DASH_END_FRONT", m_pModel->Find_Animation("AS_ch0100_151_AL_dodge_F_stop"));
		m_pASM_Base->SetSpairAnim("DASH_START_LEFT", m_pModel->Find_Animation("AS_ch0100_157_AL_dodge_L_start"));
		m_pASM_Base->SetSpairAnim("DASH_END_LEFT", m_pModel->Find_Animation("AS_ch0100_157_AL_dodge_L_stop"));
		m_pASM_Base->SetSpairAnim("DASH_START_RIGHT", m_pModel->Find_Animation("AS_ch0100_153_AL_dodge_R_start"));
		m_pASM_Base->SetSpairAnim("DASH_END_RIGHT", m_pModel->Find_Animation("AS_ch0100_153_AL_dodge_R_stop"));
		m_pASM_Base->SetSpairAnim("DASH_START_BACK", m_pModel->Find_Animation("AS_ch0100_155_AL_dodge_B_start"));
		m_pASM_Base->SetSpairAnim("DASH_END_BACK", m_pModel->Find_Animation("AS_ch0100_155_AL_dodge_B_stop"));
		m_pASM_Base->SetSpairAnim("RUN_FRONT", m_pModel->Find_Animation("AS_ch0100_136_AL_dash"));
		m_pASM_Base->SetSpairAnim("RUN_END", m_pModel->Find_Animation("AS_ch0100_138_AL_dash_stop"));
		m_pASM_Base->SetSpairAnim("JUMP_START", m_pModel->Find_Animation("AS_ch0100_141_AL_jump_start"));
		m_pASM_Base->SetSpairAnim("JUMP_RISE", m_pModel->Find_Animation("AS_ch0100_141_AL_jump_rise"));
		m_pASM_Base->SetSpairAnim("JUMP_FALL", m_pModel->Find_Animation("AS_ch0100_141_AL_jump_fall"));
		m_pASM_Base->SetSpairAnim("JUMP_LANDING", m_pModel->Find_Animation("AS_ch0100_141_AL_jump_landing"));
		m_pASM_Base->SetSpairAnim("DOUBLE_JUMP_RISE", m_pModel->Find_Animation("AS_ch0100_143_AL_doublejump_rise"));
		m_pASM_Base->SetSpairAnim("RUNJUMP_START", m_pModel->Find_Animation("AS_ch0100_144_AL_runjump_start"));
		m_pASM_Base->SetSpairAnim("RUNJUMP_RISE", m_pModel->Find_Animation("AS_ch0100_144_AL_runjump_rise"));
		m_pASM_Base->SetSpairAnim("RUNJUMP_FALL", m_pModel->Find_Animation("AS_ch0100_144_AL_runjump_fall"));
		m_pASM_Base->SetSpairAnim("RUNJUMP_LANDING", m_pModel->Find_Animation("AS_ch0100_144_AL_runjump_landing"));
		m_pASM_Base->SetSpairAnim("DOUBLE_RUNJUMP_RISE", m_pModel->Find_Animation("AS_ch0100_143_AL_doublejump_rise"));
		m_pASM_Base->SetSpairAnim("DASHJUMP_START", m_pModel->Find_Animation("AS_ch0100_142_AL_dashjump_start"));
		m_pASM_Base->SetSpairAnim("DASHJUMP_RISE", m_pModel->Find_Animation("AS_ch0100_142_AL_dashjump_rise"));
		m_pASM_Base->SetSpairAnim("DASHJUMP_FALL", m_pModel->Find_Animation("AS_ch0100_142_AL_dashjump_fall"));
		m_pASM_Base->SetSpairAnim("DASHJUMP_LANDING", m_pModel->Find_Animation("AS_ch0100_142_AL_dashjump_landing"));
		m_pASM_Base->SetSpairAnim("DOUBLE_DASHJUMP_RISE", m_pModel->Find_Animation("AS_ch0100_143_AL_doublejump_rise"));
		m_pASM_Base->SetSpairAnim("AIR_DODGE", m_pModel->Find_Animation("AS_ch0100_161_AL_air_dodge_F"));
		m_pASM_Base->SetSpairAnim("AIR_DODGE_FRONT", m_pModel->Find_Animation("AS_ch0100_161_AL_air_dodge_F"));
		m_pASM_Base->SetSpairAnim("AIR_DODGE_BACK", m_pModel->Find_Animation("AS_ch0100_165_AL_air_dodge_B"));
		m_pASM_Base->SetSpairAnim("AIR_DODGE_LEFT", m_pModel->Find_Animation("AS_ch0100_167_AL_air_dodge_L"));
		m_pASM_Base->SetSpairAnim("AIR_DODGE_RIGHT", m_pModel->Find_Animation("AS_ch0100_163_AL_air_dodge_R"));
	}
	else
	{
		for (auto& iter : m_pASM_Base->Get_MapStates())
		{
			//if (m_pASM_Base->GetCurState() == iter.second)
			//{
			//	iter.second->m_bSpairClearFlag = true;	// 변경 요청
			//	continue;
			//}
			
			iter.second->m_SpairAnimation = nullptr;
		}
	}
}

_bool CBaseAnimInstance::CheckAnim(const string & szAnimName)
{
	// 보간중에는 그냥 false 반환해서 보간중에 State가 변하는 참사를 막음
	if (m_pASM_Base->isLerping()) return false;

	return  (szAnimName == m_pModel->GetPlayAnimation()->GetName()) ? true : false;
}

_bool CBaseAnimInstance::FloorCheck()
{
	if (!m_bAir && !m_bOnFloor)
	{
		if (-15.f >= m_fYSpeed)
		{
			return true;
		}
	}

	return false;
}

CBaseAnimInstance * CBaseAnimInstance::Create(CModel * pModel, CGameObject * pGameObject)
{
	CBaseAnimInstance*		pInstance = new CBaseAnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CBaseAnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBaseAnimInstance::Free()
{
	__super::Free();
	Safe_Release(m_pASM_Base);
}
