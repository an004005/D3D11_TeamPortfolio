#include "stdafx.h"
#include "AI_CH0300_AnimInstance.h"
#include "AI_CH0300.h"
#include "Model.h"
#include "GameInstance.h"
#include "HelperClasses.h"
#include "VFX_Manager.h"
#include "PlayerInfoManager.h"

HRESULT CAI_CH0300_AnimInstance::Initialize(CModel* pModel, CGameObject* pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	m_pASM_Base = CASMBuilder()
		.InitState("IDLE")

#pragma region IDLE
		.AddState("IDLE")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0300_001_AL_wait01"))
			.StartEvent([&]() 
			{ 
				
			})
				.AddTransition("IDLE to TRANS_BATTLE", "TRANS_BATTLE")
				.Predicator([&]()->_bool {return m_bBattle && !CheckSpairExist("IDLE"); })
				.Duration(0.f)
				.Priority(0)

				.AddTransition("IDLE to TRANS_NEUTRAL", "TRANS_NEUTRAL")
				.Predicator([&]()->_bool {return !m_bBattle && CheckSpairExist("IDLE"); })
				.Duration(0.f)
				.Priority(1)

				.AddTransition("IDLE to WALK", "WALK")
				.Predicator([&]()->_bool { return (5.f <= DistanceCheck()); })
				.Duration(0.2f)
				.Priority(2)

				.AddTransition("IDLE to RUN", "RUN")
				.Predicator([&]()->_bool { return (10.f <= DistanceCheck()); })
				.Duration(0.2f)
				.Priority(3)

				.AddTransition("IDLE to ATK_A1", "ATK_A1")
				.Predicator([&]()->_bool {return (-1.f != m_fDistance_toEnemy && 3.f >= m_fDistance_toEnemy); })
				.Duration(0.2f)
				.Priority(4)

#pragma endregion IDLE

#pragma region WALK
		.AddState("WALK")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0300_006_AL_run"))
			.StartEvent([&]() 
			{ 
				
			})
				.AddTransition("WALK to RUN", "RUN")
				.Predicator([&]()->_bool { return (10.f <= DistanceCheck()); })
				.Duration(0.2f)
				.Priority(1)

				.AddTransition("WALK to IDLE", "IDLE")
				.Predicator([&]()->_bool { return (2.f > DistanceCheck()); })
				.Duration(0.2f)
				.Priority(1)

		.AddState("RUN")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0300_007_AL_dash"))
			.StartEvent([&]() 
			{ 
				
			})
				.AddTransition("RUN to RUN_STOP", "RUN_STOP")
				.Predicator([&]()->_bool { return (3.f > DistanceCheck()); })
				.Duration(0.2f)
				.Priority(0)

		.AddState("RUN_STOP")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0300_012_AL_brake_dash"))
			.StartEvent([&]() 
			{ 
				
			})
				.AddTransition("RUN_STOP to IDLE", "IDLE")
				.Duration(0.2f)
				.Priority(0)
		
#pragma endregion WALK

#pragma region BATTLECHECK

		.AddState("TRANS_BATTLE")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0300_004_Up_trans_battle"))
			.StartEvent([&]() 
			{ 
				SpairAnimationManager(true);
			})
				.AddTransition("TRANS_BATTLE to IDLE", "IDLE")
				.Duration(0.2f)
				.Priority(0)

		.AddState("TRANS_NEUTRAL")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0300_104_Up_trans_neutral"))
			.StartEvent([&]() 
			{ 
				SpairAnimationManager(false);
			})
				.AddTransition("TRANS_NEUTRAL to IDLE", "IDLE")
				.Duration(0.2f)
				.Priority(0)

#pragma endregion BATTLECHECK

#pragma region ATTACK

		.AddState("ATK_A1")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0300_201_AL_atk_a1"))
			.StartEvent([&]() 
			{ 
				
			})
				.AddTransition("ATK_A1 to ATK_A2_START", "ATK_A2_START")
				.Predicator([&]()->_bool {return (isAnimFinish("AS_ch0300_201_AL_atk_a1") && -1.f != m_fDistance_toEnemy && 3.f >= m_fDistance_toEnemy); })
				.Duration(0.2f)
				.Priority(0)

				.AddTransition("ATK_A1 to IDLE", "IDLE")
				.Duration(0.2f)
				.Priority(1)

		.AddState("ATK_A2_START")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0300_202_AL_atk_a2_start"))
			.StartEvent([&]() 
			{ 
				
			})
				.AddTransition("ATK_A2_START to ATK_A2", "ATK_A2")
				.Duration(0.f)
				.Priority(0)

		.AddState("ATK_A2")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0300_202_AL_atk_a2_loop"))
			.StartEvent([&]() 
			{ 
				
			})
				.AddTransition("ATK_A2 to ATK_A3", "ATK_A3")
				.Predicator([&]()->_bool {return (isAnimFinish("AS_ch0300_202_AL_atk_a2_loop") && -1.f != m_fDistance_toEnemy && 3.f >= m_fDistance_toEnemy); })
				.Duration(0.f)
				.Priority(0)

				.AddTransition("ATK_A2 to IDLE", "IDLE")
				.Duration(0.2f)
				.Priority(1)

		.AddState("ATK_A3")
			.SetAnimation(*m_pModel->Find_Animation("AS_ch0300_203_AL_atk_a3"))
			.StartEvent([&]() 
			{ 
				
			})
				.AddTransition("ATK_A3 to IDLE", "IDLE")
				.Duration(0.2f)
				.Priority(1)

#pragma endregion ATTACK

		.Build();

	m_pASM_Base->SetCurState("IDLE");

	return S_OK;
}

void CAI_CH0300_AnimInstance::Tick(_double TimeDelta)
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

	if (bLocalMove/* && ("" == szCurAnimName)*/)
	{
		m_pTargetObject->GetTransform()->LocalMove(vLocalMove);
	}

//	_vector vOpTest = m_pModel->GetOptionalMoveVector(m_pTargetObject->GetTransform()->Get_WorldMatrix());
//	m_pTargetObject->GetTransform()->LocalMove(vOpTest);
}

void CAI_CH0300_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CAI_CH0300* pAI = static_cast<CAI_CH0300*>(m_pTargetObject);

	m_fDistance_toPlayer = pAI->Get_Distance_Player();
	m_fDistance_toEnemy = pAI->Get_Distance_Enemy();
	m_bBattle = CPlayerInfoManager::GetInstance()->Get_PlayerStat().bBattle || (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster());
}

void CAI_CH0300_AnimInstance::Imgui_RenderState()
{
	m_pASM_Base->Imgui_RenderState();
}

void CAI_CH0300_AnimInstance::SpairAnimationManager(_bool bBattle)
{
	if (bBattle)
	{
		m_pASM_Base->SetSpairAnim("IDLE", m_pModel->Find_Animation("AS_ch0300_101_AL_wait01"));
		m_pASM_Base->SetSpairAnim("WALK", m_pModel->Find_Animation("AS_ch0300_106_AL_run"));
		m_pASM_Base->SetSpairAnim("RUN", m_pModel->Find_Animation("AS_ch0300_107_AL_dash"));
		m_pASM_Base->SetSpairAnim("RUN_STOP", m_pModel->Find_Animation("AS_ch0300_112_AL_brake_dash"));
	}
	else
	{
		for (auto& iter : m_pASM_Base->Get_MapStates())
		{
			iter.second->m_SpairAnimation = nullptr;
		}
	}
}

_bool CAI_CH0300_AnimInstance::CheckAnim(const string& strAnimName)
{
	if (m_pASM_Base->isLerping()) return false;

	return  (strAnimName == m_pModel->GetPlayAnimation()->GetName()) ? true : false;
}

_bool CAI_CH0300_AnimInstance::CheckSpairExist(const string& strAnimName)
{
	return (m_pASM_Base->GetState(strAnimName)->m_SpairAnimation != nullptr) ? true : false;
}

_bool CAI_CH0300_AnimInstance::isAnimFinish(const string& strAnimName)
{
	CAnimation* pAnimation = m_pModel->Find_Animation(strAnimName);

	if (nullptr == pAnimation) return false;

	if (false == CheckAnim(strAnimName)) return false;

	return pAnimation->IsFinished();
}

_float CAI_CH0300_AnimInstance::DistanceCheck()
{
	if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
		return m_fDistance_toEnemy;
	else
		return m_fDistance_toPlayer;
}

CAI_CH0300_AnimInstance* CAI_CH0300_AnimInstance::Create(CModel* pModel, CGameObject* pGameObject)
{
	CAI_CH0300_AnimInstance* pInstance = new CAI_CH0300_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CAI_CH0300_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAI_CH0300_AnimInstance::Free()
{
	__super::Free();
	Safe_Release(m_pASM_Base);
}
