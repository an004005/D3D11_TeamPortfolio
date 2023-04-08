#include "stdafx.h"
#include "..\public\EM0900_Controller.h"
#include <FSMComponent.h>
#include "EM0900.h"

CEM0900_Controller::CEM0900_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CEM0900_Controller::CEM0900_Controller(const CEM0900_Controller & rhs)
	: CAIController(rhs)
{
}

HRESULT CEM0900_Controller::Initialize(void * pArg)
{
	m_iNearOrder = CMathUtils::RandomUInt(3);

	//near :  돌리고 찍고 이동 
	//far : 던지기

	//던지기 start모션중 플레이어 위치가 가까워지면 feint 동작을 취한다.
	//몬스터 클래스에서 조건 처리

	//던질때 loop동안 플레이어쪽을 바라보고 HIT로 넘어감
	return S_OK;
}

void CEM0900_Controller::BeginTick()
{
	CAIController::BeginTick();

	CGameObject* pOwner = TryGetOwner();
	Assert(pOwner != nullptr);

	m_pCastedOwner = dynamic_cast<CEM0900*>(pOwner);
}

void CEM0900_Controller::AI_Tick(_double TimeDelta)
{
	if (m_pTarget == nullptr)
		return;

	if (IsCommandRunning() == false && m_pCastedOwner->IsPlayingSocket() == false)
	{
		DefineState(TimeDelta);
	}
}

void CEM0900_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;
	

	switch (m_iNearOrder)
	{
	case 0:
		AddCommand("4Attack", 0.f, &CAIController::Input, F);
		break;
	case 1:
		AddCommand("Spin", 0.f, &CAIController::Input, S);
		break;
	case 2:
		AddCommand("Walk", 2.f, &CAIController::Move_TurnToTarget, static_cast<EMoveAxis>(CMathUtils::RandomUInt(7)), 1.f);
		break;

	}

	m_iNearOrder = (m_iNearOrder + 1) % 3;
	
}


void CEM0900_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;
	AddCommand("Throw", 0.f, &CAIController::Input, T);
}

void CEM0900_Controller::Tick_Outside(_double TimeDelta)
{
	m_eDistance = DIS_OUTSIDE;
	AddCommand("Wait", 2.f, &CAIController::Wait);
}

void CEM0900_Controller::DefineState(_double TimeDelta)
{
	if (m_pCastedOwner->IsPlayingSocket() == true) return;

	if (m_fTtoM_Distance <= 7.f)
		Tick_Near(TimeDelta);
	else if (m_fTtoM_Distance <= 30.f)
		Tick_Far(TimeDelta);
	else
		Tick_Outside(TimeDelta);

}



CEM0900_Controller * CEM0900_Controller::Create()
{
	CEM0900_Controller*		pInstance = new CEM0900_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0900_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CEM0900_Controller::Clone(void * pArg)
{
	CEM0900_Controller*		pInstance = new CEM0900_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0900_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEM0900_Controller::Free()
{
	CAIController::Free();
}
