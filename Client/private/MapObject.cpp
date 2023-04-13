#include "stdafx.h"
#include "..\public\MapObject.h"
#include "GameInstance.h"
#include "Psychokinesis.h"
#include "JsonLib.h"
#include "Material.h"
CMapObject::CMapObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CMapObject::CMapObject(const CMapObject & rhs)
	:CGameObject(rhs)
{
}

HRESULT CMapObject::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CMapObject::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components());

	return S_OK;
}

void CMapObject::BeginTick()
{
	__super::BeginTick();
}

void CMapObject::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CMapObject::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CMapObject::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

HRESULT CMapObject::SetUp_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	/* For.Com_Renderer */
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	return S_OK;
}

void CMapObject::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
}
