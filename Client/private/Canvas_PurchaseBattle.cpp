#include "stdafx.h"
#include "..\public\Canvas_PurchaseBattle.h"
#include "GameInstance.h"
#include "UI_Manager.h"



CCanvas_PurchaseBattle::CCanvas_PurchaseBattle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_PurchaseBattle::CCanvas_PurchaseBattle(const CCanvas_PurchaseBattle& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_PurchaseBattle::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_PurchaseBattle::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_Canvas(L"CCanvas_PurchaseBattle", this);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	return S_OK;
}

void CCanvas_PurchaseBattle::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

}

void CCanvas_PurchaseBattle::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_PurchaseBattle::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;


	return S_OK;
}

void CCanvas_PurchaseBattle::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_PurchaseBattle::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_PurchaseBattle::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_PurchaseBattle * CCanvas_PurchaseBattle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_PurchaseBattle*      pInstance = new CCanvas_PurchaseBattle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_PurchaseBattle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_PurchaseBattle::Clone(void * pArg)
{
	CCanvas_PurchaseBattle*      pInstance = new CCanvas_PurchaseBattle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_PurchaseBattle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_PurchaseBattle::Free()
{
	CCanvas::Free();
}