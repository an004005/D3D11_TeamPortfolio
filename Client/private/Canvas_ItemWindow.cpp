#include "stdafx.h"
#include "..\public\Canvas_MainItemEtc.h"
#include "GameInstance.h"

CCanvas_MainItemEtc::CCanvas_MainItemEtc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_MainItemEtc::CCanvas_MainItemEtc(const CCanvas_MainItemEtc& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_MainItemEtc::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_MainItemEtc::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	m_bVisible = false;

	return S_OK;
}

void CCanvas_MainItemEtc::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);
}

void CCanvas_MainItemEtc::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_MainItemEtc::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_MainItemEtc::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_MainItemEtc::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_MainItemEtc::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_MainItemEtc * CCanvas_MainItemEtc::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_MainItemEtc*		pInstance = new CCanvas_MainItemEtc(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_MainItemEtc");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_MainItemEtc::Clone(void * pArg)
{
	CCanvas_MainItemEtc*		pInstance = new CCanvas_MainItemEtc(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_MainItemEtc");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_MainItemEtc::Free()
{
	CCanvas::Free();
}
