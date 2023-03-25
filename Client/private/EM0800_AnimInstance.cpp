#include "stdafx.h"
#include "EM0800_AnimInstance.h"
#include "Model.h"
#include "EM0800.h"

HRESULT CEM0800_AnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));

	m_pASM_Base = CASMBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0800_101_AL_wait01"))

			.AddTransition("Idle to Walk_F", "Walk_F")
				.Predicator([this] { return m_bMove && m_eMoveAxis == EBaseAxis::NORTH; })
				.Duration(0.1f)

			.AddTransition("Idle to Walk_L", "Walk_L")
				.Predicator([this] { return m_bMove  && m_eMoveAxis == EBaseAxis::WEST; })
				.Duration(0.1f)

			.AddTransition("Idle to Walk_R", "Walk_R")
				.Predicator([this] { return m_bMove  && m_eMoveAxis == EBaseAxis::EAST; })
				.Duration(0.1f)

			.AddTransition("Idle to Turn_R", "Turn_R")
				.Predicator([this] { return m_eTurn == EBaseTurn::TURN_RIGHT; })
				.Duration(0.1f)

			.AddTransition("Idle to Turn_L", "Turn_L")
				.Predicator([this] { return m_eTurn == EBaseTurn::TURN_LEFT; })
				.Duration(0.1f)

		.AddState("Walk_F")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0800_106_AL_walk01_loop"))
			.AddTransition("Walk_F to Idle", "Idle")
				.Predicator([this] { return !m_bMove; })
				.Duration(0.1f)

		.AddState("Walk_L")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0800_113_AL_walk_L_loop"))
			.AddTransition("Walk_L to Idle", "Idle")
				.Predicator([this] { return !m_bMove; })
				.Duration(0.1f)

		.AddState("Walk_R")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0800_116_AL_walk_R_loop"))
			.AddTransition("Walk_R to Idle", "Idle")
				.Predicator([this] { return !m_bMove; })
				.Duration(0.1f)

		.AddState("Turn_R")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0800_150_AL_turn_R"))
			.AddTransition("Turn_R to Idle", "Idle")
				.Predicator([this] { return m_eTurn != EBaseTurn::TURN_RIGHT; })
				.Duration(0.1f)

		.AddState("Turn_L")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0800_152_AL_turn_L"))
			.AddTransition("Turn_L to Idle", "Idle")
				.Predicator([this] { return m_eTurn != EBaseTurn::TURN_LEFT; })
				.Duration(0.1f)

		.Build();

	m_mapAnimSocket.insert({ "FullBody",{} });
	return S_OK;
}

void CEM0800_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	CEM0800* pEM0800 = static_cast<CEM0800*>(m_pTargetObject);

	m_bMove = pEM0800->IsMove();

	m_vMoveAxis = pEM0800->GetMoveAxis();
	m_eMoveAxis = CClientUtils::MoveAxisToBaseEnum(m_vMoveAxis);

	m_fTurnRemain = pEM0800->GetTurnRemain();
	m_eTurn = CClientUtils::TurnDeltaToEnum(m_fTurnRemain);
}

CEM0800_AnimInstance * CEM0800_AnimInstance::Create(CModel * pModel, CGameObject * pGameObject)
{
	CEM0800_AnimInstance*		pInstance = new CEM0800_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CEM0800_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0800_AnimInstance::Free()
{
	__super::Free();
	Safe_Release(m_pASM_Base);
}
