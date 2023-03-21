#include "stdafx.h"
#include "..\public\Canvas_MainItemWeapon.h"
#include "UI_Manager.h"

CCanvas_MainItemWeapon::CCanvas_MainItemWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_MainItemWeapon::CCanvas_MainItemWeapon(const CCanvas_MainItemWeapon& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_MainItemWeapon::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_MainItemWeapon::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	m_bVisible = false;

	return S_OK;
}

void CCanvas_MainItemWeapon::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);
}

void CCanvas_MainItemWeapon::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_MainItemWeapon::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_MainItemWeapon::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_MainItemWeapon::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_MainItemWeapon::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_MainItemWeapon * CCanvas_MainItemWeapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_MainItemWeapon*		pInstance = new CCanvas_MainItemWeapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_MainItemWeapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_MainItemWeapon::Clone(void * pArg)
{
	CCanvas_MainItemWeapon*		pInstance = new CCanvas_MainItemWeapon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_MainItemWeapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_MainItemWeapon::Free()
{
	CCanvas::Free();
}
