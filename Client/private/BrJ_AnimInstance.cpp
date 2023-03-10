#include "stdafx.h"
#include "..\public\BrJ_AnimInstance.h"
#include "BronJon.h"
#include "Model.h"
#include "Animation.h"

HRESULT CBrJ_AnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));

	/*m_pModel->Find_Animation("AS_em0800_115_AL_walk_R_start")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_em0800_116_AL_walk_R_loop")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_em0800_112_AL_walk_L_start")->SetLocalRotation(true);
	m_pModel->Find_Animation("AS_em0800_113_AL_walk_L_loop")->SetLocalRotation(true);*/
	
	m_pASM_Base = CASMBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0800_101_AL_wait01"))
		
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
		// Move 분기점
		.AddState("MoveStart")			
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
		// Turn 분기점
		.AddState("TurnStart")	
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
				.Duration(0.2f)

			.AddTransition("TurnStart to TurnLeftStart", "TurnLeftStart")
				.Predicator([this]
				{
					return m_eTurn == EBaseTurn::TURN_LEFT;
				})
				.Duration(0.2f)
		// 앞 이동
		.AddState("Forward_MoveStart")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0800_105_AL_walk01_start"))

			.AddTransition("Forward_MoveStart to MoveStart", "MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis != EBaseAxis::NORTH;
				})
				.Duration(0.2f)

			.AddTransition("Forward_MoveStart to Forward_MoveLoop", "Forward_MoveLoop")

		.AddState("Forward_MoveLoop")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0800_106_AL_walk01_loop"))
		
			.AddTransition("Forward_MoveLoop to MoveStart", "MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis != EBaseAxis::NORTH;
				})
				.Duration(0.1f)
		// 뒤 이동
		.AddState("Back_MoveStart")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0800_109_AL_walk_B_start"))
			
			.AddTransition("Back_MoveStart to MoveStart", "MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis != EBaseAxis::SOUTH;
				})
				.Duration(0.2f)

			.AddTransition("Back_MoveStart to Back_MoveLoop", "Back_MoveLoop")

		.AddState("Back_MoveLoop")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0800_110_AL_walk_B_loop"))
		
				.AddTransition("Back_MoveLoop to MoveStart", "MoveStart")
					.Predicator([this]
					{
						return m_eMoveAxis != EBaseAxis::SOUTH;
					})
					.Duration(0.1f)
		// 왼쪽 이동
		.AddState("Left_MoveStart")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0800_112_AL_walk_L_start"))
			
			.AddTransition("Left_MoveStart to MoveStart", "MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis != EBaseAxis::WEST;
				})
				.Duration(0.2f)

			.AddTransition("Left_MoveStart to Left_MoveLoop", "Left_MoveLoop")

		.AddState("Left_MoveLoop")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0800_113_AL_walk_L_loop"))

			.AddTransition("Left_MoveLoop to MoveStart", "MoveStart")
				.Predicator([this]				{
					return m_eMoveAxis != EBaseAxis::WEST;
				})
				.Duration(0.1f)
		// 오른쪽 이동			
		.AddState("Right_MoveStart")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0800_115_AL_walk_R_start"))
			
			.AddTransition("Right_MoveStart to MoveStart", "MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis != EBaseAxis::EAST;
				})
				.Duration(0.2f)

			.AddTransition("Right_MoveStart to Right_MoveLoop", "Right_MoveLoop")

		.AddState("Right_MoveLoop")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0800_116_AL_walk_R_loop"))
			
			.AddTransition("Right_MoveLoop to MoveStart", "MoveStart")
				.Predicator([this]
				{
					return m_eMoveAxis != EBaseAxis::EAST;
				})
				.Duration(0.1f)

		// 오른쪽 회전
		.AddState("TurnRightStart")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0800_150_AL_turn_R"))
		
			.AddTransition("TurnRightStart to TurnStart", "TurnStart")
				.Predicator([this]
				{
					return m_eTurn != EBaseTurn::TURN_RIGHT;
				})
				.Duration(0.2f)

			.AddTransition("TurnRightStart to TurnRightLoop", "TurnRightLoop")
		
		.AddState("TurnRightLoop")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0800_116_AL_walk_R_loop"))
			
			.AddTransition("TurnRightLoop to TurnStart", "TurnStart")
				.Predicator([this]
				{
					return m_eTurn != EBaseTurn::TURN_RIGHT;
				})
				.Duration(0.1f)
		// 왼쪽 회전
		.AddState("TurnLeftStart")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0800_152_AL_turn_L"))
		
			.AddTransition("TurnLeftStart to TurnStart", "TurnStart")
				.Predicator([this]
				{
					return m_eTurn != EBaseTurn::TURN_LEFT;
				})
				.Duration(0.2f)

			.AddTransition("TurnLeftStart to TurnLeftLoop", "TurnLeftLoop")
		
		.AddState("TurnLeftLoop")		
			.SetAnimation(*m_pModel->Find_Animation("AS_em0800_113_AL_walk_L_loop"))
		
			.AddTransition("TurnLeftLoop to TurnStart", "TurnStart")
				.Predicator([this]
				{
					return m_eTurn != EBaseTurn::TURN_LEFT;
				})
				.Duration(0.1f)

	.Build();				
	
	m_mapAnimSocket.insert({ "BronJon",{} });
	return S_OK;
}

void CBrJ_AnimInstance::Tick(_double TimeDelta)
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

		m_pASM_Base->SetCurState("Idle");
		//m_pASM_Base->GetCurState()->m_Animation->Reset();
		m_pModel->SetCurAnimName(m_pASM_Base->GetCurState()->m_Animation->GetName());
		m_fLerpTime = 0.f;
	}
	else if (m_fLerpTime < m_fLerpDuration)
	{
		m_pASM_Base->GetCurState()->m_Animation->Update_Bones(TimeDelta, EAnimUpdateType::BLEND, m_fLerpTime / m_fLerpDuration);
		m_fLerpTime += (_float)TimeDelta;
	}
	else
	{
		m_pASM_Base->Tick(TimeDelta);
		m_pModel->SetCurAnimName(m_pASM_Base->GetCurState()->m_Animation->GetName());
	}

	m_pModel->Compute_CombindTransformationMatrix();

	if (bLocalMove)
	{
		_matrix WorldMatrix = m_pTargetObject->GetTransform()->Get_WorldMatrix();
		_vector vLocalMove = m_pModel->GetLocalMove(WorldMatrix);
		m_pTargetObject->GetTransform()->LocalMove(vLocalMove);
	}
}

void CBrJ_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	CBronJon* pBronJon = static_cast<CBronJon*>(m_pTargetObject);

	m_ePreMoveAxis = m_eMoveAxis;	
	m_bMove = pBronJon->IsMove();

	m_vMoveAxis = pBronJon->GetMoveAxis();
	m_eMoveAxis = CClientUtils::MoveAxisToBaseEnum(m_vMoveAxis);

	m_fTurnRemain = pBronJon->GetTurnRemain();
	m_eTurn = CClientUtils::TurnDeltaToEnum(m_fTurnRemain);
}

void CBrJ_AnimInstance::Imgui_RenderState()
{
	m_pASM_Base->Imgui_RenderState();
}

void CBrJ_AnimInstance::InputAnimSocket(const string & strSocName, list<CAnimation*> AnimList)
{
	for (auto& iter : m_mapAnimSocket)
	{
		if (!iter.second.empty())
			iter.second.front()->Reset();

		list<CAnimation*> SocketList;
		iter.second = SocketList;
	}

	m_mapAnimSocket[strSocName] = (AnimList);
}

void CBrJ_AnimInstance::AttachAnimSocket(const string & strSocName, const list<CAnimation*>& AnimList)
{
	const auto itr = m_mapAnimSocket.find(strSocName);
	Assert(itr != m_mapAnimSocket.end());

	if (!itr->second.empty())
	{
		m_bAttach = true;
		itr->second.front()->Reset();
	}
	m_mapAnimSocket[strSocName] = (AnimList);
}

_bool CBrJ_AnimInstance::isSocketPassby(const string & strSocName, _float fPlayRatio)
{
	Assert(m_mapAnimSocket.find(strSocName) != m_mapAnimSocket.end());

	return m_mapAnimSocket[strSocName].empty() == false
		&& m_mapAnimSocket[strSocName].front()->GetPlayRatio() >= fPlayRatio;
}

CBrJ_AnimInstance * CBrJ_AnimInstance::Create(CModel * pModel, CGameObject * pGameObject)
{
	CBrJ_AnimInstance*		pInstance = new CBrJ_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CBrJ_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBrJ_AnimInstance::Free()
{
	CAnimationInstance::Free();
	Safe_Release(m_pASM_Base);
}
