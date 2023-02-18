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
	// �ٸ� state�� ���̰� �Ұ����� �� ���� �ݺ��Ͽ� ����
	while (--iLoopBreaker)
	{
		const auto CurItr = m_mapState.find(m_pCurStateName);
		Assert(CurItr != m_mapState.end());

		const FSM_STATE& tCurState = *CurItr->second;
		const char* pNextStateName = nullptr;

		// �� ���¿��� ���� ���·� �̵� �� �� �ִ��� Ȯ��
		for (const auto pTransition : tCurState.Transitions)
		{
			if (pTransition->Predicator())
			{
				pNextStateName = pTransition->pNextStateName;
				m_pLastTransitionName = pTransition->pTransitionName;
				break;
			}
		}

		// ���� ���� ��� �߰� �ϸ�
		if (pNextStateName)
		{
			const auto NextItr = m_mapState.find(pNextStateName);
			Assert(NextItr != m_mapState.end());
			const FSM_STATE& tNextState = *NextItr->second;

			if (tCurState.OnExit != nullptr) // �� ��� ����
				tCurState.OnExit();

			if (tNextState.OnStart != nullptr) // ���� ��� ����
				tNextState.OnStart();
#ifdef _DEBUG
			StateHistoryUpdate(tNextState.pStateName);
#endif

			m_pCurStateName = pNextStateName;
		}
		else // ���� ���� �߰� ���ϸ� tick�ϰ� ����
		{
			if (tCurState.Tick != nullptr)
				tCurState.Tick(TimeDelta);
			break;
		}
	}

	Assert(iLoopBreaker > 0); // ���ѷ��� üũ
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
	// Imgui_RenderProperty�� ���� �� ���� ���� ������ ����Ѵ�.
	if (m_bStoreHistory) // ����׿�
	{
		std::stringstream ss;
		ss << m_pLastTransitionName << " => " << pNextStateName;
		m_strDebugQue.push_front(ss.str());

		if (m_strDebugQue.size() > m_iDebugQueSize)
			m_strDebugQue.pop_back();

		m_bStoreHistory = false;
	}
}
