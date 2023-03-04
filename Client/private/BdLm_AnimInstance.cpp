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
					.Duration(0.5f)

				.AddTransition("Idle to Run", "Run")
					.Predicator([&]()->_bool {return m_bRun; })
					.Duration(0.5f)

				.AddTransition("Idle to Attack", "Attack")
					.Predicator([&]()->_bool {return m_bAttack; })
					.Duration(0.5f)

				.AddTransition("Idle to DodgeB", "DodgeB")
					.Predicator([&]()->_bool {return m_bDodgeB; })
					.Duration(0.5f)

				.AddTransition("Idle to DodgeL", "DodgeL")
					.Predicator([&]()->_bool {return m_bDodgeL; })
					.Duration(0.5f)

				.AddTransition("Idle to DodgeR", "DodgeR")
					.Predicator([&]()->_bool {return m_bDodgeR; })
					.Duration(0.5f)
			
			// ~Idle

			.AddState("Walk")		// AS_em0400_414_AL_damage_m_R	AS_em0400_106_AL_walk02
				.SetAnimation(*m_pModel->Find_Animation("AS_em0400_414_AL_damage_m_R"))
				
				.AddTransition("Walk to Idle", "Idle")
					.Predicator([&]()->_bool {return !m_bWalk && m_bIdle && m_pModel->Find_Animation("AS_em0400_414_AL_damage_m_R")->IsFinished(); })
					.Duration(0.5f)

				.AddTransition("Walk to Run", "Run")
					.Predicator([&]()->_bool {return !m_bWalk && m_bRun && m_pModel->Find_Animation("AS_em0400_414_AL_damage_m_R")->IsFinished(); })
					.Duration(0.5f)

				.AddTransition("Walk to Attack", "Attack")
					.Predicator([&]()->_bool {return !m_bWalk && m_bAttack && m_pModel->Find_Animation("AS_em0400_414_AL_damage_m_R")->IsFinished(); })
					.Duration(0.5f)

			.AddState("Run")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0400_107_AL_run01"))

				.AddTransition("Run to Idle", "Idle")
					.Predicator([&]()->_bool {return !m_bRun && m_bIdle; }) //&& m_pModel->Find_Animation("AS_em0400_107_AL_run01")->IsFinished(); })
					.Duration(0.5f)

				.AddTransition("Run to Walk", "Walk")
					.Predicator([&]()->_bool {return !m_bRun && m_bWalk; }) //&& m_pModel->Find_Animation("AS_em0400_107_AL_run01")->IsFinished(); })
					.Duration(0.5f)

				.AddTransition("Run to Attack", "Attack")
					.Predicator([&]()->_bool { return !m_bRun && m_bAttack; }) //&& m_pModel->Find_Animation("AS_em0400_107_AL_run01")->IsFinished(); })
					.Duration(0.5f)

				.AddTransition("Run to DodgeB", "DodgeB")
					.Predicator([&]()->_bool { return !m_bRun && m_bDodgeB; }) //&& m_pModel->Find_Animation("AS_em0400_107_AL_run01")->IsFinished(); })
					.Duration(0.5f)

				.AddTransition("Run to DodgeL", "DodgeL")
					.Predicator([&]()->_bool { return !m_bRun && m_bDodgeL; }) //&& m_pModel->Find_Animation("AS_em0400_107_AL_run01")->IsFinished(); })
					.Duration(0.5f)

				.AddTransition("Run to DodgeR", "DodgeR")
					.Predicator([&]()->_bool { return !m_bRun && m_bDodgeR; }) //&& m_pModel->Find_Animation("AS_em0400_107_AL_run01")->IsFinished(); })
					.Duration(0.5f)

			.AddState("Attack")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0400_200_AL_atk_a1"))
							
				.AddTransition("Attack to Idle", "Idle")
					.Predicator([&]()->_bool {return !m_bAttack && m_bIdle && m_pModel->Find_Animation("AS_em0400_200_AL_atk_a1")->IsFinished(); })
					.Duration(0.5f)

				.AddTransition("Attack to Walk", "Walk")
					.Predicator([&]()->_bool {return !m_bAttack && m_bWalk && m_pModel->Find_Animation("AS_em0400_200_AL_atk_a1")->IsFinished(); })
					.Duration(0.5f)

				.AddTransition("Attack to Run", "Run")
					.Predicator([&]()->_bool {return !m_bAttack && m_bRun && m_pModel->Find_Animation("AS_em0400_200_AL_atk_a1")->IsFinished(); })
					.Duration(0.5f)

				.AddTransition("Attack to Threat", "Threat")
					.Predicator([&]()->_bool {return !m_bAttack && m_bThreat && m_pModel->Find_Animation("AS_em0400_200_AL_atk_a1")->IsFinished(); })
					.Duration(0.5f)
			
			.AddState("DodgeB")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0400_135_AL_dodge_B"))
			
				.AddTransition("DodgeB to Idle", "Idle")
					.Predicator([&]()->_bool {return !m_bDodgeB && m_bIdle && m_pModel->Find_Animation("AS_em0400_135_AL_dodge_B")->IsFinished(); })
					.Duration(0.5f)

				.AddTransition("DodgeB to Walk", "Walk")
					.Predicator([&]()->_bool {return !m_bDodgeB && m_bWalk && m_pModel->Find_Animation("AS_em0400_135_AL_dodge_B")->IsFinished(); })
					.Duration(0.5f)

				.AddTransition("DodgeB to Run", "Run")
					.Predicator([&]()->_bool {return !m_bDodgeB && m_bRun && m_pModel->Find_Animation("AS_em0400_135_AL_dodge_B")->IsFinished(); })
					.Duration(0.5f)

				.AddTransition("DodgeB to Attack", "Attack")
					.Predicator([&]()->_bool {return !m_bDodgeB && m_bAttack && m_pModel->Find_Animation("AS_em0400_135_AL_dodge_B")->IsFinished(); })
					.Duration(0.5f)

			.AddState("DodgeL")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0400_140_AL_dodge_L"))
			
				.AddTransition("DodgeL to Idle", "Idle")
					.Predicator([&]()->_bool {return !m_bDodgeL && m_bIdle && m_pModel->Find_Animation("AS_em0400_140_AL_dodge_L")->IsFinished(); })
					.Duration(0.5f)

				.AddTransition("DodgeL to Walk", "Walk")
					.Predicator([&]()->_bool {return !m_bDodgeL && m_bWalk && m_pModel->Find_Animation("AS_em0400_140_AL_dodge_L")->IsFinished(); })
					.Duration(0.5f)

				.AddTransition("DodgeL to Run", "Run")
					.Predicator([&]()->_bool {return !m_bDodgeL && m_bRun && m_pModel->Find_Animation("AS_em0400_140_AL_dodge_L")->IsFinished(); })
					.Duration(0.5f)

				.AddTransition("DodgeL to Attack", "Attack")
					.Predicator([&]()->_bool {return !m_bDodgeL && m_bAttack && m_pModel->Find_Animation("AS_em0400_140_AL_dodge_L")->IsFinished(); })
					.Duration(0.5f)

			.AddState("DodgeR")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0400_145_AL_dodge_R"))
			
				.AddTransition("DodgeR to Idle", "Idle")
					.Predicator([&]()->_bool {return !m_bDodgeR && m_bIdle && m_pModel->Find_Animation("AS_em0400_145_AL_dodge_R")->IsFinished(); })
					.Duration(0.5f)

				.AddTransition("DodgeR to Walk", "Walk")
					.Predicator([&]()->_bool {return !m_bDodgeR && m_bWalk && m_pModel->Find_Animation("AS_em0400_145_AL_dodge_R")->IsFinished(); })
					.Duration(0.5f)

				.AddTransition("DodgeR to Run", "Run")
					.Predicator([&]()->_bool {return !m_bDodgeR && m_bRun && m_pModel->Find_Animation("AS_em0400_145_AL_dodge_R")->IsFinished(); })
					.Duration(0.5f)

				.AddTransition("DodgeR to Attack", "Attack")
					.Predicator([&]()->_bool {return !m_bDodgeR && m_bAttack && m_pModel->Find_Animation("AS_em0400_145_AL_dodge_R")->IsFinished(); })
					.Duration(0.5f)

			.AddState("Threat")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0400_160_AL_threat"))
			
				.AddTransition("Threat to Run", "Run")
					.Predicator([&]()->_bool {return !m_bThreat && m_bRun && m_pModel->Find_Animation("AS_em0400_160_AL_threat")->IsFinished(); })
					.Duration(0.5f)

				.AddTransition("Threat to Walk", "Walk")
					.Predicator([&]()->_bool {return !m_bThreat && m_bWalk && m_pModel->Find_Animation("AS_em0400_160_AL_threat")->IsFinished(); })
					.Duration(0.5f)

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
	_bool vAirEnd = bChange && m_bAir;	// 소켓이 비었고 공중 상태인지 확인 

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
			m_pModel->SetCurAnimName(Socket->GetName());
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
			m_bAttach = false;
		}
		else
		{
			szCurAnimName = Socket->GetName();
			Socket->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
		}		
	}
	else if (bChange)
	{
		bLocalMove = false;
			
		m_pASM_Base->SetCurState("Idle");

		//m_pASM_Base->GetCurState()->m_Animation->Reset();
		m_pModel->SetCurAnimName(m_pASM_Base->GetCurState()->m_Animation->GetName());
		m_fLerpTime = 0.f;
	}
	else if (m_fLerpTime < m_fLerpDuration)
	{
		m_pASM_Base->GetCurState()->m_Animation->Update_Bones(TimeDelta, EAnimUpdateType::BLEND, m_fLerpTime / m_fLerpDuration);
		m_fLerpTime += (_float)TimeDelta;
	}
	else
	{
		m_pASM_Base->Tick(TimeDelta);
		m_pModel->SetCurAnimName(m_pASM_Base->GetCurState()->m_Animation->GetName());
	}

	// 전체 본 마스킹을 할지, 상체에 대해서만 본 마스킹을 할지는 플레이어에서 던져주는 값에 따라 정해지도록 한다.

	m_pModel->Compute_CombindTransformationMatrix();

	if (bLocalMove)
	{
		_matrix WorldMatrix = m_pTargetObject->GetTransform()->Get_WorldMatrix();
		_vector vLocalMove = m_pModel->GetLocalMove(WorldMatrix);
		m_pTargetObject->GetTransform()->LocalMove(vLocalMove);

		if (0.f != XMVectorGetX(XMVector3Length(vLocalMove)))
			m_vLocalMove = vLocalMove;
	}

	if ("" != szCurAnimName)
	{
		_matrix WorldMatrix = m_pTargetObject->GetTransform()->Get_WorldMatrix();
		_vector vLocalMove = m_pModel->GetLocalMove(WorldMatrix, szCurAnimName);	
	}
		
	// 기존
	/*if (!m_mapAnimSocket.find("BuddyLumi_GroundDmgAnim")->second.empty())
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
	}*/

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

	// ASM Control
	m_bStatic = pBuddyLumi->IsStatic();

	m_bLerp = m_pASM_Base->isLerping();
}

void CBdLm_AnimInstance::Imgui_RenderState()
{
}

void CBdLm_AnimInstance::InputAnimSocket(const string & strSocName, list<CAnimation*> AnimList)
{
	//m_mapAnimSocket[strSocName] = (AnimList);

	for (auto& iter : m_mapAnimSocket)
	{
		if (!iter.second.empty())
			iter.second.front()->Reset();

		list<CAnimation*> SocketList;
		iter.second = SocketList;
	}

	m_mapAnimSocket[strSocName] = (AnimList);
}

void CBdLm_AnimInstance::AttachAnimSocket(const string & strSocName, list<CAnimation*> AnimList)
{
	// 소켓의 애니메이션을 교환하고 보간함, 아닐 경우 그냥 덮어버림
	const auto List = m_mapAnimSocket.find(strSocName);

	if (List != m_mapAnimSocket.end())
	{
		if (!List->second.empty())
		{
			m_bAttach = true;
			List->second.front()->Reset();;
		}
		m_mapAnimSocket[strSocName] = (AnimList);
	}
}

_bool CBdLm_AnimInstance::isSocketAlmostFinish(const string & strSocName)
{
	return (m_mapAnimSocket[strSocName].size() == 1) && (m_mapAnimSocket[strSocName].front()->GetPlayRatio() >= 0.95f);
}

_bool CBdLm_AnimInstance::isSocketPassby(const string & strSocName, _float fPlayRatio)
{
	return (m_mapAnimSocket[strSocName].size() == 1) && (m_mapAnimSocket[strSocName].front()->GetPlayRatio() >= fPlayRatio);
}

_bool CBdLm_AnimInstance::CheckAnim(const string & szAnimName)
{
	return  (szAnimName == m_pModel->GetPlayAnimation()->GetName()) ? true : false;
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
