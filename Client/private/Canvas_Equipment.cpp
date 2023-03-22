#include "stdafx.h"
#include "..\public\Canvas_Equipment.h"
#include "GameInstance.h"
#include "PlayerInfoManager.h"
#include "Item_Manager.h"

#include "DefaultUI.h"
#include "ItemIconUI.h"
#include "Main_PickUI.h"

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

	ChildUIPick();
}

void CCanvas_Equipment::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_Equipment::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_float4 vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_tchar szText[MAX_PATH] = TEXT("");
	_float2 vFontBigSize = { 0.5f, 0.5f };
	_float2 vFontSmaillSize = { 0.3f, 0.3f };
	PLAYER_STAT tStat = CPlayerInfoManager::GetInstance()->Get_PlayerStat();

	// 중간 하단 정보
	_float2	vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Equipment_BasicInfo"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"유이토 스메라기", vPosition + _float2(95.0f, 52.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"WSY567369", vPosition + _float2(433.0f, 56.0f), 0.f, vFontBigSize * 0.5f, vColor);

	wsprintf(szText, TEXT("%d"), tStat.iLevel);
	pGameInstance->Render_Font(L"Pretendard32", L"레벨", vPosition + _float2(95.0f, 95.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(125.0f, 87.0f), 0.f, vFontBigSize, vColor);
	wsprintf(szText, TEXT("%d"), tStat.iMaxExp - tStat.iExp);
	pGameInstance->Render_Font(L"Pretendard32", L"앞으로", vPosition + _float2(203.0f, 95.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(280.0f, 95.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"-", vPosition + _float2(359.0f, 94.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d / %d"), tStat.m_iHP, tStat.m_iMaxHP);
	pGameInstance->Render_Font(L"Pretendard32", L"체력", vPosition + _float2(203.0f, 125.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(297.0f, 125.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d"), tStat.iSprbrPower);
	pGameInstance->Render_Font(L"Pretendard32", L"초뇌능력", vPosition + _float2(203.0f, 161.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(350.0f, 160.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d"), tStat.iAttack);
	pGameInstance->Render_Font(L"Pretendard32", L"공격력", vPosition + _float2(203.0f, 183.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(350.0f, 182.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d"), tStat.iDefense);
	pGameInstance->Render_Font(L"Pretendard32", L"방어력", vPosition + _float2(203.0f, 207.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(350.0f, 205.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"작전", vPosition + _float2(96.0f, 228.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"----", vPosition + _float2(276.0f, 255.0f), 0.f, { 0.4f, 0.4f }, vColor);

	vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Equipment_CurrentWeapon"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"무기", vPosition + _float2(50.0f, -7.0f), 0.f, vFontSmaillSize, vColor);
	vector<pair<wstring, CItem_Manager::ITEMINFO>> ItemInfo = CItem_Manager::GetInstance()->Get_ItmeInfo();
	for (size_t i = 0; i < ItemInfo.size(); i++)
	{
		// 왼쪽 현재 사용 중 무기 이름 _1, 2, 3
		if (1 == m_iItmeWindowPickCount)
		{
			vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Equipment_CurrentWeapon1"))->GetScreenSpaceLeftTop();
			if (CItem_Manager::MAINITEM::WEAPON == ItemInfo[i].second.eType && 1 == ItemInfo[i].second.iWeaponNum)
				pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].first.c_str(), vPosition + _float2(150.0f, 40.0f), 0.f, vFontSmaillSize, vColor);

			vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Equipment_CurrentWeapon2"))->GetScreenSpaceLeftTop();
			if (CItem_Manager::MAINITEM::WEAPON == ItemInfo[i].second.eType && 2 == ItemInfo[i].second.iWeaponNum)
				pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].first.c_str(), vPosition + _float2(150.0f, 40.0f), 0.f, vFontSmaillSize, vColor);

			vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Equipment_CurrentWeapon3"))->GetScreenSpaceLeftTop();
			if (CItem_Manager::MAINITEM::WEAPON == ItemInfo[i].second.eType && 3 == ItemInfo[i].second.iWeaponNum)
				pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].first.c_str(), vPosition + _float2(150.0f, 40.0f), 0.f, vFontSmaillSize, vColor);
		}

		if (CItem_Manager::MAINITEM::WEAPON == ItemInfo[i].second.eType && 1 == ItemInfo[i].second.iWeaponNum)
		{
			// 왼쪽 현재 사용 중 무기 이름 _0
			vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Equipment_CurrentWeapon"))->GetScreenSpaceLeftTop();
			wsprintf(szText, TEXT("%s%s"), ItemInfo[i].first.c_str(), "*");
			pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(150.0f, 40.0f), 0.f, vFontBigSize, vColor);

			// 오른쪽 현재 무기 설명
			vFontSmaillSize = { 0.4f, 0.4f };
			vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Equipment_ItemInfo"))->GetScreenSpaceLeftTop();
			dynamic_cast<CItemIconUI*>(Find_ChildUI(L"Equipment_ItemInfoIcon"))->Set_IconIndex(ItemInfo[i].second.vIconIndex);

			pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].second.szItemEx[0], vPosition + _float2(100.0f, 149.0f), 0.f, vFontBigSize, vColor);
			pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].second.szItemEx[1], vPosition + _float2(100.0f, 179.0f), 0.f, vFontBigSize, vColor);
			pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].second.szItemEx[2], vPosition + _float2(100.0f, 209.0f), 0.f, vFontBigSize, vColor);
			pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].second.szItemEx[3], vPosition + _float2(100.0f, 239.0f), 0.f, vFontBigSize, vColor);

			wsprintf(szText, TEXT("%s%s"), ItemInfo[i].first.c_str(), "*");
			pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(168.0f, 58.0f), 0.f, vFontBigSize, vColor);
			wsprintf(szText, TEXT("%d"), ItemInfo[i].second.iWeaponAttack);
			pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(220.0f, 90.0f), 0.f, vFontSmaillSize, vColor);
			pGameInstance->Render_Font(L"Pretendard32", L"공격력", vPosition + _float2(169.0f, 90.0f), 0.f, vFontSmaillSize, vColor);
			
			pGameInstance->Render_Font(L"Pretendard32", L"소지수", vPosition + _float2(465.0f, 99.0f), 0.f, vFontSmaillSize, vColor);
			wsprintf(szText, TEXT("/ %d"), ItemInfo[i].second.iMaxCount);
			pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(540.0f, 98.0f), 0.f, vFontSmaillSize, vColor);
			if (ItemInfo[i].second.iCount == ItemInfo[i].second.iMaxCount)
				vColor = { 1.0f, 0.458f, 0.38f, 1.0f };
			else
				vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
			wsprintf(szText, TEXT("%d"), ItemInfo[i].second.iCount);
			pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(521.0f, 98.0f), 0.f, vFontSmaillSize, vColor);

			if (1 == m_iItmeInfoPickCount)
			{
				// 화살표 추가하기


			}
		}

		if (1 == m_iItmeInfoPickCount)
		{
			vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Equipment_ItemInfoMove"))->GetScreenSpaceLeftTop();
			if (CItem_Manager::MAINITEM::WEAPON == ItemInfo[i].second.eType && 1 == ItemInfo[i].second.iWeaponNum)
			{
				dynamic_cast<CItemIconUI*>(Find_ChildUI(L"Equipment_ItemInfoMoveIcon"))->Set_IconIndex(ItemInfo[i].second.vIconIndex);

				pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].second.szItemEx[0], vPosition + _float2(100.0f, 149.0f), 0.f, vFontBigSize, vColor);
				pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].second.szItemEx[1], vPosition + _float2(100.0f, 179.0f), 0.f, vFontBigSize, vColor);
				pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].second.szItemEx[2], vPosition + _float2(100.0f, 209.0f), 0.f, vFontBigSize, vColor);
				pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].second.szItemEx[3], vPosition + _float2(100.0f, 239.0f), 0.f, vFontBigSize, vColor);

				wsprintf(szText, TEXT("%s%s"), ItemInfo[i].first.c_str(), "*");
				pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(168.0f, 58.0f), 0.f, vFontBigSize, vColor);
				wsprintf(szText, TEXT("%d"), ItemInfo[i].second.iWeaponAttack);
				pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(220.0f, 90.0f), 0.f, vFontSmaillSize, vColor);
				pGameInstance->Render_Font(L"Pretendard32", L"공격력", vPosition + _float2(169.0f, 90.0f), 0.f, vFontSmaillSize, vColor);

				pGameInstance->Render_Font(L"Pretendard32", L"소지수", vPosition + _float2(465.0f, 99.0f), 0.f, vFontSmaillSize, vColor);
				wsprintf(szText, TEXT("/ %d"), ItemInfo[i].second.iMaxCount);
				pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(540.0f, 98.0f), 0.f, vFontSmaillSize, vColor);
				if (ItemInfo[i].second.iCount == ItemInfo[i].second.iMaxCount)
					vColor = { 1.0f, 0.458f, 0.38f, 1.0f };
				else
					vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
				wsprintf(szText, TEXT("%d"), ItemInfo[i].second.iCount);
				pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(521.0f, 98.0f), 0.f, vFontSmaillSize, vColor);
			}
		}
	}


	// 오른쪽 변경할 무기 설명

	return S_OK;
}

void CCanvas_Equipment::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	ImGui::DragFloat("X", &m_Posssss.x);
	ImGui::DragFloat("Y", &m_Posssss.y);
}

void CCanvas_Equipment::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_Equipment::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

void CCanvas_Equipment::ChildUIPick()
{
	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Equipment_ItemInfo_B"))->Get_OnButton())
	{
		// [0][2] 초기화 상태 [1] 한 번 누른 상태(변경할 아이템이 떠야한다.)
		++m_iItmeInfoPickCount;
		if (2 == m_iItmeInfoPickCount)
			m_iItmeInfoPickCount = 0;

		if (false == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Equipment_ItemInfo_B"))->Get_OnAlpha())
		{
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Equipment_ItemInfo_B"))->Set_OnButton();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Equipment_ItemInfo_B"))->Set_OnAlpha();
		}
	}

	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Equipment_CurrentWeapon_B"))->Get_OnButton())
	{
		// [0][2] 초기화 상태 [1] 한 번 누른 상태(여러개 무기를 선택할 수 있어야 한다.)
		++m_iItmeWindowPickCount;
		if (2 == m_iItmeWindowPickCount)
			m_iItmeWindowPickCount = 0;

		if (false == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Equipment_CurrentWeapon_B"))->Get_OnAlpha())
		{
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Equipment_CurrentWeapon_B"))->Set_OnButton();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Equipment_CurrentWeapon_B"))->Set_OnAlpha();
		}
	}
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
