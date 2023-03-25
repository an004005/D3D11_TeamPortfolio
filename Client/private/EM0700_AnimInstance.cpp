#include "stdafx.h"
#include "EM0700_AnimInstance.h"
#include "Model.h"
#include "EM0700.h"

HRESULT CEM0700_AnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));

	m_pASM_Base = CASMBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0700_107_AL_fly_wait"))

			.AddTransition("Idle to Walk_F", "Walk_F")
				.Predicator([this] { return m_bMove && m_eMoveAxis == EBaseAxis::NORTH; })
				.Duration(0.1f)

			.AddTransition("Idle to Walk_L", "Walk_L")
				.Predicator([this] { return m_bMove  && m_eMoveAxis == EBaseAxis::WEST; })
				.Duration(0.1f)

			.AddTransition("Idle to Walk_R", "Walk_R")
				.Predicator([this] { return m_bMove  && m_eMoveAxis == EBaseAxis::EAST; })
				.Duration(0.1f)

		.AddState("Walk_F")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0700_110_AL_walk_F"))
			.AddTransition("Walk_F to Idle", "Idle")
				.Predicator([this] { return !m_bMove; })
				.Duration(0.1f)

		.AddState("Walk_L")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0700_112_AL_walk_L"))
			.AddTransition("Walk_L to Idle", "Idle")
				.Predicator([this] { return !m_bMove; })
				.Duration(0.1f)

		.AddState("Walk_R")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0700_113_AL_walk_R"))
			.AddTransition("Walk_R to Idle", "Idle")
				.Predicator([this] { return !m_bMove; })
				.Duration(0.1f)

		.Build();

	m_mapAnimSocket.insert({ "FullBody",{} });
	return S_OK;
}

void CEM0700_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	CEM0700* pEM0700 = static_cast<CEM0700*>(m_pTargetObject);

	m_bMove = pEM0700->IsMove();

	m_vMoveAxis = pEM0700->GetMoveAxis();
	m_eMoveAxis = CClientUtils::MoveAxisToBaseEnum(m_vMoveAxis);
}

CEM0700_AnimInstance * CEM0700_AnimInstance::Create(CModel * pModel, CGameObject * pGameObject)
{
	CEM0700_AnimInstance*		pInstance = new CEM0700_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CEM0700_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0700_AnimInstance::Free()
{
	__super::Free();
	Safe_Release(m_pASM_Base);
}
