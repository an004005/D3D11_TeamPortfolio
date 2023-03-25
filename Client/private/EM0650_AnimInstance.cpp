#include "stdafx.h"
#include "EM0650_AnimInstance.h"
#include "Model.h"
#include "EM0650.h"

HRESULT CEM0650_AnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));

	m_pASM_Base = CASMBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0600_101_AL_wait01"))
			.AddTransition("Idle to LeftMove", "LeftMove")
				.Predicator([this] { return m_bMove && m_eMoveAxis == EBaseAxis::WEST; })
				.Duration(0.1f)

			.AddTransition("Idle to RightMove", "RightMove")
				.Predicator([this] { return m_bMove && m_eMoveAxis == EBaseAxis::EAST; })
				.Duration(0.1f)

			.AddTransition("Idle to BackMove", "BackMove")
				.Predicator([this] { return m_bMove && m_eMoveAxis == EBaseAxis::SOUTH; })
				.Duration(0.1f)

			.AddTransition("Idle to ForwardMove", "ForwardMove")
				.Predicator([this] { return m_bMove && m_eMoveAxis == EBaseAxis::NORTH; })
				.Duration(0.1f)

		.AddState("LeftMove")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0600_112_AL_walk_L"))
			.AddTransition("LeftMove to Idle", "Idle")
				.Predicator([this]
				{
					return !m_bMove;
				})
				.Duration(0.1f)

		.AddState("RightMove")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0600_115_AL_walk_R"))
			.AddTransition("RightMove to Idle", "Idle")
				.Predicator([this]
				{
					return !m_bMove;
				})
				.Duration(0.1f)

		.AddState("ForwardMove")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0600_105_AL_walk01"))

			.AddTransition("ForwardMove to Idle", "Idle")
				.Predicator([this]
				{
					return !m_bMove;
				})
				.Duration(0.1f)

		.AddState("BackMove")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0600_109_AL_walk_B"))

			.AddTransition("BackMove to Idle", "Idle")
				.Predicator([this]
				{
					return !m_bMove;
				})
				.Duration(0.1f)

		.Build();

	m_mapAnimSocket.insert({ "FullBody",{} });
	return S_OK;
}

void CEM0650_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	CEM0650* pEM0650 = static_cast<CEM0650*>(m_pTargetObject);

	m_bMove = pEM0650->IsMove();

	m_vMoveAxis = pEM0650->GetMoveAxis();
	m_eMoveAxis = CClientUtils::MoveAxisToBaseEnum(m_vMoveAxis);
}

CEM0650_AnimInstance * CEM0650_AnimInstance::Create(CModel * pModel, CGameObject * pGameObject)
{
	CEM0650_AnimInstance*		pInstance = new CEM0650_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CEM0650_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0650_AnimInstance::Free()
{
	__super::Free();
	Safe_Release(m_pASM_Base);
}
