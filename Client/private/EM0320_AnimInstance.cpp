#include "stdafx.h"
#include "..\public\EM0320_AnimInstance.h"
#include "Model.h"
#include "EM0320.h"

HRESULT CEM320_AnimInstance::Initialize(CModel* pModel, CGameObject* pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));
															// 로컬 잠금
	m_pModel->Find_Animation("AS_em0300_153_AL_turn_R_roop")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_em0300_152_AL_turn_R_start")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_em0300_151_AL_turn_L_roop")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_em0300_150_AL_turn_L_start")->SetLocalRotation(true);

	m_pASM_Base = CASMBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0300_101_AL_wait01"))
			.AddTransition("Idle to MoveStart", "MoveStart")
				.Predicator([this]
				{
					return m_bMove;
				})
				.Duration(0.1f)
			.AddTransition("Idle to TurnStart", "TurnStart")
				.Predicator([this]
				{
					return m_eTurn != EBaseTurn::TURN_END;
				})
				.Duration(0.1f)
		

		.AddState("MoveStart")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0300_105_AL_walk_F_start"))
			.AddTransition("MoveStart to Idle", "Idle")
				.Predicator([this]
				{
					return !m_bMove;
				})
				.Duration(0.2f)
			.AddTransition("MoveStart to Forward_MoveStart", "Forward_MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis == EBaseAxis::NORTH;
				})
				.Duration(0.2f)
			.AddTransition("MoveStart to Back_MoveStart", "Back_MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis == EBaseAxis::SOUTH;
				})
				.Duration(0.2f)
			.AddTransition("MoveStart to Left_MoveStart", "Left_MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis == EBaseAxis::WEST;
				})
				.Duration(0.2f)
			.AddTransition("MoveStart to Right_MoveStart", "Right_MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis == EBaseAxis::EAST;
				})
				.Duration(0.2f)

		.AddState("TurnStart")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0300_150_AL_turn_L_start"))
			.AddTransition("TurnStart to Idle", "Idle")
				.Predicator([this]
				{
					return m_eTurn == EBaseTurn::TURN_END || m_bMove;
				})
				.Duration(0.2f)
			.AddTransition("TurnStart to TurnRightStart", "TurnRightStart")
				.Predicator([this]
				{
					return m_eTurn == EBaseTurn::TURN_RIGHT;
				})
			.AddTransition("TurnStart to TurnLeftStart", "TurnLeftStart")
				.Predicator([this]
				{
					return m_eTurn == EBaseTurn::TURN_LEFT;
				})


		.AddState("Forward_MoveStart")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0300_105_AL_walk_F_start"))
			.AddTransition("Forward_MoveStart to MoveStart", "MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis != EBaseAxis::NORTH;
				})
				.Duration(0.2f)
			.AddTransition("Forward_MoveStart to Forward_MoveLoop", "Forward_MoveLoop")

		.AddState("Forward_MoveLoop")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0300_106_AL_walk_F_roop"))
			.AddTransition("Forward_MoveStart to MoveStart", "MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis != EBaseAxis::NORTH;
				})
				.Duration(0.1f)	


		.AddState("Back_MoveStart")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0300_127_AL_backrun_start"))
			.AddTransition("Back_MoveStart to MoveStart", "MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis != EBaseAxis::SOUTH;
				})
				.Duration(0.2f)
			.AddTransition("Back_MoveStart to Back_MoveLoop", "Back_MoveLoop")

		.AddState("Back_MoveLoop")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0300_128_AL_backrun_roop"))
			.AddTransition("Back_MoveStart to MoveStart", "MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis != EBaseAxis::SOUTH;
				})
				.Duration(0.1f)

		
		.AddState("Left_MoveStart")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0300_111_AL_walk_L_start"))
			.AddTransition("Left_MoveStart to MoveStart", "MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis != EBaseAxis::WEST;
				})
				.Duration(0.2f)
			.AddTransition("Left_MoveStart to Left_MoveLoop", "Left_MoveLoop")

		.AddState("Left_MoveLoop")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0300_112_AL_walk_L_roop"))
			.AddTransition("Left_MoveStart to MoveStart", "MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis != EBaseAxis::WEST;
				})
				.Duration(0.1f)


		.AddState("Right_MoveStart")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0300_114_AL_walk_R_start"))
			.AddTransition("Right_MoveStart to MoveStart", "MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis != EBaseAxis::EAST;
				})
				.Duration(0.2f)
			.AddTransition("Right_MoveStart to Left_MoveLoop", "Right_MoveLoop")

		.AddState("Right_MoveLoop")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0300_115_AL_walk_R_roop"))
			.AddTransition("Right_MoveStart to MoveStart", "MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis != EBaseAxis::EAST;
				})
				.Duration(0.1f)


		.AddState("TurnRightStart")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0300_152_AL_turn_R_start"))
			.AddTransition("TurnRightStart to TurnRightLoop", "TurnRightLoop")
			.AddTransition("TurnRightStart to TurnStart", "TurnStart")
				.Predicator([this]
				{
					return m_eTurn != EBaseTurn::TURN_RIGHT;
				})
				.Duration(0.2f)

		.AddState("TurnRightLoop")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0300_153_AL_turn_R_roop"))
			.AddTransition("TurnRightLoop to TurnStart", "TurnStart")
				.Predicator([this]
				{
					return m_eTurn != EBaseTurn::TURN_RIGHT;
				})
				.Duration(0.2f)

		.AddState("TurnLeftStart")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0300_150_AL_turn_L_start"))
			.AddTransition("TurnLeftStart to TurnLeftLoop", "TurnLeftLoop")
			.AddTransition("TurnLeftStart to TurnStart", "TurnStart")
				.Predicator([this]
				{
					return m_eTurn != EBaseTurn::TURN_LEFT;
				})
				.Duration(0.2f)

		.AddState("TurnLeftLoop")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0300_151_AL_turn_L_roop"))
			.AddTransition("TurnLeftLoop to TurnStart", "TurnStart")
				.Predicator([this]
				{
					return m_eTurn != EBaseTurn::TURN_LEFT;
				})
				.Duration(0.2f)
			
		.Build();


	m_mapAnimSocket.insert({ "FullBody", {} });



	return S_OK;
}

void CEM320_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	CEM0320* pBoss = static_cast<CEM0320*>(m_pTargetObject);

	m_bPreAir = m_bAir;
	m_ePreMoveAxis = m_eMoveAxis;

	m_bAir = !pBoss->IsOnFloor();
	m_bMove = pBoss->IsMove();

	m_vMoveAxis = pBoss->GetMoveAxis();
	m_eMoveAxis = CClientUtils::MoveAxisToBaseEnum(m_vMoveAxis);

	m_fTurnRemain = pBoss->GetTurnRemain();
	m_eTurn = CClientUtils::TurnDeltaToEnum(m_fTurnRemain);
}

CEM320_AnimInstance* CEM320_AnimInstance::Create(CModel* pModel, CGameObject* pGameObject)
{
	CEM320_AnimInstance*		pInstance = new CEM320_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CEM320_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM320_AnimInstance::Free()
{
	CEnemy_AnimInstance::Free();
	Safe_Release(m_pASM_Base);
}
