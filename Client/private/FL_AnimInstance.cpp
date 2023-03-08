#include "stdafx.h"
#include "FL_AnimInstance.h"
#include "FlowerLeg.h"
#include "Model.h"
#include "Animation.h"

HRESULT CFL_AnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(CAnimationInstance::Initialize(pModel, pGameObject));

	m_pASM_Base = CASMBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0200_102_AL_wait02"))

			.AddTransition("Idle to Walk", "Walk")
				.Predicator([this] { return m_bMove; })
				.Duration(0.1f)
		
		.AddState("Walk")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0200_106_AL_walk02"))

			.AddTransition("Walk to Idle", "Idle")
				.Predicator([this] { return !m_bMove && !m_bRun; })
				.Duration(0.1f)

			.AddTransition("Walk to Run", "Run")
				.Predicator([this] {return m_bRun; })
				.Duration(0.1f)

		.AddState("Run")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0200_107_AL_run"))

			.AddTransition("Run to Idle", "Idle")
				.Predicator([this] { return !m_bMove && !m_bRun; })
				.Duration(0.1f)

			.AddTransition("Run to Walk", "Walk")
				.Predicator([this] {return !m_bRun && m_bMove; })
				.Duration(0.1f)
		
		.Build();

	m_mapAnimSocket.insert({ "UsingControl", {} });
	return S_OK;
}

void CFL_AnimInstance::Tick(_double TimeDelta)
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

void CFL_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	CFlowerLeg* pFlowerLeg = static_cast<CFlowerLeg*>(m_pTargetObject);

	m_bPreAir = m_bAir;
	m_ePreMoveAxis = m_eMoveAxis;

	m_bAir = !pFlowerLeg->IsOnFloor();
	m_bMove = pFlowerLeg->IsMove();
	m_bRun = pFlowerLeg->IsRun();

	m_vMoveAxis = pFlowerLeg->GetMoveAxis();
	m_eMoveAxis = CClientUtils::MoveAxisToBaseEnum(m_vMoveAxis);

	m_fTurnRemain = pFlowerLeg->GetTurnRemain();
	m_eTurn = CClientUtils::TurnDeltaToEnum(m_fTurnRemain);
}

void CFL_AnimInstance::Imgui_RenderState()
{
	m_pASM_Base->Imgui_RenderState();

}

_bool CFL_AnimInstance::isSocketPassby(const string & strSocName, _float fPlayRatio)
{
	Assert(m_mapAnimSocket.find(strSocName) != m_mapAnimSocket.end());

	return m_mapAnimSocket[strSocName].empty() == false
		&& m_mapAnimSocket[strSocName].front()->GetPlayRatio() >= fPlayRatio;
}

void CFL_AnimInstance::InputAnimSocket(const string & strSocName, list<CAnimation*> AnimList)
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

void CFL_AnimInstance::AttachAnimSocket(const string & strSocName, const list<CAnimation*>& AnimList)
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

CFL_AnimInstance * CFL_AnimInstance::Create(CModel * pModel, CGameObject * pGameObject)
{
	CFL_AnimInstance*		pInstance = new CFL_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CFL_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFL_AnimInstance::Free()
{
	CAnimationInstance::Free();
	Safe_Release(m_pASM_Base);
}
