#include "stdafx.h"
#include "..\public\Canvas_PurchaseKinds.h"
#include "GameInstance.h"
#include "JsonStorage.h"

#include "Canvas_ShopListBar.h"

CCanvas_PurchaseKinds::CCanvas_PurchaseKinds(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_PurchaseKinds::CCanvas_PurchaseKinds(const CCanvas_PurchaseKinds& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_PurchaseKinds::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_PurchaseKinds::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	m_bVisible = false;
	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);

	Add_ShopListCanvas();

	return S_OK;
}

void CCanvas_PurchaseKinds::Tick(_double TimeDelta)
{
	if (m_bVisible == false)
	{
		for (vector<pair<wstring, CCanvas_ShopListBar*>>::iterator iter = m_vecShopCanvass.begin(); iter != m_vecShopCanvass.end(); ++iter)
			iter->second->SetVisible(m_bVisible);
		return;
	}

	CCanvas::Tick(TimeDelta);

	for (vector<pair<wstring, CCanvas_ShopListBar*>>::iterator iter = m_vecShopCanvass.begin(); iter != m_vecShopCanvass.end(); ++iter)
		iter->second->SetVisible(m_bVisible);
}

void CCanvas_PurchaseKinds::Late_Tick(_double TimeDelta)
{
	if (m_bVisible == false) return;

	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_PurchaseKinds::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;


	return S_OK;
}

void CCanvas_PurchaseKinds::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_PurchaseKinds::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);

	m_eMainItem = json["ITemType"];
}

void CCanvas_PurchaseKinds::Add_ShopListCanvas()
{
	m_vecItemInfo = CItem_Manager::GetInstance()->Get_ItmeInfo();
	size_t iVecSize = m_vecItemInfo.size();

	if (m_eMainItem == CItem_Manager::MAINITEM::ALL)
	{
		// ALL 아이템 즉, BATTAL 과 WEAPON 의 아이템을 모두 만든다.
		for (size_t i = 0; i < iVecSize; i++)
		{
			if (m_vecItemInfo[i].second.eType == CItem_Manager::MAINITEM::BATTLE || m_vecItemInfo[i].second.eType == CItem_Manager::MAINITEM::WEAPON)
			{
				Clone_ShopListCanvas(i);
			}
		}
	}
	else if (m_eMainItem == CItem_Manager::MAINITEM::BATTLE)
	{
		for (size_t i = 0; i < iVecSize; i++)
		{
			if (m_vecItemInfo[i].second.eType == CItem_Manager::MAINITEM::BATTLE)
			{
				Clone_ShopListCanvas(i);
			}
		}
	}
	else if (m_eMainItem == CItem_Manager::MAINITEM::WEAPON)
	{
		for (size_t i = 0; i < iVecSize; i++)
		{
			if (m_vecItemInfo[i].second.eType == CItem_Manager::MAINITEM::WEAPON)
			{
				Clone_ShopListCanvas(i);
			}
		}
	}
}

void CCanvas_PurchaseKinds::Clone_ShopListCanvas(const size_t iIndex)
{
	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_ShopListBar.json");
	CCanvas_ShopListBar* pCanvas = dynamic_cast<CCanvas_ShopListBar*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Lyaer_ShopWindowUI", L"Canvas_ShopListBar", &json));
	assert(pCanvas != nullptr && "Failed to Cloned : CCanvas_ShopListBar");

	size_t iVecSize = m_vecShopCanvass.size();
	pCanvas->Set_ShopListBar(CItem_Manager::GetInstance()->Get_ShopListPos(iVecSize), CItem_Manager::GetInstance()->Get_ShopIconPos(iVecSize), iIndex);
	m_vecShopCanvass.emplace_back(m_vecItemInfo[iIndex].first, pCanvas);
}

CCanvas_PurchaseKinds * CCanvas_PurchaseKinds::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_PurchaseKinds*      pInstance = new CCanvas_PurchaseKinds(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_PurchaseKinds");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_PurchaseKinds::Clone(void * pArg)
{
	CCanvas_PurchaseKinds*      pInstance = new CCanvas_PurchaseKinds(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_PurchaseKinds");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_PurchaseKinds::Free()
{
	CCanvas::Free();
}