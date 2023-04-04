#include "stdafx.h"
#include "EMCable.h"
#include "VFX_Manager.h"

CEMCable::CEMCable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CEMCable::CEMCable(const CGameObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEMCable::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(nullptr));

	return S_OK;
}

void CEMCable::BeginTick()
{
	__super::BeginTick();

	m_pCableEff = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em1200_Ribbon");
	m_pCableEff->Start_NoAttach(this, true);
	Safe_AddRef(m_pCableEff);
	
	//�Ʒ����� �ö�;��ϴϱ� ������� �Ʒ��� ����
	m_Depart = XMLoadFloat4(&m_Dest) - XMVectorSet(0.f, 1.f, 0.f, 0.f) * 8.f;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, m_Depart);
}

void CEMCable::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	if (m_bReverse == false && m_Dest.y <= XMVectorGetY(vPosition))
	{
		m_Delay += TimeDelta;

		if (m_Delay > 0.3)
			m_bReverse = true;

		return;
	}
	else if (m_bReverse == true && m_Depart.y >= XMVectorGetY(vPosition))
	{
		m_pCableEff->SetDelete();
		Safe_Release(m_pCableEff);
		m_pCableEff = nullptr;
		
		m_bDelete = true;
		return;
	}


	if (m_bReverse)
	{
		vPosition -= XMVectorSet(0.f, 1.f, 0.f, 0.f) * 40.f * TimeDelta;
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPosition);
	}
	else
	{
		vPosition += XMVectorSet(0.f, 1.f, 0.f, 0.f) * 40.f * TimeDelta;
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPosition);
	}

	
}


CEMCable* CEMCable::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEMCable* pInstance = new CEMCable(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEMCable");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEMCable::Clone(void* pArg)
{
	CEMCable* pInstance = new CEMCable(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEMCable");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEMCable::Free()
{
	__super::Free();
}
