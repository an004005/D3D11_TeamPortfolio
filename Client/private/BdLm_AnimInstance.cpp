#include "stdafx.h"
#include "BdLm_AnimInstance.h"
#include "BuddyLumi.h"
#include "Model.h"
#include "Animation.h"

HRESULT CBdLm_AnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(CAnimationInstance::Initialize(pModel, pGameObject));

	m_pASM_Base = CASMBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0400_101_AL_wait01"))

			.AddTransition("Idle to Walk", "Walk")
				.Predicator([this] { return m_bMove; })
				.Duration(0.1f)

			.AddTransition("Idle to Run", "Run")
				.Predicator([this] { return m_bRun; })
				.Duration(0.1f)

		.AddState("Walk")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0400_106_AL_walk02"))

			.AddTransition("Walk to Idle", "Idle")
				.Predicator([this] { return !m_bMove && !m_bRun; })
				.Duration(0.1f)

			.AddTransition("Walk to Run", "Run")
				.Predicator([this] {return m_bRun; })
				.Duration(0.1f)

		.AddState("Run")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0400_107_AL_run01"))

			.AddTransition("Run to Idle", "Idle")
				.Predicator([this] { return !m_bMove && !m_bRun; })
				.Duration(0.1f)

			.AddTransition("Run to Walk", "Walk")
				.Predicator([this] {return !m_bRun && m_bMove; })
				.Duration(0.1f)


		.Build();

	m_mapAnimSocket.insert({ "Buddy", {} });
	return S_OK;
}

void CBdLm_AnimInstance::Tick(_double TimeDelta)
{
	UpdateTargetState(TimeDelta);

	_bool bChange = CheckFinishedAnimSocket();
	_bool bLocalMove = true;

	string szCurAnimName = "";

	// 소켓이 비어있는지 탐색
	list<CAnimation*> CurSocket;

	for (auto& iter : m_mapAnimSocket)
	{
		if (iter.second.empty() == false)
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

	m_pModel->Compute_CombindTransformationMatrix();

	if (bLocalMove)
	{
		_matrix WorldMatrix = m_pTargetObject->GetTransform()->Get_WorldMatrix();
		_vector vLocalMove = m_pModel->GetLocalMove(WorldMatrix);
		m_pTargetObject->GetTransform()->LocalMove(vLocalMove);
	}
}

void CBdLm_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	// 해당 애니메이션 스테이트 머신을 작동시키는 객체의 상태를 갱신받는 함수
	CBuddyLumi* pBuddyLumi = static_cast<CBuddyLumi*>(m_pTargetObject);

	m_bPreAir = m_bAir;
	m_ePreMoveAxis = m_eMoveAxis;

	m_bAir = !pBuddyLumi->IsOnFloor();
	m_bMove = pBuddyLumi->IsMove();
	m_bRun = pBuddyLumi->IsRun();

	m_vMoveAxis = pBuddyLumi->GetMoveAxis();
	m_eMoveAxis = CClientUtils::MoveAxisToBaseEnum(m_vMoveAxis);

	m_fTurnRemain = pBuddyLumi->GetTurnRemain();
	m_eTurn = CClientUtils::TurnDeltaToEnum(m_fTurnRemain);
	
}

void CBdLm_AnimInstance::Imgui_RenderState()
{
	m_pASM_Base->Imgui_RenderState();
}

void CBdLm_AnimInstance::InputAnimSocket(const string & strSocName, list<CAnimation*> AnimList)
{
	for (auto& iter : m_mapAnimSocket)
	{
		if (!iter.second.empty())
			iter.second.front()->Reset();

		list<CAnimation*> SocketList;
		iter.second = SocketList;
	}

	m_mapAnimSocket[strSocName] = (AnimList);
}

void CBdLm_AnimInstance::AttachAnimSocket(const string & strSocName, const list<CAnimation*>& AnimList)
{
	const auto itr = m_mapAnimSocket.find(strSocName);
	Assert(itr != m_mapAnimSocket.end());

	if (!itr->second.empty())
	{
		m_bAttach = true;
		itr->second.front()->Reset();
	}
	m_mapAnimSocket[strSocName] = (AnimList);
}

_bool CBdLm_AnimInstance::isSocketPassby(const string & strSocName, _float fPlayRatio)
{
	Assert(m_mapAnimSocket.find(strSocName) != m_mapAnimSocket.end());

	return m_mapAnimSocket[strSocName].empty() == false
		&& m_mapAnimSocket[strSocName].front()->GetPlayRatio() >= fPlayRatio;
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
	CAnimationInstance::Free();
	Safe_Release(m_pASM_Base);
}
