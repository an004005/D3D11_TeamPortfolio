#include "stdafx.h"
#include "PostVFX_Cinema.h"

CPostVFX_Cinema::CPostVFX_Cinema(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPostProcess(pDevice, pContext)
{
}

CPostVFX_Cinema::CPostVFX_Cinema(const CPostVFX_Cinema& rhs)
	: CPostProcess(rhs)
{

}

HRESULT CPostVFX_Cinema::Initialize(void* pArg)
{
	FAILED_CHECK(CPostProcess::Initialize(pArg));

	m_iPriority = 999999;
	m_tParam.iPass = 11;
	if (m_tParam.Floats.empty())
		m_tParam.Floats.push_back(0.f);
	m_tParam.Floats[0] = 0.f;

	return S_OK;
}

void CPostVFX_Cinema::Tick(_double TimeDelta)
{
	CPostProcess::Tick(TimeDelta);

	if (m_bActive)
	{
		if (m_tParam.Floats[0] < 1.f)
			m_tParam.Floats[0] += (_float)TimeDelta;
		else
			m_tParam.Floats[0] = 1.f;
	}
	else
	{
		if (m_tParam.Floats[0] > 0.f)
			m_tParam.Floats[0] -= (_float)TimeDelta;
		else
			m_tParam.Floats[0] = 0.f;
	}

	if (m_tParam.Floats[0] <= 0.f)
		m_bVisible = false;
	else
		m_bVisible = true;
}

void CPostVFX_Cinema::Active(_bool bActive)
{
	m_bActive = bActive;
}

CPostVFX_Loading::CPostVFX_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPostProcess(pDevice, pContext)
{
}

CPostVFX_Loading::CPostVFX_Loading(const CPostVFX_Loading& rhs)
	: CPostProcess(rhs)
{
}

HRESULT CPostVFX_Loading::Initialize(void* pArg)
{
	FAILED_CHECK(CPostProcess::Initialize(pArg));

	m_iPriority = 999999;
	if (m_tParam.Floats.empty())
		m_tParam.Floats.push_back(0.f);

	m_tParam.Floats[0] = 0.f;
	m_bVisible = true;

	return S_OK;
}

void CPostVFX_Loading::Tick(_double TimeDelta)
{
	CPostProcess::Tick(TimeDelta);
	if (m_tParam.Floats[0] < 1.f)
	{
		m_tParam.Floats[0] += TimeDelta;
	}
	else
	{
		m_tParam.Floats[0] = 1.f;
	}

	if (m_tParam.Floats[0] <= 0.f)
		m_bVisible = false;
	else
		m_bVisible = true;
}

