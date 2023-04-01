#include "stdafx.h"
#include "..\public\EM1100_Controller.h"
#include <FSMComponent.h>
#include "EM1100.h"

CEM1100_Controller::CEM1100_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CEM1100_Controller::CEM1100_Controller(const CEM1100_Controller & rhs)
	: CAIController(rhs)
{
}

HRESULT CEM1100_Controller::Initialize(void * pArg)
{
	m_iNearOrder = CMathUtils::RandomUInt(1);
	m_iFarOrder = CMathUtils::RandomUInt(2);
	//Move�Լ� �� �÷��̾�� �Ÿ��� ����������� ����������� �������� ������
	m_fNearestTargetDist = 5.f;

	//near :  ���� , ����ġ��(����ġ���� ȸ���ϸ鼭 ������, �÷��̾ �ڿ� ��������)
	//mid : ������ ,�ָ�, 
	//far : ����(��Ÿ�� ����), �޷��� �÷��̾� �ձ��� ��, �����(ü���� 50�� �����϶� ���)

	//ü���� 50���� �Ʒ��� �������� ���Ÿ� ������� �߰�

	//Rush ������ ���� ������� �ʱ� ������ ��Ÿ���� ��.

	//��� ���ݸ�� �ڿ� wait �Ǵ� walk�� �߰��ؼ� ���� ���ݱ��� ���� ��
	return S_OK;
}

void CEM1100_Controller::BeginTick()
{
	CAIController::BeginTick();

	CGameObject* pOwner = TryGetOwner();
	Assert(pOwner != nullptr);

	m_pCastedOwner = dynamic_cast<CEM1100*>(pOwner);
}

void CEM1100_Controller::AI_Tick(_double TimeDelta)
{
	if (m_pTarget == nullptr)
		return;

	m_bRun = false;
	m_dRushCoolTime[CURTIME] += TimeDelta;

	if (IsCommandRunning() == false && m_pCastedOwner->IsPlayingSocket() == false)
	{
		DefineState(TimeDelta);
	}
}

void CEM1100_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;
	
	//����
	if (m_pCastedOwner->IsTargetFront() && m_bAttack == true)
	{
		AddCommand("Turn", 0.5f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Dodge", 0.f, &CAIController::Input, SHIFT);	
		m_bAttack = false;
		return;
	}

	switch (m_iNearOrder)
	{
	case 0:
		if (m_pCastedOwner->IsTargetFront())
		{
			AddCommand("Turn", 2.f, &CAIController::TurnToTargetStop, 1.f);
			AddCommand("WaterAttack", 0.f, &CAIController::Input, W);
			AddCommand("Wait", 1.f, &CAIController::Wait);
			m_bAttack = true;
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
			m_bAttack = true;
		}
		else
			AddCommand("TailSwing", 0.f, &CAIController::Input, T);
		break;

	}

	m_iNearOrder = (m_iNearOrder + 1) % 2;
	
}


void CEM1100_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;

	if (m_dRushCoolTime[CURTIME] >= m_dRushCoolTime[MAXTIME])
	{
		//rush�ϱ��� �غ����� �� �ű⼭ Turn�� ����
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
			AddCommand("Run", 5.f, &CEM1100_Controller::Run_TurnToTarget, EMoveAxis::NORTH, 1.f);
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
	
	m_iFarOrder = (m_iFarOrder + 1) % 3;
}

void CEM1100_Controller::Tick_Outside(_double TimeDelta)
{
	m_eDistance = DIS_OUTSIDE;
	AddCommand("Wait", 2.f, &CAIController::Wait);
}

void CEM1100_Controller::Run_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio)
{
	m_bRun = true;

	//�̺�Ʈ�� ������ �ٴ� Ÿ�̹�
	if (m_pCastedOwner->Get_RunStart())
	{
		Move(eAxis);
		TurnToTarget(fSpeedRatio);
	}
}

void CEM1100_Controller::DefineState(_double TimeDelta)
{
	if (m_pCastedOwner->IsPlayingSocket() == true) return;

	if (m_fTtoM_Distance <= 7.f)
		Tick_Near(TimeDelta);
	else if (m_fTtoM_Distance <= 30.f)
		Tick_Far(TimeDelta);
	else
		Tick_Outside(TimeDelta);

}



CEM1100_Controller * CEM1100_Controller::Create()
{
	CEM1100_Controller*		pInstance = new CEM1100_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM1100_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CEM1100_Controller::Clone(void * pArg)
{
	CEM1100_Controller*		pInstance = new CEM1100_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM1100_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEM1100_Controller::Free()
{
	CAIController::Free();
}
