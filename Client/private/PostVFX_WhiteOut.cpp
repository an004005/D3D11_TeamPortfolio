#include "stdafx.h"
#include "..\public\PostVFX_WhiteOut.h"


CPostVFX_WhiteOut::CPostVFX_WhiteOut(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPostProcess(pDevice, pContext)
{
}

CPostVFX_WhiteOut::CPostVFX_WhiteOut(const CPostVFX_WhiteOut& rhs)
	: CPostProcess(rhs)
{
}

HRESULT CPostVFX_WhiteOut::Initialize(void* pArg)
{
	FAILED_CHECK(CPostProcess::Initialize(pArg));

	m_iPriority = 99999;// ���� ���߿� ����
	// m_bBeforeUI = false; // after ui
	m_tParam.iPass = 1;
	m_tParam.Floats.push_back(0.f);
	m_tParam.Ints.push_back(0);

	m_Timeline.SetCurve("RiseDown");
	return S_OK;
}

void CPostVFX_WhiteOut::Tick(_double TimeDelta)
{
	CPostProcess::Tick(TimeDelta);

	if (m_tParam.Floats[0] <= 0.f)
		m_bVisible = false;
	else
		m_bVisible = true;


	m_Timeline.Tick(TimeDelta, m_tParam.Floats[0]);
}

void CPostVFX_WhiteOut::SetWhiteOutTime(_float fWhiteOutTime)
{
	m_tParam.Floats[0] = fWhiteOutTime;
}

void CPostVFX_WhiteOut::BlackInOut()
{
	m_Timeline.PlayFromStart();
	m_tParam.Ints[0] = 1;
}
