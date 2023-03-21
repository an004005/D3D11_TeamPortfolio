#include "stdafx.h"
#include "..\public\Canvas_MainItemBattle.h"
#include "GameInstance.h"

CCanvas_MainItemBattle::CCanvas_MainItemBattle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_MainItemBattle::CCanvas_MainItemBattle(const CCanvas_MainItemBattle& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_MainItemBattle::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_MainItemBattle::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	m_bVisible = false;

	return S_OK;
}

void CCanvas_MainItemBattle::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);
}

void CCanvas_MainItemBattle::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_MainItemBattle::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_MainItemBattle::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_MainItemBattle::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_MainItemBattle::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_MainItemBattle * CCanvas_MainItemBattle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_MainItemBattle*		pInstance = new CCanvas_MainItemBattle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_MainItemBattle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_MainItemBattle::Clone(void * pArg)
{
	CCanvas_MainItemBattle*		pInstance = new CCanvas_MainItemBattle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_MainItemBattle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_MainItemBattle::Free()
{
	CCanvas::Free();
}
