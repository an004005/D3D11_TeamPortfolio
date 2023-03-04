#include "stdafx.h"
#include "..\public\PostVFX_Scifi.h"


CPostVFX_Scifi::CPostVFX_Scifi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPostProcess(pDevice, pContext)
{
}

CPostVFX_Scifi::CPostVFX_Scifi(const CPostVFX_Scifi& rhs)
	: CPostProcess(rhs)
{
}

HRESULT CPostVFX_Scifi::Initialize(void* pArg)
{
	FAILED_CHECK(CPostProcess::Initialize(pArg));

	m_iPriority = 9999;

	// if(pArg == nullptr)
	// {
		m_tParam.Floats.push_back(0.f);
	// }
	// if(m_tParam.Floats.size() < 1)

	// m_bVisible = true;
	return S_OK;
}

void CPostVFX_Scifi::Tick(_double TimeDelta)
{
	CPostProcess::Tick(TimeDelta);

	if (m_tParam.Floats[0] <= 0.f)
		m_bVisible = false;
	else
		m_bVisible = true;
}


CPostVFX_Scifi* CPostVFX_Scifi::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPostVFX_Scifi*		pInstance = new CPostVFX_Scifi(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPostVFX_Scifi");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPostVFX_Scifi::Clone(void* pArg)
{
	CPostVFX_Scifi*		pInstance = new CPostVFX_Scifi(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPostVFX_Scifi");
		Safe_Release(pInstance);
	}
	return pInstance;
}
