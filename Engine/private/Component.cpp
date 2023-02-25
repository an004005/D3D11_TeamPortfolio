#include "stdafx.h"
#include "..\public\Component.h"
#include "Object_Manager.h"

CComponent::CComponent(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_isCloned(false)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CComponent::CComponent(const CComponent & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_isCloned(true)
	, m_pPrototypeTag(rhs.m_pPrototypeTag)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CComponent::Initialize(void * pArg)
{
	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		LoadFromJson(json);
	}

	return S_OK;
}

CGameObject* CComponent::TryGetOwner()
{
	if (CObject_Manager::Check_ObjectAlive(m_pOwner))
		return m_pOwner;

	return nullptr;
}

void CComponent::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
