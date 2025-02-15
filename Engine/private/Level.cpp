#include "stdafx.h"
#include "..\public\Level.h"

CLevel::CLevel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLevel::Initialize()
{
	return S_OK;
}

void CLevel::Tick(_double TimeDelta)
{
}

void CLevel::Late_Tick(_double TimeDelta)
{
}

HRESULT CLevel::Render()
{
	return S_OK;
}

void CLevel::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}

