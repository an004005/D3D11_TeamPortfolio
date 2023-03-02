#include "stdafx.h"
#include "..\public\Boss1_AnimationInstance.h"
#include "Model.h"
#include "Boss1.h"

HRESULT CBoss1_AnimationInstance::Initialize(CModel* pModel, CGameObject* pGameObject)
{
	FAILED_CHECK(CAnimationInstance::Initialize(pModel, pGameObject));


	m_pASM = CASMBuilder()
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
			.AddTransition("MoveStart to Back_MoveStart", "Back_MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis == EBaseAxis::SOUTH;
				})
			.AddTransition("MoveStart to Left_MoveStart", "Left_MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis == EBaseAxis::WEST;
				})
			.AddTransition("MoveStart to Right_MoveStart", "Right_MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis == EBaseAxis::EAST;
				})

		.AddState("TurnStart")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0300_150_AL_turn_L_start"))
			.AddTransition("TurnStart to Idle", "Idle")
				.Predicator([this]
				{
					return m_eTurn == EBaseTurn::TURN_END;
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

	return S_OK;
}

void CBoss1_AnimationInstance::Tick(_double TimeDelta)
{
	UpdateTargetState(TimeDelta);
	m_pASM->Tick(TimeDelta);
	m_pModel->SetCurAnimName(m_pASM->GetCurState()->m_Animation->GetName());
	m_pModel->Compute_CombindTransformationMatrix();



	// _vector RotationDelta = m_pModel->GetLocalRotationDelta();
	// _float fYaw = 0.f;
	// XMQuaternionToAxisAngle(&XMVectorSet(0.f, 1.f, 0.f, 0.f), &fYaw, RotationDelta);
	//
	//
	// _float4 tmp = CMathUtils::Quat2Euler( RotationDelta);
	// fYaw = tmp.y;
	//
	// if (m_eTurn != EBaseTurn::TURN_END)
	// {
	// 	if (abs(fYaw) >= abs(m_fTurnRemain))
	// 	{
	// 		RotationDelta = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(m_fTurnRemain));
	// 	}
	//
	// 	m_pTargetObject->GetTransform()->Turn_Fixed(XMVectorSet(0.f, 1.f, 0.f, 0.f), fYaw);
	// }
}

void CBoss1_AnimationInstance::UpdateTargetState(_double TimeDelta)
{
	CBoss1* pBoss = static_cast<CBoss1*>(m_pTargetObject);

	m_bPreAir = m_bAir;
	m_ePreMoveAxis = m_eMoveAxis;

	m_bAir = !pBoss->IsOnFloor();
	m_bMove = pBoss->IsMove();

	m_vMoveAxis = pBoss->GetMoveAxis();
	m_eMoveAxis = CClientUtils::MoveAxisToBaseEnum(m_vMoveAxis);

	m_fTurnRemain = pBoss->GetTurnRemain();
	m_eTurn = CClientUtils::TurnDeltaToEnum(m_fTurnRemain);
}

void CBoss1_AnimationInstance::Imgui_RenderState()
{
	m_pASM->Imgui_RenderState();
}

CBoss1_AnimationInstance* CBoss1_AnimationInstance::Create(CModel* pModel, CGameObject* pGameObject)
{
	CBoss1_AnimationInstance*		pInstance = new CBoss1_AnimationInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CBoss1_AnimationInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss1_AnimationInstance::Free()
{
	CAnimationInstance::Free();
	Safe_Release(m_pASM);
}
