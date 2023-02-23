#include "stdafx.h"
#include "BdLm_AnimInstance.h"
#include "BuddyLumi.h"
#include "Model.h"
#include "Animation.h"

HRESULT CBdLm_AnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));
	// .Predicator([&]()->_bool {return m_pModel->Find_Animation("AS_em0400_106_AL_walk02")->IsFinished(); })
	{
		m_pASM_Base = CASMBuilder()
			.InitState("Idle")
			.AddState("Idle")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0400_101_AL_wait01"))
			
				.AddTransition("Idle to Walk", "Walk")
					.Predicator([&]()->_bool {return m_bWalk; })
					.Duration(0.2f)

				.AddTransition("Idle to Run", "Run")
					.Predicator([&]()->_bool {return m_bRun; })
					.Duration(0.2f)

				.AddTransition("Idle to Attack", "Attack")
					.Predicator([&]()->_bool {return m_bAttack; })
					.Duration(0.2f)

				.AddTransition("Idle to DodgeB", "DodgeB")
					.Predicator([&]()->_bool {return m_bDodgeB; })
					.Duration(0.2f)

				.AddTransition("Idle to DodgeL", "DodgeL")
					.Predicator([&]()->_bool {return m_bDodgeL; })
					.Duration(0.2f)

				.AddTransition("Idle to DodgeR", "DodgeR")
					.Predicator([&]()->_bool {return m_bDodgeR; })
					.Duration(0.2f)
			
			// ~Idle

			.AddState("Walk")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0400_106_AL_walk02"))
				
				.AddTransition("Walk to Idle", "Idle")
					.Predicator([&]()->_bool {return !m_bWalk && m_bIdle && m_pModel->Find_Animation("AS_em0400_106_AL_walk02")->IsFinished(); })
					.Duration(0.2f)

				.AddTransition("Walk to Run", "Run")
					.Predicator([&]()->_bool {return !m_bWalk && m_bRun && m_pModel->Find_Animation("AS_em0400_106_AL_walk02")->IsFinished(); })
					.Duration(0.2f)

				.AddTransition("Walk to Attack", "Attack")
					.Predicator([&]()->_bool {return !m_bWalk && m_bAttack && m_pModel->Find_Animation("AS_em0400_106_AL_walk02")->IsFinished(); })
					.Duration(0.2f)

			.AddState("Run")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0400_107_AL_run01"))

				.AddTransition("Run to Idle", "Idle")
					.Predicator([&]()->_bool {return !m_bRun && m_bIdle; }) //&& m_pModel->Find_Animation("AS_em0400_107_AL_run01")->IsFinished(); })
					.Duration(0.2f)

				.AddTransition("Run to Walk", "Walk")
					.Predicator([&]()->_bool {return !m_bRun && m_bWalk; }) //&& m_pModel->Find_Animation("AS_em0400_107_AL_run01")->IsFinished(); })
					.Duration(0.2f)

				.AddTransition("Run to Attack", "Attack")
					.Predicator([&]()->_bool { return !m_bRun && m_bAttack; }) //&& m_pModel->Find_Animation("AS_em0400_107_AL_run01")->IsFinished(); })
					.Duration(0.2f)

				.AddTransition("Run to DodgeB", "DodgeB")
					.Predicator([&]()->_bool { return !m_bRun && m_bDodgeB; }) //&& m_pModel->Find_Animation("AS_em0400_107_AL_run01")->IsFinished(); })
					.Duration(0.2f)

				.AddTransition("Run to DodgeL", "DodgeL")
					.Predicator([&]()->_bool { return !m_bRun && m_bDodgeL; }) //&& m_pModel->Find_Animation("AS_em0400_107_AL_run01")->IsFinished(); })
					.Duration(0.2f)

				.AddTransition("Run to DodgeR", "DodgeR")
					.Predicator([&]()->_bool { return !m_bRun && m_bDodgeR; }) //&& m_pModel->Find_Animation("AS_em0400_107_AL_run01")->IsFinished(); })
					.Duration(0.2f)

			.AddState("Attack")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0400_200_AL_atk_a1"))
				
				.AddTransition("Attack to Idle", "Idle")
					.Predicator([&]()->_bool {return !m_bAttack && m_bIdle && m_pModel->Find_Animation("AS_em0400_200_AL_atk_a1")->IsFinished(); })
					.Duration(0.2f)

				.AddTransition("Attack to Walk", "Walk")
					.Predicator([&]()->_bool {return !m_bAttack && m_bWalk && m_pModel->Find_Animation("AS_em0400_200_AL_atk_a1")->IsFinished(); })
					.Duration(0.2f)

				.AddTransition("Attack to Run", "Run")
					.Predicator([&]()->_bool {return !m_bAttack && m_bRun && m_pModel->Find_Animation("AS_em0400_200_AL_atk_a1")->IsFinished(); })
					.Duration(0.2f)

				.AddTransition("Attack to Threat", "Threat")
					.Predicator([&]()->_bool {return !m_bAttack && m_bThreat && m_pModel->Find_Animation("AS_em0400_200_AL_atk_a1")->IsFinished(); })
					.Duration(0.2f)
			
			.AddState("DodgeB")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0400_135_AL_dodge_B"))
			
				.AddTransition("DodgeB to Idle", "Idle")
					.Predicator([&]()->_bool {return !m_bDodgeB && m_bIdle && m_pModel->Find_Animation("AS_em0400_135_AL_dodge_B")->IsFinished(); })
					.Duration(0.2f)

				.AddTransition("DodgeB to Run", "Run")
					.Predicator([&]()->_bool {return !m_bDodgeB && m_bRun && m_pModel->Find_Animation("AS_em0400_135_AL_dodge_B")->IsFinished(); })
					.Duration(0.2f)

				.AddTransition("DodgeB to Attack", "Attack")
					.Predicator([&]()->_bool {return !m_bDodgeB && m_bAttack && m_pModel->Find_Animation("AS_em0400_135_AL_dodge_B")->IsFinished(); })
					.Duration(0.2f)

			.AddState("DodgeL")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0400_140_AL_dodge_L"))
			
				.AddTransition("DodgeL to Idle", "Idle")
					.Predicator([&]()->_bool {return !m_bDodgeL && m_bIdle && m_pModel->Find_Animation("AS_em0400_140_AL_dodge_L")->IsFinished(); })
					.Duration(0.2f)

				.AddTransition("DodgeL to Run", "Run")
					.Predicator([&]()->_bool {return !m_bDodgeL && m_bRun && m_pModel->Find_Animation("AS_em0400_140_AL_dodge_L")->IsFinished(); })
					.Duration(0.2f)

				.AddTransition("DodgeL to Attack", "Attack")
					.Predicator([&]()->_bool {return !m_bDodgeL && m_bAttack && m_pModel->Find_Animation("AS_em0400_140_AL_dodge_L")->IsFinished(); })
					.Duration(0.2f)

			.AddState("DodgeR")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0400_145_AL_dodge_R"))
			
				.AddTransition("DodgeR to Idle", "Idle")
					.Predicator([&]()->_bool {return !m_bDodgeR && m_bIdle && m_pModel->Find_Animation("AS_em0400_145_AL_dodge_R")->IsFinished(); })
					.Duration(0.2f)

				.AddTransition("DodgeR to Run", "Run")
					.Predicator([&]()->_bool {return !m_bDodgeR && m_bRun && m_pModel->Find_Animation("AS_em0400_145_AL_dodge_R")->IsFinished(); })
					.Duration(0.2f)

				.AddTransition("DodgeR to Attack", "Attack")
					.Predicator([&]()->_bool {return !m_bDodgeR && m_bAttack && m_pModel->Find_Animation("AS_em0400_145_AL_dodge_R")->IsFinished(); })
					.Duration(0.2f)

			.AddState("Threat")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0400_160_AL_threat"))
			
				.AddTransition("Threat to Run", "Run")
					.Predicator([&]()->_bool {return !m_bThreat && m_bRun && m_pModel->Find_Animation("AS_em0400_160_AL_threat")->IsFinished(); })
					.Duration(0.2f)

			.Build();
	}
	
	m_pASM_Base->SetCurState("Idle");

	list<CAnimation*> AddAnimSocket;
	m_mapAnimSocket.emplace("BuddyLumi_GroundDmgAnim", AddAnimSocket);
	m_mapAnimSocket.emplace("BuddyLumi_AirDmgAnim", AddAnimSocket);
	m_mapAnimSocket.emplace("BuddyLumi_DeadAnim", AddAnimSocket);

	return S_OK;
}

void CBdLm_AnimInstance::Tick(_double TimeDelta)
{
	UpdateTargetState(TimeDelta);

	_bool bChange = CheckFinishedAnimSocket();
	_bool bLocalMove = true;

	if (!m_mapAnimSocket.find("BuddyLumi_GroundDmgAnim")->second.empty())
	{
		auto Socket = m_mapAnimSocket.find("BuddyLumi_GroundDmgAnim")->second.front();
		if (bChange)
		{
			Socket = m_mapAnimSocket.find("BuddyLumi_GroundDmgAnim")->second.front();
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

	else if (!m_mapAnimSocket.find("BuddyLumi_AirDmgAnim")->second.empty())
	{
		auto Socket = m_mapAnimSocket.find("BuddyLumi_AirDmgAnim")->second.front();
		if (bChange)
		{
			Socket = m_mapAnimSocket.find("BuddyLumi_AirDmgAnim")->second.front();
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

	else if (!m_mapAnimSocket.find("BuddyLumi_DeadAnim")->second.empty())
	{
		auto Socket = m_mapAnimSocket.find("BuddyLumi_DeadAnim")->second.front();
		if (bChange)
		{
			Socket = m_mapAnimSocket.find("BuddyLumi_DeadAnim")->second.front();
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

void CBdLm_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	// 해당 애니메이션 스테이트 머신을 작동시키는 객체의 상태를 갱신받는 함수
	CBuddyLumi* pBuddyLumi = static_cast<CBuddyLumi*>(m_pTargetObject);

	m_bPreAir = m_bAir;

	m_bIdle = pBuddyLumi->IsIdle();
	m_bAir = pBuddyLumi->IsAir();
	m_bWalk = pBuddyLumi->IsWalk();
	m_bRun = pBuddyLumi->IsRun();
	m_bAttack = pBuddyLumi->IsAttack();
	m_bThreat = pBuddyLumi->IsThreat();

	// Socket List
	m_bDamage = pBuddyLumi->IsDamage();
	m_bDead = pBuddyLumi->IsDead();
	
	// Dodge 3Direct
	m_bDodgeB = pBuddyLumi->IsDodgeB();
	m_bDodgeL = pBuddyLumi->IsDodgeL();
	m_bDodgeR = pBuddyLumi->IsDodgeR();
}


void CBdLm_AnimInstance::Imgui_RenderState()
{
}

void CBdLm_AnimInstance::InputAnimSocket(const string & strSocName, list<CAnimation*> AnimList)
{
	m_mapAnimSocket[strSocName] = (AnimList);
}

CBdLm_AnimInstance * CBdLm_AnimInstance::Create(CModel * pModel, CGameObject * pGameObject)
{
	CBdLm_AnimInstance*		pInstance = new CBdLm_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CBdLm_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBdLm_AnimInstance::Free()
{
	__super::Free();
	Safe_Release(m_pASM_Base);
}
