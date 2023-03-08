#include "stdafx.h"
#include "..\public\PostVFX_ColorGrading.h"

CPostVFX_ColorGrading::CPostVFX_ColorGrading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPostProcess(pDevice, pContext)
{
}

CPostVFX_ColorGrading::CPostVFX_ColorGrading(const CPostVFX_ColorGrading& rhs)
	: CPostProcess(rhs)
{
}

HRESULT CPostVFX_ColorGrading::Initialize(void* pArg)
{
	FAILED_CHECK(CPostProcess::Initialize(pArg));

	m_tParam.Floats.push_back(1.f);

	m_tParam.iPass = 6;

	return S_OK;
}

void CPostVFX_ColorGrading::Tick(_double TimeDelta)
{
	CPostProcess::Tick(TimeDelta);

	if (m_tParam.Floats[0] <= 0.f)
		m_bVisible = false;
	else
		m_bVisible = true;
}

CPostVFX_ColorGrading* CPostVFX_ColorGrading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPostVFX_ColorGrading*		pInstance = new CPostVFX_ColorGrading(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPostVFX_ColorGrading");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPostVFX_ColorGrading::Clone(void* pArg)
{
	CPostVFX_ColorGrading*		pInstance = new CPostVFX_ColorGrading(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPostVFX_ColorGrading");
		Safe_Release(pInstance);
	}
	return pInstance;
}

