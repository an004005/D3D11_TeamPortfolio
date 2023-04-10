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
		// 버튼, 체크박스 등 이용하여 재생성

		if (ImGui::Button("FSM_Recompile"))
		{
			FSM_Recompile();
		}

	}
	ImGui::CollapsingHeader("~HotFixer");
}

void CFSM_Recompiler::FSM_Recompile()
{
	// 변경할 FSM의 주소를 변경 후 지우기 위해 가져옴
//	CFSMComponent* pOriginFSM = m_pPlayer->m_pBrainCrashStateMachine;

	// 새로 생성하여 대입
//	m_pPlayer->m_pBrainCrashStateMachine = CFSMComponentBuilder()
//		.InitState("BRAINCRASH_NOUSE")

//		.Build();

	// 이전의 FSM주소는 할당 해제
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
