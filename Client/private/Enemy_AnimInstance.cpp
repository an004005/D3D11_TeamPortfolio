#include "stdafx.h"
#include "..\public\Enemy_AnimInstance.h"
#include "Model.h"
#include "Animation.h"
#include "GameObject.h"

HRESULT CEnemy_AnimInstance::Initialize(CModel* pModel, CGameObject* pGameObject)
{
	return CAnimationInstance::Initialize(pModel, pGameObject);
}

void CEnemy_AnimInstance::Tick(_double TimeDelta)
{
	UpdateTargetState(TimeDelta);

	_bool bChange = CheckFinishedAnimSocket();
	_bool bLocalMove = true;

	string szCurAnimName;

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


void CEnemy_AnimInstance::Imgui_RenderState()
{
	m_pASM_Base->Imgui_RenderState();
}

_bool CEnemy_AnimInstance::isSocketPassby(const string& strSocName, _float fPlayRatio)
{
	Assert(m_mapAnimSocket.find(strSocName) != m_mapAnimSocket.end());

	return m_mapAnimSocket[strSocName].empty() == false
		&& m_mapAnimSocket[strSocName].front()->GetPlayRatio() >= fPlayRatio;
}

void CEnemy_AnimInstance::InputAnimSocket(const string& strSocName, list<CAnimation*> AnimList)
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

void CEnemy_AnimInstance::AttachAnimSocket(const string& strSocName, const list<CAnimation*>& AnimList)
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

void CEnemy_AnimInstance::InputAnimSocketOne(const string& strSocName, const string& strAnimName)
{
	for (auto& iter : m_mapAnimSocket)
	{
		if (!iter.second.empty())
			iter.second.front()->Reset();

		list<CAnimation*> SocketList;
		iter.second = SocketList;
	}

	m_mapAnimSocket[strSocName] = { m_pModel->Find_Animation(strAnimName) };
}

void CEnemy_AnimInstance::AttachAnimSocketOne(const string& strSocName, const string& strAnimName)
{
	const auto itr = m_mapAnimSocket.find(strSocName);
	Assert(itr != m_mapAnimSocket.end());

	if (!itr->second.empty())
	{
		m_bAttach = true;
		itr->second.front()->Reset();
	}
	m_mapAnimSocket[strSocName] = { m_pModel->Find_Animation(strAnimName) };
}

void CEnemy_AnimInstance::InputAnimSocket(const string& strSocName, const list<string>& AnimNameList)
{
	for (auto& iter : m_mapAnimSocket)
	{
		if (!iter.second.empty())
			iter.second.front()->Reset();

		list<CAnimation*> SocketList;
		iter.second = SocketList;
	}
	list<CAnimation*> AnimList;
	for (auto& strAnimName : AnimNameList)
		AnimList.push_back(m_pModel->Find_Animation(strAnimName));

	m_mapAnimSocket[strSocName] = AnimList;
}

void CEnemy_AnimInstance::AttachAnimSocket(const string& strSocName, const list<string>& AnimNameList)
{
	const auto itr = m_mapAnimSocket.find(strSocName);
	Assert(itr != m_mapAnimSocket.end());

	if (!itr->second.empty())
	{
		m_bAttach = true;
		itr->second.front()->Reset();
	}
	list<CAnimation*> AnimList;
	for (auto& strAnimName : AnimNameList)
		AnimList.push_back(m_pModel->Find_Animation(strAnimName));

	m_mapAnimSocket[strSocName] = AnimList;
}

const string & CEnemy_AnimInstance::GetCurSocketAnimName()
{
	for (auto& iter : m_mapAnimSocket)
	{
		if (!iter.second.empty())
		{
			return iter.second.front()->GetName();
		}
	}

	return "";
}

void CEnemy_AnimInstance::Free()
{
	CAnimationInstance::Free();
	Safe_Release(m_pASM_Base);
}
