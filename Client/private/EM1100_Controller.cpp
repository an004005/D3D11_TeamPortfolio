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
	m_iMidOrder = CMathUtils::RandomUInt(3);

	//Move�Լ� �� �÷��̾�� �Ÿ��� ����������� ����������� �������� ������
	m_fTargetDist = 3.f;


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

	// ������ �Ÿ� 
	m_fToTargetDistance = XMVectorGetX(XMVector3LengthEst(
		m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)
		- m_pCastedOwner->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)));

	if (IsCommandRunning() == false && m_pCastedOwner->IsPlayingSocket() == false)
	{
		DefineState(TimeDelta);
	}
}

void CEM1100_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;
	
	//����
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

void CEM1100_Controller::Tick_Mid(_double TimeDelta)
{ 
	m_eDistance = DIS_MIDDLE;

	switch (m_iMidOrder)
	{
	case 0:
		AddCommand("Turn", 2.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("WaterAttack", 0.f, &CAIController::Input, W);
		AddCommand("Wait", 1.f, &CAIController::Wait);
		break;
	case 1:
		AddCommand("Walk", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::WEST, 1.f);
		break;
	case 2:
		AddCommand("Turn", 2.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Stamp", 0.f, &CAIController::Input, S);
		AddCommand("Wait", 1.f, &CAIController::Wait);
		break;
	case 3:
		AddCommand("Walk", 2.f, &CAIController::Move_TurnToTarget, EMoveAxis::EAST, 1.f);
		break;
	}

	m_iMidOrder = (m_iMidOrder + 1) % 4;
}

void CEM1100_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;

	if (m_dRushCoolTime[CURTIME] >= m_dRushCoolTime[MAXTIME])
	{
		AddCommand("Rush", 0.f, &CAIController::Input, R);
		AddCommand("Wait", 1.5f, &CAIController::Wait);
		m_dRushCoolTime[CURTIME] = 0.0;
		return;
	}

	if (m_pCastedOwner->GetHpRatio() >= 0.5f)
	{
		AddCommand("Turn", 2.f, &CAIController::TurnToTargetStop, 1.f);
		AddCommand("Run", 1.f, &CEM1100_Controller::Run_TurnToTarget, EMoveAxis::NORTH, 1.f);
	}
	else
	{
		AddCommand("ElectricBall", 0.f, &CAIController::Input, E);
		AddCommand("Wait", 1.5f, &CAIController::Wait);
	}
	
	m_dRushCoolTime[CURTIME] += TimeDelta;
}

void CEM1100_Controller::Tick_Outside(_double TimeDelta)
{
	m_eDistance = DIS_OUTSIDE;
	AddCommand("Wait", 2.f, &CAIController::Wait);
}

void CEM1100_Controller::Run_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio)
{
	m_bRun = true;
	Move(eAxis);
	TurnToTarget(fSpeedRatio);
}

void CEM1100_Controller::DefineState(_double TimeDelta)
{
	if (m_pCastedOwner->IsPlayingSocket() == true) return;

	if (m_fToTargetDistance <= 3.f)
		Tick_Near(TimeDelta);
	else if (m_fToTargetDistance <= 8.f)
		Tick_Mid(TimeDelta);
	else if (m_fToTargetDistance <= 30.f)
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
