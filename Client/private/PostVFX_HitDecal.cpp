#include "stdafx.h"
#include "..\public\PostVFX_HitDecal.h"



CPostVFX_HitDecal::CPostVFX_HitDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPostProcess(pDevice, pContext)
{
}

CPostVFX_HitDecal::CPostVFX_HitDecal(const CPostVFX_HitDecal& rhs)
	: CPostProcess(rhs)
{
}

HRESULT CPostVFX_HitDecal::Initialize(void* pArg)
{
	FAILED_CHECK(CPostProcess::Initialize(pArg));

	m_iPriority = 9999;

	m_tParam.Floats.push_back(0.f);

	return S_OK;
}

void CPostVFX_HitDecal::Tick(_double TimeDelta)
{
	CPostProcess::Tick(TimeDelta);

	if (m_tParam.Floats[0] <= 0.f)
		m_bVisible = false;
	else
		m_bVisible = true;
}

CPostVFX_HitDecal* CPostVFX_HitDecal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPostVFX_HitDecal*		pInstance = new CPostVFX_HitDecal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPostVFX_HitDecal");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPostVFX_HitDecal::Clone(void* pArg)
{
	CPostVFX_HitDecal*		pInstance = new CPostVFX_HitDecal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPostVFX_HitDecal");
		Safe_Release(pInstance);
	}
	return pInstance;
}
