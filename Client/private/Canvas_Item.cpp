#include "stdafx.h"
#include "..\public\Canvas_Item.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "Item_IconUI.h"
#include "Item_LightUI.h"
#include "Item_NameUI.h"
#include "ItemIconUI.h"

CCanvas_Item::CCanvas_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_Item::CCanvas_Item(const CCanvas_Item& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_Item::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_Item::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_Canvas(L"Canvas_Item", this);
	m_bVisible = true;

	return S_OK;
}

void CCanvas_Item::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	BattleItem_Tick();
	CurrentItem();

}

HRESULT CCanvas_Item::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_tchar szText[MAX_PATH] = TEXT("");
	_float2 vFontSize = { 0.3f, 0.3f };
	_float2	vPosition = Find_ChildUI(L"Item_Icon")->GetScreenSpaceLeftTop();



	//if (0 != vecItemInfo[0].iCount)
	//{
	//	if(10 <= vecItemInfo[0].iCount)
	//		wsprintf(szText, TEXT("%d"), vecItemInfo[0].iCount);
	//	else
	//		wsprintf(szText, TEXT("0%d"), vecItemInfo[0].iCount);

	//	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(25.0f, 25.0f), 0.f, vFontSize);
	//}

	//if (0 != vecItemInfo[1].iCount)
	//{
	//	if (10 <= vecItemInfo[1].iCount)
	//		wsprintf(szText, TEXT("%d"), vecItemInfo[0].iCount);
	//	else
	//		wsprintf(szText, TEXT("0%d"), vecItemInfo[0].iCount);

	//	wsprintf(szText, TEXT("%d"), vecItemInfo[1].iCount);
	//	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(89.0f, 20.0f), 0.f, vFontSize);
	//}

	//if (0 != vecItemInfo[vecItemInfo.size() - 1].iCount)
	//{
	//	if (10 <= vecItemInfo[vecItemInfo.size() - 1].iCount)
	//		wsprintf(szText, TEXT("%d"), vecItemInfo[0].iCount);
	//	else
	//		wsprintf(szText, TEXT("0%d"), vecItemInfo[0].iCount);

	//	wsprintf(szText, TEXT("%d"), vecItemInfo[vecItemInfo.size() - 1].iCount);
	//	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(46.0f, 26.0f), 0.f, vFontSize);
	//}

	return S_OK;
}

void CCanvas_Item::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	if (ImGui::Button("Jelly5"))
	{
		CItem_Manager::GetInstance()->Set_ItemCount(L"회복(소) 젤리\n", 5);
	}

	if (ImGui::Button("Jelly-1"))
	{
		CItem_Manager::GetInstance()->Set_ItemCount(L"회복(소) 젤리\n", -1);
	}

	if (ImGui::Button("test"))
	{
		CItem_Manager::GetInstance()->Set_ItemCount(L"SAS 보급수\n", 1);
	}
	
}

void CCanvas_Item::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_Item::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

void CCanvas_Item::Set_ItmeUse()
{
	dynamic_cast<CItem_LightUI*>(Find_ChildUI(L"Item_Light"))->Set_ItemLightUse();
	dynamic_cast<CItem_IconUI*>(Find_ChildUI(L"Item_Icon"))->Set_ItemIconUse();

}

void CCanvas_Item::BattleItem_Tick()
{
	m_vecBattleItme = CItem_Manager::GetInstance()->Get_BattleItem();

	if (CGameInstance::GetInstance()->KeyDown(DIK_LEFT))
	{
		size_t iTtemSize = m_vecBattleItme.size();
		if (iTtemSize == 1)
		{
			m_iCurrentIndex = 0;
		}
		else if (iTtemSize == 2)
		{
			++m_iCurrentIndex;
		}
		else if (iTtemSize >= 3)
		{
			if (m_iCurrentIndex == iTtemSize - 1)
			{

			}
			if (m_iCurrentIndex == 0)
			{

			}
		}
	}
}

	void CCanvas_Item::CurrentItem()
{
//	// 중간
//#pragma region ItemName
//	_float fItemIndex = 0.0f;
//	//if (vecItemInfo[0].wsName == L"회복(소) 젤리\n")
//	//	fItemIndex = 0.0f;
//	//else if (vecItemInfo[0].wsName == L"회복(중) 젤리\n")
//	//	fItemIndex = 1.0f;
//	//else if (vecItemInfo[0].wsName == L"회복(대) 젤리\n")
//	//	fItemIndex = 2.0f;
//	//else if (vecItemInfo[0].wsName == L"전체: 회복(소) 젤리\n")
//	//	fItemIndex = 3.0f;
//	//else if (vecItemInfo[0].wsName == L"전체: 회복(중) 젤리\n")
//	//	fItemIndex = 4.0f;
//	//else if (vecItemInfo[0].wsName == L"전체: 회복(대) 젤리\n")
//	//	fItemIndex = 5.0f;
//	//else if (vecItemInfo[0].wsName == L"정상화 태블릿\n")
//	//	fItemIndex = 6.0f;
//	//else if (vecItemInfo[0].wsName == L"전체: 정상화 태블릿\n")
//	//	fItemIndex = 7.0f;
//	//else if (vecItemInfo[0].wsName == L"브레인 필드 기어\n")
//	//	fItemIndex = 8.0f;
//	//else if (vecItemInfo[0].wsName == L"SAS 보급수\n")
//	//	fItemIndex = 9.0f;
//#pragma endregion ItemName
//
//	dynamic_cast<CItem_NameUI*>(Find_ChildUI(L"Item_Name"))->Set_ItemName(fItemIndex);
//	dynamic_cast<CItem_IconUI*>(Find_ChildUI(L"Item_Icon"))->Set_IconIndex({ vecItemInfo[0].vIconIndex });
//
//	// 왼쪽
//	dynamic_cast<CItemIconUI*>(Find_ChildUI(L"Item_IconLeft"))->Set_IconIndex({ vecItemInfo[1].vIconIndex });
//
//
//	// 오른쪽
//	dynamic_cast<CItemIconUI*>(Find_ChildUI(L"Item_IconRight"))->Set_IconIndex({ vecItemInfo[vecItemInfo.size() - 1].vIconIndex });

}

CCanvas_Item * CCanvas_Item::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_Item*		pInstance = new CCanvas_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_Item");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_Item::Clone(void * pArg)
{
	CCanvas_Item*		pInstance = new CCanvas_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_Item");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_Item::Free()
{
	CCanvas::Free();
}
