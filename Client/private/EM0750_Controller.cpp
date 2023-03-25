#include "stdafx.h"
#include "..\public\EM0750_Controller.h"
#include <FSMComponent.h>
#include "EM0750.h"

CEM0750_Controller::CEM0750_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CEM0750_Controller::CEM0750_Controller(const CEM0750_Controller & rhs)
	: CAIController(rhs)
{
}

HRESULT CEM0750_Controller::Initialize(void * pArg)
{
	m_iNearOrder = CMathUtils::RandomUInt(3);
	m_iMidOrder = CMathUtils::RandomUInt(6);
	m_iOutOrder = CMathUtils::RandomUInt(1);

	return S_OK;
}

void CEM0750_Controller::BeginTick()
{
	CAIController::BeginTick();

	CGameObject* pOwner = TryGetOwner();
	Assert(pOwner != nullptr);

	m_pCastedOwner = dynamic_cast<CEM0750*>(pOwner);
}

void CEM0750_Controller::AI_Tick(_double TimeDelta)
{

	if (m_pTarget == nullptr)
		return;

	// 대상과의 거리 
	m_fToTargetDistance = XMVectorGetX(XMVector3LengthEst(
		m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)
		- m_pCastedOwner->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)));

	if (IsCommandRunning() == false && m_pCastedOwner->IsPlayingSocket() == false)
	{
		DefineState(TimeDelta);
	}
}

void CEM0750_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;

	switch (m_iNearOrder)
	{
	case 0:
		AddCommand("Turn", 1.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Rush", 0.f, &CAIController::Input, C);
		break;
	case 1:
		AddCommand("Turn", 1.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Walk", 4.f, &CAIController::Move_TurnToTarget, EMoveAxis::EAST, 1.f);
		break;
	case 2:
		AddCommand("Turn", 1.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Walk", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::WEST, 1.f);
		break;
	}

	m_iNearOrder = (m_iNearOrder + 1) % 3;
}

void CEM0750_Controller::Tick_Mid(_double TimeDelta)
{ 
	m_eDistance = DIS_MIDDLE;

	switch (m_iMidOrder)
	{
	case 0:
		AddCommand("Turn", 1.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Walk", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::WEST, 1.f);
		break;
	case 1:
		AddCommand("Turn", 1.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Walk", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::SOUTH, 1.f);
		break;
	case 2:
		AddCommand("Turn", 1.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Walk", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::EAST, 1.f);
		break;
	case 3:
		AddCommand("Turn", 1.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Rush", 0.f, &CAIController::Input, C);
		break;
	case 4:
		AddCommand("Turn", 1.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Walk", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::WEST, 1.f);
		break;
	case 5:
		AddCommand("Turn", 1.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Walk", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::EAST, 1.f);
		break;
	}

	m_iMidOrder = (m_iMidOrder + 1) % 6;
}

void CEM0750_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;
	AddCommand("Move", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
}

void CEM0750_Controller::Tick_Outside(_double TimeDelta)
{
	m_eDistance = DIS_OUTSIDE;

	switch (m_iOutOrder)
	{
	case 0:
		AddCommand("Wait", 2.f, &CAIController::Wait);
		break;
	case 1:
		AddCommand("Threat", 0.f, &CAIController::Input, SHIFT);
		break;
	}
	m_iOutOrder = (m_iOutOrder + 1) % 2;

}

void CEM0750_Controller::DefineState(_double TimeDelta)
{
	if (m_pCastedOwner->IsPlayingSocket() == true) return;

	//if (m_fToTargetDistance <= 8.f)
	//	Tick_Near(TimeDelta);
	//else if (m_fToTargetDistance <= 15.f)
	//	Tick_Mid(TimeDelta);
	//else if (m_fToTargetDistance <= 25.f)
	//	Tick_Far(TimeDelta);
	//else
	//	Tick_Outside(TimeDelta);

	if (m_fToTargetDistance <= 8.f)
		Tick_Near(TimeDelta);
	else if (m_fToTargetDistance <= 15.f)
		Tick_Mid(TimeDelta);
	else
		Tick_Far(TimeDelta);
	
}



CEM0750_Controller * CEM0750_Controller::Create()
{
	CEM0750_Controller*		pInstance = new CEM0750_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0750_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CEM0750_Controller::Clone(void * pArg)
{
	CEM0750_Controller*		pInstance = new CEM0750_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0750_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEM0750_Controller::Free()
{
	CAIController::Free();
}
