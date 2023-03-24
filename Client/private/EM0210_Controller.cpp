#include "stdafx.h"
#include "..\public\EM0210_Controller.h"
#include <FSMComponent.h>
#include "EM0210.h"

CEM0210_Controller::CEM0210_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CEM0210_Controller::CEM0210_Controller(const CEM0210_Controller & rhs)
	: CAIController(rhs)
{
}

HRESULT CEM0210_Controller::Initialize(void * pArg)
{
	m_iNearOrder = CMathUtils::RandomUInt(6);
	m_iMidOrder = CMathUtils::RandomUInt(3);
	m_iFarOrder = CMathUtils::RandomUInt(3);
	
	//멀리 있을땐 달려서 가까이 옴
	//중간 정도에선 걷거나 윈드밀 하면서 가까이 옴
	//가까이 있을때는 윈드밀, 올려차기

	//스킬쓰고나서도 가까이 있으면 한번 뒤로 점프,
	//뒤로 점프하면 중간거리로 바로 되니 걸어오거나 윈드밀

	//위에 패턴 반복
	return S_OK;
}

void CEM0210_Controller::BeginTick()
{
	CAIController::BeginTick();

	CGameObject* pOwner = TryGetOwner();
	Assert(pOwner != nullptr);

	m_pCastedOwner = dynamic_cast<CEM0210*>(pOwner);
}

void CEM0210_Controller::AI_Tick(_double TimeDelta)
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

void CEM0210_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;

	switch (m_iNearOrder)
	{
	case 0:
		AddCommand("Turn", 2.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_Spin", 0.f, &CAIController::Input, R);
		break;
	case 1:
		AddCommand("Dodge_L", 0.f, &CAIController::Input, NUM_1);
		break;
	case 2:
		AddCommand("Turn", 2.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_Somersault", 0.f, &CAIController::Input, G);
		break;
	case 3:
		AddCommand("Dodge_B", 0.f, &CAIController::Input, NUM_2);
		break;
	case 4:
		AddCommand("Turn", 2.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_Spin", 0.f, &CAIController::Input, R);
		break;
	case 5:
		AddCommand("Dodge_R", 0.f, &CAIController::Input, NUM_3);
		break;
	case 6:
		AddCommand("Turn", 2.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_Somersault", 0.f, &CAIController::Input, G);
		break;
	}
	
	m_iNearOrder = (m_iNearOrder + 1) % 7;
}

void CEM0210_Controller::Tick_Mid(_double TimeDelta)
{
	m_eDistance = DIS_MIDDLE;

	switch (m_iMidOrder)
	{
	case 0:
		AddCommand("WalkTurn", 2.5f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	case 1:
		AddCommand("Turn", 2.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Attack_Spin", 0.f, &CAIController::Input, R);
		break;
	case 2:
		AddCommand("Wait", 2.f, &CAIController::Wait);
		break;
	case 3:
		AddCommand("WalkTurn", 2.5f, &CAIController::Move_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
		
	}
	m_iMidOrder = (m_iMidOrder + 1) % 4;
}


void CEM0210_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;

	switch (m_iFarOrder)
	{
	case 0:
		AddCommand("Wait", 2.f, &CAIController::Wait);
		break;
	case 1:
		AddCommand("Run", 1.f, &CEM0210_Controller::Run_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	case 2:
		AddCommand("Threat", 0.f, &CAIController::Input, C);
		break;
	case 3:
		AddCommand("Run", 1.f, &CEM0210_Controller::Run_TurnToTarget, EMoveAxis::NORTH, 1.f);
		break;
	}

	m_iFarOrder = (m_iFarOrder + 1) % 4;

}

void CEM0210_Controller::Tick_Outside(_double TimeDelta)
{
	m_eDistance = DIS_OUTSIDE;
	AddCommand("Wait", 2.f, &CAIController::Wait);
}


void CEM0210_Controller::Run_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio)
{
	m_bRun = true;
	Move(eAxis);
	TurnToTarget(fSpeedRatio);
}

void CEM0210_Controller::DefineState(_double TimeDelta)
{
	if (m_pCastedOwner->IsPlayingSocket() == true) return;

	if (m_fToTargetDistance <= 6.f)
		Tick_Near(TimeDelta);
	else if (m_fToTargetDistance <= 12.f)
		Tick_Mid(TimeDelta);
	else if (m_fToTargetDistance <= 25.f)
		Tick_Far(TimeDelta);
	else
		Tick_Outside(TimeDelta);
}



CEM0210_Controller * CEM0210_Controller::Create()
{
	CEM0210_Controller*		pInstance = new CEM0210_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0210_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CEM0210_Controller::Clone(void * pArg)
{
	CEM0210_Controller*		pInstance = new CEM0210_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0210_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEM0210_Controller::Free()
{
	CAIController::Free();
}
