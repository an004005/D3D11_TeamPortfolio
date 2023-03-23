#include "stdafx.h"
#include "EM0210_AnimInstance.h"
#include "Model.h"
#include "EM0210.h"

HRESULT CEM0210_AnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));

	m_pASM_Base = CASMBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0200_102_AL_wait02"))
			.AddTransition("Idle to Walk", "Walk")
				.Predicator([this] { return m_bMove; })
				.Duration(0.2f)

		.AddState("Walk")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0200_106_AL_walk02"))
			.AddTransition("Walk to Idle", "Idle")
				.Predicator([this] { return !m_bMove && !m_bRun; })
				.Duration(0.2f)

			.AddTransition("Walk to Run", "Run")
				.Predicator([this] {return m_bRun; })
				.Duration(0.2f)

		.AddState("Run")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0200_107_AL_run"))

			.AddTransition("Run to Idle", "Idle")
				.Predicator([this] { return !m_bMove && !m_bRun; })
				.Duration(0.2f)

			.AddTransition("Run to Walk", "Walk")
				.Predicator([this] {return !m_bRun && m_bMove; })
				.Duration(0.2f)

		.Build();

	m_mapAnimSocket.insert({ "FullBody",{} });
	return S_OK;
}

void CEM0210_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	CEM0210* pEnemy = static_cast<CEM0210*>(m_pTargetObject);

	m_bMove = pEnemy->IsMove();
	m_bRun = pEnemy->IsRun();

	m_vMoveAxis = pEnemy->GetMoveAxis();
	m_eMoveAxis = CClientUtils::MoveAxisToBaseEnum(m_vMoveAxis);

	m_fTurnRemain = pEnemy->GetTurnRemain();
	m_eTurn = CClientUtils::TurnDeltaToEnum(m_fTurnRemain);
}

CEM0210_AnimInstance * CEM0210_AnimInstance::Create(CModel * pModel, CGameObject * pGameObject)
{
	CEM0210_AnimInstance*		pInstance = new CEM0210_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CEM0210_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0210_AnimInstance::Free()
{
	__super::Free();
	Safe_Release(m_pASM_Base);
}
