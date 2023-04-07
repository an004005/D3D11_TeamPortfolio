#include "stdafx.h"
#include "FSM_Recompiler.h"

#include "Player.h"
#include "FSMComponent.h"

CFSM_Recompiler::CFSM_Recompiler()
{
}

HRESULT CFSM_Recompiler::Initialize(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;

	return S_OK;
}

void CFSM_Recompiler::Tick()
{
	if (ImGui::CollapsingHeader("HotFixer"))
	{
		// ��ư, üũ�ڽ� �� �̿��Ͽ� �����

		if (ImGui::Button("FSM_Recompile"))
		{
			FSM_Recompile();
		}

	}
	ImGui::CollapsingHeader("~HotFixer");
}

void CFSM_Recompiler::FSM_Recompile()
{
	// ������ FSM�� �ּҸ� ���� �� ����� ���� ������
//	CFSMComponent* pOriginFSM = m_pPlayer->m_pBrainCrashStateMachine;

	// ���� �����Ͽ� ����
//	m_pPlayer->m_pBrainCrashStateMachine = CFSMComponentBuilder()
//		.InitState("BRAINCRASH_NOUSE")

//		.Build();

	// ������ FSM�ּҴ� �Ҵ� ����
//	Safe_Release(pOriginFSM);
}

CFSM_Recompiler* CFSM_Recompiler::Create(CPlayer* pPlayer)
{
	CFSM_Recompiler* pInstance = new CFSM_Recompiler();

	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CFSM_Recompiler");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CFSM_Recompiler::Free()
{
}
