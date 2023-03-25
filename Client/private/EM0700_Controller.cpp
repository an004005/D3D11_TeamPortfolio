#include "stdafx.h"
#include "..\public\EM0700_Controller.h"
#include <FSMComponent.h>
#include "EM0700.h"

CEM0700_Controller::CEM0700_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CEM0700_Controller::CEM0700_Controller(const CEM0700_Controller & rhs)
	: CAIController(rhs)
{
}

HRESULT CEM0700_Controller::Initialize(void * pArg)
{
	m_iNearOrder = CMathUtils::RandomUInt(3);
	m_iMidOrder = CMathUtils::RandomUInt(3);
	m_iOutOrder = CMathUtils::RandomUInt(1);

	//near : 회피( randomR, randomL, escape) , 돌진
	//mid :  발사, move
	//far :  move(발사 거리가 될때까지 앞으로 옴)

	//escape하면 뒤로 감
	//random은 제자리 돌아옴

	return S_OK;
}

void CEM0700_Controller::BeginTick()
{
	CAIController::BeginTick();

	CGameObject* pOwner = TryGetOwner();
	Assert(pOwner != nullptr);

	m_pCastedOwner = dynamic_cast<CEM0700*>(pOwner);
}

void CEM0700_Controller::AI_Tick(_double TimeDelta)
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

void CEM0700_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;


	//escape 실행시 몬스터에서 뒤에 벽이있는지 판단하에
	//있으면 randommove, 없으면 escape 실행

	switch (m_iNearOrder)
	{
	case 0:
		AddCommand("Turn", 1.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Escape", 0.f, &CAIController::Input, SPACE);
		break;
	case 1:
		AddCommand("Turn", 1.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Escape", 0.f, &CAIController::Input, SPACE);
		break;
	case 2:
		AddCommand("Turn", 1.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Escape", 0.f, &CAIController::Input, SPACE);
		break;
	case 3:
		AddCommand("Rush", 0.f, &CAIController::Input, C);
		break;
	}

	m_iNearOrder = (m_iNearOrder + 1) % 4;
}

void CEM0700_Controller::Tick_Mid(_double TimeDelta)
{ 
	m_eDistance = DIS_MIDDLE;

	switch (m_iMidOrder)
	{
	case 0:
		AddCommand("Turn", 1.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Shot", 0.f, &CAIController::Input, R);
		break;
	case 1:
		AddCommand("Turn", 1.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Walk", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::WEST, 1.f);
		break;
	case 2:
		AddCommand("Turn", 1.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Shot", 0.f, &CAIController::Input, R);
		break;
	case 3:
		AddCommand("Turn", 1.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Walk", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::EAST, 1.f);
		break;
	}

	m_iMidOrder = (m_iMidOrder + 1) % 4;
}

void CEM0700_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;
	AddCommand("Move", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
}

void CEM0700_Controller::Tick_Outside(_double TimeDelta)
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

void CEM0700_Controller::DefineState(_double TimeDelta)
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



CEM0700_Controller * CEM0700_Controller::Create()
{
	CEM0700_Controller*		pInstance = new CEM0700_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0700_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CEM0700_Controller::Clone(void * pArg)
{
	CEM0700_Controller*		pInstance = new CEM0700_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0700_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEM0700_Controller::Free()
{
	CAIController::Free();
}
