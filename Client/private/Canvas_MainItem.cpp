#include "stdafx.h"
#include "..\public\Canvas_MainItem.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CCanvas_MainItem::CCanvas_MainItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_MainItem::CCanvas_MainItem(const CCanvas_MainItem& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_MainItem::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_MainItem::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	m_bVisible = false;

	return S_OK;
}

void CCanvas_MainItem::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);
}

void CCanvas_MainItem::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_MainItem::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_MainItem::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_MainItem::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_MainItem::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_MainItem * CCanvas_MainItem::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_MainItem*		pInstance = new CCanvas_MainItem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_MainItem");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_MainItem::Clone(void * pArg)
{
	CCanvas_MainItem*		pInstance = new CCanvas_MainItem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_MainItem");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_MainItem::Free()
{
	CCanvas::Free();
}
