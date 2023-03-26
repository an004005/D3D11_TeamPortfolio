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
