#include "stdafx.h"
#include "..\public\EM0800_Controller.h"
#include <FSMComponent.h>
#include "EM0800.h"

CEM0800_Controller::CEM0800_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CEM0800_Controller::CEM0800_Controller(const CEM0800_Controller & rhs)
	: CAIController(rhs)
{
}

HRESULT CEM0800_Controller::Initialize(void * pArg)
{
	m_iNearOrder = CMathUtils::RandomUInt(9);
	m_iMidOrder = CMathUtils::RandomUInt(7);
	m_iFarOrder = CMathUtils::RandomUInt(3);
	//m_iOutOrder = CMathUtils::RandomUInt(1);

	//near : 회피( randomR, randomL, escape) , 돌진
	//mid :  발사, move
	//far :  move(발사 거리가 될때까지 앞으로 옴)

	//escape하면 뒤로 감
	//random은 제자리 돌아옴

	return S_OK;
}

void CEM0800_Controller::BeginTick()
{
	CAIController::BeginTick();

	CGameObject* pOwner = TryGetOwner();
	Assert(pOwner != nullptr);

	m_pCastedOwner = dynamic_cast<CEM0800*>(pOwner);
}

void CEM0800_Controller::AI_Tick(_double TimeDelta)
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

void CEM0800_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;
	// 1. 좌 우 뒤 이동(플레이어 보면서) 2. 전방 물기 공격 3. 레이저 공격
	EMoveAxis eMove = rand() % 2 ? EMoveAxis::WEST : EMoveAxis::EAST;
	switch (m_iNearOrder)
	{
	case 0:
		AddCommand("MoveL_or_R", 2.7f, &CAIController::Move_TurnToTarget, eMove, 1.f);
		break;
	case 1:
		AddCommand("Wait", 1.3f, &CAIController::Wait);
		break;
	case 2:	// Bite_Atk
		AddCommand("Attack_Bite", 0.f, &CAIController::Input, MOUSE_RB);
		break;
	case 3:
		AddCommand("MoveL_or_R", 2.7f, &CAIController::Move_TurnToTarget, eMove, 1.f);
		break;
	case 4:
		AddCommand("Wait", 1.3f, &CAIController::Wait);
		break;
	case 5:
		AddCommand("Turn", 3.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_Laser", 0.f, &CAIController::Input, MOUSE_LB);
		AddCommand("TurnSlow", 0.7f, &CAIController::TurnToTarget, 0.5f);
		break;
	case 6:
		AddCommand("Wait", 1.3f, &CAIController::Wait);
		break;
	case 7:
		AddCommand("BackMove", 2.3f, &CAIController::Move, EMoveAxis::SOUTH);
		break;
	case 8:
		AddCommand("Attack_Bite", 0.f, &CAIController::Input, MOUSE_RB);
		break;
	case 9:
		AddCommand("Turn", 3.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_Laser", 0.f, &CAIController::Input, MOUSE_LB);
		AddCommand("TurnSlow", 0.7f, &CAIController::TurnToTarget, 0.5f);
		break;
	}

	//	_uint iRand = (rand() % 2) + 1;
	m_iNearOrder = (m_iNearOrder + 1) % 10;
}

void CEM0800_Controller::Tick_Mid(_double TimeDelta)
{ 
	m_eDistance = DIS_MIDDLE;
	EMoveAxis eMove = rand() % 2 ? EMoveAxis::WEST : EMoveAxis::EAST;
	// 1. 좌 우 앞 이동 2. 레이저 공격
	switch (m_iMidOrder)
	{
	case 0:
		AddCommand("MoveL_or_R", 2.7f, &CAIController::Move_TurnToTarget, eMove, 1.f);
		break;
	case 1:
		AddCommand("Wait", 1.3f, &CAIController::Wait);
		break;
	case 2:
		AddCommand("Turn", 3.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_Laser", 0.f, &CAIController::Input, MOUSE_LB);
		AddCommand("TurnSlow", 0.7f, &CAIController::TurnToTarget, 0.5f);
		break;
	case 3:
		AddCommand("Wait", 2.f, &CAIController::Wait);
		break;
	case 4:
		AddCommand("MoveL_or_R", 2.7f, &CAIController::Move_TurnToTarget, eMove, 1.f);
		break;
	case 5:
		AddCommand("Wait", 1.3f, &CAIController::Wait);
		break;
	case 6:
		AddCommand("ForwardMove", 2.3f, &CAIController::Move, EMoveAxis::NORTH);
		break;
	case 7:
		AddCommand("Turn", 3.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_Laser", 0.f, &CAIController::Input, MOUSE_LB);
		AddCommand("TurnSlow", 0.7f, &CAIController::TurnToTarget, 0.5f);
		break;
	}

	//	_uint iRand = (rand() % 2) + 1;
	m_iMidOrder = (m_iMidOrder + 1) % 8;
}

void CEM0800_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;

	switch (m_iFarOrder)
	{
	case 0:
		AddCommand("Wait", 4.f, &CAIController::Wait);
		break;
	case 1:
		AddCommand("Wait", 4.f, &CAIController::Wait);
		break;
	case 2:
		AddCommand("Threat", 0.f, &CAIController::Input, G);
		break;
	case 3:
		AddCommand("Wait", 4.f, &CAIController::Wait);
		break;
	}

	m_iFarOrder = (m_iFarOrder + 1) % 4;
}

void CEM0800_Controller::Tick_Outside(_double TimeDelta)
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

void CEM0800_Controller::DefineState(_double TimeDelta)
{
	if (m_pCastedOwner->IsPlayingSocket() == true) return;

	if (m_fToTargetDistance <= 8.f)
		Tick_Near(TimeDelta);
	else if (m_fToTargetDistance <= 20.f)
		Tick_Mid(TimeDelta);
	else
		Tick_Far(TimeDelta);
	

}



CEM0800_Controller * CEM0800_Controller::Create()
{
	CEM0800_Controller*		pInstance = new CEM0800_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0800_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CEM0800_Controller::Clone(void * pArg)
{
	CEM0800_Controller*		pInstance = new CEM0800_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0800_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEM0800_Controller::Free()
{
	CAIController::Free();
}
