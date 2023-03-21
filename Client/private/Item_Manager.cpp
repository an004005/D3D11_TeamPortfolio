#include "stdafx.h"
#include "..\public\Item_Manager.h"
#include "Canvas.h"

IMPLEMENT_SINGLETON(CItem_Manager)

CItem_Manager::CItem_Manager()
{
}

HRESULT CItem_Manager::Initialize()
{
	ITEMINFO	tItemInfo;
	tItemInfo.eType = BATTLE;
	tItemInfo.bNew = false;
	tItemInfo.iCount = 0;
	tItemInfo.iMaxCount = 10;
	tItemInfo.iWeaponAttack = 0;
	tItemInfo.vIconIndex = { 4.0f, 1.0f };

	tItemInfo.szItemEx[0] = L"체력을 최대치 30%만큼 회복한다.";
	tItemInfo.szItemEx[1] = L"약간 달콤한 귤맛이 나는 음료형 젤리.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"회복(소) 젤리", tItemInfo);

	tItemInfo.szItemEx[0] = L"체력을 최대치 60%만큼 회복한다.";
	tItemInfo.szItemEx[1] = L"적당히 달콤한 감맛이 나는 음료형 젤리.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"회복(중) 젤리", tItemInfo);

	tItemInfo.szItemEx[0] = L"체력을 완전히 회복한다.";
	tItemInfo.szItemEx[1] = L"엄청 달콤한 배맛이 나는 음료형 젤리.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"회복(대) 젤리", tItemInfo);

	tItemInfo.szItemEx[0] = L"아군 전체의 체력을 최대치 30%만큼 회복한다.";
	tItemInfo.szItemEx[1] = L"다 같이 나눠 먹는 패밀리 팩..";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"전체: 회복(소) 젤리", tItemInfo);

	tItemInfo.szItemEx[0] = L"아군 전체의 체력을 최대치 60%만큼 회복한다.";
	tItemInfo.szItemEx[1] = L"다 같이 나눠 먹는 패밀리 팩.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"전체: 회복(중) 젤리", tItemInfo);

	tItemInfo.szItemEx[0] = L"아군 전체의 체력을 완전히 회복한다.";
	tItemInfo.szItemEx[1] = L"다 같이 나눠 먹는 패밀리 팩.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"전체: 회복(대) 젤리", tItemInfo);

	tItemInfo.szItemEx[0] = L"SAS 게이지를 최대까지 회복한다.";
	tItemInfo.szItemEx[1] = L"머리가 상쾌해지는 시워한 음료.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"SAS 보급수", tItemInfo);

	////////////////////////////////////////////////////////////////////////////////
	tItemInfo.eType = WEAPON;
	tItemInfo.iMaxCount = 1;
	tItemInfo.iWeaponAttack = 27;

	tItemInfo.szItemEx[0] = L"유이토가 훈련생 시절부터 애용하는 검.";
	tItemInfo.szItemEx[1] = L"칼날이 얇고 가벼워, 초급에서 중급 정도의 전투기술로도";
	tItemInfo.szItemEx[2] = L"다루기 쉽다. 고대 명검의 이름이 붙어 있다.";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"묘호 무라마사", tItemInfo);

	////////////////////////////////////////////////////////////////////////////////
	tItemInfo.eType = ETC;
	tItemInfo.iMaxCount = 1;
	tItemInfo.iWeaponAttack = 0;

	tItemInfo.szItemEx[0] = L"구 토벌군병원에서 주운 텍스트 데이터.";
	tItemInfo.szItemEx[1] = L"누군가의 개인적인 메모 같다. 괴이의 습격으로 인해";
	tItemInfo.szItemEx[2] = L"병원을 포기하면서 회수되지 않고 그대로 버려진 모양.";
	tItemInfo.szItemEx[3] = L" "; // 내용물은 브레인 메시지 화면에서 확인할 수 있다.
	m_vecItem.emplace_back(L"수수께끼의 텍스트 데이터", tItemInfo);


	return S_OK;
}

void CItem_Manager::Free()
{


}
