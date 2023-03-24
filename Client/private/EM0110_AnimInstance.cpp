#include "stdafx.h"
#include "EM0110_AnimInstance.h"
#include "Model.h"
#include "EM0110.h"

HRESULT CEM0110_AnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));

	m_pASM_Base = CASMBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0100_151_AL_wait01"))
			.AddTransition("Idle to Walk", "Walk")
				.Predicator([this] 
				{
					return m_bMove && m_eMoveAxis == EBaseAxis::NORTH;
				})
				.Duration(0.2f)

			.AddTransition("Idle to Walk_B", "Walk_B")
				.Predicator([this]
				{
					return m_bMove && m_eMoveAxis == EBaseAxis::SOUTH;
				})
				.Duration(0.2f)
			.AddTransition("Idle to Run", "Run")
				.Predicator([this]	{ return m_bRun; })
				.Duration(0.2f)

		.AddState("Walk")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0100_155_AL_walk"))
			.AddTransition("Walk to Idle", "Idle")
				.Predicator([this] { return !m_bMove && !m_bRun; })
				.Duration(0.2f)

			.AddTransition("Walk to Run", "Run")
				.Predicator([this] {return m_bRun; })
				.Duration(0.2f)

		.AddState("Walk_B")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0100_158_AL_walk_B"))
			.AddTransition("Walk_B to Idle", "Idle")
				.Predicator([this] { return !m_bMove && !m_bRun; })
				.Duration(0.2f)

			.AddTransition("Walk_B to Run", "Run")
				.Predicator([this] {return m_bRun; })
				.Duration(0.2f)

		.AddState("Run")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0100_156_AL_run"))

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

void CEM0110_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	CEM0110* pEnemy = static_cast<CEM0110*>(m_pTargetObject);

	m_bMove = pEnemy->IsMove();
	m_bRun = pEnemy->IsRun();

	m_vMoveAxis = pEnemy->GetMoveAxis();
	m_eMoveAxis = CClientUtils::MoveAxisToBaseEnum(m_vMoveAxis);
}

CEM0110_AnimInstance * CEM0110_AnimInstance::Create(CModel * pModel, CGameObject * pGameObject)
{
	CEM0110_AnimInstance*		pInstance = new CEM0110_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CEM0110_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0110_AnimInstance::Free()
{
	__super::Free();
	Safe_Release(m_pASM_Base);
}
