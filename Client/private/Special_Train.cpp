#include "stdafx.h"
#include "..\public\Special_Train.h"

CSpecial_Train::CSpecial_Train(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CSpecialObject(pDevice, pContext)
{
}

CSpecial_Train::CSpecial_Train(const CSpecial_Train & rhs)
	: CSpecialObject(rhs)
{
}

HRESULT CSpecial_Train::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecial_Train::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	return S_OK;
}

void CSpecial_Train::BeginTick()
{
	__super::BeginTick();
}

void CSpecial_Train::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CSpecial_Train::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CSpecial_Train::AfterPhysX()
{
	__super::AfterPhysX();
}

HRESULT CSpecial_Train::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CSpecial_Train::Imgui_RenderProperty()
{
}

HRESULT CSpecial_Train::SetUp_Components(void * pArg)
{
	return S_OK;
}

CSpecial_Train * CSpecial_Train::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	return nullptr;
}

CGameObject * CSpecial_Train::Clone(void * pArg)
{
	return nullptr;
}

void CSpecial_Train::Free()
{
}
