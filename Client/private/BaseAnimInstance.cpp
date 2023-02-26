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

	m_pASM_Base = CASMBuilder()
		.InitState("IDLE")
			
		
#pragma region IDLE
		.AddState("IDLE")
		.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_002_AL_wait02"))
		.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanTurn(false).SetCanMove(false); })

			.AddTransition("IDLE to WALK", "WALK")
			.Predicator([&]()->_bool { return m_bWalk; })
			.Duration(0.1f).Priority(0)
				
			.AddTransition("IDLE to ATK_A1", "ATK_A1")
			.Predicator([&]()->_bool {return m_bLeftClick; })
			.Duration(0.1f).Priority(1)

			.AddTransition("IDLE to DASH", "DASH")
			.Predicator([&]()->_bool {return m_bDash; })
			.Duration(0.1f).Priority(1)

#pragma endregion IDLE

#pragma region Walk

			.AddState("WALK")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_021_AL_run_start_F"))
				.AddTransition("WALK to WALK_START_FRONT", "WALK_START_FRONT")
				.Predicator([&]()->_bool { return m_eMoveDir == CPlayer::DIR_F; })
				.Duration(0.1f).Priority(0)

				.AddTransition("WALK to WALK_START_LEFT", "WALK_START_LEFT")
				.Predicator([&]()->_bool { return m_eMoveDir == CPlayer::DIR_L; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK to WALK_START_RIGHT", "WALK_START_RIGHT")
				.Predicator([&]()->_bool { return m_eMoveDir == CPlayer::DIR_R; })
				.Duration(0.1f).Priority(2)

				.AddTransition("WALK to WALK_START_BACK_LEFT", "WALK_START_BACK_LEFT")
				.Predicator([&]()->_bool { return m_eMoveDir == CPlayer::DIR_B; })
				.Duration(0.1f).Priority(4)

				.AddTransition("WALK to WALK_START_BACK_RIGHT", "WALK_START_BACK_RIGHT")
				.Predicator([&]()->_bool { return m_eMoveDir == CPlayer::DIR_B; })
				.Duration(0.1f).Priority(3)
				
				.AddTransition("WALK to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK to IDLE", "IDLE")
				.Predicator([&]()->_bool { return !m_bWalk; })
				.Duration(0.1f).Priority(100)

			.AddState("WALK_START_FRONT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_021_AL_run_start_F"))
			.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanTurn(true).SetCanMove(false); })
				.FinishEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->LookAtDir(m_vLocalMove); })

				.AddTransition("WALK to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_FRONT to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_FRONT to WALK_LOOP", "WALK_LOOP")
					.Priority(0)
					.Duration(0.05f)

				.AddTransition("WALK_START_FRONT to WALK_END", "WALK_END")
					.Predicator([&]()->_bool { return !m_bWalk && (m_eMoveDir != CPlayer::DIR_F); })
					.Priority(1)
					.Duration(0.1f)

			.AddState("WALK_START_LEFT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_022_AL_run_start_L"))
				.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanTurn(false).SetCanMove(false); })
				.FinishEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->LookAtDir(m_vLocalMove); })

				.AddTransition("WALK to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_LEFT to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash && (0.4255f <= m_fPlayRatio); })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_LEFT to WALK_LOOP", "WALK_LOOP")
				.Priority(0).Duration(0.05f)

				.AddTransition("WALK_START_LEFT to WALK_END", "WALK_END")
					.Predicator([&]()->_bool { return !m_bWalk && (0.4255f <= m_fPlayRatio) && (m_eMoveDir != CPlayer::DIR_L); })
					.Priority(1)
					.Duration(0.1f)

			.AddState("WALK_START_RIGHT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_023_AL_run_start_R"))
				.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanTurn(false).SetCanMove(false); })
				.FinishEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->LookAtDir(m_vLocalMove); })

				.AddTransition("WALK to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_RIGHT to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash && (0.4255f <= m_fPlayRatio); })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_RIGHT to WALK_LOOP", "WALK_LOOP")
				.Duration(0.05f).Priority(0)

				.AddTransition("WALK_START_RIGHT to WALK_END", "WALK_END")
					.Predicator([&]()->_bool { return !m_bWalk && (0.4255f <= m_fPlayRatio) && (m_eMoveDir != CPlayer::DIR_R); })
					.Priority(1)
					.Duration(0.1f)

			.AddState("WALK_START_BACK_LEFT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_024_AL_run_start_BL"))
			.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanTurn(false).SetCanMove(false); })
			.FinishEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->LookAtDir(m_vLocalMove); })

				.AddTransition("WALK to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_BACK_LEFT to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash && (0.4255f <= m_fPlayRatio); })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_BACK_LEFT to WALK_LOOP", "WALK_LOOP")
				.Duration(0.05f).Priority(0)

				.AddTransition("WALK_START_BACK_LEFT to WALK_END", "WALK_END")
					.Predicator([&]()->_bool { return !m_bWalk && (0.4255f <= m_fPlayRatio) && (m_eMoveDir != CPlayer::DIR_B); })
					.Priority(1)
					.Duration(0.1f)

			.AddState("WALK_START_BACK_RIGHT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_025_AL_run_start_BR"))
			.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanTurn(false).SetCanMove(false); })
			.FinishEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->LookAtDir(m_vLocalMove); })

				.AddTransition("WALK to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_BACK_RIGHT to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash && (0.4255f <= m_fPlayRatio); })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_START_BACK_RIGHT to WALK_LOOP", "WALK_LOOP")
				.Duration(0.05f).Priority(0)

				.AddTransition("WALK_START_BACK_RIGHT to WALK_END", "WALK_END")
					.Predicator([&]()->_bool { return !m_bWalk && (0.4255f <= m_fPlayRatio) && (m_eMoveDir != CPlayer::DIR_B); })
					.Priority(1)
					.Duration(0.1f)

			.AddState("WALK_LOOP")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_026_AL_run"))
			.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanTurn(true).SetCanMove(true); })
				.AddTransition("WALK to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_LOOP to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_LOOP to WALK_END", "WALK_END")
				.Predicator([&]()->_bool { return !m_bWalk; })
				.Duration(0.1f).Priority(0)

			.AddState("WALK_END")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_028_AL_run_end"))
				.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanTurn(false).SetCanMove(false); })

				.AddTransition("WALK to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return m_bLeftClick; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_END to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash; })
				.Duration(0.1f).Priority(1)

				.AddTransition("WALK_END to IDLE", "IDLE")
				.Duration(0.1f).Priority(0)

				.AddTransition("WALK_END to WALK", "WALK")
				.Predicator([&]()->_bool { return m_bWalk; })
				.Duration(0.1f).Priority(0)

#pragma endregion Walk

#pragma region Attack_A

		// 공격 끝나면 공격 IDLE로 들어가야 되는데... 공격 연계 끝나고 하기
		.AddState("ATK_A1")
		.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_201_AL_atk_a1"))
		.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanMove(false).SetCanTurn(false); })
			.AddTransition("ATK_A1 to ATK_A2", "ATK_A2")
			.Predicator([&]()->_bool {return m_bLeftClick && (0.15f <= m_fPlayRatio) && (0.3f > m_fPlayRatio) && (CheckAnim("AS_ch0100_201_AL_atk_a1")) && (!m_bLerp); })
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
		.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanMove(false).SetCanTurn(false); })
			.AddTransition("ATK_A2 to ATK_A3", "ATK_A3")
			.Predicator([&]()->_bool {return m_bLeftClick && (0.15f <= m_fPlayRatio) && (0.5f > m_fPlayRatio) && (CheckAnim("AS_ch0100_202_AL_atk_a2")) && (!m_bLerp); })
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
		.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanMove(false).SetCanTurn(false); })
			.AddTransition("ATK_A3 to ATK_A4", "ATK_A4")
			.Predicator([&]()->_bool {return m_bLeftClick && (0.25f <= m_fPlayRatio) && (0.65f > m_fPlayRatio) && (CheckAnim("AS_ch0100_203_AL_atk_a3")) && (!m_bLerp); })
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
		.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanMove(false).SetCanTurn(false); })
			.AddTransition("ATK_A4 to ATK_A5", "ATK_A5")
			.Predicator([&]()->_bool {return m_bLeftClick && (0.25f <= m_fPlayRatio) && (0.6f > m_fPlayRatio) && (CheckAnim("AS_ch0100_204_AL_atk_a4")) && (!m_bLerp); })
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
		.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanMove(false).SetCanTurn(false); })

			.AddTransition("ATK_A5 to ATK_A1", "ATK_A1")
			.Predicator([&]()->_bool {return m_bLeftClick && (0.4f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_205_AL_atk_a5")) && (!m_bLerp); })
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
			.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanMove(false).SetCanTurn(false); })

			.AddTransition("DASH to ATTACK_DASH_START", "ATTACK_DASH_START")
			.Predicator([&]()->_bool {return m_bLeftClick; })
			.Duration(0.1f).Priority(0)

			.AddTransition("DASH to DASH_START_FRONT", "DASH_START_FRONT")
			.Predicator([&]()->_bool { return m_eMoveDir == CPlayer::DIR_F; })
			.Duration(0.05f).Priority(0)

			.AddTransition("DASH to DASH_START_LEFT", "DASH_START_LEFT")
			.Predicator([&]()->_bool { return m_eMoveDir == CPlayer::DIR_L; })
			.Duration(0.05f).Priority(0)

			.AddTransition("DASH to DASH_START_RIGHT", "DASH_START_RIGHT")
			.Predicator([&]()->_bool { return m_eMoveDir == CPlayer::DIR_R; })
			.Duration(0.05f).Priority(0)

			.AddTransition("DASH to DASH_START_BACK", "DASH_START_BACK")
			.Predicator([&]()->_bool { return m_eMoveDir == CPlayer::DIR_B; })
			.Duration(0.05f).Priority(0)

			.AddTransition("DASH to DASH_END_FRONT", "DASH_END_FRONT")
			.Duration(0.05f).Priority(0)
			
			// 앞쪽 대시 관련
			.AddState("DASH_START_FRONT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_051_AL_dodge_F_start"))
			.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanMove(false).SetCanTurn(false); })

			.AddTransition("DASH_START_FRONT to ATTACK_DASH_START", "ATTACK_DASH_START")
			.Predicator([&]()->_bool {return m_bLeftClick; })
			.Duration(0.1f).Priority(0)

			.AddTransition("DASH_START_FRONT to RUN_FRONT", "RUN_FRONT")
			.Predicator([&]()->_bool {return m_bWalk && (0.8f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_051_AL_dodge_F_start")); })
			.Duration(0.05f)
			.Priority(100)

			.AddTransition("DASH_START_FRONT to DASH_END_FRONT", "DASH_END_FRONT")
			.Duration(0.1f)
			.Priority(100)

			.AddState("DASH_END_FRONT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_051_AL_dodge_F_stop"))
			.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanMove(false).SetCanTurn(false); })
			.AddTransition("DASH_END_FRONT to IDLE", "IDLE")
			.Duration(0.1f)
			.Priority(100)

			// 좌측 대시 관련
			.AddState("DASH_START_LEFT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_057_AL_dodge_L_start"))
			.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanMove(false).SetCanTurn(false); })

			.AddTransition("DASH_START_LEFT to RUN_FRONT", "RUN_FRONT")
			.Predicator([&]()->_bool {return m_bWalk && (0.8f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_057_AL_dodge_L_start")); })
			.Duration(0.05f)
			.Priority(100)

			.AddTransition("DASH_START_LEFT to DASH_END_LEFT", "DASH_END_LEFT")
			.Duration(0.1f)
			.Priority(100)

			.AddState("DASH_END_LEFT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_057_AL_dodge_L_stop"))
			.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanMove(false).SetCanTurn(false); })
			.AddTransition("DASH_END_LEFT to IDLE", "IDLE")
			.Duration(0.1f)
			.Priority(100)

			// 우측 대시 관련
			.AddState("DASH_START_RIGHT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_053_AL_dodge_R_start"))
			.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanMove(false).SetCanTurn(false); })

			.AddTransition("DASH_START_RIGHT to RUN_FRONT", "RUN_FRONT")
			.Predicator([&]()->_bool {return m_bWalk && (0.8f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_053_AL_dodge_R_start")); })
			.Duration(0.05f)
			.Priority(100)

			.AddTransition("DASH_START_RIGHT to DASH_END_RIGHT", "DASH_END_RIGHT")
			.Duration(0.1f)
			.Priority(100)

			.AddState("DASH_END_RIGHT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_053_AL_dodge_R_stop"))
			.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanMove(false).SetCanTurn(false); })
			.AddTransition("DASH_END_RIGHT to IDLE", "IDLE")
			.Duration(0.1f)
			.Priority(100)

			// 후방 대시 관련
			.AddState("DASH_START_BACK")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_055_AL_dodge_B_start"))
			.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanMove(false).SetCanTurn(false); })

			.AddTransition("DASH_START_BACK to DASH_END_BACK", "DASH_END_BACK")
			.Duration(0.1f)
			.Priority(100)

			.AddState("DASH_END_BACK")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_055_AL_dodge_B_stop"))
			.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanMove(false).SetCanTurn(false); })

			.AddTransition("DASH_END_BACK to RUN_FRONT", "RUN_FRONT")
			.Predicator([&]()->_bool {return m_bWalk && (0.5f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_055_AL_dodge_B_stop")); })
			.Duration(0.05f)
			.Priority(100)

			.AddTransition("DASH_END_BACK to IDLE", "IDLE")
			.Duration(0.1f)
			.Priority(100)

			// 달리기
			.AddState("RUN_FRONT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_036_AL_dash"))
			.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanMove(true).SetCanTurn(true).SetCanRun(true); })
			.FinishEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanMove(false).SetCanTurn(false).SetCanRun(false); })
			
			.AddTransition("RUN_FRONT to ATTACK_DASH_START", "ATTACK_DASH_START")
			.Predicator([&]()->_bool {return m_bLeftClick; })
			.Duration(0.1f).Priority(0)

			.AddTransition("RUN_FRONT to DASH", "DASH")
			.Predicator([&]()->_bool {return m_bDash; })
			.Duration(0.1f).Priority(0)
				
			.AddTransition("RUN_FRONT to RUN_END", "RUN_END")
			.Predicator([&]()->_bool {return !m_bWalk; })
			.Duration(0.1f)
			.Priority(100)

			// 달리기 종료
			.AddState("RUN_END")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_038_AL_dash_stop"))
			.AddTransition("RUN_END to IDLE", "IDLE")
			.Duration(0.1f)
			.Priority(100)
#pragma endregion Dash And Run

#pragma region DashAttack

				.AddState("ATTACK_DASH_START")
				.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_214_AL_atk_dash_start"))
				.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanTurn(false).SetCanMove(false); })
				.AddTransition("ATTACK_DASH_START to ATTACK_DASH_END", "ATTACK_DASH_END")
				.Duration(0.1f).Priority(1)

				.AddState("ATTACK_DASH_END")
				.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_214_AL_atk_dash_end"))
				.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanTurn(false).SetCanMove(false); })

				.AddTransition("ATTACK_DASH_END to ATK_A3", "ATK_A3")
				.Predicator([&]()->_bool {return m_bLeftClick && (0.07f <= m_fPlayRatio) && (CheckAnim("AS_ch0100_214_AL_atk_dash_end")); })
				.Duration(0.1f).Priority(1)

				.AddTransition("ATTACK_DASH_END to DASH", "DASH")
				.Predicator([&]()->_bool {return m_bDash && (0.07f <= m_fPlayRatio); })
				.Duration(0.1f).Priority(0)

				.AddTransition("ATTACK_DASH_END to IDLE", "IDLE")
				.Duration(0.1f).Priority(1)

#pragma endregion DashAttack

#pragma region ChargeAttack

				/*.AddState("ATTACK_NONCHARGE")
				.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_215_AL_atk_dash_hold"))
				.StartEvent([&]() { static_cast<CPlayer*>(m_pTargetObject)->SetCanTurn(false).SetCanMove(false); })
				.AddTransition("ATTACK_DASH_START to ATTACK_DASH_END", "ATTACK_DASH_END")
				.Duration(0.1f).Priority(1)*/


#pragma endregion ChargeAttack

		.Build();

	m_pASM_Base->SetCurState("IDLE");

	list<CAnimation*> SocketList;
	m_mapAnimSocket.emplace("AnimSocket_Test", SocketList);

	return S_OK;
}

void CBaseAnimInstance::Tick(_double TimeDelta)
{
	UpdateTargetState(TimeDelta);

	_bool bChange = CheckFinishedAnimSocket();
	_bool bLocalMove = true;

	if (!m_mapAnimSocket.find("AnimSocket_Test")->second.empty())
	{
		auto Socket = m_mapAnimSocket.find("AnimSocket_Test")->second.front();
		if (bChange)
		{
			Socket = m_mapAnimSocket.find("AnimSocket_Test")->second.front();
			m_pModel->SetPlayAnimation(Socket->GetName());
			m_pModel->SetCurAnimName(Socket->GetName());
			m_fLerpTime = 0.f;
		}

		if (1.f > m_fLerpTime / m_fLerpDuration)
		{
			Socket->Update_Bones(TimeDelta, EAnimUpdateType::BLEND, m_fLerpTime / m_fLerpDuration);
			m_fLerpTime += (_float)TimeDelta;
		}
		else
		{
			Socket->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
		}
	}
	else if (bChange)
	{
		bLocalMove = false;
		m_pASM_Base->SetCurState("IDLE");
		m_pASM_Base->GetCurState()->m_Animation->Reset();
		m_pModel->SetCurAnimName(m_pASM_Base->GetCurState()->m_Animation->GetName());
	}
	else
	{
		m_pASM_Base->Tick(TimeDelta);
		m_pModel->SetCurAnimName(m_pASM_Base->GetCurState()->m_Animation->GetName());
	}

	m_pModel->Compute_CombindTransformationMatrix();

	if (bLocalMove)
	{
		_matrix WorldMatrix = m_pTargetObject->GetTransform()->Get_WorldMatrix();
		_vector vLocalMove = m_pModel->GetLocalMove(WorldMatrix);
		m_pTargetObject->GetTransform()->LocalMove(vLocalMove);

		if (0.f != XMVectorGetX(XMVector3Length(vLocalMove)))
			m_vLocalMove = vLocalMove;
	}

	_vector vOpTest = m_pModel->GetOptionalMoveVector(m_pTargetObject->GetTransform()->Get_WorldMatrix());
	m_pTargetObject->GetTransform()->LocalMove(vOpTest);
}

void CBaseAnimInstance::UpdateTargetState(_double TimeDelta)
{
	// 해당 애니메이션 스테이트 머신을 작동시키는 객체의 상태를 갱신받는 함수

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pTargetObject);

	m_bPreAir = m_bAir;

	m_bAir = pPlayer->isAir();
	m_bMove = pPlayer->isMove();
	m_bWalk = pPlayer->isWalk();

	m_bLeftClick = pPlayer->isLeftClick();
	m_bDash = pPlayer->isDash();
	
	m_fPlayRatio = 0.f;
	m_fPlayRatio = pPlayer->GetPlayRatio();

	m_eMoveDir = pPlayer->GetMoveDir();

	m_bLerp = m_pASM_Base->isLerping();
}

void CBaseAnimInstance::Imgui_RenderState()
{
}

void CBaseAnimInstance::InputAnimSocket(const string& strSocName, list<CAnimation*> AnimList)
{
	for (auto& iter : m_mapAnimSocket)
	{
		list<CAnimation*> SocketList;
		iter.second = SocketList;
	}

	m_mapAnimSocket[strSocName] = (AnimList);
}

_bool CBaseAnimInstance::CheckAnim(const string & szAnimName)
{
	return  (szAnimName == m_pModel->GetPlayAnimation()->GetName()) ? true : false;
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
