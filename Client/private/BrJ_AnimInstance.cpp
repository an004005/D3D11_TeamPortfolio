#include "stdafx.h"
#include "..\public\BrJ_AnimInstance.h"
#include "BronJon.h"
#include "Model.h"
#include "Animation.h"

HRESULT CBrJ_AnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));

	{
		m_pASM_Base = CASMBuilder()
			.InitState("Idle")
			.AddState("Idle")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0800_101_AL_wait01"))

				.AddTransition("Idle to BiteAtk", "BiteAtk")
					.Predicator([&]()->_bool {return !m_bIdle && m_bBiteAtk; })
					.Duration(0.4f)

			.AddState("BiteAtk")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0810_201_AL_atk_a1_bite"))
				
				.AddTransition("BiteAtk to Idle", "Idle")
					.Predicator([&]()->_bool {return !m_bBiteAtk && m_bIdle; })
					.Duration(0.4f)

			.Build();
		
		// Atk Start : AS_em0800_214_AL_atk_a7_laser2_start
		// Atk Loop : 
	}

	m_pASM_Base->SetCurState("Idle");

	list<CAnimation*> AddAnimSocket;
	m_mapAnimSocket.emplace("BronJon_GroundDmgAnim", AddAnimSocket);
	m_mapAnimSocket.emplace("BronJon_DeadAnim", AddAnimSocket);
			
	return S_OK;
}

void CBrJ_AnimInstance::Tick(_double TimeDelta)
{
	UpdateTargetState(TimeDelta);

	if (!m_bStatic)
	{
		_bool bChange = CheckFinishedAnimSocket();
		_bool bLocalMove = true;

		if (!m_mapAnimSocket.find("BronJon_GroundDmgAnim")->second.empty())
		{
			auto Socket = m_mapAnimSocket.find("BronJon_GroundDmgAnim")->second.front();
			if (bChange)
			{
				Socket = m_mapAnimSocket.find("BronJon_GroundDmgAnim")->second.front();
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

		else if (!m_mapAnimSocket.find("BronJon_DeadAnim")->second.empty())
		{
			auto Socket = m_mapAnimSocket.find("BronJon_DeadAnim")->second.front();
			if (bChange)
			{
				Socket = m_mapAnimSocket.find("BronJon_DeadAnim")->second.front();
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

void CBrJ_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	CBronJon* pBronJon = static_cast<CBronJon*>(m_pTargetObject);

	m_bIdle = pBronJon->IsIdle();

	m_bMoveF = pBronJon->IsMoveF();
	m_bMoveB = pBronJon->IsMoveB();
	m_bMoveL = pBronJon->IsMoveL();
	m_bMoveR = pBronJon->IsMoveR();

	m_bLaserAtk = pBronJon->IsLaserAtk();
	m_bBiteAtk = pBronJon->IsBiteAtk();
	m_bThreat = pBronJon->IsThreat();

	m_bDamage = pBronJon->IsDamage();
	m_bDead = pBronJon->IsDead();

	m_bDodgeB = pBronJon->IsDodgeB();
	m_bDodgeL = pBronJon->IsDodgeL();
	m_bDodgeR = pBronJon->IsDodgeR();
}

void CBrJ_AnimInstance::Imgui_RenderState()
{
}

void CBrJ_AnimInstance::InputAnimSocket(const string & strSocName, list<CAnimation*> AnimList)
{
	m_mapAnimSocket[strSocName] = (AnimList);
}

CBrJ_AnimInstance * CBrJ_AnimInstance::Create(CModel * pModel, CGameObject * pGameObject)
{
	CBrJ_AnimInstance*		pInstance = new CBrJ_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CBrJ_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBrJ_AnimInstance::Free()
{
	__super::Free();
	Safe_Release(m_pASM_Base);
}
