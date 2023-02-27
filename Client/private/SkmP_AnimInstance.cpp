#include "stdafx.h"
#include "..\public\SkmP_AnimInstance.h"
#include "SkummyPool.h"
#include "Model.h"
#include "Animation.h"

HRESULT CSkmP_AnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));

	{
		m_pASM_Base = CASMBuilder()
			.InitState("Idle")
			.AddState("Idle")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0600_101_AL_wait01"))

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

				.AddTransition("Idle to Attack", "Attack")
					.Predicator([&]()->_bool {return !m_bIdle && m_bAttack; })
					.Duration(0.2f)

			.AddState("MoveF")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0600_105_AL_walk01"))

				.AddTransition("MoveF to Attack", "Attack")
					.Predicator([&]()->_bool {return !m_bMoveF && m_bAttack; })
					.Duration(0.2f)

			.AddState("MoveB")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0600_109_AL_walk_B"))
				
				.AddTransition("MoveB to Attack", "Attack")
					.Predicator([&]()->_bool {return !m_bMoveB && m_bAttack; })
					.Duration(0.2f)

			.AddState("MoveL")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0600_112_AL_walk_L"))

				.AddTransition("MoveL to Attack", "Attack")
					.Predicator([&]()->_bool {return !m_bMoveL && m_bAttack; })
					.Duration(0.2f)

			.AddState("MoveR")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0600_115_AL_walk_R"))

				.AddTransition("MoveR to Attack", "Attack")
					.Predicator([&]()->_bool {return !m_bMoveR && m_bAttack; })
					.Duration(0.2f)

			.AddState("Attack")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0600_204_AL_atk_a3_shot"))

				.AddTransition("Attack to Idle", "Idle")
					.Predicator([&]()->_bool {return !m_bAttack && m_bIdle; })
					.Duration(0.2f)
				
			.AddState("Threat")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0600_160_AL_threat"))

				.AddTransition("Threat to Attack", "Attack")
					.Predicator([&]()->_bool {return !m_bThreat && m_bAttack; })
					.Duration(0.2f)

				.AddTransition("Threat to Idle", "Idle")
					.Predicator([&]()->_bool {return !m_bThreat && m_bIdle; })
					.Duration(0.2f)

			.Build();
	}

	m_pASM_Base->SetCurState("Idle");

	list<CAnimation*> AddAnimSocket;
	m_mapAnimSocket.emplace("SkummyPool_GroundDmgAnim", AddAnimSocket);
	m_mapAnimSocket.emplace("SkummyPool_DeadAnim", AddAnimSocket);

	return S_OK;
}

void CSkmP_AnimInstance::Tick(_double TimeDelta)
{
	UpdateTargetState(TimeDelta);

	if (!m_bStatic)
	{
		_bool bChange = CheckFinishedAnimSocket();
		_bool bLocalMove = true;

		if (!m_mapAnimSocket.find("SkummyPool_GroundDmgAnim")->second.empty())
		{
			auto Socket = m_mapAnimSocket.find("SkummyPool_GroundDmgAnim")->second.front();
			if (bChange)
			{
				Socket = m_mapAnimSocket.find("SkummyPool_GroundDmgAnim")->second.front();
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

		else if (!m_mapAnimSocket.find("SkummyPool_DeadAnim")->second.empty())
		{
			auto Socket = m_mapAnimSocket.find("SkummyPool_DeadAnim")->second.front();
			if (bChange)
			{
				Socket = m_mapAnimSocket.find("SkummyPool_DeadAnim")->second.front();
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

void CSkmP_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	CSkummyPool* pSkummyPool = static_cast<CSkummyPool*>(m_pTargetObject);

	m_bPreAir = m_bAir;

	m_bIdle = pSkummyPool->IsIdle();
	
	// Move
	m_bMoveF = pSkummyPool->IsMoveF();
	m_bMoveB = pSkummyPool->IsMoveB();
	m_bMoveL = pSkummyPool->IsMoveL();
	m_bMoveR = pSkummyPool->IsMoveR();

	m_bAttack = pSkummyPool->IsAttack();
	m_bThreat = pSkummyPool->IsThreat();
	
	// Socket List
	m_bDamage = pSkummyPool->IsDamage();
	m_bDead = pSkummyPool->IsDead();

	// ASM Control
	m_bStatic = pSkummyPool->IsStatic();
}

void CSkmP_AnimInstance::Imgui_RenderState()
{
}

void CSkmP_AnimInstance::InputAnimSocket(const string & strSocName, list<CAnimation*> AnimList)
{
	m_mapAnimSocket[strSocName] = (AnimList);
}

CSkmP_AnimInstance * CSkmP_AnimInstance::Create(CModel * pModel, CGameObject * pGameObject)
{
	CSkmP_AnimInstance*		pInstance = new CSkmP_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CSkmP_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkmP_AnimInstance::Free()
{
	__super::Free();
	Safe_Release(m_pASM_Base);
}
