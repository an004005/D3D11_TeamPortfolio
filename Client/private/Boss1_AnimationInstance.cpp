#include "stdafx.h"
#include "..\public\Boss1_AnimationInstance.h"
#include "Model.h"
#include "Boss1.h"

HRESULT CBoss1_AnimationInstance::Initialize(CModel* pModel, CGameObject* pGameObject)
{
	FAILED_CHECK(CAnimationInstance::Initialize(pModel, pGameObject));
															// 로컬 잠금
	m_pModel->Find_Animation("AS_em0300_153_AL_turn_R_roop")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_em0300_152_AL_turn_R_start")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_em0300_151_AL_turn_L_roop")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_em0300_150_AL_turn_L_start")->SetLocalRotation(true);

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

void CBoss1_AnimationInstance::Tick(_double TimeDelta)
{
	UpdateTargetState(TimeDelta);

	_bool bChange = CheckFinishedAnimSocket();	
	_bool bLocalMove = true;

	string szCurAnimName = "";

	// 소켓이 비어있는지 탐색
	list<CAnimation*> CurSocket;

	for (auto& iter : m_mapAnimSocket)
	{
		if (iter.second.empty() == false)
		{
			CurSocket = iter.second;
			break;
		}
	}

	// 발견한 소켓이 있으면 해당 소켓을 실행
	if (!CurSocket.empty())
	{
		auto Socket = CurSocket.front();
		if (bChange)
		{
			Socket = CurSocket.front();
			m_pModel->SetPlayAnimation(Socket->GetName());
			m_pModel->SetCurAnimName(Socket->GetName());
			m_fLerpTime = 0.f;
		}

		if (1.f > m_fLerpTime / m_fLerpDuration)
		{
			Socket->Update_Bones(TimeDelta, EAnimUpdateType::BLEND, m_fLerpTime / m_fLerpDuration);
			m_fLerpTime += (_float)TimeDelta;
		}
		else if (m_bAttach)
		{
			m_fLerpTime = 0.f;	// 어태치면 바로 보간
			m_bAttach = false;
		}
		else
		{
			szCurAnimName = Socket->GetName();
			Socket->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
		}
	
	}
	else if (bChange)
	{
		bLocalMove = false;

		m_pASM->SetCurState("Idle");

		//m_pASM_Base->GetCurState()->m_Animation->Reset();
		m_pModel->SetCurAnimName(m_pASM->GetCurState()->m_Animation->GetName());
		m_fLerpTime = 0.f;
	}
	else if (m_fLerpTime < m_fLerpDuration)
	{
		m_pASM->GetCurState()->m_Animation->Update_Bones(TimeDelta, EAnimUpdateType::BLEND, m_fLerpTime / m_fLerpDuration);
		m_fLerpTime += (_float)TimeDelta;
	}
	else
	{
		m_pASM->Tick(TimeDelta);
		m_pModel->SetCurAnimName(m_pASM->GetCurState()->m_Animation->GetName());
	}

	m_pModel->Compute_CombindTransformationMatrix();

	if (bLocalMove)
	{
		_matrix WorldMatrix = m_pTargetObject->GetTransform()->Get_WorldMatrix();
		_vector vLocalMove = m_pModel->GetLocalMove(WorldMatrix);
		m_pTargetObject->GetTransform()->LocalMove(vLocalMove);
	}
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

_bool CBoss1_AnimationInstance::isSocketPassby(const string& strSocName, _float fPlayRatio)
{
	Assert(m_mapAnimSocket.find(strSocName) != m_mapAnimSocket.end());

	return m_mapAnimSocket[strSocName].empty() == false
		&& m_mapAnimSocket[strSocName].front()->GetPlayRatio() >= fPlayRatio;
}

void CBoss1_AnimationInstance::AttachAnimSocket(const string& strSocName, const list<CAnimation*>& AnimList)
{
	const auto itr = m_mapAnimSocket.find(strSocName);
	Assert(itr != m_mapAnimSocket.end());

	if (!itr->second.empty())
	{
		m_bAttach = true;
		itr->second.front()->Reset();;
	}
	m_mapAnimSocket[strSocName] = (AnimList);
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
