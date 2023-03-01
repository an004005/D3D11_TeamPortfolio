#include "stdafx.h"
#include "..\public\PostVFX_Distortion.h"


CPostVFX_Distortion::CPostVFX_Distortion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPostProcess(pDevice, pContext)
{
}

CPostVFX_Distortion::CPostVFX_Distortion(const CPostVFX_Distortion& rhs)
	: CPostProcess(rhs)
{
}

HRESULT CPostVFX_Distortion::Initialize(void* pArg)
{
	FAILED_CHECK(CPostProcess::Initialize(pArg));

	m_iPriority = 9999;

	m_tParam.Floats.push_back(0.f);

	return S_OK;
}

void CPostVFX_Distortion::Tick(_double TimeDelta)
{
	CPostProcess::Tick(TimeDelta);

	if (m_tParam.Floats[0] <= 0.f)
		m_bVisible = false;
	else
		m_bVisible = true;
}

// void CPostVFX_Distortion::SetWhiteOutTime(_float fWhiteOutTime)
// {
// 	m_tParam.Floats[0] = fWhiteOutTime;
// }

CPostVFX_Distortion* CPostVFX_Distortion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPostVFX_Distortion*		pInstance = new CPostVFX_Distortion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPostVFX_Distortion");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPostVFX_Distortion::Clone(void* pArg)
{
	CPostVFX_Distortion*		pInstance = new CPostVFX_Distortion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPostVFX_Distortion");
		Safe_Release(pInstance);
	}
	return pInstance;
}
