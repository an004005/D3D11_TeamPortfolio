#include "stdafx.h"
#include "EM0400_AnimInstance.h"
#include "Model.h"
#include "EM0400.h"

HRESULT CEM0400_AnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));

	m_pASM_Base = CASMBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0400_101_AL_wait01"))

			.AddTransition("Idle to Walk", "Walk")
				.Predicator([this] { return m_bMove; })
				.Duration(0.1f)

			.AddTransition("Idle to Run", "Run")
				.Predicator([this] { return m_bRun; })
				.Duration(0.1f)

		.AddState("Walk")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0400_106_AL_walk02"))

			.AddTransition("Walk to Idle", "Idle")
				.Predicator([this] { return !m_bMove && !m_bRun; })
				.Duration(0.1f)

			.AddTransition("Walk to Run", "Run")
				.Predicator([this] {return m_bRun; })
				.Duration(0.1f)

		.AddState("Run")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0400_107_AL_run01"))

			.AddTransition("Run to Idle", "Idle")
				.Predicator([this] { return !m_bMove && !m_bRun; })
				.Duration(0.1f)

			.AddTransition("Run to Walk", "Walk")
				.Predicator([this] {return !m_bRun && m_bMove; })
				.Duration(0.1f)

		.Build();

	m_mapAnimSocket.insert({ "FullBody",{} });
	return S_OK;
}

void CEM0400_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	CEM0400* pEM0400 = static_cast<CEM0400*>(m_pTargetObject);


	m_bPreAir = m_bAir;
	m_ePreMoveAxis = m_eMoveAxis;

	m_bAir = !pEM0400->IsOnFloor();
	m_bMove = pEM0400->IsMove();
	m_bRun = pEM0400->IsRun();

	m_vMoveAxis = pEM0400->GetMoveAxis();
	m_eMoveAxis = CClientUtils::MoveAxisToBaseEnum(m_vMoveAxis);

	m_fTurnRemain = pEM0400->GetTurnRemain();
	m_eTurn = CClientUtils::TurnDeltaToEnum(m_fTurnRemain);

}

CEM0400_AnimInstance * CEM0400_AnimInstance::Create(CModel * pModel, CGameObject * pGameObject)
{
	CEM0400_AnimInstance*		pInstance = new CEM0400_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CEM0400_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0400_AnimInstance::Free()
{
	__super::Free();
	Safe_Release(m_pASM_Base);
}
