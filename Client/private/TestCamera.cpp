#include "stdafx.h"
#include "..\public\TestCamera.h"

#include <MathUtils.h>

#include "GameInstance.h"

CTestCamera::CTestCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CTestCamera::CTestCamera(const CTestCamera& rhs)
	: CCamera(rhs)
{
}

HRESULT CTestCamera::Initialize(void* pArg)
{
	FAILED_CHECK(CCamera::Initialize(pArg));

	m_pTransformCom->SetSpeed(1.f);
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(90.f));
	m_pTransformCom->SetPitchLock_Deg(70.f);
	m_fBoomLength = 15.f;

	return S_OK;
}

void CTestCamera::Tick(_double TimeDelta)
{
	CCamera::Tick(TimeDelta);

	if (CMathUtils::FloatCmp(m_vMouseAxis.x, 0.f) == false
		|| CMathUtils::FloatCmp(m_vMouseAxis.y, 0.f) == false)
	{
		m_pTransformCom->Turn(_float4::UnitY, TimeDelta * m_vMouseAxis.x * 0.1f);
		m_pTransformCom->TurnPitch(TimeDelta * m_vMouseAxis.y * 0.1f);
	}
}

void CTestCamera::Late_Tick(_double TimeDelta)
{
	CCamera::Late_Tick(TimeDelta);
}

void CTestCamera::AttachTo(_fvector vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
	m_pTransformCom->Go_Backward(m_fBoomLength);
}

CTestCamera* CTestCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTestCamera*		pInstance = new CTestCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTestCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTestCamera::Clone(void* pArg)
{
	CTestCamera*		pInstance = new CTestCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTestCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTestCamera::Free()
{
	CCamera::Free();
}
