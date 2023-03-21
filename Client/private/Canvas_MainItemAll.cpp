#include "stdafx.h"
#include "..\public\Canvas_MainItemAll.h"
#include "GameInstance.h"

CCanvas_MainItemAll::CCanvas_MainItemAll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_MainItemAll::CCanvas_MainItemAll(const CCanvas_MainItemAll& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_MainItemAll::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_MainItemAll::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	m_bVisible = false;

	return S_OK;
}

void CCanvas_MainItemAll::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);
}

void CCanvas_MainItemAll::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_MainItemAll::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_MainItemAll::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_MainItemAll::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_MainItemAll::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_MainItemAll * CCanvas_MainItemAll::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_MainItemAll*		pInstance = new CCanvas_MainItemAll(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_MainItemAll");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_MainItemAll::Clone(void * pArg)
{
	CCanvas_MainItemAll*		pInstance = new CCanvas_MainItemAll(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_MainItemAll");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_MainItemAll::Free()
{
	CCanvas::Free();
}
