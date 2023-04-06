#include "stdafx.h"
#include "../public/EM8200_AnimInstance.h"
#include "Model.h"
#include "EM8200.h"

HRESULT CEM8200_AnimInstance::Initialize(CModel* pModel, CGameObject* pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));

	m_pASM_Base = CASMBuilder()
		 .InitState("Idle")
		 .AddState("Idle")
		 .SetAnimation(*m_pModel->Find_Animation("AS_em8200_001_AL_wait01"))
		//
		// .AddTransition("Idle to Walk", "Walk")
		// .Predicator([this] { return m_bMove; })
		// .Duration(0.2f)
		//
		// .AddState("Walk")
		// .SetAnimation(*m_pModel->Find_Animation("AS_em0200_106_AL_walk02"))
		//
		// .AddTransition("Walk to Idle", "Idle")
		// .Predicator([this] { return !m_bMove && !m_bRun; })
		// .Duration(0.2f)
		//
		// .AddTransition("Walk to Run", "Run")
		// .Predicator([this] {return m_bRun; })
		// .Duration(0.2f)
		//
		// .AddState("Run")
		// .SetAnimation(*m_pModel->Find_Animation("AS_em0200_107_AL_run"))
		//
		// .AddTransition("Run to Idle", "Idle")
		// .Predicator([this] { return !m_bMove && !m_bRun; })
		// .Duration(0.2f)
		//
		// .AddTransition("Run to Walk", "Walk")
		// .Predicator([this] {return !m_bRun && m_bMove; })
		// .Duration(0.2f)

		.Build();

	m_mapAnimSocket.insert({ "FullBody", {} });
	return S_OK;
}

void CEM8200_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	CEM8200* pEM0200 = static_cast<CEM8200*>(m_pTargetObject);

	// 필요한것만 만들것
	m_ePreMoveAxis = m_eMoveAxis;

	m_bMove = pEM0200->IsMove();
	m_bRun = pEM0200->IsRun();

	m_vMoveAxis = pEM0200->GetMoveAxis();
	m_eMoveAxis = CClientUtils::MoveAxisToBaseEnum(m_vMoveAxis);

	m_fTurnRemain = pEM0200->GetTurnRemain();
	m_eTurn = CClientUtils::TurnDeltaToEnum(m_fTurnRemain);
}

CEM8200_AnimInstance* CEM8200_AnimInstance::Create(CModel* pModel, CGameObject* pGameObject)
{
	CEM8200_AnimInstance* pInstance = new CEM8200_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CEM0200_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM8200_AnimInstance::Free()
{
	__super::Free();
	Safe_Release(m_pASM_Base);
}
