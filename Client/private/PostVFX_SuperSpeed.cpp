#include "stdafx.h"
#include "..\public\PostVFX_SuperSpeed.h"


CPostVFX_SuperSpeed::CPostVFX_SuperSpeed(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPostProcess(pDevice, pContext)
{
}

CPostVFX_SuperSpeed::CPostVFX_SuperSpeed(const CPostVFX_SuperSpeed& rhs)
	: CPostProcess(rhs)
{
}

HRESULT CPostVFX_SuperSpeed::Initialize(void* pArg)
{
	FAILED_CHECK(CPostProcess::Initialize(pArg));

	m_iPriority = 998;
	m_tParam.iPass = 11;
	if (m_tParam.Floats.empty())
		m_tParam.Floats.push_back(0.f);
	m_tParam.Floats[0] = 0.f;

	return S_OK;
}

void CPostVFX_SuperSpeed::Tick(_double TimeDelta)
{
	CPostProcess::Tick(TimeDelta);

	if (m_bActive)
	{
		if (m_tParam.Floats[0] < 1.f)
			m_tParam.Floats[0] += (_float)TimeDelta * 2.f;
		else
			m_tParam.Floats[0] = 1.f;
	}
	else
	{
		if (m_tParam.Floats[0] > 0.f)
			m_tParam.Floats[0] -= (_float)TimeDelta * 2.f;
		else
			m_tParam.Floats[0] = 0.f;
	}

	if (m_tParam.Floats[0] <= 0.f)
		m_bVisible = false;
	else
		m_bVisible = true;
}

void CPostVFX_SuperSpeed::Active(_bool bActive)
{
	m_bActive = bActive;
}
