#include "stdafx.h"
#include "..\public\PostVFX_Penetrate.h"

CPostVFX_Penetrate::CPostVFX_Penetrate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPostProcess(pDevice, pContext)
{
}

CPostVFX_Penetrate::CPostVFX_Penetrate(const CPostVFX_Penetrate& rhs)
	: CPostProcess(rhs)
{
}

HRESULT CPostVFX_Penetrate::Initialize(void* pArg)
{
	FAILED_CHECK(CPostProcess::Initialize(pArg));

	m_iPriority = 1000;
	m_tParam.Floats.push_back(0.f);
	m_tParam.iPass = 8;
	return S_OK;
}

void CPostVFX_Penetrate::Tick(_double TimeDelta)
{
	CPostProcess::Tick(TimeDelta);

	if (m_tParam.Floats[0] <= 0.f)
		m_bVisible = false;
	else
		m_bVisible = true;
}
