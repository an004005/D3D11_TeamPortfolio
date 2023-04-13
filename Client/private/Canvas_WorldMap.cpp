#include "stdafx.h"
#include "..\public\Canvas_WorldMap.h"
#include "GameInstance.h"
#include "JsonStorage.h"

#include "Canvas_MapBar.h"

CCanvas_WorldMap::CCanvas_WorldMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_WorldMap::CCanvas_WorldMap(const CCanvas_WorldMap& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_WorldMap::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_WorldMap::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MapBar.json");
	m_pCanvasMapBar = dynamic_cast<CCanvas_MapBar*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"Canvas_MapBar", &json));
	assert(m_pCanvasMapBar != nullptr && "Failed to Cloned : CCanvas_MapBar");

	return S_OK;
}

void CCanvas_WorldMap::Tick(_double TimeDelta)
{
	KeyInput();

	if (false == m_bMapUI) return;

	CCanvas::Tick(TimeDelta);

}

void CCanvas_WorldMap::Late_Tick(_double TimeDelta)
{
	if (false == m_bMapUI) return;

	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_WorldMap::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_WorldMap::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_WorldMap::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_WorldMap::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

void CCanvas_WorldMap::KeyInput()
{
	if (CGameInstance::GetInstance()->KeyDown(DIK_TAB))
	{
		m_bMapUI = !m_bMapUI;
	}

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bMapUI);
}

CCanvas_WorldMap * CCanvas_WorldMap::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_WorldMap*		pInstance = new CCanvas_WorldMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_WorldMap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_WorldMap::Clone(void * pArg)
{
	CCanvas_WorldMap*		pInstance = new CCanvas_WorldMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_WorldMap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_WorldMap::Free()
{
	CCanvas::Free();
}
