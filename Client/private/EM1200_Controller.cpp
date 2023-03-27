#include "stdafx.h"
#include "..\public\EM1200_Controller.h"
#include <FSMComponent.h>
#include "EM1200.h"

CEM1200_Controller::CEM1200_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CEM1200_Controller::CEM1200_Controller(const CEM1200_Controller & rhs)
	: CAIController(rhs)
{
}

HRESULT CEM1200_Controller::Initialize(void * pArg)
{
	m_iMidOrder = CMathUtils::RandomUInt(1);
	m_iFarOrder = CMathUtils::RandomUInt(2);
	//Move함수 시 플레이어와 거리가 어느정도까지 가까워졌을때 멈출지를 정해줌
	m_fTargetDist = 5.f;


	//near :  닷지 , 꼬리치기(꼬리치기후 회전하면서 도망감, 플레이어가 뒤에 있을때만)
	//mid : 물대포 ,주먹, 
	//far : 돌진(쿨타임 있음), 달려서 플레이어 앞까지 옴, 전기공(체력이 50퍼 이하일때 사용)

	//체력이 50프로 아래로 내려가면 원거리 전기공격 추가

	//Rush 공격은 자주 사용하지 않기 때문에 쿨타임을 줌.

	//모든 공격모션 뒤에 wait 또는 walk를 추가해서 다음 공격까지 텀을 줌
	return S_OK;
}

void CEM1200_Controller::BeginTick()
{
	CAIController::BeginTick();

	CGameObject* pOwner = TryGetOwner();
	Assert(pOwner != nullptr);

	m_pCastedOwner = dynamic_cast<CEM1200*>(pOwner);
}

void CEM1200_Controller::AI_Tick(_double TimeDelta)
{
	if (m_pTarget == nullptr)
		return;

	m_bRun = false;
	m_dRushCoolTime[CURTIME] += TimeDelta;

	// 대상과의 거리 
	m_fToTargetDistance = XMVectorGetX(XMVector3LengthEst(
		m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)
		- m_pCastedOwner->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)));

	if (IsCommandRunning() == false && m_pCastedOwner->IsPlayingSocket() == false)
	{
		DefineState(TimeDelta);
	}
}

void CEM1200_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;
	
	//정면
	if (m_pCastedOwner->IsTargetFront())
	{
		AddCommand("Turn", 3.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Dodge", 0.f, &CAIController::Input, SHIFT);	
	}
	else
	{
		AddCommand("TailSwing", 0.f, &CAIController::Input, T);
	}
}

void CEM1200_Controller::Tick_Mid(_double TimeDelta)
{ 
	m_eDistance = DIS_MIDDLE;

	switch (m_iMidOrder)
	{
	case 0:
		if (m_pCastedOwner->IsTargetFront())
		{
			AddCommand("Turn", 2.f, &CAIController::TurnToTargetStop, 1.f);
			AddCommand("WaterAttack", 0.f, &CAIController::Input, W);
			AddCommand("Wait", 1.f, &CAIController::Wait);
		}
		else
			AddCommand("TailSwing", 0.f, &CAIController::Input, T);
		break;
	case 1:
		if (m_pCastedOwner->IsTargetFront())
		{
			AddCommand("Turn", 2.f, &CAIController::TurnToTargetStop, 1.f);
			AddCommand("Stamp", 0.f, &CAIController::Input, S);
			AddCommand("Wait", 1.f, &CAIController::Wait);
		}
		else
			AddCommand("TailSwing", 0.f, &CAIController::Input, T);
		break;
	
	}

	m_iMidOrder = (m_iMidOrder + 1) % 2;
}

void CEM1200_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;

	if (m_dRushCoolTime[CURTIME] >= m_dRushCoolTime[MAXTIME])
	{
		//rush하기전 준비동작이 길어서 거기서 Turn을 해줌
		AddCommand("Rush", 0.f, &CAIController::Input, R);
		AddCommand("Wait", 1.5f, &CAIController::Wait);
		m_dRushCoolTime[CURTIME] = 0.0;
		return;
	}

	if (m_pCastedOwner->GetHpRatio() >= 0.5f)
	{
		switch (m_iFarOrder)
		{
		case 0:
		case 1:
			AddCommand("Turn", 2.f, &CAIController::TurnToTargetStop, 1.f);
			AddCommand("Run", 5.f, &CEM1200_Controller::Run_TurnToTarget, EMoveAxis::NORTH, 1.f);
			break;
		case 2:
			AddCommand("Walk", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::EAST, 1.f);
			break;
		}
		
	}
	else
	{
		switch (m_iFarOrder)
		{
		case 0:
		case 1:
			AddCommand("ElectricBall", 0.f, &CAIController::Input, E);
			AddCommand("Wait", 1.5f, &CAIController::Wait);
			break;
		case 2:
			AddCommand("Walk", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::WEST, 1.f);
			break;
		}
	}
	
	m_iMidOrder = (m_iMidOrder + 1) % 3;
}

void CEM1200_Controller::Tick_Outside(_double TimeDelta)
{
	m_eDistance = DIS_OUTSIDE;
	AddCommand("Wait", 2.f, &CAIController::Wait);
}

void CEM1200_Controller::Run_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio)
{
	m_bRun = true;

	//이벤트로 실제로 뛰는 타이밍
	if (m_pCastedOwner->Get_RunStart())
	{
		Move(eAxis);
		TurnToTarget(fSpeedRatio);
	}
}

void CEM1200_Controller::DefineState(_double TimeDelta)
{
	if (m_pCastedOwner->IsPlayingSocket() == true) return;

	if (m_fToTargetDistance <= 5.f)
		Tick_Near(TimeDelta);
	else if (m_fToTargetDistance <= 10.f)
		Tick_Mid(TimeDelta);
	else if (m_fToTargetDistance <= 30.f)
		Tick_Far(TimeDelta);
	else
		Tick_Outside(TimeDelta);
	
}



CEM1200_Controller * CEM1200_Controller::Create()
{
	CEM1200_Controller*		pInstance = new CEM1200_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM1200_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CEM1200_Controller::Clone(void * pArg)
{
	CEM1200_Controller*		pInstance = new CEM1200_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM1200_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEM1200_Controller::Free()
{
	CAIController::Free();
}
