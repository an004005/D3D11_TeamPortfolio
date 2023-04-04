#include "stdafx.h"
#include "../public/PlayerStartPosition.h"

CPlayerStartPosition::CPlayerStartPosition(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayerStartPosition::CPlayerStartPosition(const CPlayerStartPosition& rhs)
	: CGameObject(rhs)
{
}

CPlayerStartPosition* CPlayerStartPosition::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	auto pInstance = new CPlayerStartPosition(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerStartPosition");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayerStartPosition::Clone(void* pArg)
{
	auto pInstance = new CPlayerStartPosition(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayerStartPosition");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerStartPosition::Free()
{
	CGameObject::Free();
}

CHanabiStartPosition::CHanabiStartPosition(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CHanabiStartPosition::CHanabiStartPosition(const CHanabiStartPosition& rhs)
	: CGameObject(rhs)
{
}

CHanabiStartPosition* CHanabiStartPosition::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	auto pInstance = new CHanabiStartPosition(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CHanabiStartPosition");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHanabiStartPosition::Clone(void* pArg)
{
	auto pInstance = new CHanabiStartPosition(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CHanabiStartPosition");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHanabiStartPosition::Free()
{
	CGameObject::Free();
}

CTsugumiStartPosition::CTsugumiStartPosition(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CTsugumiStartPosition::CTsugumiStartPosition(const CTsugumiStartPosition& rhs)
	: CGameObject(rhs)

{
}

CTsugumiStartPosition* CTsugumiStartPosition::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	auto pInstance = new CTsugumiStartPosition(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTsugumiStartPosition");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTsugumiStartPosition::Clone(void* pArg)
{
	auto pInstance = new CTsugumiStartPosition(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTsugumiStartPosition");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTsugumiStartPosition::Free()
{
	CGameObject::Free();
}
