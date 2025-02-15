#include "stdafx.h"
#include "..\public\Item_Manager.h"
#include "Canvas.h"
#include "UI_Manager.h"
#include "GameManager.h"

#include "Canvas_Item.h"
#include "Canvas_Acquisition.h"
#include  "JsonStorage.h"

IMPLEMENT_SINGLETON(CItem_Manager)

CItem_Manager::CItem_Manager()
{
}

HRESULT CItem_Manager::Initialize()
{
	ItemListPos_Initialize();
	ItemInfo_Initialize();
	ShopListPos_Initialize();

	BATTLEITEM	tBattleItme;
	tBattleItme.wsName = L"회복(소) 젤리";
	tBattleItme.vIconIndex = { 1.0f, 0.0f };
	tBattleItme.iCount = 3;
	m_vecBattleItem.push_back(tBattleItme);

	m_arrBrain.fill(false);

	return S_OK;
}

void CItem_Manager::Set_ItemCount(const wstring& wszName, const _int iCount)
{
	// wszName 이름으로 찾아낸 데이터의 아이템 개수를 iCount 로 추가 또는 감소한다.
	auto iter = find_if(m_vecItem.begin(), m_vecItem.end(), [&](pair<wstring, ITEMINFO> pair) {
		return pair.first == wszName;
		});

	if (iter != m_vecItem.end())
	{
		// 배틀 아이템 이라면 Front UI에 띄우기 위해 다음을 추가한다.
		if ((*iter).second.eType == MAINITEM::BATTLE)
		{
			// 예외처리 : 아이템은 각각 최대 '10개' 까지만 보관할 수 있다.
			if ((*iter).second.iCount + iCount > 10 && 0 < iCount) // if ((*iter).second.iCount >= 10 && 0 < iCount)
			{
				CGameManager::GetInstance()->Set_FullItem(wszName);
				return;
			}

			(*iter).second.iCount += iCount;

			// 예외처리 : 개수가 0개가 된 경우 m_vecBattleItem 에서 제외한다.
			if (0 >= (*iter).second.iCount)
			{
				for (size_t i = 0; i < m_vecBattleItem.size(); i++)
				{
					if (m_vecBattleItem[i].wsName == wszName)
					{
						m_vecBattleItem.erase(m_vecBattleItem.begin() + i);
						break;
					}
				}

				dynamic_cast<CCanvas_Item*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Item"))->Set_Input();
				//dynamic_cast<CCanvas_ItemMove*>(CUI_Manager::GetInstance()->Find_MoveCanvas(L"Canvas_ItemMove"))->Set_Input();
				return;
			}

			BATTLEITEM tBattleItem;
			tBattleItem.wsName = (*iter).first;
			tBattleItem.iCount = (*iter).second.iCount;
			tBattleItem.vIconIndex = (*iter).second.vIconIndex;

			// 이미 이전에 넣어둔 데이터 라면 반복문에서 찾아서 데이터를 갱신하고,
			_bool bExist = false;
			_uint iIndex = 0;

			for (auto it : m_vecBattleItem)
			{
				if (it.wsName == wszName)
				{
					bExist = true;
					break;
				}
				iIndex++;
			}

			// Front UI 에 적용하기
			dynamic_cast<CCanvas_Item*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Item"))->Set_Input();

			if (bExist)
			{
				m_vecBattleItem[iIndex] = tBattleItem;
			}
			else
			{
				// 이전에 없던 데이터라면 새로 추가한다.
				m_vecBattleItem.emplace_back(tBattleItem);
			}
		}
		else
		{
			// 배틀 아이템을 제외한 다른 아이템의 경우 최대 개수는 1개 이다.
			if ((*iter).second.iCount >= 1)
			{
				CGameManager::GetInstance()->Set_FullItem(wszName);
				return;
			}

			(*iter).second.iCount += iCount;
		}
	}
}

void CItem_Manager::Set_Weapon(const wstring& wszName, const _bool bCurrent)
{
	auto iter = find_if(m_vecItem.begin(), m_vecItem.end(), [&](pair<wstring, ITEMINFO> pair) {
		return pair.first == wszName;
		});

	if (iter != m_vecItem.end())
	{
		(*iter).second.bWeapon = bCurrent;
	}
}

struct ItemSaveInfo
{
	_uint iIdx = 0;
	_uint iCnt = 0;
};



void CItem_Manager::Save(const string& path)
{
	Json json;
	json["Items"] = Json::array();
	json["ETC"] = Json::array();

	for (auto battleItem : m_vecBattleItem)
	{
		for (int i = 0; i < m_vecItem.size(); ++i)
		{
			if (battleItem.wsName == m_vecItem[i].first)
			{
				Json itemJson;
				itemJson["Idx"] = i;
				itemJson["Cnt"] = battleItem.iCount;
				json["Items"].push_back(itemJson);
			}
		}
	}

	for (int i = 7; i < m_vecItem.size(); ++i)
	{
		if (m_vecItem[i].second.iCount > 0)
		{
			json["ETC"].push_back(i);
		}
	}


	std::ofstream file(path);
	file << json;
}

void CItem_Manager::Load(const string& path)
{
	Json json = CJsonStorage::GetInstance()->FindOrLoadJson(path);

	m_vecBattleItem.clear();
	for (auto itemJson : json["Items"])
	{
		_uint iIdx = itemJson["Idx"];
		_uint iCnt = itemJson["Cnt"];

		BATTLEITEM	tBattleItme;
		tBattleItme.wsName = m_vecItem[iIdx].first;
		tBattleItme.vIconIndex = m_vecItem[iIdx].second.vIconIndex;
		tBattleItme.iCount = iCnt;
		m_vecBattleItem.push_back(tBattleItme);
	}

	for (auto itemJson : json["ETC"])
	{
		_uint num = itemJson;
		m_vecItem[num].second.iCount = 1;
	}

}

void CItem_Manager::ItemListPos_Initialize()
{
	m_arrItemIndexPos.fill(static_cast<_float2>(0.0f, 0.0f));
	m_arrIconIndexPos.fill(static_cast<_float2>(0.0f, 0.0f));

	m_arrItemIndexPos[0] = { -357.0f, 328.0f };
	m_arrItemIndexPos[1] = { 357.0f, 328.0f };
	m_arrItemIndexPos[2] = { -357.0f, 260.0f };
	m_arrItemIndexPos[3] = { 357.0f, 260.0f };
	m_arrItemIndexPos[4] = { -357.0f, 194.0f };
	m_arrItemIndexPos[5] = { 357.0f, 194.0f };
	m_arrItemIndexPos[6] = { -357.0f, 129.0f };
	m_arrItemIndexPos[7] = { 357.0f, 129.0f };
	m_arrItemIndexPos[8] = { -357.0f, 62.0f };
	m_arrItemIndexPos[9] = { 357.0f, 62.0f };
	m_arrItemIndexPos[10] = { -357.0f, -5.0f };
	m_arrItemIndexPos[11] = { 357.0f, -5.0f };
	m_arrItemIndexPos[12] = { -357.0f, -72.0f };
	m_arrItemIndexPos[13] = { 357.0f, -72.0f };
	//
	m_arrIconIndexPos[0] = { -657.0f, 327.0f };
	m_arrIconIndexPos[1] = { 58.0f, 327.0f };
	m_arrIconIndexPos[2] = { -657.0f, 260.0f };
	m_arrIconIndexPos[3] = { 58.0f, 260.0f };
	m_arrIconIndexPos[4] = { -657.0f, 195.0f };
	m_arrIconIndexPos[5] = { 58.0f, 195.0f };
	m_arrIconIndexPos[6] = { -657.0f, 130.0f };
	m_arrIconIndexPos[7] = { 58.0f, 130.0f };
	m_arrIconIndexPos[8] = { -657.0f, 65.0f };
	m_arrIconIndexPos[9] = { 58.0f, 65.0f };
	m_arrIconIndexPos[10] = { -657.0f, 0.0f };
	m_arrIconIndexPos[11] = { 58.0f, 0.0f };
	m_arrIconIndexPos[12] = { -657.0f, -65.0f };
	m_arrIconIndexPos[13] = { 58.0f, -65.0f };
}

void CItem_Manager::ItemInfo_Initialize()
{
	ITEMINFO	tItemInfo;
	tItemInfo.eType = MAINITEM::BATTLE;
	tItemInfo.bNew = false;
	tItemInfo.iMaxCount = 10;
	tItemInfo.bWeapon = false;
	tItemInfo.iWeaponNum = 0;
	tItemInfo.iWeaponAttack = 0;

	tItemInfo.iCount = 3;
	tItemInfo.iPrice = 100;
	tItemInfo.iSellingPrice = 20;
	tItemInfo.vIconIndex = { 1.0f, 0.0f };
	tItemInfo.szItemEx[0] = L"체력을 최대치 30%만큼 회복한다.";
	tItemInfo.szItemEx[1] = L"약간 달콤한 귤맛이 나는 음료형 젤리.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"회복(소) 젤리", tItemInfo); //0 

	tItemInfo.iCount = 0;
	tItemInfo.iPrice = 300;
	tItemInfo.iSellingPrice = 60;
	tItemInfo.vIconIndex = { 0.0f, 5.0f };
	tItemInfo.szItemEx[0] = L"체력을 최대치 60%만큼 회복한다.";
	tItemInfo.szItemEx[1] = L"적당히 달콤한 감맛이 나는 음료형 젤리.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"회복(중) 젤리", tItemInfo);

	tItemInfo.iPrice = 1000;
	tItemInfo.iSellingPrice = 180;
	tItemInfo.vIconIndex = { 3.0f, 0.0f };
	tItemInfo.szItemEx[0] = L"체력을 완전히 회복한다.";
	tItemInfo.szItemEx[1] = L"엄청 달콤한 배맛이 나는 음료형 젤리.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"회복(대) 젤리", tItemInfo);

	tItemInfo.iPrice = 250;
	tItemInfo.iSellingPrice = 40;
	tItemInfo.vIconIndex = { 2.0f, 0.0f };
	tItemInfo.szItemEx[0] = L"아군 전체의 체력을 최대치 30%만큼 회복한다.";
	tItemInfo.szItemEx[1] = L"다 같이 나눠 먹는 패밀리 팩..";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"전체: 회복(소) 젤리", tItemInfo);

	tItemInfo.iPrice = 900;
	tItemInfo.iSellingPrice = 150;
	tItemInfo.vIconIndex = { 4.0f, 0.0f };
	tItemInfo.szItemEx[0] = L"아군 전체의 체력을 최대치 60%만큼 회복한다.";
	tItemInfo.szItemEx[1] = L"다 같이 나눠 먹는 패밀리 팩.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"전체: 회복(중) 젤리", tItemInfo);

	tItemInfo.iPrice = 3000;
	tItemInfo.iSellingPrice = 600;
	tItemInfo.vIconIndex = { 1.0f, 5.0f };
	tItemInfo.szItemEx[0] = L"아군 전체의 체력을 완전히 회복한다.";
	tItemInfo.szItemEx[1] = L"다 같이 나눠 먹는 패밀리 팩.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"전체: 회복(대) 젤리", tItemInfo);

	tItemInfo.iPrice = 3000;
	tItemInfo.iSellingPrice = 600;
	tItemInfo.vIconIndex = { 3.0f, 1.0f };
	tItemInfo.szItemEx[0] = L"SAS 게이지를 최대까지 회복한다.";
	tItemInfo.szItemEx[1] = L"머리가 상쾌해지는 시원한 음료.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"SAS 보급수", tItemInfo);

	////////////////////////////////////////////////////////////////////////////////
	tItemInfo.eType = MAINITEM::WEAPON;
	tItemInfo.iMaxCount = 1;
	tItemInfo.vIconIndex = { 4.0f, 2.0f };

	tItemInfo.iCount = 1;
	tItemInfo.iPrice = 200;
	tItemInfo.iSellingPrice = 20;
	tItemInfo.bWeapon = true;
	tItemInfo.iWeaponNum = 1;
	tItemInfo.iWeaponAttack = 27;
	tItemInfo.szItemEx[0] = L"유이토가 훈련생 시절부터 애용하는 검.";
	tItemInfo.szItemEx[1] = L"칼날이 얇고 가벼워, 초급에서 중급 정도의 전투기술로도";
	tItemInfo.szItemEx[2] = L"다루기 쉽다. 고대 명검의 이름이 붙어 있다.";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"묘호 무라마사", tItemInfo);

	tItemInfo.iCount = 0;
	tItemInfo.iPrice = 1000;
	tItemInfo.iSellingPrice = 320;
	tItemInfo.bWeapon = false;
	tItemInfo.iWeaponNum = 2;
	tItemInfo.iWeaponAttack = 35;
	tItemInfo.szItemEx[0] = L"칼날 부분이 작고 날카로운 검의 집합체로 구성된 검.";
	tItemInfo.szItemEx[1] = L"일반적인 검처럼 사용할 수도 있고, 힘을 주면 딱딱한";
	tItemInfo.szItemEx[2] = L"대상물을 톱처럼 당겨서 절단할 수도 있다.";
	tItemInfo.szItemEx[3] = L"";
	m_vecItem.emplace_back(L"이치고히토후리", tItemInfo);

	tItemInfo.iCount = 0;
	tItemInfo.iPrice = 2000;
	tItemInfo.iSellingPrice = 780;
	tItemInfo.bWeapon = false;
	tItemInfo.iWeaponNum = 3;
	tItemInfo.iWeaponAttack = 47;
	tItemInfo.szItemEx[0] = L"충후한 도신이 튼튼한 인상을 주는 검";
	tItemInfo.szItemEx[1] = L"무거운 데다 사용자를 조금 가리지만 손에 익으면";
	tItemInfo.szItemEx[2] = L"가벼운 검보다 더 많은 대상을 파괴할 수 있다.";
	tItemInfo.szItemEx[3] = L"";
	m_vecItem.emplace_back(L"주즈마루 츠네츠구", tItemInfo);

	////////////////////////////////////////////////////////////////////////////////
	tItemInfo.eType = MAINITEM::ETC;
	tItemInfo.iPrice = 0;
	tItemInfo.iSellingPrice = 0;
	tItemInfo.iMaxCount = 1;
	tItemInfo.iWeaponAttack = 0;
	tItemInfo.iPrice = 0;

	tItemInfo.iCount = 1;

	tItemInfo.vIconIndex = { 0.0f, 2.0f };
	tItemInfo.szItemEx[0] = L"이는 복제를 이용하여 ";
	tItemInfo.szItemEx[1] = L"매일 말도 안 되는 코드량을 작성했다고 하는데";
	tItemInfo.szItemEx[2] = L"이는 여기서 이렇게 불린다고 한다.";
	tItemInfo.szItemEx[3] = L"갓기범...";
	m_vecItem.emplace_back(L"복제의 김기범", tItemInfo);

	tItemInfo.szItemEx[0] = L"이는 발화를 이용하여";
	tItemInfo.szItemEx[1] = L"마지막까지 하얗게 불태웠다고 하는데";
	tItemInfo.szItemEx[2] = L"과연 결과가 괜찮을까...";
	tItemInfo.szItemEx[3] = L"";
	m_vecItem.emplace_back(L"발화의 옥수현", tItemInfo);

	tItemInfo.szItemEx[0] = L"이는 초고속으로 항상";
	tItemInfo.szItemEx[1] = L"말을 초고속으로 쉬지 않고 했다고 하는데";
	tItemInfo.szItemEx[2] = L"당연 팀폴실에서 시끄러운 애다.";
	tItemInfo.szItemEx[3] = L"";
	m_vecItem.emplace_back(L"초고속의 전인복", tItemInfo);

	tItemInfo.szItemEx[0] = L"화려한 방전 이펙트 처럼";
	tItemInfo.szItemEx[1] = L"이는 이런 화려한 이펙트들을 만들었다고 하는데 그저 감탄만 나온다... ";
	tItemInfo.szItemEx[2] = L"마지막으로 하고 싶은 말이 있다는데.";
	tItemInfo.szItemEx[3] = L"\"난 끝나고 천국에 갈거야... 김밥천국\"";
	m_vecItem.emplace_back(L"방전의 정지훈", tItemInfo);

	tItemInfo.szItemEx[0] = L"이는 순간 이동을 사용하여 매일 맛집만을 찾아다녔다고 하는데";
	tItemInfo.szItemEx[1] = L"점심은 \"윤쉐프\", 일식은 \"가츠몽\", 햄버거는 \"파파이스\"";
	tItemInfo.szItemEx[2] = L"부대찌개는 \"낭만 부대찌개\", 쌀국수는 \"미분당\"";
	tItemInfo.szItemEx[3] = L"마지막으로 가장 중요한 고기는 \"육향\" 이라고 한다.";
	m_vecItem.emplace_back(L"순간이동의 박종욱", tItemInfo);

	tItemInfo.szItemEx[0] = L"유이토가 염력으로 물건을 가지고 노는 것 처럼";
	tItemInfo.szItemEx[1] = L"비주얼 스튜디오에서 코드를 가지고 놀았다고 하는데...";
	tItemInfo.szItemEx[2] = L"역시 그의 실력은 엄청나다.";
	tItemInfo.szItemEx[3] = L"이는 역대 최고의 팀장이라고 할 수 있다.";
	m_vecItem.emplace_back(L"염력의 안중환", tItemInfo);
}

void CItem_Manager::ShopListPos_Initialize()
{
	m_arrShopListPos.fill(static_cast<_float2>(0.0f, 0.0f));
	m_arrShopIconPos.fill(static_cast<_float2>(0.0f, 0.0f));

	m_arrShopListPos[0] = { 555.0f, 285.0f };
	m_arrShopListPos[1] = { 555.0f, 220.0f };
	m_arrShopListPos[2] = { 555.0f, 155.0f };
	m_arrShopListPos[3] = { 555.0f, 90.0f };
	m_arrShopListPos[4] = { 555.0f, 25.0f };
	m_arrShopListPos[5] = { 555.0f, -40.0f };
	m_arrShopListPos[6] = { 555.0f, -105.0f };
	m_arrShopListPos[7] = { 555.0f, -170.0f };
	m_arrShopListPos[8] = { 555.0f, -235.0f };
	m_arrShopListPos[9] = { 555.0f, -300.0f };

	m_arrShopIconPos[0] = { 132.0f, m_arrShopListPos[0].y - 1.0f };
	m_arrShopIconPos[1] = { 132.0f, m_arrShopListPos[1].y - 1.0f };
	m_arrShopIconPos[2] = { 132.0f, m_arrShopListPos[2].y - 1.0f };
	m_arrShopIconPos[3] = { 132.0f, m_arrShopListPos[3].y - 1.0f };
	m_arrShopIconPos[4] = { 132.0f, m_arrShopListPos[4].y - 1.0f };
	m_arrShopIconPos[5] = { 132.0f, m_arrShopListPos[5].y - 1.0f };
	m_arrShopIconPos[6] = { 132.0f, m_arrShopListPos[6].y - 1.0f };
	m_arrShopIconPos[7] = { 132.0f, m_arrShopListPos[7].y - 1.0f };
	m_arrShopIconPos[8] = { 132.0f, m_arrShopListPos[8].y - 1.0f };
	m_arrShopIconPos[9] = { 132.0f, m_arrShopListPos[9].y - 1.0f };
}

void CItem_Manager::Free()
{
	m_vecItem.clear();
	m_vecBattleItem.clear();

}
