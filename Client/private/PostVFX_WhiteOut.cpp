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

	m_iPriority = 9999;// 제일 나중에 실행
	// m_bBeforeUI = false; // after ui
	m_tParam.Floats.push_back(0.f);
	return S_OK;
}

void CPostVFX_WhiteOut::Tick(_double TimeDelta)
{
	CPostProcess::Tick(TimeDelta);

	if (m_tParam.Floats[0] <= 0.f)
		m_bVisible = false;
	else
		m_bVisible = true;
}

void CPostVFX_WhiteOut::SetWhiteOutTime(_float fWhiteOutTime)
{
	m_tParam.Floats[0] = fWhiteOutTime;
}
