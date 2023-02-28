#include "stdafx.h"
#include "..\public\AnimationInstance.h"

CAnimTransition::CAnimTransition(const string& strName, const string& strNextStateName, _float fTransitionDuration,
	const std::function<_bool()>& Predicator, _uint Priority): m_strName(strName),
	                                                           m_strNextStateName(strNextStateName),
	                                                           m_fTransitionDuration(fTransitionDuration),
	                                                           m_Predicator(Predicator),
	                                                           m_iPriority(Priority)
{}


CAnimationStateMachine::CAnimationStateMachine()
{
}

HRESULT CAnimationStateMachine::Initialize(CASMBuilder * pBuilder)
{
	m_szCurStateName = pBuilder->GetInitAnimStateName();

	for (auto& Pair : pBuilder->GetAnimState())
	{
		m_mapStates.emplace(Pair.first, Pair.second);
	}

	m_pCurState = &pBuilder->GetInitAnimState();

	return S_OK;
}

void CAnimationStateMachine::Tick(_double TimeDelta, _bool bUpdateBone)
{
	Assert(m_pCurState != nullptr);

	//IM_LOG(to_string(m_fCurTransitionTime / m_fTransitionDuration).c_str());
	string szTransitionName = "";

	_bool bFirstChange = true;
	_int iLoopBreaker = 100;
	while (--iLoopBreaker)
	{
		const _bool bNullAnim = m_pCurState->m_Animation == &CAnimation::s_NullAnimation;
		_bool bChange = false;

		for (const auto pTransition : m_pCurState->m_Transitions)
		{
			if (pTransition->m_Predicator == nullptr)
				bChange = m_pCurState->m_Animation->IsFinished();
			else
				bChange = pTransition->m_Predicator();

			szTransitionName = pTransition->m_strName;

			if (bChange)
			{
				if (bFirstChange) // 최조 state전환시 이전 상태 저장후 blend에 사용
				{
					m_pPreState = m_pCurState;

					m_fPreStatePlayAt = m_pCurState->m_Animation->GetPlayTime();
					bFirstChange = false;
					
					// 종료 이벤트가 있으면 실행
					if (nullptr != m_pCurState->m_FinishEvent)
						m_pCurState->m_FinishEvent();
#ifdef _DEBUG
					if (m_bStoreHistory)
					{
						m_strFromState = m_pPreState->m_strName;
						m_strTransition = "";
					}
#endif
				}
#ifdef _DEBUG
				if (m_bStoreHistory)
				{
					m_strTransition += pTransition->m_strName;
					m_strTransition += " -> \n";
				}
#endif

				m_pCurState = m_mapStates.find(pTransition->m_strNextStateName)->second;
				m_fTransitionDuration = pTransition->m_fTransitionDuration;
				m_fCurTransitionTime = 0.f;
				IM_LOG(m_pCurState->m_strName.c_str());

				// 시작 이벤트가 있으면 실행
				if (nullptr != m_pCurState->m_StartEvent)
					m_pCurState->m_StartEvent();

				if (nullptr == m_pCurState->m_Animation)
					return;

				if (m_pCurState->m_Animation != &CAnimation::s_NullAnimation)
					m_pCurState->m_Animation->Reset();

				break;
			}
		}

		// state 변경이 없으면 종료
		if (bChange == false)
		{
#ifdef _DEBUG
			if (!bFirstChange && m_bStoreHistory)
			{
				m_strToState = m_pCurState->m_strName;
				m_DebugQue.push_front("from " + m_strFromState + " | by " + m_strTransition + " | to " + m_strToState);

				if (m_DebugQue.size() > m_iDebugQueSize)
					m_DebugQue.pop_back();
				m_bStoreHistory = false;
			}
#endif

			Assert(bNullAnim == false); // null anim이면 항상 변경해야한다.
			break;
		}

		// REPEAT라는 키를 가지는 애니메이션의 경우 그냥 탈출시킨다.
		if ("REPEAT" == szTransitionName)
			break;
	}
	Assert(iLoopBreaker > 0); // 무한루프 방치

	if (nullptr != m_pCurState->m_OptionalEvent/* && (m_pCurState->m_bOptionalEvent)*/)
		m_pCurState->m_bOptionalEvent = m_pCurState->m_OptionalEvent();

	if (m_fCurTransitionTime < m_fTransitionDuration)
	{
		m_pPreState->m_Animation->Update_BonesAtTime(m_fPreStatePlayAt, EAnimUpdateType::NORMAL);

		m_pCurState->m_Animation->Update_Bones(TimeDelta, EAnimUpdateType::BLEND, m_fCurTransitionTime / m_fTransitionDuration);

		m_fCurTransitionTime += (_float)TimeDelta;

		m_bLerp = true;
	}
	else
	{
		if (m_pCurState->m_Animation->IsFinished())
			m_pCurState->m_Animation->Reset();

		m_pCurState->m_Animation->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);

		m_bLerp = false;
	}
}

CAnimState* CAnimationStateMachine::GetState(const string& stateName)
{
	auto itr = m_mapStates.find(stateName);
	if (itr != m_mapStates.end())
		return itr->second;
	return nullptr;
}

void CAnimationStateMachine::Imgui_RenderState()
{
	ImGui::Text("%s Anim State : %s", m_strName.c_str(), m_pCurState->m_strName.c_str());


#ifdef _DEBUG
	if (ImGui::BeginListBox("Anim State History"))
	{
		for (const auto& e : m_DebugQue)
			ImGui::Selectable(e.c_str());
		ImGui::EndListBox();
	}
	_int iInput = static_cast<_int>(m_iDebugQueSize);
	ImGui::InputInt("Debug History Size(unsigned)", &iInput);
	if (iInput >= 0) m_iDebugQueSize = iInput;
	m_bStoreHistory = true;
#endif
}

CAnimationStateMachine * CAnimationStateMachine::Create(CASMBuilder * pBuilder)
{
	CAnimationStateMachine* pASM = new CAnimationStateMachine;
	FAILED_CHECK(pASM->Initialize(pBuilder));
	return pASM;
}

HRESULT CAnimationInstance::Initialize(CModel* pModel, CGameObject* pTargetObject)
{
	// 상호참조 방지용으로 addref는 하지 않는다.
	Assert(pModel != nullptr);
	Assert(pTargetObject != nullptr);
	m_pModel = pModel;
	m_pTargetObject = pTargetObject;
	return S_OK;
}

void CAnimationInstance::PlayAnimationSocket(const string& strSocName, CAnimation* pAnimation, _float fPlayAt)
{
	Assert(pAnimation != nullptr);
	const auto& SocketPair = m_mapAnimSocket.find(strSocName);
	Assert(SocketPair != m_mapAnimSocket.end());

	if (SocketPair->second.front())
	{
		SocketPair->second.front()->SetFinish();
	}

	SocketPair->second.front() = pAnimation;
	SocketPair->second.front()->Reset();
	SocketPair->second.front()->SetPlayTime((_double)fPlayAt);
}

void CAnimationInstance::StopAnimationSocket(const string& strSocName)
{
	const auto& SocketPair = m_mapAnimSocket.find(strSocName);
	Assert(SocketPair != m_mapAnimSocket.end());
	if (SocketPair->second.front())
	{
		SocketPair->second.front()->SetPlayTime(0.0);
		SocketPair->second.front()->SetFinish();
	}
	SocketPair->second.front() = nullptr;
}

CAnimation* CAnimationInstance::GetSocketAnimation(const string& strSocName)
{
	return m_mapAnimSocket.find(strSocName)->second.front();
}

_bool CAnimationInstance::CheckFinishedAnimSocket()
{
	for (auto& PairSoc : m_mapAnimSocket)
	{
		if (PairSoc.second.empty())
			continue;

		if (auto pAnim = PairSoc.second.front())
		{
			if (!pAnim->IsLooping() && pAnim->IsFinished())
			{
				PairSoc.second.pop_front();
				return true;
			}
		}
	}
	return false;
}

void CAnimationInstance::Free()
{
	m_mapAnimSocket.clear();
}
