#include "stdafx.h"
#include "EMGift.h"

CEMGift::CEMGift(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CEMGift::CEMGift(const CEMGift& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEMGift::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CEMGift::BeginTick()
{
}

void CEMGift::Tick(_double TimeDelta)
{
}

void CEMGift::Late_Tick(_double TimeDelta)
{
}

HRESULT CEMGift::Render()
{
	return E_NOTIMPL;
}

CEMGift* CEMGift::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CGameObject* CEMGift::Clone(void* pArg)
{
	return nullptr;
}

void CEMGift::Free()
{
}
