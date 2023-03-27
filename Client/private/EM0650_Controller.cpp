#include "stdafx.h"
#include "..\public\EM0650_Controller.h"
#include <FSMComponent.h>
#include "EM0650.h"

CEM0650_Controller::CEM0650_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CEM0650_Controller::CEM0650_Controller(const CEM0650_Controller & rhs)
	: CAIController(rhs)
{
}

HRESULT CEM0650_Controller::Initialize(void * pArg)
{
	m_iMidOrder = CMathUtils::RandomUInt(3);

	//플레이어와 거리가 가까워지면 후퇴,
	//플레이어와 거리가 어느정도 되면 좌, 우 이동 또는 공격
	//플레이어와 거리가 멀어지면 플레이어쪽으로 이동
	return S_OK;
}

void CEM0650_Controller::BeginTick()
{
	CAIController::BeginTick();

	CGameObject* pOwner = TryGetOwner();
	Assert(pOwner != nullptr);

	m_pCastedOwner = dynamic_cast<CEM0650*>(pOwner);
}

void CEM0650_Controller::AI_Tick(_double TimeDelta)
{
	if (m_pTarget == nullptr)
		return;

	if (IsCommandRunning() == false && m_pCastedOwner->IsPlayingSocket() == false)
	{
		DefineState(TimeDelta);
	}
}

void CEM0650_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;
	AddCommand("BackMove", 2.5f, &CAIController::Move_TurnToTarget, EMoveAxis::SOUTH, 1.f);
}

void CEM0650_Controller::Tick_Mid(_double TimeDelta)
{
	m_eDistance = DIS_MIDDLE;
	switch (m_iMidOrder)
	{
	case 0:
		AddCommand("Turn", 3.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack", 0.f, &CAIController::Input, MOUSE_LB); // Shoot
		break;
	case 1:
		AddCommand("RightMove", 3.5f, &CAIController::Move_TurnToTarget, EMoveAxis::EAST, 1.f);
		break;
	case 2:
		AddCommand("Turn", 3.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack", 0.f, &CAIController::Input, MOUSE_LB); // Shoot
		break;
	case 3:
		AddCommand("LeftMove", 3.5f, &CAIController::Move_TurnToTarget, EMoveAxis::WEST, 1.f);
		break;
	}

	m_iMidOrder = (m_iMidOrder + 1) % 4;
}

void CEM0650_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;
	AddCommand("ForwardMove", 2.5f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
}

void CEM0650_Controller::Tick_Outside(_double TimeDelta)
{
	m_eDistance = DIS_OUTSIDE;
	AddCommand("Wait", 2.f, &CAIController::Wait);
}

void CEM0650_Controller::DefineState(_double TimeDelta)
{
	if (m_pCastedOwner->IsPlayingSocket() == true) return;

	if (m_fTtoM_Distance <= 8.f)
		Tick_Near(TimeDelta);
	else if (m_fTtoM_Distance <= 13.f)
		Tick_Mid(TimeDelta);
	else if (m_fTtoM_Distance <= 18.f)
		Tick_Far(TimeDelta);
	else
		Tick_Outside(TimeDelta);
}



CEM0650_Controller * CEM0650_Controller::Create()
{
	CEM0650_Controller*		pInstance = new CEM0650_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0650_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CEM0650_Controller::Clone(void * pArg)
{
	CEM0650_Controller*		pInstance = new CEM0650_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0650_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEM0650_Controller::Free()
{
	CAIController::Free();
}
