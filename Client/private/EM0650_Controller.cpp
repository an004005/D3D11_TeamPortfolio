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
	m_iNearOrder = CMathUtils::RandomUInt(1);
	m_iMidOrder = CMathUtils::RandomUInt(2);
	m_iFarOrder = CMathUtils::RandomUInt(1);
	m_iOutOrder = CMathUtils::RandomUInt(1);

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

	// 대상과의 거리 
	m_fToTargetDistance = XMVectorGetX(XMVector3LengthEst(
		m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)
		- m_pCastedOwner->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)));

	if (IsCommandRunning() == false && m_pCastedOwner->IsPlayingSocket() == false)
	{
		DefineState(TimeDelta);
	}
}

void CEM0650_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;

	switch (m_iNearOrder)
	{
	case 0:
		AddCommand("Escape", 0.f, &CAIController::Input, SPACE);
		break;
	case 1:
		AddCommand("Attack_a1", 0.f, &CAIController::Input, NUM_1);
		break;
	}

	m_iNearOrder = (m_iNearOrder + 1) % 2;
}

void CEM0650_Controller::Tick_Mid(_double TimeDelta)
{ 
	m_eDistance = DIS_MIDDLE;

	switch (m_iMidOrder)
	{
	case 0:
		AddCommand("RandomMove_L", 0.f, &CAIController::Input, MOUSE_LB);
		break;
	case 1:
		AddCommand("RandomMove_R", 0.f, &CAIController::Input, MOUSE_RB);
		break;
	case 2:
		AddCommand("Threat", 0.f, &CAIController::Input, C);
		break;
	}

	m_iMidOrder = (m_iMidOrder + 1) % 3;
}

void CEM0650_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;

	switch (m_iFarOrder)
	{
	case 0:
		AddCommand("Turn", 1.5f, &CAIController::TurnToTarget, 1.f);
		AddCommand("Attack_a3", 0.f, &CAIController::Input, NUM_2);
		break;
	case 1:
		AddCommand("Threat", 0.f, &CAIController::Input, C);
		break;
	}

	m_iFarOrder = (m_iFarOrder + 1) % 2;
}

void CEM0650_Controller::Tick_Outside(_double TimeDelta)
{

	m_eDistance = DIS_OUTSIDE;

	switch (m_iOutOrder)
	{
	case 0:
		AddCommand("Wait", 2.f, &CAIController::Wait);
		break;
	case 1:
		AddCommand("Threat", 0.f, &CAIController::Input, C);
		break;
	}
	m_iOutOrder = (m_iFarOrder + 1) % 2;
}

void CEM0650_Controller::DefineState(_double TimeDelta)
{
	if (m_pCastedOwner->IsPlayingSocket() == true) return;

	if (m_fToTargetDistance <= 4.f)
		Tick_Near(TimeDelta);
	else if (m_fToTargetDistance <= 12.f)
		Tick_Mid(TimeDelta);
	else if (m_fToTargetDistance <= 20.f)
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
