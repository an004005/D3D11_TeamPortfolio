#include "stdafx.h"
#include "../public/BaseAnimInstance.h"
#include "Player.h"
#include "Model.h"

HRESULT CBaseAnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));

	m_pASM_Base = CASMBuilder()
		.InitState("IDLE")
		.AddState("IDLE")
		.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_002_AL_wait02"))
		.AddTransition("IDLE to WALK", "WALK")
		.Priority(0)
		.Predicator([&]()->_bool { return m_bWalk; })
		.Duration(0.2f)

		.AddState("WALK")
		.SetAnimation(*m_pModel->Find_Animation("AS_ch0100_051_AL_dodge_F_start"))

		.AddTransition("WALK to IDLE", "IDLE")
		.Priority(0)
		.Predicator([&]()->_bool { return !m_bWalk; })
		.Duration(0.2f)

		/*.AddTransition("WALK END", "IDLE")
		.Priority(1)
		.Predicator([&]()->_bool { return m_pASM_Base->GetCurState()->m_Animation->GetPlayRatio() >= 1.f ? true : false; })
		.Duration(0.2f)*/

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
		m_pTargetObject->GetTransform()->LocalMove(m_pModel->GetLocalMove(WorldMatrix));
	}
}

void CBaseAnimInstance::UpdateTargetState(_double TimeDelta)
{
	// 해당 애니메이션 스테이트 머신을 작동시키는 객체의 상태를 갱신받는 함수

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pTargetObject);

	m_bPreAir = m_bAir;

	m_bAir = pPlayer->isAir();
	m_bMove = pPlayer->isMove();
	m_bWalk = pPlayer->isWalk();
}

void CBaseAnimInstance::Imgui_RenderState()
{
}

void CBaseAnimInstance::InputAnimSocket(const string& strSocName, list<CAnimation*> AnimList)
{
	m_mapAnimSocket[strSocName] = (AnimList);
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
