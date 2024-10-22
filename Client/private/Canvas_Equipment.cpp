#include "stdafx.h"
#include "..\public\Canvas_Equipment.h"
#include "GameInstance.h"
#include "PlayerInfoManager.h"
#include "UI_Manager.h"

#include "DefaultUI.h"
#include "ShaderUI.h"
#include "ItemIconUI.h"
#include "Main_PickUI.h"
#include "Main_OnePickUI.h"
#include "Main_BarUI.h"

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

	// 원래의 위치
	m_arrAddWeaponPos.fill(1.0f);
	m_arrOriginalPos.fill(0.0f);
	m_arrOriginalPos[0] = Find_ChildUI(L"Equipment_CurrentWeapon1")->Get_Position().y;
	m_arrOriginalPos[1] = Find_ChildUI(L"Equipment_CurrentWeapon2")->Get_Position().y;
	m_arrOriginalPos[2] = Find_ChildUI(L"Equipment_CurrentWeapon3")->Get_Position().y;

	m_fOriginalPos = Find_ChildUI(L"Equipment_ItemInfoMove")->Get_Position().y;	// -170.0f

	vector<pair<wstring, CItem_Manager::ITEMINFO>> vecItmeInfo = CItem_Manager::GetInstance()->Get_ItmeInfo();
	for (size_t i = 0; i < vecItmeInfo.size(); i++)
	{
		if (CItem_Manager::MAINITEM::WEAPON == vecItmeInfo[i].second.eType)
			m_vecItmeInfo.push_back(vecItmeInfo[i]);
	}

	return S_OK;
}

void CCanvas_Equipment::BeginTick()
{ 
	// 처음에 현재 무기의 위치로 옮겨 둔다.
	Find_ChildUI(L"Equipment_CurrentWeapon1")->Set_Position(Find_ChildUI(L"Equipment_CurrentWeapon")->Get_Position());
	Find_ChildUI(L"Equipment_CurrentWeapon2")->Set_Position(Find_ChildUI(L"Equipment_CurrentWeapon")->Get_Position());
	Find_ChildUI(L"Equipment_CurrentWeapon3")->Set_Position(Find_ChildUI(L"Equipment_CurrentWeapon")->Get_Position());
	Find_ChildUI(L"Equipment_CurrentWeapon_B1")->Set_Position(Find_ChildUI(L"Equipment_CurrentWeapon_B")->Get_Position());
	Find_ChildUI(L"Equipment_CurrentWeapon_B2")->Set_Position(Find_ChildUI(L"Equipment_CurrentWeapon_B")->Get_Position());
	Find_ChildUI(L"Equipment_CurrentWeapon_B3")->Set_Position(Find_ChildUI(L"Equipment_CurrentWeapon_B")->Get_Position());

	Find_ChildUI(L"Equipment_ItemInfoMove")->Set_Position(Find_ChildUI(L"Equipment_ItemInfo")->Get_Position());
	Find_ChildUI(L"Equipment_ItemInfo_BMove")->Set_Position(Find_ChildUI(L"Equipment_ItemInfo_B")->Get_Position());
	Find_ChildUI(L"Equipment_ItemInfoMoveIcon")->Set_Position(Find_ChildUI(L"Equipment_ItemInfoIcon")->Get_Position());
}

void CCanvas_Equipment::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
	{
		if (true == m_bVisible)
		{
			wstring	wsCurrentWeapon = L"Equipment_CurrentWeapon" + to_wstring(m_iCurrentWeaponCount);
			if (iter->first == wsCurrentWeapon)
			{
				++m_iCurrentWeaponCount;
				if (4 == m_iCurrentWeaponCount)
					m_iCurrentWeaponCount = 1;
				continue;
			}
			wsCurrentWeapon = L"Equipment_CurrentWeapon_B" + to_wstring(m_iCurrentWeaponBCount);
			if (iter->first == wsCurrentWeapon)
			{
				++m_iCurrentWeaponBCount;
				if (4 == m_iCurrentWeaponBCount)
					m_iCurrentWeaponBCount = 1;
				continue;
			}

			if (iter->first == L"Equipment_ItemInfo" || 
				iter->first == L"Equipment_ItemInfo_B" ||
				iter->first == L"Equipment_ItemInfoIcon" ||
				iter->first == L"Arrow")
				continue;
		}

		iter->second->SetVisible(m_bVisible);
	}

	if (false == m_bVisible) return;

	vector<pair<wstring, CItem_Manager::ITEMINFO>> vecItmeInfo = CItem_Manager::GetInstance()->Get_ItmeInfo();
	m_vecItmeInfo[0] = vecItmeInfo[7];
	m_vecItmeInfo[1] = vecItmeInfo[8];
	m_vecItmeInfo[2] = vecItmeInfo[9];

	Weaponkind_Tick(TimeDelta);
	WeaponkindPick_Tick();
	RihgtInfoMove_Tick(TimeDelta);
	ArrowTick();

	PLAYER_STAT tStat = CPlayerInfoManager::GetInstance()->Get_PlayerStat();
	dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Equipment_BasicInfo_LevelBer"))->Set_Ber(static_cast<_float>(tStat.iExp) / tStat.iMaxExp);
	dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Equipment_BasicInfo_HpBar"))->Set_Ber(static_cast<_float>(tStat.m_iHP) / tStat.m_iMaxHP);
}

void CCanvas_Equipment::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_Equipment::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_tchar szText[MAX_PATH] = TEXT("");
	_float2 vFontBigSize = { 0.6f, 0.6f };
	_float2 vFontSmaillSize = { 0.4f, 0.4f };
	_float4 vColor = { 1.0f, 0.99f, 0.87f, 1.0f };

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	PLAYER_STAT tStat = CPlayerInfoManager::GetInstance()->Get_PlayerStat();

	// 중간 하단 정보
	_float2	vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Equipment_BasicInfo"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"유이토 스메라기", vPosition + _float2(60.0f, 35.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"WSY567369", vPosition + _float2(453.0f, 37.0f), 0.f, vFontBigSize * 0.5f, vColor);

	wsprintf(szText, TEXT("%d"), tStat.iLevel);
	pGameInstance->Render_Font(L"Pretendard32", L"레벨", vPosition + _float2(55.0f, 83.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(90.0f, 73.0f), 0.f, vFontBigSize, vColor);
	wsprintf(szText, TEXT("%d"), tStat.iMaxExp - tStat.iExp);
	pGameInstance->Render_Font(L"Pretendard32", L"앞으로", vPosition + _float2(180.0f, 83.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(270.0f, 83.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"-", vPosition + _float2(359.0f, 94.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d / %d"), tStat.m_iHP, tStat.m_iMaxHP);
	pGameInstance->Render_Font(L"Pretendard32", L"체력", vPosition + _float2(185.0f, 118.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(280.0f, 118.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d"), tStat.iSprbrPower);
	pGameInstance->Render_Font(L"Pretendard32", L"초뇌능력", vPosition + _float2(185.0f, 157.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(350.0f, 157.0f), 0.f, vFontSmaillSize, vColor);
	_int iAttack = static_cast<_int>(tStat.m_fBaseAttackDamage);
	wsprintf(szText, TEXT("%d"), iAttack);
	pGameInstance->Render_Font(L"Pretendard32", L"공격력", vPosition + _float2(185.0f, 183.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(350.0f, 183.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d"), tStat.iDefense);
	pGameInstance->Render_Font(L"Pretendard32", L"방어력", vPosition + _float2(185.0f, 207.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(350.0f, 207.0f), 0.f, vFontSmaillSize, vColor);

	pGameInstance->Render_Font(L"Pretendard32", L"작전", vPosition + _float2(55.0f, 237.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"----", vPosition + _float2(276.0f, 265.0f), 0.f, { 0.5f, 0.5f }, vColor);

	vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Equipment_CurrentWeapon"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"무기", vPosition + _float2(10.0f, -17.0f), 0.f, vFontSmaillSize, vColor);
	vector<pair<wstring, CItem_Manager::ITEMINFO>> ItemInfo = CItem_Manager::GetInstance()->Get_ItmeInfo();
	for (size_t i = 0; i < ItemInfo.size(); i++)
	{
		// 왼쪽 현재 사용 중 무기 이름 _1, 2, 3
		vColor = { 1.0f, 0.99f, 0.87f, 1.0f };
		if (m_arrItmeWindowPick[0] == true)
		{
			vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Equipment_CurrentWeapon1"))->GetScreenSpaceLeftTop();
			if (CItem_Manager::MAINITEM::WEAPON == ItemInfo[i].second.eType && 1 == ItemInfo[i].second.iWeaponNum)
				pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].first.c_str(), vPosition + _float2(130.0f, 40.0f), 0.f, vFontBigSize, vColor);
		}
		if (m_arrItmeWindowPick[1] == true)
		{
			vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Equipment_CurrentWeapon2"))->GetScreenSpaceLeftTop();
			if (CItem_Manager::MAINITEM::WEAPON == ItemInfo[i].second.eType && 2 == ItemInfo[i].second.iWeaponNum)
			{
				if (0 == ItemInfo[i].second.iCount)
					pGameInstance->Render_Font(L"Pretendard32", L"???", vPosition + _float2(130.0f, 30.0f), 0.f, vFontBigSize, vColor);
				else
					pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].first.c_str(), vPosition + _float2(130.0f, 30.0f), 0.f, vFontBigSize, vColor);
			}
		}
		if (m_arrItmeWindowPick[2] == true)
		{
			vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Equipment_CurrentWeapon3"))->GetScreenSpaceLeftTop();
			if (CItem_Manager::MAINITEM::WEAPON == ItemInfo[i].second.eType && 3 == ItemInfo[i].second.iWeaponNum)
			{
				if (0 == ItemInfo[i].second.iCount)
					pGameInstance->Render_Font(L"Pretendard32", L"???", vPosition + _float2(130.0f, 30.0f), 0.f, vFontBigSize, vColor);
				else
					pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].first.c_str(), vPosition + _float2(130.0f, 30.0f), 0.f, vFontBigSize, vColor);
			}
		}

		vFontSmaillSize = { 0.4f, 0.4f };
		if (CItem_Manager::MAINITEM::WEAPON == ItemInfo[i].second.eType && true == ItemInfo[i].second.bWeapon)
		{
			// 왼쪽 현재 사용 중 무기 이름 _0
			vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Equipment_CurrentWeapon"))->GetScreenSpaceLeftTop();
			wsprintf(szText, TEXT("%s%s"), ItemInfo[i].first.c_str(), "*");
			pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(130.0f, 40.0f), 0.f, vFontBigSize, vColor);

			// 오른쪽 현재 무기 설명
			vPosition = Find_ChildUI(L"Equipment_ItemInfoMove")->GetScreenSpaceLeftTop();
			dynamic_cast<CItemIconUI*>(Find_ChildUI(L"Equipment_ItemInfoIcon"))->Set_IconIndex(ItemInfo[i].second.vIconIndex);

			pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].second.szItemEx[0], vPosition + _float2(50.0f, 150.0f), 0.f, vFontBigSize, vColor);
			pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].second.szItemEx[1], vPosition + _float2(50.0f, 190.0f), 0.f, vFontBigSize, vColor);
			pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].second.szItemEx[2], vPosition + _float2(50.0f, 230.0f), 0.f, vFontBigSize, vColor);
			pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].second.szItemEx[3], vPosition + _float2(50.0f, 270.0f), 0.f, vFontBigSize, vColor);

			wsprintf(szText, TEXT("%s%s"), ItemInfo[i].first.c_str(), "*");
			pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(130.0f, 58.0f), 0.f, vFontBigSize, vColor);
			wsprintf(szText, TEXT("%d"), ItemInfo[i].second.iWeaponAttack);
			pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(180.0f, 90.0f), 0.f, vFontSmaillSize, vColor);
			pGameInstance->Render_Font(L"Pretendard32", L"공격력", vPosition + _float2(130.0f, 90.0f), 0.f, vFontSmaillSize, vColor);
			
			pGameInstance->Render_Font(L"Pretendard32", L"소지수", vPosition + _float2(465.0f, 99.0f), 0.f, vFontSmaillSize, vColor);
			wsprintf(szText, TEXT("/ %d"), ItemInfo[i].second.iMaxCount);
			pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(540.0f, 98.0f), 0.f, vFontSmaillSize, vColor);
			if (ItemInfo[i].second.iCount == ItemInfo[i].second.iMaxCount)
				vColor = { 1.0f, 0.458f, 0.38f, 1.0f };
			else
				vColor = { 1.0f, 0.99f, 0.87f, 1.0f };
			wsprintf(szText, TEXT("%d"), ItemInfo[i].second.iCount);
			pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(521.0f, 98.0f), 0.f, vFontSmaillSize, vColor);
		}

		vColor = { 1.0f, 0.99f, 0.87f, 1.0f };
		if (m_bWeaponPick == true)
		{
			if (CItem_Manager::MAINITEM::WEAPON == ItemInfo[i].second.eType && m_iPickIndex == ItemInfo[i].second.iWeaponNum)
			{
				vPosition = Find_ChildUI(L"Equipment_ItemInfo")->GetScreenSpaceLeftTop();
				dynamic_cast<CItemIconUI*>(Find_ChildUI(L"Equipment_ItemInfoMoveIcon"))->Set_IconIndex(ItemInfo[i].second.vIconIndex);

				pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].second.szItemEx[0], vPosition + _float2(50.0f, 150.0f), 0.f, vFontBigSize, vColor);
				pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].second.szItemEx[1], vPosition + _float2(50.0f, 190.0f), 0.f, vFontBigSize, vColor);
				pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].second.szItemEx[2], vPosition + _float2(50.0f, 230.0f), 0.f, vFontBigSize, vColor);
				pGameInstance->Render_Font(L"Pretendard32", ItemInfo[i].second.szItemEx[3], vPosition + _float2(50.0f, 270.0f), 0.f, vFontBigSize, vColor);

				wsprintf(szText, TEXT("%s"), ItemInfo[i].first.c_str());
				pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(130.0f, 58.0f), 0.f, vFontBigSize, vColor);
				wsprintf(szText, TEXT("%d"), ItemInfo[i].second.iWeaponAttack);
				pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(180.0f, 90.0f), 0.f, vFontSmaillSize, vColor);
				pGameInstance->Render_Font(L"Pretendard32", L"공격력", vPosition + _float2(130.0f, 90.0f), 0.f, vFontSmaillSize, vColor);

				pGameInstance->Render_Font(L"Pretendard32", L"소지수", vPosition + _float2(465.0f, 99.0f), 0.f, vFontSmaillSize, vColor);
				wsprintf(szText, TEXT("/ %d"), ItemInfo[i].second.iMaxCount);
				pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(540.0f, 98.0f), 0.f, vFontSmaillSize, vColor);
				if (ItemInfo[i].second.iCount == ItemInfo[i].second.iMaxCount)
					vColor = { 1.0f, 0.458f, 0.38f, 1.0f };
				else
					vColor = { 1.0f, 0.99f, 0.87f, 1.0f };
				wsprintf(szText, TEXT("%d"), ItemInfo[i].second.iCount);
				pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(521.0f, 98.0f), 0.f, vFontSmaillSize, vColor);

				_uint iAttack0 = 0;
				_uint iAttack1 = 0;
				for (size_t i = 0; i < m_vecItmeInfo.size(); i++)
				{
					if (true == m_vecItmeInfo[i].second.bWeapon)	// 현재 사용 중인 무기
						iAttack0 = m_vecItmeInfo[i].second.iWeaponAttack;
					if (m_iPickIndex == m_vecItmeInfo[i].second.iWeaponNum)	// 현재 클리한 무기
						iAttack1 = m_vecItmeInfo[i].second.iWeaponAttack;
				}

				_int iReault = _int(iAttack1) - _int(iAttack0);
				if (0 != iReault)
				{
					vColor = { 1.0f, 0.99f, 0.87f, 1.0f };
					wsprintf(szText, TEXT("%i"), iReault);
				}
				else
				{
					vColor = { 1.0f, 0.458f, 0.38f, 1.0f };
					wsprintf(szText, TEXT("사용 중"));
				}
				pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(390.0f, 99.0f), 0.f, vFontSmaillSize, vColor);
			}
		}
	}

	return S_OK;
}

void CCanvas_Equipment::Weaponkind_Tick(const _double & TimeDelta)
{
	_float fSpeed = 300.0f;
	_float fPositionX = 327.0f;
	_tchar szWeapon[MAX_PATH] = TEXT("");
	_tchar szWeaponB[MAX_PATH] = TEXT("");
	_tchar szIcon[MAX_PATH] = TEXT("");

	if (true == dynamic_cast<CMain_OnePickUI*>(Find_ChildUI(L"Equipment_CurrentWeapon_B"))->Get_OnButton())
	{
		for (size_t i = 0; i < 3; i++)
		{
			_int	iWeaponIndex = _int(i) + 1;
			wsprintf(szWeapon, TEXT("Equipment_CurrentWeapon%d"), iWeaponIndex);
			wsprintf(szWeaponB, TEXT("Equipment_CurrentWeapon_B%d"), iWeaponIndex);
			wsprintf(szIcon, TEXT("Equipment_CurrentWeaponIcon%d"), iWeaponIndex);

			if (m_arrOriginalPos[i] < Find_ChildUI(szWeapon)->Get_Position().y)
			{
				m_arrItmeWindowPick[i] = true;
				m_arrAddWeaponPos[i] = Find_ChildUI(szWeapon)->Get_Position().y;
				m_arrAddWeaponPos[i] -= _float(TimeDelta) * fSpeed;
				Find_ChildUI(szWeapon)->Set_Position({ fPositionX, m_arrAddWeaponPos[i] });
				Find_ChildUI(szWeaponB)->Set_Position({ fPositionX, m_arrAddWeaponPos[i] });
				Find_ChildUI(szIcon)->Set_Position({ Find_ChildUI(szIcon)->Get_Position().x, m_arrAddWeaponPos[i] });
			}
			else
			{
				Find_ChildUI(szWeapon)->Set_Position({ fPositionX, m_arrOriginalPos[i] });
				Find_ChildUI(szWeaponB)->Set_Position({ fPositionX, m_arrOriginalPos[i] });
				if(i == 0)
					Find_ChildUI(szIcon)->Set_Position({ Find_ChildUI(szIcon)->Get_Position().x, -14.0f });
				else
					Find_ChildUI(szIcon)->Set_Position({ Find_ChildUI(szIcon)->Get_Position().x, m_arrOriginalPos[i] });
			}

			CCanvas::Find_ChildUI(szWeapon)->SetVisible(true);
			CCanvas::Find_ChildUI(szWeaponB)->SetVisible(true);
			CCanvas::Find_ChildUI(szIcon)->SetVisible(true);
		}
	}
	else
	{
		_float fStartPos = 102.0f;
		for (size_t i = 0; i < 3; i++)
		{
			_int	iWeaponIndex = _int(i) + 1;
			wsprintf(szWeapon, TEXT("Equipment_CurrentWeapon%d"), iWeaponIndex);
			wsprintf(szWeaponB, TEXT("Equipment_CurrentWeapon_B%d"), iWeaponIndex);
			wsprintf(szIcon, TEXT("Equipment_CurrentWeaponIcon%d"), iWeaponIndex);

			if (fStartPos > Find_ChildUI(szWeapon)->Get_Position().y)
			{
				m_arrAddWeaponPos[i] = Find_ChildUI(szWeapon)->Get_Position().y;
				m_arrAddWeaponPos[i] += _float(TimeDelta) * fSpeed;
				Find_ChildUI(szWeapon)->Set_Position({ fPositionX, m_arrAddWeaponPos[i] });
				Find_ChildUI(szWeaponB)->Set_Position({ fPositionX, m_arrAddWeaponPos[i] });
				Find_ChildUI(szIcon)->Set_Position({ Find_ChildUI(szIcon)->Get_Position().x, m_arrAddWeaponPos[i] });

				dynamic_cast<CMain_PickUI*>(Find_ChildUI(szWeaponB))->Set_OnAlpha();
			}
			else
			{
				m_bWeaponPick = false;
				m_arrItmeWindowPick[i] = false;
				Find_ChildUI(szWeapon)->Set_Position({ fPositionX, fStartPos });
				Find_ChildUI(szWeaponB)->Set_Position({ fPositionX, fStartPos });
				Find_ChildUI(szIcon)->Set_Position({ Find_ChildUI(szIcon)->Get_Position().x, fStartPos });

				CCanvas::Find_ChildUI(szWeapon)->SetVisible(false);
				CCanvas::Find_ChildUI(szWeaponB)->SetVisible(false);
				CCanvas::Find_ChildUI(szIcon)->SetVisible(false);
			}
		}
	}
}

void CCanvas_Equipment::RihgtInfoMove_Tick(const _double& TimeDelta)
{
	Find_ChildUI(L"Equipment_ItemInfo")->SetVisible(m_bWeaponPick);
	Find_ChildUI(L"Equipment_ItemInfo_B")->SetVisible(m_bWeaponPick);
	Find_ChildUI(L"Equipment_ItemInfoIcon")->SetVisible(m_bWeaponPick);

	_float2 vPosition = Find_ChildUI(L"Equipment_ItemInfoMove")->Get_Position();
	Find_ChildUI(L"Equipment_ItemInfoMoveIcon")->Set_Position(vPosition + _float2(-255.0f, 88.0f));

	// 오른쪽에 무기 정보 띄우기
	_float fPositionX = Find_ChildUI(L"Equipment_ItemInfoMove")->Get_Position().x;
	_float fSpeed = 300.0f;
	if (m_bWeaponPick == false)
	{
		_float fStartPosY = 170.0f;
		if (fStartPosY > Find_ChildUI(L"Equipment_ItemInfoMove")->Get_Position().y)
		{
			m_fAddWeaponPos = Find_ChildUI(L"Equipment_ItemInfoMove")->Get_Position().y;
			m_fAddWeaponPos += _float(TimeDelta) * fSpeed;
			Find_ChildUI(L"Equipment_ItemInfoMove")->Set_Position({ fPositionX, m_fAddWeaponPos });
			Find_ChildUI(L"Equipment_ItemInfo_BMove")->Set_Position({ fPositionX, m_fAddWeaponPos });
		}
		else
		{
			Find_ChildUI(L"Equipment_ItemInfoMove")->Set_Position({ fPositionX, fStartPosY });
			Find_ChildUI(L"Equipment_ItemInfo_BMove")->Set_Position({ fPositionX, fStartPosY });
		}

		return;
	}

	if (m_fOriginalPos < Find_ChildUI(L"Equipment_ItemInfoMove")->Get_Position().y)
	{
		m_fAddWeaponPos = Find_ChildUI(L"Equipment_ItemInfoMove")->Get_Position().y;
		m_fAddWeaponPos -= _float(TimeDelta) * fSpeed;
		Find_ChildUI(L"Equipment_ItemInfoMove")->Set_Position({ fPositionX, m_fAddWeaponPos });
		Find_ChildUI(L"Equipment_ItemInfo_BMove")->Set_Position({ fPositionX, m_fAddWeaponPos });
	}
	else
	{
		Find_ChildUI(L"Equipment_ItemInfoMove")->Set_Position({ fPositionX, m_fOriginalPos });
		Find_ChildUI(L"Equipment_ItemInfo_BMove")->Set_Position({ fPositionX, m_fOriginalPos });
	}
}

void CCanvas_Equipment::WeaponkindPick_Tick()
{
	_tchar szWeaponB[MAX_PATH] = TEXT("");

	// 아이템 획득하기 전에는 정보를 선택할 수 없도록
	for (size_t i = 0; i < m_vecItmeInfo.size(); i++)
	{
		if (2 == m_vecItmeInfo[i].second.iWeaponNum)
		{
			if (0 == m_vecItmeInfo[i].second.iCount)
			{
				dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Equipment_CurrentWeapon_B2"))->Set_OnButton();
				dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Equipment_CurrentWeapon_B2"))->Set_OnAlpha();
			}
		}
		if (3 == m_vecItmeInfo[i].second.iWeaponNum)
		{
			if (0 == m_vecItmeInfo[i].second.iCount)
			{
				dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Equipment_CurrentWeapon_B3"))->Set_OnButton();
				dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Equipment_CurrentWeapon_B3"))->Set_OnAlpha();
			}
		}
	}

	for (_uint i = 0; i < 3; i++)
	{
		_int	iWeaponIndex = _int(i) + 1;
		wsprintf(szWeaponB, TEXT("Equipment_CurrentWeapon_B%d"), iWeaponIndex);

		if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(szWeaponB))->Get_OnButton())
		{
			CUI_Manager::GetInstance()->PlaySound("Weapon");
		
			m_bWeaponPick = true;
			m_iPickIndex = iWeaponIndex;
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(szWeaponB))->Set_OnButton();
			
			// 실제로 플레이어 무기 변경
			CPlayerInfoManager::GetInstance()->Set_PlayerWeapon(i);
		}
	}

	// 아이템 매니저 에게도 데이터 전달
	if (CGameInstance::GetInstance()->KeyDown(DIK_RETURN))
	{
		vector<pair<wstring, CItem_Manager::ITEMINFO>> ItemInfo = CItem_Manager::GetInstance()->Get_ItmeInfo();
		auto iterCurrent = find_if(ItemInfo.begin(), ItemInfo.end(), [&](pair<wstring, CItem_Manager::ITEMINFO> pair) {
			return pair.second.bWeapon == true;
			});

		auto iterPick = find_if(ItemInfo.begin(), ItemInfo.end(), [&](pair<wstring, CItem_Manager::ITEMINFO> pair) {
			return pair.second.iWeaponNum == m_iPickIndex;
			});

		if (iterCurrent != ItemInfo.end())
		{
			CItem_Manager::GetInstance()->Set_Weapon(iterCurrent->first, false);
		}
		if (iterPick != ItemInfo.end())
		{
			CItem_Manager::GetInstance()->Set_Weapon(iterPick->first, true);
		}
	}

	for (size_t i = 0; i < 3; i++)
	{
		if (i + 1 == m_iPickIndex)
			continue;

		_int	iWeaponIndex = _int(i) + 1;
		wsprintf(szWeaponB, TEXT("Equipment_CurrentWeapon_B%d"), iWeaponIndex);
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(szWeaponB))->Set_OnAlpha();
	}
}

void CCanvas_Equipment::ArrowTick()
{
	// 화살표 추가하기

	if (false == m_bWeaponPick)
	{
		Find_ChildUI(L"Arrow")->SetVisible(false);
		return;
	}
	Find_ChildUI(L"Arrow")->SetVisible(true);

	_uint iAttack0 = 0;
	_uint iAttack1 = 0;

	for (size_t i = 0; i < m_vecItmeInfo.size(); i++)
	{
		if (true == m_vecItmeInfo[i].second.bWeapon)	// 현재 사용 중인 무기
			iAttack0 = m_vecItmeInfo[i].second.iWeaponAttack;
		if (m_iPickIndex == m_vecItmeInfo[i].second.iWeaponNum)		// 현재 클리한 무기
			iAttack1 = m_vecItmeInfo[i].second.iWeaponAttack;
	}

	if(iAttack0 == iAttack1)
		Find_ChildUI(L"Arrow")->SetVisible(false);
	else if (iAttack0 < iAttack1)
		dynamic_cast<CShaderUI*>(Find_ChildUI(L"Arrow"))->Set_Float2sX(0);
	else
		dynamic_cast<CShaderUI*>(Find_ChildUI(L"Arrow"))->Set_Float2sX(1);
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
