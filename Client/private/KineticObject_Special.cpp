#include "stdafx.h"
#include "..\public\KineticObject_Special.h"

CKineticObject_Special::CKineticObject_Special(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CKineticObject(pDevice, pContext)
{
}

CKineticObject_Special::CKineticObject_Special(const CKineticObject_Special & rhs)
	:CKineticObject(rhs)
{
}

HRESULT CKineticObject_Special::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CKineticObject_Special::Initialize(void * pArg)
{
	return S_OK;
}

void CKineticObject_Special::BeginTick()
{
}

void CKineticObject_Special::Tick(_double TimeDelta)
{
}

void CKineticObject_Special::Late_Tick(_double TimeDelta)
{
}

void CKineticObject_Special::AfterPhysX()
{
}

HRESULT CKineticObject_Special::Render()
{
	return E_NOTIMPL;
}

void CKineticObject_Special::Imgui_RenderProperty()
{
}

void CKineticObject_Special::Free()
{
}
