#include "stdafx.h"
#include "..\public\Item_Manager.h"
#include "Canvas.h"

IMPLEMENT_SINGLETON(CItem_Manager)

CItem_Manager::CItem_Manager()
{
}

HRESULT CItem_Manager::Initialize()
{
	ItemPosition_Intiialize();
	ItemInfo_Intiialize();

	return S_OK;
}

void CItem_Manager::Set_ItemCount(const wstring & wszName, const _uint iCount)
{
	auto iter = find_if(m_vecItem.begin(), m_vecItem.end(), [&](pair<wstring, ITEMINFO> pair) {
		return pair.first == wszName;
	});

	if (iter != m_vecItem.end())
	{
		(*iter).second.iCount += iCount;

		//벡터가 아닌 배열로 수정해야 할듯?
		BATTLEITEM tBattleItem;
		tBattleItem.wsName = (*iter).first;
		tBattleItem.iCount = (*iter).second.iCount;
		tBattleItem.vIconIndex = (*iter).second.vIconIndex;
		m_vecBattleItem.emplace_back(tBattleItem);
	}
}

void CItem_Manager::ItemPosition_Intiialize()
{
	m_arrItemIndexPos.fill(_float2(0.0f, 0.0f));
	m_arrIconIndexPos.fill(_float2(0.0f, 0.0f));

	m_arrItemIndexPos[0] = { -306.0f, 270.0f };
	m_arrItemIndexPos[1] = { 306.0f, 270.0f };
	m_arrItemIndexPos[2] = { -306.0f, 210.0f };
	m_arrItemIndexPos[3] = { 306.0f, 210.0f };
	m_arrItemIndexPos[4] = { -306.0f, 150.0f };
	m_arrItemIndexPos[5] = { 306.0f, 150.0f };
	m_arrItemIndexPos[6] = { -306.0f, 90.0f };
	m_arrItemIndexPos[7] = { 306.0f, 90.0f };
	m_arrItemIndexPos[8] = { -306.0f, 30.0f };
	m_arrItemIndexPos[9] = { 306.0f, 30.0f };
	m_arrItemIndexPos[10] = { -306.0f, -30.0f };
	m_arrItemIndexPos[11] = { 306.0f, -30.0f };
	m_arrItemIndexPos[12] = { -306.0f, -90.0f };
	m_arrItemIndexPos[13] = { 306.0f, -90.0f };
	//
	m_arrIconIndexPos[0] = { -566.0f, 271.0f };
	m_arrIconIndexPos[1] = { 46.0f, 271.0f };
	m_arrIconIndexPos[2] = { -566.0f, 211.0f };
	m_arrIconIndexPos[3] = { 46.0f, 211.0f };
	m_arrIconIndexPos[4] = { -566.0f, 151.0f };
	m_arrIconIndexPos[5] = { 46.0f, 151.0f };
	m_arrIconIndexPos[6] = { -566.0f, 91.0f };
	m_arrIconIndexPos[7] = { 46.0f, 91.0f };
	m_arrIconIndexPos[8] = { -566.0f, 31.0f };
	m_arrIconIndexPos[9] = { 46.0f, 31.0f };
	m_arrIconIndexPos[10] = { -566.0f, -29.0f };
	m_arrIconIndexPos[11] = { 46.0f, -29.0f };
	m_arrIconIndexPos[12] = { -566.0f, -29.0f };
	m_arrIconIndexPos[13] = { 46.0f, -89.0f };
}

void CItem_Manager::ItemInfo_Intiialize()
{
	ITEMINFO	tItemInfo;
	tItemInfo.eType = BATTLE;
	tItemInfo.bNew = false;
	tItemInfo.iCount = 0;
	tItemInfo.iMaxCount = 10;
	tItemInfo.iWeaponNum = 0;
	tItemInfo.iWeaponAttack = 0;

	tItemInfo.vIconIndex = { 1.0f, 0.0f };
	tItemInfo.szItemEx[0] = L"체력을 최대치 30%만큼 회복한다.";
	tItemInfo.szItemEx[1] = L"약간 달콤한 귤맛이 나는 음료형 젤리.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"회복(소) 젤리", tItemInfo);

	tItemInfo.vIconIndex = { 0.0f, 5.0f };
	tItemInfo.szItemEx[0] = L"체력을 최대치 60%만큼 회복한다.";
	tItemInfo.szItemEx[1] = L"적당히 달콤한 감맛이 나는 음료형 젤리.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"회복(중) 젤리", tItemInfo);

	tItemInfo.vIconIndex = { 3.0f, 0.0f };
	tItemInfo.szItemEx[0] = L"체력을 완전히 회복한다.";
	tItemInfo.szItemEx[1] = L"엄청 달콤한 배맛이 나는 음료형 젤리.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"회복(대) 젤리", tItemInfo);

	tItemInfo.vIconIndex = { 2.0f, 0.0f };
	tItemInfo.szItemEx[0] = L"아군 전체의 체력을 최대치 30%만큼 회복한다.";
	tItemInfo.szItemEx[1] = L"다 같이 나눠 먹는 패밀리 팩..";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"전체: 회복(소) 젤리", tItemInfo);

	tItemInfo.vIconIndex = { 4.0f, 0.0f };
	tItemInfo.szItemEx[0] = L"아군 전체의 체력을 최대치 60%만큼 회복한다.";
	tItemInfo.szItemEx[1] = L"다 같이 나눠 먹는 패밀리 팩.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"전체: 회복(중) 젤리", tItemInfo);

	tItemInfo.vIconIndex = { 1.0f, 5.0f };
	tItemInfo.szItemEx[0] = L"아군 전체의 체력을 완전히 회복한다.";
	tItemInfo.szItemEx[1] = L"다 같이 나눠 먹는 패밀리 팩.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"전체: 회복(대) 젤리", tItemInfo);

	tItemInfo.vIconIndex = { 3.0f, 1.0f };
	tItemInfo.szItemEx[0] = L"SAS 게이지를 최대까지 회복한다.";
	tItemInfo.szItemEx[1] = L"머리가 상쾌해지는 시워한 음료.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"SAS 보급수", tItemInfo);

	////////////////////////////////////////////////////////////////////////////////
	tItemInfo.eType = WEAPON;
	tItemInfo.iMaxCount = 1;
	tItemInfo.iWeaponAttack = 27;
	tItemInfo.vIconIndex = { 4.0f, 2.0f };

	tItemInfo.iWeaponNum = 1;
	tItemInfo.szItemEx[0] = L"유이토가 훈련생 시절부터 애용하는 검.";
	tItemInfo.szItemEx[1] = L"칼날이 얇고 가벼워, 초급에서 중급 정도의 전투기술로도";
	tItemInfo.szItemEx[2] = L"다루기 쉽다. 고대 명검의 이름이 붙어 있다.";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"묘호 무라마사", tItemInfo);

	tItemInfo.iWeaponNum = 2;
	tItemInfo.szItemEx[0] = L"무기 테스트.";
	tItemInfo.szItemEx[1] = L"칼날이 얇고 가벼워, 초급에서 중급 정도의 전투기술로도";
	tItemInfo.szItemEx[2] = L"다루기 쉽다. 고대 명검의 이름이 붙어 있다.";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"무기 테스트", tItemInfo);

	tItemInfo.iWeaponNum = 3;
	tItemInfo.szItemEx[0] = L"무기 테스트2.";
	tItemInfo.szItemEx[1] = L"칼날이 얇고 가벼워, 초급에서 중급 정도의 전투기술로도";
	tItemInfo.szItemEx[2] = L"다루기 쉽다. 고대 명검의 이름이 붙어 있다.";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"무기 테스트2", tItemInfo);

	////////////////////////////////////////////////////////////////////////////////
	tItemInfo.eType = ETC;
	tItemInfo.iMaxCount = 1;
	tItemInfo.iWeaponAttack = 0;

	tItemInfo.vIconIndex = { 1.0f, 2.0f };
	tItemInfo.szItemEx[0] = L"구 토벌군병원에서 주운 텍스트 데이터.";
	tItemInfo.szItemEx[1] = L"누군가의 개인적인 메모 같다. 괴이의 습격으로 인해";
	tItemInfo.szItemEx[2] = L"병원을 포기하면서 회수되지 않고 그대로 버려진 모양.";
	tItemInfo.szItemEx[3] = L" "; // 내용물은 브레인 메시지 화면에서 확인할 수 있다.
	m_vecItem.emplace_back(L"수수께끼의 텍스트 데이터", tItemInfo);

	tItemInfo.szItemEx[0] = L"구 토벌군병원에서 주운 텍스트 데이터.";
	tItemInfo.szItemEx[1] = L"누군가의 개인적인 메모 같다. 괴이의 습격으로 인해";
	tItemInfo.szItemEx[2] = L"병원을 포기하면서 회수되지 않고 그대로 버려진 모양.";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"기타 테스트", tItemInfo);
}

void CItem_Manager::Free()
{
	m_vecItem.clear();

}
