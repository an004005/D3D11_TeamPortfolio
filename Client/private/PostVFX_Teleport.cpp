#include "stdafx.h"
#include "..\public\PostVFX_Teleport.h"

CPostVFX_Teleport::CPostVFX_Teleport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPostProcess(pDevice, pContext)
{
}

CPostVFX_Teleport::CPostVFX_Teleport(const CPostVFX_Teleport& rhs)
	: CPostProcess(rhs)
{
}

HRESULT CPostVFX_Teleport::Initialize(void* pArg)
{
	FAILED_CHECK(CPostProcess::Initialize(pArg));

	m_iPriority = 999;
	m_tParam.Floats.push_back(0.f);
	m_tParam.iPass = 10;
	return S_OK;
}

void CPostVFX_Teleport::Tick(_double TimeDelta)
{
	CPostProcess::Tick(TimeDelta);

	if (m_tParam.Floats[0] <= 0.f)
		m_bVisible = false;
	else
		m_bVisible = true;
}
