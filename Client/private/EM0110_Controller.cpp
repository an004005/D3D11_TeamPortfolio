#include "stdafx.h"
#include "..\public\EM0110_Controller.h"
#include <FSMComponent.h>
#include "EM0110.h"

CEM0110_Controller::CEM0110_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CEM0110_Controller::CEM0110_Controller(const CEM0110_Controller & rhs)
	: CAIController(rhs)
{
}

HRESULT CEM0110_Controller::Initialize(void * pArg)
{
	m_iNearOrder = CMathUtils::RandomUInt(3);
	m_iMidOrder = CMathUtils::RandomUInt(1);
	m_iFarOrder = CMathUtils::RandomUInt(1);

	//near : 발차기, 돌진, 광역기
	//공격 하고나서 무조건 한턴 쉼. wait 또는 walk(상하좌우 다)
	//mid : 돌진 또는 run 후 멀어지면 한번 쉬고 돌진 or 광역기
	//far : walk or wait 

	//돌진은 벽 앞에서 멈춤


	
	return S_OK;
}

void CEM0110_Controller::BeginTick()
{
	CAIController::BeginTick();

	CGameObject* pOwner = TryGetOwner();
	Assert(pOwner != nullptr);

	m_pCastedOwner = dynamic_cast<CEM0110*>(pOwner);
}

void CEM0110_Controller::AI_Tick(_double TimeDelta)
{
	if (m_pTarget == nullptr)
		return;

	m_bRun = false;

	// 대상과의 거리 
	m_fToTargetDistance = XMVectorGetX(XMVector3LengthEst(
		m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)
		- m_pCastedOwner->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)));

	if (IsCommandRunning() == false && m_pCastedOwner->IsPlayingSocket() == false)
	{
		DefineState(TimeDelta);
	}
}

void CEM0110_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;

	//가까이에 있을때는 돌려차기랑, 광역기 두개만 사용
	
	switch (m_iNearOrder)
	{
	case 0:
		AddCommand("Attack_turn", 0.f, &CAIController::Input, R);
		AddCommand("Wait", 2.f, &CAIController::Wait);
		break;
	case 2:
		AddCommand("Walk", 2.f, &CAIController::Move, EMoveAxis::NORTH);
		break;
	case 3:
		AddCommand("Attack_c1", 0.f, &CAIController::Input, G);
		AddCommand("Wait", 2.f, &CAIController::Wait);
		break;
	}
	
	m_iNearOrder = (m_iNearOrder + 1) % 4;
}

void CEM0110_Controller::Tick_Mid(_double TimeDelta)
{
	m_eDistance = DIS_MIDDLE;

	switch (m_iMidOrder)
	{
	case 0:
		AddCommand("Turn", 2.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_b2", 0.f, &CAIController::Input, C);
		AddCommand("Walk", 2.5f, &CAIController::Move, EMoveAxis::NORTH);
		break;
	case 1:
		AddCommand("Run", 1.5f, &CEM0110_Controller::Run_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	}

	m_iMidOrder = (m_iMidOrder + 1) % 2;
}


void CEM0110_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;

	switch (m_iFarOrder)
	{
	case 0:
		AddCommand("Walk", 4.f, &CAIController::Move, EMoveAxis::NORTH);
		break;
	case 1:
		AddCommand("Wait", 3.f, &CAIController::Wait);
		break;
	}

	m_iFarOrder = (m_iFarOrder + 1) % 2;

}

void CEM0110_Controller::Tick_Outside(_double TimeDelta)
{
	m_eDistance = DIS_OUTSIDE;
	AddCommand("Wait", 0.f, &CAIController::Wait);
}


void CEM0110_Controller::Run_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio)
{
	m_bRun = true;
	Move(eAxis);
	TurnToTarget(fSpeedRatio);
}

void CEM0110_Controller::DefineState(_double TimeDelta)
{
	if (m_pCastedOwner->IsPlayingSocket() == true) return;

	if (m_fToTargetDistance <= 5.f)
		Tick_Near(TimeDelta);
	else if (m_fToTargetDistance <= 13.f)
		Tick_Mid(TimeDelta);
	else if (m_fToTargetDistance <= 25.f)
		Tick_Far(TimeDelta);
	else
		Tick_Outside(TimeDelta);

}

CEM0110_Controller * CEM0110_Controller::Create()
{
	CEM0110_Controller*		pInstance = new CEM0110_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0110_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CEM0110_Controller::Clone(void * pArg)
{
	CEM0110_Controller*		pInstance = new CEM0110_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0110_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEM0110_Controller::Free()
{
	CAIController::Free();
}
