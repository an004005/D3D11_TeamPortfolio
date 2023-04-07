#include "stdafx.h"
#include "EM0900_AnimInstance.h"
#include "Model.h"
#include "EM0900.h"

HRESULT CEM0900_AnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));

	m_pASM_Base = CASMBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.SetAnimation(*m_pModel->Find_Animation("AS_em1100_101_AL_wait01"))

			.AddTransition("Idle to Walk_F", "Walk_F")
				.Predicator([this] { return m_bMove && m_eMoveAxis == EBaseAxis::NORTH; })
				.Duration(0.1f)

			.AddTransition("Idle to Walk_L", "Walk_L")
				.Predicator([this] { return m_bMove  && m_eMoveAxis == EBaseAxis::WEST; })
				.Duration(0.1f)

			.AddTransition("Idle to Walk_R", "Walk_R")
				.Predicator([this] { return m_bMove  && m_eMoveAxis == EBaseAxis::EAST; })
				.Duration(0.1f)

			.AddTransition("Idle to Run_Start", "Run_Start")
				.Predicator([this] { return m_bRun; })
				.Duration(0.1f)

		.AddState("Walk_F")
			.SetAnimation(*m_pModel->Find_Animation("AS_em1100_106_AL_walk_loop"))
			.AddTransition("Walk_F to Idle", "Idle")
				.Predicator([this] { return !m_bMove; })
				.Duration(0.1f)

		.AddState("Walk_L")
			.SetAnimation(*m_pModel->Find_Animation("AS_em1100_113_AL_walk_L_loop"))
			.AddTransition("Walk_L to Idle", "Idle")
				.Predicator([this] { return !m_bMove; })
				.Duration(0.1f)

		.AddState("Walk_R")
			.SetAnimation(*m_pModel->Find_Animation("AS_em1100_116_AL_walk_R_loop"))
			.AddTransition("Walk_R to Idle", "Idle")
				.Predicator([this] { return !m_bMove; })
				.Duration(0.1f)

		.AddState("Run_Start")
			.SetAnimation(*m_pModel->Find_Animation("AS_em1100_107_AL_run_start"))
			.AddTransition("Run_Start to Run_Loop", "Run_Loop")
				.Predicator([this] 
				{ 
					return !m_bRun || static_cast<CEM0900*>(m_pTargetObject)->Get_RunStart();
				})
				.Duration(0.1f)

		.AddState("Run_Loop")
			.SetAnimation(*m_pModel->Find_Animation("AS_em1100_108_AL_run_loop"))
			.AddTransition("Run_Loop to Idle", "Idle")
				.Predicator([this] { return !m_bRun; })
				.Duration(0.1f)

		.Build();

	m_mapAnimSocket.insert({ "FullBody",{} });

	return S_OK;
}

void CEM0900_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	CEM0900* pEM1100 = static_cast<CEM0900*>(m_pTargetObject);

	m_bMove = pEM1100->IsMove();
	m_bRun = pEM1100->IsRun();
	m_vMoveAxis = pEM1100->GetMoveAxis();
	m_eMoveAxis = CClientUtils::MoveAxisToBaseEnum(m_vMoveAxis);
}

CEM0900_AnimInstance * CEM0900_AnimInstance::Create(CModel * pModel, CGameObject * pGameObject)
{
	CEM0900_AnimInstance*		pInstance = new CEM0900_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CEM0900_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0900_AnimInstance::Free()
{
	__super::Free();
	Safe_Release(m_pASM_Base);
}
