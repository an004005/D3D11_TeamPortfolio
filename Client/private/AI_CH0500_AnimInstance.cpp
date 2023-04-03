#include "stdafx.h"
#include "AI_CH0500_AnimInstance.h"
#include "AI_CH0500.h"
#include "Model.h"
#include "GameInstance.h"
#include "HelperClasses.h"
#include "VFX_Manager.h"
#include "PlayerInfoManager.h"

HRESULT CAI_CH0500_AnimInstance::Initialize(CModel* pModel, CGameObject* pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	m_pASM_Base = CASMBuilder()
		.InitState("IDLE")

#pragma region IDLE
		.AddState("IDLE")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_001_AL_wait01"))
			.StartEvent([&]() 
			{ 
				
			})
				.AddTransition("IDLE to HIT", "HIT")
				.Predicator([&]()->_bool {return m_bHit; })
				.Duration(0.f)
				.Priority(0)

				.AddTransition("IDLE to TRANS_BATTLE", "TRANS_BATTLE")
				.Predicator([&]()->_bool {return m_bBattle && !CheckSpairExist("IDLE"); })
				.Duration(0.f)
				.Priority(1)

				.AddTransition("IDLE to TRANS_NEUTRAL", "TRANS_NEUTRAL")
				.Predicator([&]()->_bool {return !m_bBattle && CheckSpairExist("IDLE"); })
				.Duration(0.f)
				.Priority(2)

				.AddTransition("IDLE to WALK", "WALK")
				.Predicator([&]()->_bool { return (5.f <= DistanceCheck()); })
				.Duration(0.2f)
				.Priority(3)

				.AddTransition("IDLE to RUN", "RUN")
				.Predicator([&]()->_bool { return (10.f <= DistanceCheck()); })
				.Duration(0.2f)
				.Priority(4)

				.AddTransition("IDLE to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return (-1.f != m_fDistance_toEnemy && 3.f >= m_fDistance_toEnemy); })
				.Duration(0.2f)
				.Priority(5)

				.AddTransition("IDLE to APPROACH_START", "APPROACH_START")
				.Predicator([&]()->_bool {return (-1.f != m_fDistance_toEnemy); })
				.Duration(0.2f)
				.Priority(6)



#pragma endregion IDLE

#pragma region WALK
		.AddState("WALK")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_006_AL_run"))
			.StartEvent([&]() 
			{ 
				
			})
				.AddTransition("WALK to HIT", "HIT")
				.Predicator([&]()->_bool {return m_bHit; })
				.Duration(0.f)
				.Priority(0)

				.AddTransition("WALK to RUN", "RUN")
				.Predicator([&]()->_bool { return (10.f <= DistanceCheck()); })
				.Duration(0.2f)
				.Priority(1)

				.AddTransition("WALK to JUMP_START", "JUMP_START")
				.Predicator([&]()->_bool { return m_bJump; })
				.Duration(0.2f)
				.Priority(2)

				.AddTransition("WALK to IDLE", "IDLE")
				.Predicator([&]()->_bool { return (2.f > DistanceCheck()); })
				.Duration(0.2f)
				.Priority(3)

		.AddState("RUN")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_007_AL_dash"))
			.StartEvent([&]() 
			{ 
				
			})
				.AddTransition("RUN to HIT", "HIT")
				.Predicator([&]()->_bool {return m_bHit; })
				.Duration(0.f)
				.Priority(0)

				.AddTransition("WALK to JUMP_START", "JUMP_START")
				.Predicator([&]()->_bool { return m_bJump; })
				.Duration(0.2f)
				.Priority(1)

				.AddTransition("RUN to RUN_STOP", "RUN_STOP")
				.Predicator([&]()->_bool { return (3.f > DistanceCheck()); })
				.Duration(0.2f)
				.Priority(2)

		.AddState("RUN_STOP")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_012_AL_brake_dash"))
			.StartEvent([&]() 
			{ 
				
			})
				.AddTransition("RUN_STOP to HIT", "HIT")
				.Predicator([&]()->_bool {return m_bHit; })
				.Duration(0.f)
				.Priority(0)

				.AddTransition("RUN_STOP to IDLE", "IDLE")
				.Duration(0.2f)
				.Priority(1)
		
#pragma endregion WALK

#pragma region BATTLECHECK

		.AddState("TRANS_BATTLE")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_004_Up_trans_battle"))
			.StartEvent([&]() 
			{ 
				SpairAnimationManager(true);
			})
				.AddTransition("TRANS_BATTLE to HIT", "HIT")
				.Predicator([&]()->_bool {return m_bHit; })
				.Duration(0.f)
				.Priority(0)

				.AddTransition("TRANS_BATTLE to IDLE", "IDLE")
				.Duration(0.2f)
				.Priority(1)

		.AddState("TRANS_NEUTRAL")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_104_Up_trans_neutral"))
			.StartEvent([&]() 
			{ 
				SpairAnimationManager(false);
			})
				.AddTransition("TRANS_NEUTRAL to HIT", "HIT")
				.Predicator([&]()->_bool {return m_bHit; })
				.Duration(0.f)
				.Priority(0)

				.AddTransition("TRANS_NEUTRAL to IDLE", "IDLE")
				.Duration(0.2f)
				.Priority(1)

#pragma endregion BATTLECHECK

#pragma region ATTACK

		.AddState("ATK_A1")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_201_AL_atk_a1"))
			.StartEvent([&]() 
			{ 
				_vector vTargetPos = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				m_pTargetObject->GetTransform()->LookAt_NonY(vTargetPos);
			})
				.AddTransition("ATK_A1 to HIT", "HIT")
				.Predicator([&]()->_bool {return m_bHit; })
				.Duration(0.f)
				.Priority(0)

				.AddTransition("ATK_A1 to ATK_A2_START", "ATK_A2_START")
				.Predicator([&]()->_bool {return (isAnimFinish("AS_CH0500_201_AL_atk_a1") && -1.f != m_fDistance_toEnemy && 3.f >= m_fDistance_toEnemy); })
				.Duration(0.2f)
				.Priority(1)

				.AddTransition("ATK_A1 to DODGE_START", "DODGE_START")
				.Duration(0.2f)
				.Priority(2)

		/*.AddState("ATK_A2_START")
			.SetAnimation(*m_pModel->Find_Animation("AS_CH0500_202_AL_atk_a2_start"))
			.StartEvent([&]() 
			{ 
				_vector vTargetPos = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				m_pTargetObject->GetTransform()->LookAt_NonY(vTargetPos);
			})
				.AddTransition("ATK_A2_START to HIT", "HIT")
				.Predicator([&]()->_bool {return m_bHit; })
				.Duration(0.f)
				.Priority(0)

				.AddTransition("ATK_A2_START to ATK_A2", "ATK_A2")
				.Predicator([&]()->_bool {return (isAnimFinish("AS_CH0500_202_AL_atk_a2_start") && -1.f != m_fDistance_toEnemy && 3.f >= m_fDistance_toEnemy); })
				.Duration(0.f)
				.Priority(1)

				.AddTransition("ATK_A2_START to DODGE_START", "DODGE_START")
				.Duration(0.f)
				.Priority(2)

		.AddState("ATK_A2")
			.SetAnimation(*m_pModel->Find_Animation("AS_CH0500_202_AL_atk_a2_loop"))
			.StartEvent([&]() 
			{ 
				_vector vTargetPos = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				m_pTargetObject->GetTransform()->LookAt_NonY(vTargetPos);
			})
				.AddTransition("ATK_A2 to HIT", "HIT")
				.Predicator([&]()->_bool {return m_bHit; })
				.Duration(0.f)
				.Priority(0)

				.AddTransition("ATK_A2 to ATK_A3", "ATK_A3")
				.Predicator([&]()->_bool {return (isAnimFinish("AS_CH0500_202_AL_atk_a2_loop") && -1.f != m_fDistance_toEnemy && 3.f >= m_fDistance_toEnemy); })
				.Duration(0.f)
				.Priority(1)

				.AddTransition("ATK_A2 to DODGE_START", "DODGE_START")
				.Duration(0.2f)
				.Priority(2)

		.AddState("ATK_A3")
			.SetAnimation(*m_pModel->Find_Animation("AS_CH0500_203_AL_atk_a3"))
			.StartEvent([&]() 
			{ 
				_vector vTargetPos = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				m_pTargetObject->GetTransform()->LookAt_NonY(vTargetPos);
			})
				.AddTransition("ATK_A3 to HIT", "HIT")
				.Predicator([&]()->_bool {return m_bHit; })
				.Duration(0.f)
				.Priority(0)

				.AddTransition("ATK_A3 to IDLE", "IDLE")
				.Duration(0.2f)
				.Priority(1)*/

#pragma endregion ATTACK

#pragma region DAMAGE

		.AddState("HIT")
			
			.AddTransition("HIT to HIT_LIGHT", "HIT_LIGHT")
			.Predicator([&]()->_bool {return m_tDamageDesc.m_iDamageType == EAttackType::ATK_LIGHT; })
			.Duration(0.f)
			.Priority(0)

			.AddTransition("HIT to HIT_MIDDLE", "HIT_MIDDLE")
			.Predicator([&]()->_bool {return m_tDamageDesc.m_iDamageType == EAttackType::ATK_MIDDLE; })
			.Duration(0.f)
			.Priority(1)

			.AddTransition("HIT to HIT_KNUCKBACK_START", "HIT_KNUCKBACK_START")
			.Predicator([&]()->_bool {return m_tDamageDesc.m_iDamageType == EAttackType::ATK_HEAVY; })
			.Duration(0.f)
			.Priority(2)

			.AddTransition("HIT to HIT_AIRBORNE", "HIT_AIRBORNE")
			.Predicator([&]()->_bool {return m_tDamageDesc.m_iDamageType == EAttackType::ATK_TO_AIR; })
			.Duration(0.f)
			.Priority(3)

			.AddTransition("HIT to IDLE", "IDLE")
			.Predicator([&]()->_bool 
			{
				Reset_HitCheck();
				return true;
			})
			.Duration(0.f)
			.Priority(100)

#pragma region HIT_LIGHT

		.AddState("HIT_LIGHT")

			.AddTransition("HIT_LIGHT to HIT_LIGHT_FRONT", "HIT_LIGHT_FRONT")
			.Predicator([&]()->_bool 
			{
				_float fDot = XMVectorGetX(XMVector3Dot(m_pTargetObject->GetTransform()->Get_State(CTransform::STATE_LOOK), m_tDamageDesc.m_vHitDir));
				return (fDot < 0.f) ? true : false; 
			})
			.Duration(0.f)
			.Priority(0)

			.AddTransition("HIT_LIGHT to HIT_LIGHT_BACK", "HIT_LIGHT_BACK")
			.Predicator([&]()->_bool 
			{
				_float fDot = XMVectorGetX(XMVector3Dot(m_pTargetObject->GetTransform()->Get_State(CTransform::STATE_LOOK), m_tDamageDesc.m_vHitDir));
				return (fDot > 0.f) ? true : false; 
			})
			.Duration(0.f)
			.Priority(1)

			.AddTransition("HIT_LIGHT to IDLE", "IDLE")
			.Predicator([&]()->_bool 
			{
				Reset_HitCheck();
				return true;
			})
			.Duration(0.f)
			.Priority(100)

		.AddState("HIT_LIGHT_FRONT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_401_AL_damage_l_F"))
			.StartEvent([&]() 
			{ 
				Reset_HitCheck();
			})
				.AddTransition("HIT_LIGHT_FRONT to HIT", "HIT")
				.Predicator([&]()->_bool {return m_bHit; })
				.Duration(0.f)
				.Priority(0)

				.AddTransition("HIT_LIGHT_FRONT to IDLE", "IDLE")
				.Duration(0.f)
				.Priority(1)

		.AddState("HIT_LIGHT_BACK")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_402_AL_damage_l_B"))
			.StartEvent([&]() 
			{ 
				Reset_HitCheck();
			})
				.AddTransition("HIT_LIGHT_BACK to HIT", "HIT")
				.Predicator([&]()->_bool {return m_bHit; })
				.Duration(0.f)
				.Priority(0)

				.AddTransition("HIT_LIGHT_BACK to IDLE", "IDLE")
				.Duration(0.f)
				.Priority(1)

#pragma endregion HIT_LIGHT

#pragma region HIT_MIDDLE

		.AddState("HIT_MIDDLE")

			.AddTransition("HIT_MIDDLE to HIT_MIDDLE_FRONT", "HIT_MIDDLE_FRONT")
			.Predicator([&]()->_bool 
			{
				return m_tDamageDesc.m_eHitDir == EBaseAxis::NORTH ? true : false;
			})
			.Priority(0)

			.AddTransition("HIT_MIDDLE to HIT_MIDDLE_BACK", "HIT_MIDDLE_BACK")
			.Predicator([&]()->_bool 
			{
				return m_tDamageDesc.m_eHitDir == EBaseAxis::SOUTH ? true : false;
			})
			.Priority(0)

			.AddTransition("HIT_MIDDLE to HIT_MIDDLE_LEFT", "HIT_MIDDLE_LEFT")
			.Predicator([&]()->_bool 
			{
				return m_tDamageDesc.m_eHitDir == EBaseAxis::WEST ? true : false;
			})
			.Priority(0)

			.AddTransition("HIT_MIDDLE to HIT_MIDDLE_RIGHT", "HIT_MIDDLE_RIGHT")
			.Predicator([&]()->_bool 
			{
				return m_tDamageDesc.m_eHitDir == EBaseAxis::EAST ? true : false;
			})
			.Priority(0)

			.AddTransition("HIT_MIDDLE to IDLE", "IDLE")
			.Predicator([&]()->_bool 
			{
				Reset_HitCheck();
				return true;
			})
			.Duration(0.f)
			.Priority(100)

		.AddState("HIT_MIDDLE_FRONT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_411_AL_damage_h_F"))
			.StartEvent([&]() 
			{ 
				Reset_HitCheck();
			})
				.AddTransition("HIT_MIDDLE_FRONT to HIT", "HIT")
				.Predicator([&]()->_bool {return m_bHit; })
				.Duration(0.f)
				.Priority(0)

				.AddTransition("HIT_MIDDLE_FRONT to IDLE", "IDLE")
				.Duration(0.f)
				.Priority(1)

		.AddState("HIT_MIDDLE_BACK")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_412_AL_damage_h_B"))
			.StartEvent([&]() 
			{ 
				Reset_HitCheck();
			})
				.AddTransition("HIT_MIDDLE_BACK to HIT", "HIT")
				.Predicator([&]()->_bool {return m_bHit; })
				.Duration(0.f)
				.Priority(0)

				.AddTransition("HIT_MIDDLE_BACK to IDLE", "IDLE")
				.Duration(0.f)
				.Priority(1)

		.AddState("HIT_MIDDLE_LEFT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_413_AL_damage_h_L"))
			.StartEvent([&]() 
			{ 
				Reset_HitCheck();
			})
				.AddTransition("HIT_MIDDLE_LEFT to HIT", "HIT")
				.Predicator([&]()->_bool {return m_bHit; })
				.Duration(0.f)
				.Priority(0)

				.AddTransition("HIT_MIDDLE_LEFT to IDLE", "IDLE")
				.Duration(0.f)
				.Priority(1)

		.AddState("HIT_MIDDLE_RIGHT")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_414_AL_damage_h_R"))
			.StartEvent([&]() 
			{ 
				Reset_HitCheck();
			})
				.AddTransition("HIT_MIDDLE_RIGHT to HIT", "HIT")
				.Predicator([&]()->_bool {return m_bHit; })
				.Duration(0.f)
				.Priority(0)

				.AddTransition("HIT_MIDDLE_RIGHT to IDLE", "IDLE")
				.Duration(0.f)
				.Priority(1)

#pragma endregion HIT_MIDDLE

#pragma region HIT_KNUCKBACK
		.AddState("HIT_KNUCKBACK_START")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_455_AL_rise_start"))
				.AddTransition("HIT_KNUCKBACK_START to HIT_BLOW_LANDING", "HIT_BLOW_LANDING")
				.Duration(0.f)
				.Priority(0)

		.AddState("HIT_BLOW_LANDING")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_433_AL_blow_landing_F"))
				.AddTransition("HIT_FALLDOWN to HIT_DOWN", "HIT_DOWN")
				.Duration(0.f)
				.Priority(0)

		.AddState("HIT_DOWN")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_434_AL_down_F"))
				.AddTransition("HIT_DOWN to HIT_WAKEUP", "HIT_WAKEUP")
				.Duration(0.f)
				.Priority(0)

		.AddState("HIT_WAKEUP")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_435_AL_wakeup_F"))
			.StartEvent([&]() 
			{ 
				Reset_HitCheck();
			})

				.AddTransition("HIT_WAKEUP to IDLE", "IDLE")
				.Duration(0.f)
				.Priority(0)
		
#pragma endregion HIT_KNUCKBACK

#pragma region HIT_AIRBORNE

	.AddState("HIT_AIRBORNE")
		.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_455_AL_rise_start"))
		.StartEvent([&]() 
		{ 
			static_cast<CAI_CH0500*>(m_pTargetObject)->Set_Jump(10.f);
		})
			.AddTransition("HIT_AIRBORNE to HIT_FALLDOWN", "HIT_FALLDOWN")
			.Duration(0.2f)
			.Priority(0)

	.AddState("HIT_FALLDOWN")
		.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_457_AL_fall_start"))
			.AddTransition("HIT_FALLDOWN to HIT_BLOW_LANDING", "HIT_BLOW_LANDING")
			.Predicator([&]()->_bool {return static_cast<CAI_CH0500*>(m_pTargetObject)->Get_OnFloor(); })
			.Duration(0.f)
			.Priority(0)

#pragma endregion HIT_AIRBORNE

#pragma endregion DAMAGE

#pragma region DODGE

		.AddState("DODGE_START")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_035_AL_dodge_B_start"))
				.AddTransition("DODGE_START to DODGE_END", "DODGE_END")
				.Duration(0.f)
				.Priority(0)

		.AddState("DODGE_END")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_036_AL_dodge_B_stop"))

				.AddTransition("DODGE_END to HIT", "HIT")
				.Predicator([&]()->_bool {return m_bHit; })
				.Duration(0.f)
				.Priority(0)

				.AddTransition("DODGE_END to IDLE", "IDLE")
				.Duration(0.f)
				.Priority(0)

		.AddState("APPROACH_START")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_030_AL_dodge_F_start"))
				.AddTransition("APPROACH_START to APPROACH_END", "APPROACH_END")
				.Duration(0.f)
				.Priority(0)

		.AddState("APPROACH_END")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_031_AL_dodge_F_stop"))

				.AddTransition("APPROACH_END to HIT", "HIT")
				.Predicator([&]()->_bool {return m_bHit; })
				.Duration(0.f)
				.Priority(0)

				.AddTransition("APPROACH_END to IDLE", "IDLE")
				.Duration(0.f)
				.Priority(0)

#pragma endregion DODGE

#pragma region JUMP

	.AddState("JUMP_START")
		.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_023_AL_jump_start"))
		.AddTransition("JUMP_START to JUMP_RISE", "JUMP_RISE")
		.Duration(0.1f)
		.Priority(0)

	.AddState("JUMP_RISE")
		.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_024_AL_jump_rise"))
		.StartEvent([&]() { static_cast<CAI_CH0500*>(m_pTargetObject)->Set_Jump(10.f); })
		.AddTransition("JUMP_RISE to JUMP_FALL", "JUMP_FALL")
		.Duration(0.1f)
		.Priority(0)

		.AddTransition("JUMP_RISE to JUMP_LANDING", "JUMP_LANDING")
		.Predicator([&]()->_bool 
		{
			_bool bResult =
				static_cast<CAI_CH0500*>(m_pTargetObject)->Get_OnFloor() &&
				(0.2f <= m_pModel->Find_Animation("AS_ch0500_024_AL_jump_rise")->GetPlayRatio());

			return bResult;
		})
		.Duration(0.1f)
		.Priority(0)

	.AddState("JUMP_FALL")
		.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_021_AL_jump_fall"))
		.AddTransition("JUMP_FALL to JUMP_LANDING", "JUMP_LANDING")
		.Predicator([&]()->_bool {return static_cast<CAI_CH0500*>(m_pTargetObject)->Get_OnFloor(); })
		.Duration(0.f)
		.Priority(0)

	.AddState("JUMP_LANDING")
		.SetAnimation(*m_pModel->Find_Animation("AS_ch0500_022_AL_jump_landing"))
		.StartEvent([&]() 
		{ 
			static_cast<CAI_CH0500*>(m_pTargetObject)->Reset_Jump();
			m_bJump = false;
		})
		.AddTransition("JUMP_LANDING to IDLE", "IDLE")
		.Duration(0.f)
		.Priority(0)

#pragma endregion JUMP

		.Build();

	m_pASM_Base->SetCurState("IDLE");

	ZeroMemory(&m_tDamageDesc, sizeof(DAMAGE_DESC));

	return S_OK;
}

void CAI_CH0500_AnimInstance::Tick(_double TimeDelta)
{
	UpdateTargetState(TimeDelta);

	_bool bLocalMove = true;

	string szCurAnimName = "";

	if (m_fLerpTime < m_fLerpDuration)
	{
		if (m_pASM_Base->GetCurState()->m_SpairAnimation != nullptr)
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

	_matrix WorldMatrix = m_pTargetObject->GetTransform()->Get_WorldMatrix();
	_vector vLocalMove = m_pModel->GetLocalMove(WorldMatrix);

	if (bLocalMove)
	{
		m_pTargetObject->GetTransform()->LocalMove(vLocalMove);
	}

//	_vector vOpTest = m_pModel->GetOptionalMoveVector(m_pTargetObject->GetTransform()->Get_WorldMatrix());
//	m_pTargetObject->GetTransform()->LocalMove(vOpTest);
}

void CAI_CH0500_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CAI_CH0500* pAI = static_cast<CAI_CH0500*>(m_pTargetObject);

	m_fDistance_toPlayer = pAI->Get_Distance_Player();
	m_fDistance_toEnemy = pAI->Get_Distance_Enemy();
	m_bBattle = CPlayerInfoManager::GetInstance()->Get_PlayerStat().bBattle || (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster());

	m_bHit = pAI->Get_Hit();
	m_tDamageDesc = pAI->Get_DamageDesc();
	m_bJump = pAI->Get_Jump();
}

void CAI_CH0500_AnimInstance::Imgui_RenderState()
{
	m_pASM_Base->Imgui_RenderState();
}

void CAI_CH0500_AnimInstance::SpairAnimationManager(_bool bBattle)
{
	if (bBattle)
	{
		m_pASM_Base->SetSpairAnim("IDLE", m_pModel->Find_Animation("AS_ch0500_101_AL_wait01"));
		m_pASM_Base->SetSpairAnim("WALK", m_pModel->Find_Animation("AS_ch0500_106_AL_run"));
		m_pASM_Base->SetSpairAnim("RUN", m_pModel->Find_Animation("AS_ch0500_107_AL_dash"));
		m_pASM_Base->SetSpairAnim("RUN_STOP", m_pModel->Find_Animation("AS_ch0500_112_AL_brake_dash"));
		m_pASM_Base->SetSpairAnim("JUMP_START", m_pModel->Find_Animation("AS_ch0500_123_AL_jump_start"));
		m_pASM_Base->SetSpairAnim("JUMP_RISE", m_pModel->Find_Animation("AS_ch0500_124_AL_jump_rise"));
		m_pASM_Base->SetSpairAnim("JUMP_FALL", m_pModel->Find_Animation("AS_ch0500_121_AL_jump_fall"));
		m_pASM_Base->SetSpairAnim("JUMP_LANGIND", m_pModel->Find_Animation("AS_ch0500_122_AL_jump_landing"));
		m_pASM_Base->SetSpairAnim("DODGE_START", m_pModel->Find_Animation("AS_ch0500_135_AL_dodge_B_start"));
		m_pASM_Base->SetSpairAnim("DODGE_END", m_pModel->Find_Animation("AS_ch0500_136_AL_dodge_B_stop"));
		m_pASM_Base->SetSpairAnim("APPROACH_START", m_pModel->Find_Animation("AS_ch0500_130_AL_dodge_F_start"));
		m_pASM_Base->SetSpairAnim("APPROACH_STOP", m_pModel->Find_Animation("AS_ch0500_131_AL_dodge_F_stop"));
	}
	else
	{
		for (auto& iter : m_pASM_Base->Get_MapStates())
		{
			iter.second->m_SpairAnimation = nullptr;
		}
	}
}

_bool CAI_CH0500_AnimInstance::CheckAnim(const string& strAnimName)
{
	if (m_pASM_Base->isLerping()) return false;

	return  (strAnimName == m_pModel->GetPlayAnimation()->GetName()) ? true : false;
}

_bool CAI_CH0500_AnimInstance::CheckSpairExist(const string& strAnimName)
{
	return (m_pASM_Base->GetState(strAnimName)->m_SpairAnimation != nullptr) ? true : false;
}

_bool CAI_CH0500_AnimInstance::isAnimFinish(const string& strAnimName)
{
	CAnimation* pAnimation = m_pModel->Find_Animation(strAnimName);

	if (nullptr == pAnimation) return false;

	if (false == CheckAnim(strAnimName)) return false;

	return pAnimation->IsFinished();
}

_float CAI_CH0500_AnimInstance::DistanceCheck()
{
	if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
		return m_fDistance_toEnemy;
	else
		return m_fDistance_toPlayer;
}

void CAI_CH0500_AnimInstance::Reset_HitCheck()
{
	static_cast<CAI_CH0500*>(m_pTargetObject)->Reset_Hit();
	static_cast<CAI_CH0500*>(m_pTargetObject)->Reset_DamageDesc();
	m_bHit = false;
	ZeroMemory(&m_tDamageDesc, sizeof(DAMAGE_DESC));
}

CAI_CH0500_AnimInstance* CAI_CH0500_AnimInstance::Create(CModel* pModel, CGameObject* pGameObject)
{
	CAI_CH0500_AnimInstance* pInstance = new CAI_CH0500_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CAI_CH0500_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAI_CH0500_AnimInstance::Free()
{
	__super::Free();
	Safe_Release(m_pASM_Base);
}
