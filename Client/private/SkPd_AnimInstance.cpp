#include "stdafx.h"
#include "..\public\SkPd_AnimInstance.h"
#include "SkummyPandou.h"
#include "Model.h"
#include "Animation.h"

HRESULT CSkPd_AnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));

	{
		m_pASM_Base = CASMBuilder()
			.InitState("Idle")
			.AddState("Idle")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0700_107_AL_fly_wait"))
				
				.AddTransition("Idle to RandomMove", "RandomMove")
					.Predicator([&]()->_bool {return !m_bIdle && m_bRandomMove; })
					.Duration(0.2f)

				.AddTransition("Idle to Attack_Start", "Attack_Start")
					.Predicator([&]()->_bool {return !m_bIdle && m_bAttackStart; })
					.Duration(0.2f)

				.AddTransition("Idle to MoveF", "MoveF")
					.Predicator([&]()->_bool {return !m_bIdle && m_bMoveF; })
					.Duration(0.2f)

				.AddTransition("Idle to MoveB", "MoveB")
					.Predicator([&]()->_bool {return !m_bIdle && m_bMoveB; })
					.Duration(0.2f)

				.AddTransition("Idle to MoveL", "MoveL")
					.Predicator([&]()->_bool {return !m_bIdle && m_bMoveL; })
					.Duration(0.2f)

				.AddTransition("Idle to MoveR", "MoveR")
					.Predicator([&]()->_bool {return !m_bIdle && m_bMoveR; })
					.Duration(0.2f)

				.AddTransition("Idle to Threat", "Threat")
					.Predicator([&]()->_bool {return !m_bIdle && m_bThreat; })
					.Duration(0.2f)

			.AddState("MoveF")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0700_110_AL_walk_F"))
				
				.AddTransition("MoveF to Attack_Start", "Attack_Start")
					.Predicator([&]()->_bool {return !m_bMoveF && m_bAttackStart; })
					.Duration(0.2f)

			.AddState("MoveB")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0700_111_AL_walk_B"))

				.AddTransition("MoveB to Attack_Start", "Attack_Start")
					.Predicator([&]()->_bool {return !m_bMoveB && m_bAttackStart; })
					.Duration(0.2f)

				.AddTransition("MoveB to Threat", "Threat")
					.Predicator([&]()->_bool {return !m_bMoveB && m_bThreat; })
					.Duration(0.2f)

			// 플레이어를 중심으로 Turn 해줄 것
			.AddState("MoveL")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0700_112_AL_walk_L"))

				.AddTransition("MoveL to Attack_Start", "Attack_Start")
					.Predicator([&]()->_bool {return !m_bMoveL && m_bAttackStart; })
					.Duration(0.2f)

				.AddTransition("MoveL to Threat", "Threat")
					.Predicator([&]()->_bool {return !m_bMoveL && m_bThreat; })
					.Duration(0.2f)

			.AddState("MoveR")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0700_113_AL_walk_R"))

			.AddTransition("MoveR to Attack_Start", "Attack_Start")
			.Predicator([&]()->_bool {return !m_bMoveR && m_bAttackStart; })
			.Duration(0.2f)

			.AddTransition("MoveR to Threat", "Threat")
			.Predicator([&]()->_bool {return !m_bMoveR && m_bThreat; })
			.Duration(0.2f)
			
			// Attack
			.AddState("Attack_Start")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0700_201_AL_atk_a1_start"))
				
				.AddTransition("Attack_Start to Attack_Ing", "Attack_Ing")
					.Predicator([&]()->_bool {return !m_bAttackStart && m_bAttacking; })
					.Duration(0.2f)

			.AddState("Attack_Ing")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0700_202_AL_atk_a1_loop"))

				.AddTransition("Attack_Ing to Attack_End", "Attack_End")
					.Predicator([&]()->_bool {return !m_bAttacking && m_bAttackEnd; })
					.Duration(0.2f)
			
			.AddState("Attack_End")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0700_203_AL_atk_a1_end"))

				.AddTransition("Attack_End to Idle", "Idle")
					.Predicator([&]()->_bool {return !m_bAttackEnd && m_bIdle; })
					.Duration(0.2f)
			// ~Attack

			.AddState("RandomMove")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0700_121_AL_randommove_L"))

				.AddTransition("RandomMove to Idle", "Idle")
					.Predicator([&]()->_bool {return !m_bRandomMove && m_bIdle; })
					.Duration(0.2f)

			.AddState("Threat")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0700_160_AL_threat"))

				.AddTransition("Threat to Idle", "Idle")
					.Predicator([&]()->_bool { return !m_bThreat && m_bIdle; })
					.Duration(0.2f)

			.Build();
	}

	m_pASM_Base->SetCurState("Idle");

	list<CAnimation*> AddAnimSocket;
	m_mapAnimSocket.emplace("SkummyPandou_AirDmgAnim", AddAnimSocket);
	m_mapAnimSocket.emplace("SkummyPandou_DeadAnim", AddAnimSocket);

	return S_OK;
}

void CSkPd_AnimInstance::Tick(_double TimeDelta)
{
	UpdateTargetState(TimeDelta);

	if (!m_bStatic)
	{
		_bool bChange = CheckFinishedAnimSocket();
		_bool bLocalMove = true;

		if (!m_mapAnimSocket.find("SkummyPandou_AirDmgAnim")->second.empty())
		{
			auto Socket = m_mapAnimSocket.find("SkummyPandou_AirDmgAnim")->second.front();
			if (bChange)
			{
				Socket = m_mapAnimSocket.find("SkummyPandou_AirDmgAnim")->second.front();
				m_pModel->SetPlayAnimation(Socket->GetName());
				m_pModel->SetCurAnimName(Socket->GetName());
				m_fLerpTime = 0.f;
			}

			if (1.f > m_fLerpTime / m_fLerpDuration)
			{
				Socket->Update_Bones(TimeDelta, EAnimUpdateType::BLEND, m_fLerpTime / m_fLerpDuration);
				m_fLerpTime += TimeDelta;
			}
			else
			{
				Socket->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
			}
		}

		else if (!m_mapAnimSocket.find("SkummyPandou_DeadAnim")->second.empty())
		{
			auto Socket = m_mapAnimSocket.find("SkummyPandou_DeadAnim")->second.front();
			if (bChange)
			{
				Socket = m_mapAnimSocket.find("SkummyPandou_DeadAnim")->second.front();
				m_pModel->SetPlayAnimation(Socket->GetName());
				m_pModel->SetCurAnimName(Socket->GetName());
				m_fLerpTime = 0.f;
			}

			if (1.f > m_fLerpTime / m_fLerpDuration)
			{
				Socket->Update_Bones(TimeDelta, EAnimUpdateType::BLEND, m_fLerpTime / m_fLerpDuration);
				m_fLerpTime += TimeDelta;
			}
			else
			{
				Socket->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
			}
		}
		else if (bChange)
		{
			bLocalMove = false;
			m_pASM_Base->SetCurState("Idle");
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
			m_pTargetObject->GetTransform()->LocalMove(m_pModel->GetLocalMove(WorldMatrix));
		}
	}
}

void CSkPd_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	CSkummyPandou* pSkummyPandou = static_cast<CSkummyPandou*>(m_pTargetObject);
	
	m_bIdle = pSkummyPandou->IsIdle();

	m_bMoveF = pSkummyPandou->IsMoveF();
	m_bMoveB = pSkummyPandou->IsMoveB();
	m_bMoveL = pSkummyPandou->IsMoveL();
	m_bMoveR = pSkummyPandou->IsMoveR();
	m_bRandomMove = pSkummyPandou->IsRandomMove();
	
	m_bAttackStart = pSkummyPandou->IsAttackStart();
	m_bAttacking = pSkummyPandou->IsAttacking();
	m_bAttackEnd = pSkummyPandou->IsAttackEnd();

	m_bThreat = pSkummyPandou->IsThreat();
	
	m_bDamage = pSkummyPandou->IsDamage();
	m_bDead = pSkummyPandou->IsDead();

	m_bStatic = pSkummyPandou->IsStatic();

	m_fPlayRatio = 0.f;
	m_fPlayRatio = pSkummyPandou->GetPlayRatio();
}

void CSkPd_AnimInstance::Imgui_RenderState()
{
}

void CSkPd_AnimInstance::InputAnimSocket(const string & strSocName, list<CAnimation*> AnimList)
{
	m_mapAnimSocket[strSocName] = (AnimList);
}

CSkPd_AnimInstance * CSkPd_AnimInstance::Create(CModel * pModel, CGameObject * pGameObject)
{
	CSkPd_AnimInstance*		pInstance = new CSkPd_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CSkPd_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkPd_AnimInstance::Free()
{
	__super::Free();
	Safe_Release(m_pASM_Base);
}
