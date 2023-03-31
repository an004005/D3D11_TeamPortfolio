#include "stdafx.h"
#include "..\public\Canvas_PurchaseWeapon.h"
#include "GameInstance.h"
#include "UI_Manager.h"



CCanvas_PurchaseWeapon::CCanvas_PurchaseWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_PurchaseWeapon::CCanvas_PurchaseWeapon(const CCanvas_PurchaseWeapon& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_PurchaseWeapon::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_PurchaseWeapon::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_Canvas(L"CCanvas_PurchaseWeapon", this);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	return S_OK;
}

void CCanvas_PurchaseWeapon::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

}

void CCanvas_PurchaseWeapon::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_PurchaseWeapon::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;


	return S_OK;
}

void CCanvas_PurchaseWeapon::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_PurchaseWeapon::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_PurchaseWeapon::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_PurchaseWeapon * CCanvas_PurchaseWeapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_PurchaseWeapon*      pInstance = new CCanvas_PurchaseWeapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_PurchaseWeapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_PurchaseWeapon::Clone(void * pArg)
{
	CCanvas_PurchaseWeapon*      pInstance = new CCanvas_PurchaseWeapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_PurchaseWeapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_PurchaseWeapon::Free()
{
	CCanvas::Free();
}