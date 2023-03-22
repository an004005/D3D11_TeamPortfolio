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
		.Duration(0.2f)

		.AddTransition("Idle to RightMove", "RightMove")
		.Predicator([this] { return m_bMove && m_eMoveAxis == EBaseAxis::EAST; })
		.Duration(0.2f)

		.AddTransition("Idle to BackMove", "BackMove")
		.Predicator([this] { return m_bMove && m_eMoveAxis == EBaseAxis::SOUTH; })
		.Duration(0.2f)

		.AddTransition("Idle to ForwardMove", "ForwardMove")
		.Predicator([this] { return m_bMove && m_eMoveAxis == EBaseAxis::NORTH; })
		.Duration(0.2f)

		.AddState("LeftMove")
		.SetAnimation(*m_pModel->Find_Animation("AS_em0600_112_AL_walk_L"))

		.AddTransition("LeftMove to Idle", "Idle")
		.Predicator([this]
	{
		return !m_bMove;
	})
		.Duration(0.2f)

		.AddTransition("LeftMove to BackMove", "BackMove")
		.Predicator([this]
	{
		return m_eMoveAxis == EBaseAxis::SOUTH;
	})
		.Duration(0.2f)

		.AddTransition("LeftMove to ForwardMove", "ForwardMove")
		.Predicator([this]
	{
		return m_eMoveAxis == EBaseAxis::NORTH;
	})
		.Duration(0.2f)

		.AddTransition("LeftMove to RightMove", "RightMove")
		.Predicator([this]
	{
		return m_eMoveAxis == EBaseAxis::EAST;
	})
		.Duration(0.2f)

		.AddState("RightMove")
		.SetAnimation(*m_pModel->Find_Animation("AS_em0600_115_AL_walk_R"))

		.AddTransition("RightMove to Idle", "Idle")
		.Predicator([this]
	{
		return !m_bMove;
	})
		.Duration(0.2f)

		.AddTransition("RightMove to BackMove", "BackMove")
		.Predicator([this]
	{
		return m_eMoveAxis == EBaseAxis::SOUTH;
	})
		.Duration(0.2f)

		.AddTransition("RightMove to ForwardMove", "ForwardMove")
		.Predicator([this]
	{
		return m_eMoveAxis == EBaseAxis::NORTH;
	})
		.Duration(0.2f)

		.AddTransition("RightMove to LeftMove", "LeftMove")
		.Predicator([this]
	{
		return m_eMoveAxis == EBaseAxis::WEST;
	})
		.Duration(0.2f)

		.AddState("ForwardMove")
		.SetAnimation(*m_pModel->Find_Animation("AS_em0600_105_AL_walk01"))

		.AddTransition("ForwardMove to Idle", "Idle")
		.Predicator([this]
	{
		return !m_bMove;
	})
		.Duration(0.2f)

		.AddTransition("ForwardMove to BackMove", "BackMove")
		.Predicator([this]
	{
		return m_eMoveAxis == EBaseAxis::SOUTH;
	})
		.Duration(0.2f)

		.AddTransition("ForwardMove to RightMove", "RightMove")
		.Predicator([this]
	{
		return m_eMoveAxis == EBaseAxis::EAST;
	})
		.Duration(0.2f)

		.AddTransition("ForwardMove to LeftMove", "LeftMove")
		.Predicator([this]
	{
		return m_eMoveAxis == EBaseAxis::WEST;
	})
		.Duration(0.2f)

		.AddState("BackMove")
		.SetAnimation(*m_pModel->Find_Animation("AS_em0600_109_AL_walk_B"))

		.AddTransition("BackMove to Idle", "Idle")
		.Predicator([this]
	{
		return !m_bMove;
	})
		.Duration(0.2f)

		.AddTransition("BackMove to ForwardMove", "ForwardMove")
		.Predicator([this]
	{
		return m_eMoveAxis == EBaseAxis::NORTH;
	})
		.Duration(0.2f)

		.AddTransition("BackMove to RightMove", "RightMove")
		.Predicator([this]
	{
		return m_eMoveAxis == EBaseAxis::EAST;
	})
		.Duration(0.2f)

		.AddTransition("BackMove to LeftMove", "LeftMove")
		.Predicator([this]
	{
		return m_eMoveAxis == EBaseAxis::WEST;
	})
		.Duration(0.2f)



		.Build();

	m_mapAnimSocket.insert({ "FullBody",{} });

	m_fLerpDuration = 0.2f;

	return S_OK;
}

void CEM0650_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	CEM0650* pEM0650 = static_cast<CEM0650*>(m_pTargetObject);

	m_bPreAir = m_bAir;
	m_ePreMoveAxis = m_eMoveAxis;

	m_bAir = !pEM0650->IsOnFloor();
	m_bMove = pEM0650->IsMove();

	m_vMoveAxis = pEM0650->GetMoveAxis();
	m_eMoveAxis = CClientUtils::MoveAxisToBaseEnum(m_vMoveAxis);

	m_fTurnRemain = pEM0650->GetTurnRemain();
	m_eTurn = CClientUtils::TurnDeltaToEnum(m_fTurnRemain);
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
