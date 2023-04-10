#include "stdafx.h"
#include "EM1200_AnimInstance.h"
#include "Model.h"
#include "EM1200.h"

HRESULT CEM1200_AnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));

	m_pModel->Find_Animation("AS_em1200_153_AL_turn_R_loop1")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_em1200_151_AL_turn_L_loop1")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_em1200_157_AL_turn_R_loop2")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_em1200_155_AL_turn_L_loop2")->SetLocalRotation(true);

	m_pASM_Base = CASMBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.SetAnimation(*m_pModel->Find_Animation("AS_em1200_101_AL_wait01"))

			.AddTransition("Idle to Walk", "Walk")
				.Predicator([this] { return !m_bChangePhase && m_bMove; })
				.Duration(0.1f)

			.AddTransition("Idle to Turn_R", "Turn_R")
				.Predicator([this] { return m_eTurn == EBaseTurn::TURN_RIGHT; })
				.Duration(0.1f)

			.AddTransition("Idle to Turn_L", "Turn_L")
				.Predicator([this] { return m_eTurn == EBaseTurn::TURN_LEFT; })
				.Duration(0.1f)

		.AddState("Walk")
			.SetAnimation(*m_pModel->Find_Animation("AS_em1200_106_AL_walk01_loop"))
			.AddTransition("Walk to Idle", "Idle")
				.Predicator([this] { return !m_bMove; })
				.Duration(0.1f)

		.AddState("Turn_R")
			.SetAnimation(*m_pModel->Find_Animation("AS_em1200_153_AL_turn_R_loop1"))
			.AddTransition("Turn_R1 to Idle", "Idle")
				.Predicator([this] { return m_eTurn != EBaseTurn::TURN_RIGHT; })
				.Duration(0.f)

		.AddState("Turn_L")
			.SetAnimation(*m_pModel->Find_Animation("AS_em1200_151_AL_turn_L_loop1"))
			.AddTransition("Turn_L1 to Idle", "Idle")
				.Predicator([this] { return m_eTurn != EBaseTurn::TURN_LEFT; })
				.Duration(0.f)

		.Build();

	m_mapAnimSocket.insert({ "FullBody",{} });

	return S_OK;
}

void CEM1200_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	CEM1200* pEM1200 = static_cast<CEM1200*>(m_pTargetObject);

	m_bMove = pEM1200->IsMove();
	m_bRun = pEM1200->IsRun();
	m_eTurn = pEM1200->GetBaseTurn();
	
	if (m_bChangePhase == false && pEM1200->IsChangePhase() == true)
	{
		Make2PhaseASM();
		m_bChangePhase = true;
	}
}

void CEM1200_AnimInstance::Make2PhaseASM()
{
	Safe_Release(m_pASM_Base);

	m_pASM_Base = CASMBuilder()
		.InitState("Idle")
		.AddState("Idle")
		.SetAnimation(*m_pModel->Find_Animation("AS_em1200_170_AL_wait02"))

		.AddTransition("Idle to Run", "Run")
			.Predicator([this] { return m_bRun; })
			.Duration(0.1f)

		.AddTransition("Idle to Turn_R", "Turn_R")
			.Predicator([this] { return m_eTurn == EBaseTurn::TURN_RIGHT; })
			.Duration(0.2f)

		.AddTransition("Idle to Turn_L", "Turn_L")
			.Predicator([this] { return m_eTurn == EBaseTurn::TURN_LEFT; })
			.Duration(0.2f)

		.AddState("Run")
		.SetAnimation(*m_pModel->Find_Animation("AS_em1200_184_AL_run02_loop"))
			.AddTransition("Run to Idle", "Idle")
			.Predicator([this] { return !m_bRun; })
			.Duration(0.1f)

		.AddState("Turn_R")
		.SetAnimation(*m_pModel->Find_Animation("AS_em1200_157_AL_turn_R_loop2"))
			.AddTransition("Turn_R1 to Idle", "Idle")
			.Predicator([this] { return m_eTurn != EBaseTurn::TURN_RIGHT; })
			.Duration(0.2f)

		.AddState("Turn_L")
		.SetAnimation(*m_pModel->Find_Animation("AS_em1200_155_AL_turn_L_loop2"))
			.AddTransition("Turn_L1 to Idle", "Idle")
			.Predicator([this] { return m_eTurn != EBaseTurn::TURN_LEFT; })
			.Duration(0.2f)

		.Build();

	m_mapAnimSocket.insert({ "FullBody",{} });
}

CEM1200_AnimInstance * CEM1200_AnimInstance::Create(CModel * pModel, CGameObject * pGameObject)
{
	CEM1200_AnimInstance*		pInstance = new CEM1200_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CEM1200_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM1200_AnimInstance::Free()
{
	__super::Free();
	Safe_Release(m_pASM_Base);
}
