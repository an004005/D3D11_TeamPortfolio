#include "stdafx.h"
#include "..\public\FSMComponent.h"
#include "GameUtils.h"
#include <sstream>

/*-------------------
 *    CFSMComponent
 --------------------*/
CFSMComponent::CFSMComponent()
{
}

void CFSMComponent::Tick(_double TimeDelta)
{
	int iLoopBreaker = 100;
	// 다른 state로 전이가 불가해질 때 가지 반복하여 전이
	while (--iLoopBreaker)
	{
		const auto CurItr = m_mapState.find(m_pCurStateName);
		Assert(CurItr != m_mapState.end());

		const FSM_STATE& tCurState = *CurItr->second;
		const char* pNextStateName = nullptr;

		// 현 상태에서 다음 상태로 이동 할 수 있는지 확인
		for (const auto pTransition : tCurState.Transitions)
		{
			if (pTransition->Predicator())
			{
				pNextStateName = pTransition->pNextStateName;
				m_pLastTransitionName = pTransition->pTransitionName;
				break;
			}
		}

		// 다음 상태 노드 발견 하면
		if (pNextStateName)
		{
			const auto NextItr = m_mapState.find(pNextStateName);
			Assert(NextItr != m_mapState.end());
			const FSM_STATE& tNextState = *NextItr->second;

			if (tCurState.OnExit != nullptr) // 현 노드 종료
				tCurState.OnExit();

			if (tNextState.OnStart != nullptr) // 다음 노드 시작
				tNextState.OnStart();
#ifdef _DEBUG
			StateHistoryUpdate(tNextState.pStateName);
#endif

			m_pCurStateName = pNextStateName;
		}
		else // 다음 상태 발견 못하면 tick하고 종료
		{
			if (tCurState.Tick != nullptr)
				tCurState.Tick(TimeDelta);
			break;
		}
	}

	Assert(iLoopBreaker > 0); // 무한루프 체크
}

HRESULT CFSMComponent::Initialize(CFSMComponentBuilder* pBuilder)
{
	m_pCurStateName = pBuilder->GetInitStateName();

	for (auto& Pair : pBuilder->GetStates())
		m_mapState.emplace(Pair.first, Pair.second);

	return S_OK;
}

CFSMComponent* CFSMComponent::Create(CFSMComponentBuilder* pBuilder)
{
	auto pFSM = new CFSMComponent;
	FAILED_CHECK(pFSM->Initialize(pBuilder));
	return pFSM;
}

void CFSMComponent::Free()
{
	for (auto& PairState : m_mapState)
		Safe_Release(PairState.second);
	m_mapState.clear();
}

void CFSMComponent::Imgui_RenderProperty()
{
	// char szCurNode[256];
	ImGui::Text("Cur State : %s", m_pCurStateName);

	if (ImGui::BeginListBox("Node Transition History"))
	{
		for (const auto& e : m_strDebugQue)
			ImGui::Selectable(e.c_str());
		ImGui::EndListBox();
	}

	_int iInput = static_cast<_int>(m_iDebugQueSize);
	ImGui::InputInt("Debug History Size(unsigned)", &iInput);
	if (iInput >= 0) m_iDebugQueSize = iInput;

	m_bStoreHistory = true;
}

void CFSMComponent::StateHistoryUpdate(const char* pNextStateName)
{
	// Imgui_RenderProperty가 실행 될 때만 상태 변경을 기록한다.
	if (m_bStoreHistory) // 디버그용
	{
		std::stringstream ss;
		ss << m_pLastTransitionName << " => " << pNextStateName;
		m_strDebugQue.push_front(ss.str());

		if (m_strDebugQue.size() > m_iDebugQueSize)
			m_strDebugQue.pop_back();

		m_bStoreHistory = false;
	}
}
