#include "stdafx.h"
#include "..\public\Canvas_MainItemAll.h"
#include "GameInstance.h"
#include "Item_Manager.h"

#include "DefaultUI.h"

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

	//_tchar szText[MAX_PATH] = TEXT("");
	//_float2 vFontSize = { 0.4f, 0.4f };
	//_float4 vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
	//CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//// »ó´Ü
	//_float2 vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Item_ItemInfo"))->GetScreenSpaceLeftTop();
	//vector<pair<wstring, CItem_Manager::ITEMINFO>>	vecItemInfo = CItem_Manager::GetInstance()->Get_ItmeInfo();
	//pGameInstance->Render_Font(L"Pretendard32", vecItemInfo[0].first.c_str(), vPosition + _float2(m_vPosssss), 0.f, vFontSize, vColor);
	//wsprintf(szText, TEXT("%d"), vecItemInfo[0].second.iCount);
	//pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(m_vPosssss), 0.f, vFontSize, vColor);

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
