#include "stdafx.h"
#include "..\public\SkPd_AnimInstance.h"
#include "SkummyPandou.h"
#include "Model.h"
#include "Animation.h"

HRESULT CSkPd_AnimInstance::Initialize(CModel * pModel, CGameObject * pGameObject)
{
	FAILED_CHECK(__super::Initialize(pModel, pGameObject));


	m_pASM_Base = CASMBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0700_107_AL_fly_wait"))

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
			.SetAnimation(*m_pModel->Find_Animation("AS_em0700_112_AL_walk_L"))

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
				.Duration(0.1f)

			.AddTransition("LeftMove to ForwardMove", "ForwardMove")
				.Predicator([this]
				{
					return m_eMoveAxis == EBaseAxis::NORTH;
				})
				.Duration(0.1f)

			.AddTransition("LeftMove to RightMove", "RightMove")
				.Predicator([this]
				{
					return m_eMoveAxis == EBaseAxis::EAST;
				})
				.Duration(0.1f)

		.AddState("RightMove")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0700_113_AL_walk_R"))

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
				.Duration(0.1f)

			.AddTransition("RightMove to ForwardMove", "ForwardMove")
				.Predicator([this]
				{
					return m_eMoveAxis == EBaseAxis::NORTH;
				})
				.Duration(0.1f)

			.AddTransition("RightMove to LeftMove", "LeftMove")
				.Predicator([this]
				{
					return m_eMoveAxis == EBaseAxis::WEST;
				})
				.Duration(0.1f)

		.AddState("ForwardMove")
				.SetAnimation(*m_pModel->Find_Animation("AS_em0700_110_AL_walk_F"))

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
				.Duration(0.1f)

			.AddTransition("ForwardMove to RightMove", "RightMove")
				.Predicator([this]
				{
					return m_eMoveAxis == EBaseAxis::EAST;
				})
				.Duration(0.1f)

			.AddTransition("ForwardMove to LeftMove", "LeftMove")
				.Predicator([this]
				{
					return m_eMoveAxis == EBaseAxis::WEST;
				})
				.Duration(0.1f)

		.AddState("BackMove")
			.SetAnimation(*m_pModel->Find_Animation("AS_em0700_111_AL_walk_B"))

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
				.Duration(0.1f)

			.AddTransition("BackMove to RightMove", "RightMove")
				.Predicator([this]
				{
					return m_eMoveAxis == EBaseAxis::EAST;
				})
				.Duration(0.1f)

			.AddTransition("BackMove to LeftMove", "LeftMove")
				.Predicator([this]
				{
					return m_eMoveAxis == EBaseAxis::WEST;
				})
				.Duration(0.1f)

		.Build();

	m_mapAnimSocket.insert({ "Bee", {} });
	return S_OK;
}

void CSkPd_AnimInstance::Tick(_double TimeDelta)
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

void CSkPd_AnimInstance::UpdateTargetState(_double TimeDelta)
{
	CSkummyPandou* pSkummyPandou = static_cast<CSkummyPandou*>(m_pTargetObject);

	m_bPreAir = m_bAir;
	m_ePreMoveAxis = m_eMoveAxis;

	m_bAir = !pSkummyPandou->IsOnFloor();
	m_bMove = pSkummyPandou->IsMove();

	m_vMoveAxis = pSkummyPandou->GetMoveAxis();
	m_eMoveAxis = CClientUtils::MoveAxisToBaseEnum(m_vMoveAxis);

	m_fTurnRemain = pSkummyPandou->GetTurnRemain();
	m_eTurn = CClientUtils::TurnDeltaToEnum(m_fTurnRemain);

}

void CSkPd_AnimInstance::Imgui_RenderState()
{
	m_pASM_Base->Imgui_RenderState();
}

void CSkPd_AnimInstance::InputAnimSocket(const string & strSocName, list<CAnimation*> AnimList)
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

void CSkPd_AnimInstance::AttachAnimSocket(const string & strSocName, list<CAnimation*> AnimList)
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

_bool CSkPd_AnimInstance::isSocketPassby(const string & strSocName, _float fPlayRatio)
{
	Assert(m_mapAnimSocket.find(strSocName) != m_mapAnimSocket.end());

	return m_mapAnimSocket[strSocName].empty() == false
		&& m_mapAnimSocket[strSocName].front()->GetPlayRatio() >= fPlayRatio;
}

CSkPd_AnimInstance * CSkPd_AnimInstance::Create(CModel * pModel, CGameObject * pGameObject)
{
	CSkPd_AnimInstance*		pInstance = new CSkPd_AnimInstance();

	if (FAILED(pInstance->Initialize(pModel, pGameObject)))
	{
		MSG_BOX("Failed to Created : CSkPd_AnimInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkPd_AnimInstance::Free()
{
	CAnimationInstance::Free();
	Safe_Release(m_pASM_Base);
}
