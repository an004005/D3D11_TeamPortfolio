#include "stdafx.h"
#include "..\public\MapNonAnim_Object.h"
#include "GameInstance.h"


CMapNonAnim_Object::CMapNonAnim_Object(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMapObject(pDevice, pContext)
{
}

CMapNonAnim_Object::CMapNonAnim_Object(const CMapNonAnim_Object & rhs)
	: CMapObject(rhs)
{
}

HRESULT CMapNonAnim_Object::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CMapNonAnim_Object::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components());

	return S_OK;
}

void CMapNonAnim_Object::BeginTick()
{
}

void CMapNonAnim_Object::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CMapNonAnim_Object::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CMapNonAnim_Object::Render()
{
	FAILED_CHECK(__super::Render());
	return S_OK;
}

HRESULT CMapNonAnim_Object::SetUp_Components()
{
	return S_OK;
}

CMapNonAnim_Object * CMapNonAnim_Object::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMapNonAnim_Object*		pInstance = new CMapNonAnim_Object(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapNonAnim_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapNonAnim_Object::Clone(void * pArg)
{
	CMapNonAnim_Object*		pInstance = new CMapNonAnim_Object(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapNonAnim_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapNonAnim_Object::Free()
{
	__super::Free();
}
