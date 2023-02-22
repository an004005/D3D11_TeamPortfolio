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
//	Assert(m_pCurState != nullptr);
//
//	_bool bFirstChange = true;
//	_int iLoopBreaker = 100;
//	while(--iLoopBreaker)
//	{
//		_bool bChange = false;
//
//		for (const auto pTransition : m_pCurState->m_Transitions)
//		{
//			bChange = pTransition->m_Predicator();
//
//			if (bChange)	// 애니메이션이 바로 변경되는 시점
//			{
//				if (bFirstChange) // 최조 state전환시 이전 상태 저장후 blend에 사용
//				{
//					m_pPreState = m_pCurState;
//					bFirstChange = false;
//					m_fBlend = 0.f;
//#ifdef _DEBUG
//					if (m_bStoreHistory)
//					{
//						m_strFromState = m_pPreState->m_strName;
//						m_strTransition = "";
//					}
//#endif
//				}
//#ifdef _DEBUG
//				if (m_bStoreHistory)
//				{
//					m_strTransition += pTransition->m_strName;
//					m_strTransition += " -> \n";
//				}
//#endif
//
//				m_pCurState = m_mapStates.find(pTransition->m_strNextStateName)->second;
//				m_fTransitionDuration = pTransition->m_fTransitionDuration;
//				m_fCurTransitionTime = 0.f;
//
//				break;
//			}
//			else if (m_fBlend < 1.f)		// 애니메이션 사이 보간
//			{
//				m_pCurState->m_Animation->Update_Bones(TimeDelta, EAnimUpdateType::BLEND, m_fBlend);
//				m_fBlend += m_fTransitionDuration;
//				break;
//			}
//			else		// 그냥 애니메이션 실행
//			{
//				m_pCurState->m_Animation->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
//				break;
//			}
//		}
//
//		// state 변경이 없으면 종료
//		if (bChange == false)
//		{
//#ifdef _DEBUG
//			if (!bFirstChange && m_bStoreHistory)
//			{
//				m_strToState = m_pCurState->m_strName;
//				m_DebugQue.push_front("from " + m_strFromState + " | by " + m_strTransition + " | to " + m_strToState);
//
//				if (m_DebugQue.size() > m_iDebugQueSize)
//					m_DebugQue.pop_back();
//				m_bStoreHistory = false;
//			}
//#endif
//			break;
//		}
//	}
//	Assert(iLoopBreaker > 0); // 무한루프 방치
//
//	if (m_fCurTransitionTime < m_fTransitionDuration)
//	{
//		m_fCurTransitionTime += (_float)TimeDelta;
//	}
	Assert(m_pCurState != nullptr);

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

			if (bChange)
			{
				if (bFirstChange) // 최조 state전환시 이전 상태 저장후 blend에 사용
				{
					m_pPreState = m_pCurState;
					m_fPreStatePlayAt = m_pCurState->m_Animation->GetPlayTime();
					bFirstChange = false;
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
	}
	Assert(iLoopBreaker > 0); // 무한루프 방치

	if (m_fCurTransitionTime < m_fTransitionDuration)
	{
		m_pPreState->m_Animation->Update_BonesAtTime(m_fPreStatePlayAt, EAnimUpdateType::NORMAL, bUpdateBone);

		m_pCurState->m_Animation->Update_Bones(TimeDelta, EAnimUpdateType::BLEND, m_fCurTransitionTime / m_fTransitionDuration);

		m_fCurTransitionTime += (_float)TimeDelta;
	}
	else
	{
		m_pCurState->m_Animation->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL, bUpdateBone);
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
	//Safe_Delete(pBuilder);
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

	if (SocketPair->second)
	{
		SocketPair->second->SetFinish();
	}

	SocketPair->second = pAnimation;
	SocketPair->second->Reset();
	SocketPair->second->SetPlayTime((_double)fPlayAt);
}

void CAnimationInstance::StopAnimationSocket(const string& strSocName)
{
	const auto& SocketPair = m_mapAnimSocket.find(strSocName);
	Assert(SocketPair != m_mapAnimSocket.end());
	if (SocketPair->second)
	{
		SocketPair->second->SetPlayTime(0.0);
		SocketPair->second->SetFinish();
	}
	SocketPair->second = nullptr;
}

CAnimation* CAnimationInstance::GetSocketAnimation(const string& strSocName)
{
	return m_mapAnimSocket.find(strSocName)->second;
}

void CAnimationInstance::CheckFinishedAnimSocket()
{
	for (auto& PairSoc : m_mapAnimSocket)
	{
		if (auto pAnim = PairSoc.second)
		{
			if (!pAnim->IsLooping() && pAnim->IsFinished())
			{
				PairSoc.second = nullptr;
			}
		}
	}
}

void CAnimationInstance::Free()
{
	m_mapAnimSocket.clear();
}
