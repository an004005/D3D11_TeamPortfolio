#include "stdafx.h"
#include "..\public\Canvas_Equipment.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CCanvas_Equipment::CCanvas_Equipment(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_Equipment::CCanvas_Equipment(const CCanvas_Equipment& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_Equipment::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_Equipment::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	m_bVisible = false;

	return S_OK;
}

void CCanvas_Equipment::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);
}

void CCanvas_Equipment::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_Equipment::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_Equipment::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_Equipment::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_Equipment::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_Equipment * CCanvas_Equipment::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_Equipment*		pInstance = new CCanvas_Equipment(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_Equipment");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_Equipment::Clone(void * pArg)
{
	CCanvas_Equipment*		pInstance = new CCanvas_Equipment(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_Equipment");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_Equipment::Free()
{
	CCanvas::Free();
}
