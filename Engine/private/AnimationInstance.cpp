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

//void CAnimationStateMachine::Tick(_double TimeDelta, _bool bUpdateBone)
//{
//	Assert(m_pCurState != nullptr);
//
//	//IM_LOG(to_string(m_fCurTransitionTime / m_fTransitionDuration).c_str());
//	string szTransitionName = "";
//
//	_bool bFirstChange = true;
//	_int iLoopBreaker = 100;
//	while (--iLoopBreaker)
//	{
//		const _bool bNullAnim = m_pCurState->m_Animation == &CAnimation::s_NullAnimation;
//		_bool bChange = false;
//
//		for (const auto pTransition : m_pCurState->m_Transitions)
//		{
//			if (pTransition->m_Predicator == nullptr)
//			{
//				// 스페어 애니메이션
//				if (nullptr != m_pCurState->m_SpairAnimation)
//					bChange = m_pCurState->m_SpairAnimation->IsFinished();
//				else
//					bChange = m_pCurState->m_Animation->IsFinished();
//				// ~스페어 애니메이션
//			}
//			else
//				bChange = pTransition->m_Predicator();
//
//			szTransitionName = pTransition->m_strName;
//
//			if (bChange)
//			{
//				if (bFirstChange) // 최조 state전환시 이전 상태 저장후 blend에 사용
//				{
//					m_pPreState = m_pCurState;
//
//					// 스페어 애니메이션
//					if (nullptr != m_pCurState->m_SpairAnimation)
//						m_fPreStatePlayAt = m_pCurState->m_SpairAnimation->GetPlayTime();
//					else
//						m_fPreStatePlayAt = m_pCurState->m_Animation->GetPlayTime();
//					// ~스페어 애니메이션
//
//					bFirstChange = false;
//
//					// 종료 이벤트가 있으면 실행
//					if (nullptr != m_pCurState->m_FinishEvent)
//						m_pCurState->m_FinishEvent();
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
//				//				IM_LOG(m_pCurState->m_strName.c_str());
//
//								// 시작 이벤트가 있으면 실행
//				if (nullptr != m_pCurState->m_StartEvent)
//					m_pCurState->m_StartEvent();
//
//				if (nullptr == m_pCurState->m_Animation)
//					return;
//
//				// 스페어 애니메이션
//				if (m_pCurState->m_Animation != &CAnimation::s_NullAnimation)
//					m_pCurState->m_Animation->Reset();
//
//				if (m_pCurState->m_SpairAnimation != nullptr)
//					m_pCurState->m_SpairAnimation->Reset();
//				// ~스페어 애니메이션
//
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
//
//			Assert(bNullAnim == false); // null anim이면 항상 변경해야한다.
//			break;
//		}
//
//		// REPEAT라는 키를 가지는 애니메이션의 경우 그냥 탈출시킨다.
//		if ("REPEAT" == szTransitionName)
//			break;
//
//		if (iLoopBreaker < 10)
//			MSG_BOX("A");
//	}
//
//	Assert(iLoopBreaker > 0); // 무한루프 방치
//
//	if (nullptr != m_pCurState->m_OptionalEvent/* && (m_pCurState->m_bOptionalEvent)*/)
//		m_pCurState->m_bOptionalEvent = m_pCurState->m_OptionalEvent();
//
//	if (m_fCurTransitionTime < m_fTransitionDuration)
//	{
//		// 스페어 애니메이션
//		if (nullptr != m_pPreState->m_SpairAnimation)
//			m_pPreState->m_SpairAnimation->Update_BonesAtTime(m_fPreStatePlayAt, EAnimUpdateType::NORMAL);
//		else
//			m_pPreState->m_Animation->Update_BonesAtTime(m_fPreStatePlayAt, EAnimUpdateType::NORMAL);
//
//
//		if (nullptr != m_pCurState->m_SpairAnimation)
//			m_pCurState->m_SpairAnimation->Update_Bones(TimeDelta, EAnimUpdateType::BLEND, m_fCurTransitionTime / m_fTransitionDuration);
//		else
//			m_pCurState->m_Animation->Update_Bones(TimeDelta, EAnimUpdateType::BLEND, m_fCurTransitionTime / m_fTransitionDuration);
//		// ~스페어 애니메이션
//
//		m_fCurTransitionTime += (_float)TimeDelta;
//
//		m_bLerp = true;
//	}
//	else
//	{
//		if (nullptr != m_pCurState->m_SpairAnimation)
//		{
//			if (m_pCurState->m_SpairAnimation->IsFinished())
//			{
//				m_pCurState->m_SpairAnimation->Reset();
//			}
//
//			m_pCurState->m_SpairAnimation->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
//
//			if (m_pCurState->m_bSpairClearFlag)
//			{
//				m_pCurState->m_SpairAnimation = nullptr;
//				m_pCurState->m_bSpairClearFlag = false;
//				m_fSpairTransitionTime = 0.f;
//			}
//		}
//		// 스페어가 빌 때 보간을 하지 않아도 이상하지 않은가? 그러면 SpairFlag를 사용할 필요가 없어진다.
//		/*else if (m_fSpairTransitionTime < m_fTransitionDuration)
//		{
//			m_pCurState->m_Animation->Update_Bones(TimeDelta, EAnimUpdateType::BLEND, m_fSpairTransitionTime / m_fTransitionDuration);
//			m_fSpairTransitionTime += (_float)TimeDelta;
//
//			m_bLerp = true;
//		}*/
//		else
//		{
//			if (m_pCurState->m_Animation->IsFinished())
//				m_pCurState->m_Animation->Reset();
//
//			m_pCurState->m_Animation->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
//		}
//
//		m_bLerp = false;
//	}
//}

void CAnimationStateMachine::Tick(_double TimeDelta, _bool bUpdateBone)
{
	Assert(m_pCurState != nullptr);

	_bool	bFirstChange = true;
	_int	iLoopBreaker = 100;
	string	szNextStateName = "";
	string	szTransitionName = "";
	_float	fTransitionDuration = 0.f;
	
	while (--iLoopBreaker)
	{
		if (10 >= iLoopBreaker)
		{
			int iA = 0;
		}

		const _bool bNullAnim = m_pCurState->m_Animation == &CAnimation::s_NullAnimation;
		_bool bChange = false;
		szNextStateName = "";
		szTransitionName = "";
		fTransitionDuration = 0.f;
		
		for (const auto pTransition : m_pCurState->m_Transitions)
		{
			// 트랜지션이 없으면 애니메이션 종료로 트랜지션을 잡아줌
			if (pTransition->m_Predicator == nullptr)
			{
				// 스페어 애니메이션
				if (nullptr != m_pCurState->m_SpairAnimation)
					bChange = m_pCurState->m_SpairAnimation->IsFinished();
				else
					bChange = m_pCurState->m_Animation->IsFinished();
				// ~스페어 애니메이션
			}
			else
				bChange = pTransition->m_Predicator();

			if (bChange)
			{
				szNextStateName = pTransition->m_strNextStateName;
				szTransitionName = pTransition->m_strName;
				fTransitionDuration = pTransition->m_fTransitionDuration;
				break;
			}
		}

		if (szNextStateName != "")
		{
			if (bFirstChange) // 최조 state전환시 이전 상태 저장후 blend에 사용
			{
				m_pPreState = m_pCurState;

				// 스페어 애니메이션
				if (nullptr != m_pCurState->m_SpairAnimation)
					m_fPreStatePlayAt = m_pCurState->m_SpairAnimation->GetPlayTime();
				else
					m_fPreStatePlayAt = m_pCurState->m_Animation->GetPlayTime();
				// ~스페어 애니메이션

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
				m_strTransition += szTransitionName;
				m_strTransition += " -> \n";
			}
#endif

			m_pCurState = m_mapStates.find(szNextStateName)->second;
			m_fTransitionDuration = fTransitionDuration;
			m_fCurTransitionTime = 0.f;
//			IM_LOG(m_pCurState->m_strName.c_str());

			if (nullptr != m_pCurState->m_StartEvent)
				m_pCurState->m_StartEvent();

			// 스페어 애니메이션
			if (m_pCurState->m_Animation != nullptr)
				m_pCurState->m_Animation->Reset();

			if (m_pCurState->m_SpairAnimation != nullptr)
			{
				m_pCurState->m_SpairAnimation->Reset();

				if (m_pCurState->m_Animation == m_pCurState->m_SpairAnimation)
					m_pCurState->m_SpairAnimation = nullptr;
			}

			if (m_pCurState->m_ReserveAnimation != nullptr)
			{
				if (m_pCurState->m_SpairAnimation == m_pCurState->m_ReserveAnimation)
				{
					m_pCurState->m_ReserveAnimation = nullptr;
				}
				else if (m_pCurState->m_Animation == m_pCurState->m_ReserveAnimation)
				{
					m_pCurState->m_SpairAnimation = nullptr;
					m_pCurState->m_ReserveAnimation = nullptr;
				}
				else if (m_pCurState->m_Animation != m_pCurState->m_ReserveAnimation)
				{
					_float fRatio = 0.f;
					if (nullptr != m_pCurState->m_SpairAnimation)
					{
						fRatio = m_pCurState->m_SpairAnimation->GetPlayRatio();
					}
					else
					{
						fRatio = m_pCurState->m_Animation->GetPlayRatio();
					}

					m_pCurState->m_SpairAnimation = m_pCurState->m_ReserveAnimation;
					m_pCurState->m_ReserveAnimation = nullptr;

					m_pCurState->m_SpairAnimation->SetPlayRatio(fRatio);
				}
			}
		}
		else
		{
			if (m_fCurTransitionTime < m_fTransitionDuration)
			{
				// 스페어 애니메이션
				if (nullptr != m_pPreState->m_SpairAnimation)
					m_pPreState->m_SpairAnimation->Update_BonesAtTime(m_fPreStatePlayAt, EAnimUpdateType::NORMAL);
				else
					m_pPreState->m_Animation->Update_BonesAtTime(m_fPreStatePlayAt, EAnimUpdateType::NORMAL);


				if (nullptr != m_pCurState->m_SpairAnimation)
					m_pCurState->m_SpairAnimation->Update_Bones(TimeDelta, EAnimUpdateType::BLEND, m_fCurTransitionTime / m_fTransitionDuration);
				else
					m_pCurState->m_Animation->Update_Bones(TimeDelta, EAnimUpdateType::BLEND, m_fCurTransitionTime / m_fTransitionDuration);
				// ~스페어 애니메이션

				m_fCurTransitionTime += (_float)TimeDelta;

				m_bLerp = true;

				break;
			}
			else
			{
				if (nullptr != m_pCurState->m_ReserveAnimation)
				{
					if (m_pCurState->m_SpairAnimation == m_pCurState->m_ReserveAnimation)
					{
t						m_pCurState->m_ReserveAnimation = nullptr;
					}
					else if (m_pCurState->m_Animation != m_pCurState->m_ReserveAnimation)
					{
						_float fRatio = 0.f;
						if (nullptr != m_pCurState->m_SpairAnimation)
						{
							fRatio = m_pCurState->m_SpairAnimation->GetPlayRatio();
						}
						else
						{
							fRatio = m_pCurState->m_Animation->GetPlayRatio();
						}

						m_pCurState->m_ReserveAnimation->SetPlayRatio(fRatio);
						m_pCurState->m_SpairAnimation = m_pCurState->m_ReserveAnimation;
						m_pCurState->m_ReserveAnimation = nullptr;
					}
					else if (m_pCurState->m_Animation == m_pCurState->m_ReserveAnimation)
					{
						//m_pCurState->m_Animation->SetPlayRatio(m_pCurState->m_Animation->GetPlayRatio())
						m_pCurState->m_SpairAnimation = nullptr;
						m_pCurState->m_ReserveAnimation = nullptr;
					}
				}


				if (nullptr != m_pCurState->m_SpairAnimation)
				{
					if (m_pCurState->m_SpairAnimation->IsFinished())
					{
						m_pCurState->m_SpairAnimation->Reset();

						if (nullptr != m_pCurState->m_ReserveAnimation && (m_pCurState->m_Animation != m_pCurState->m_ReserveAnimation))
						{
							// 스페어 애니메이션이 종료되었고 리저브 애니메이션이 존재하며, 일반 모션과 리저브 모션이 다를 경우 스페어 모션을 리저브 모션으로 변경
							m_pCurState->m_SpairAnimation = m_pCurState->m_ReserveAnimation;
							m_pCurState->m_ReserveAnimation = nullptr;
						}
						else if (nullptr != m_pCurState->m_ReserveAnimation && (m_pCurState->m_Animation == m_pCurState->m_ReserveAnimation))
						{
							// 스페어 애니메이션이 종료되었고 리저브 애니메이션이 존재하며, 일반 모션과 리저브 모션이 같을 경우 스페어 모션과 리저브 모션을 전부 제거
							m_pCurState->m_SpairAnimation = nullptr;
							m_pCurState->m_ReserveAnimation = nullptr;
						}
					}
					
					// 스페어 애니메이션과 리저브 애니메이션이 전부 비워진(else if) 경우 일반 애니메이션의 업데이트를 돌리기 위함
					if (nullptr != m_pCurState->m_SpairAnimation)
						m_pCurState->m_SpairAnimation->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
					else
						m_pCurState->m_Animation->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);

				}
				else
				{
					if (m_pCurState->m_Animation->IsFinished())
						m_pCurState->m_Animation->Reset();

					m_pCurState->m_Animation->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
				}

				m_bLerp = false;

				break;
			}
		}
	}

	Assert(iLoopBreaker > 0); // 무한루프 체크
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

_bool CAnimationStateMachine::isLerping()
{
	return (1.f > (m_fCurTransitionTime / m_fTransitionDuration)) ? true : false;
}

void CAnimationStateMachine::SetSpairAnim(const string & stateName, CAnimation * pSpairAnim)
{
	const auto pState = m_mapStates.find(stateName);

	if (pState == m_mapStates.end())
		return;

	if (nullptr == pSpairAnim)
		return;

	if (nullptr == pState->second->m_SpairAnimation && nullptr == pState->second->m_ReserveAnimation)
	{
		pState->second->m_SpairAnimation = pSpairAnim;
	}
	else if (nullptr == pState->second->m_SpairAnimation && nullptr != pState->second->m_ReserveAnimation)
	{
		pState->second->m_ReserveAnimation = pSpairAnim;
	}
	else
	{
		pState->second->m_ReserveAnimation = pSpairAnim;
	}

	//m_fCurTransitionTime = 0.f;
}

void CAnimationStateMachine::ResetSpairAnim()
{
	for (auto& iter : m_mapStates)
	{
		if (m_pCurState == iter.second)
		{
			m_pCurState->m_bSpairClearFlag = true;
			continue;
		}

		iter.second->m_SpairAnimation = nullptr;
	}
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
	const auto iter = m_mapAnimSocket.find(strSocName);

	if (iter == m_mapAnimSocket.end())
		return nullptr;

	if (iter->second.empty())
		return nullptr;

	return iter->second.front();
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
				pAnim->Reset();
				PairSoc.second.pop_front();
				
				// 소켓 이름에 Saperate가 들어갈 경우 전부 비게 되면 false를 반환하여 IDLE상태로 가는 것을 막는다.
				//Netual_Saperate_Animation
				if (PairSoc.second.empty() && PairSoc.first.find("Saperate") != (string::npos))
					return false;

				if (PairSoc.second.empty() && PairSoc.first.find("Kinetic_Air") != (string::npos))
					return true;

				if (PairSoc.second.empty() && PairSoc.first.find("Kinetic_Combo_AnimSocket") != (string::npos))
					return true;

				// 소켓 이름에 Kinetic이 들어갈 경우 전부 비게 되면 false를 반환하여 기존 상태를 유지하도록 한다.
				if (PairSoc.second.empty() && PairSoc.first.find("Kinetic") != (string::npos))
					return false;

				//몬스터
				//if (PairSoc.second.empty() && PairSoc.first.find("Kinetic_Combo_AnimSocket") != (string::npos))
				//	return true;

				return true;
			}
			else if (pAnim->IsLooping() && pAnim->IsFinished())
			{
				pAnim->Reset();
			}
		}
	}
	return false;
}

void CAnimationInstance::Free()
{
	m_mapAnimSocket.clear();
}
