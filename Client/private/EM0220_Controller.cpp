#include "stdafx.h"
#include "..\public\EM0220_Controller.h"
#include <FSMComponent.h>
#include "EM0220.h"

CEM0220_Controller::CEM0220_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CEM0220_Controller::CEM0220_Controller(const CEM0220_Controller & rhs)
	: CAIController(rhs)
{
}

HRESULT CEM0220_Controller::Initialize(void * pArg)
{
	return S_OK;
}

void CEM0220_Controller::BeginTick()
{
	CAIController::BeginTick();

	CGameObject* pOwner = TryGetOwner();
	Assert(pOwner != nullptr);

	m_pCastedOwner = dynamic_cast<CEM0220*>(pOwner);
}

void CEM0220_Controller::AI_Tick(_double TimeDelta)
{
	if (m_pTarget == nullptr)
		return;

	// 대상과의 거리 
	m_fToTargetDistance = XMVectorGetX(XMVector3LengthEst(
		m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)
		- m_pCastedOwner->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)));

	_bool tt = IsCommandRunning() == false;
	_bool ttt = m_pCastedOwner->IsPlayingSocket() == false;
	if (IsCommandRunning() == false && m_pCastedOwner->IsPlayingSocket() == false)
	{
		DefineState(TimeDelta);
	}
}

void CEM0220_Controller::Tick_Near(_double TimeDelta)
{
	m_eDistance = DIS_NEAR;
	AddCommand("Guard", 0.f, &CAIController::Input, SHIFT);
}

void CEM0220_Controller::Tick_Far(_double TimeDelta)
{
	m_eDistance = DIS_FAR;
	AddCommand("Attack_Shot", 0.f, &CAIController::Input, C);
}

void CEM0220_Controller::Tick_Outside(_double TimeDelta)
{
	m_eDistance = DIS_OUTSIDE;
	AddCommand("Wait", 3.f, &CAIController::Wait);
}

_bool CEM0220_Controller::Check_TargetDist(_float fDist, _bool IsGreater)
{
	if (IsGreater == true)
		return m_fToTargetDistance >= fDist;
	else
		return m_fToTargetDistance <= fDist;
}

void CEM0220_Controller::DefineState(_double TimeDelta)
{
	if (m_pCastedOwner->IsPlayingSocket() == true) return;

	if (m_fToTargetDistance <= 10.f)
		Tick_Near(TimeDelta);
	else
		Tick_Far(TimeDelta);
}



CEM0220_Controller * CEM0220_Controller::Create()
{
	CEM0220_Controller*		pInstance = new CEM0220_Controller(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0220_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CEM0220_Controller::Clone(void * pArg)
{
	CEM0220_Controller*		pInstance = new CEM0220_Controller(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0220_Controller");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEM0220_Controller::Free()
{
	CAIController::Free();
}
