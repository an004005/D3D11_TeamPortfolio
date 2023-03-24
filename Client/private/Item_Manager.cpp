#include "stdafx.h"
#include "..\public\Item_Manager.h"
#include "Canvas.h"

IMPLEMENT_SINGLETON(CItem_Manager)

CItem_Manager::CItem_Manager()
{
}

HRESULT CItem_Manager::Initialize()
{
	ItemIndex_Intiialize();
	ItemInfo_Intiialize();
	Brain_Intiialize();

	return S_OK;
}

void CItem_Manager::Set_ItemCount(const wstring & wszName, const _uint iCount)
{
	auto iter = find_if(m_vecItem.begin(), m_vecItem.end(), [&](pair<wstring, ITEMINFO> pair) {
		return pair.first == wszName;
	});

	if (iter != m_vecItem.end())
		(*iter).second.iCount = iCount;
}

void CItem_Manager::ItemIndex_Intiialize()
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

void CItem_Manager::Brain_Intiialize()
{
	BRAININFO	tBrainInfo;

	// 드라이브
	tBrainInfo.iBP = 1;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"드라이브 게이지 축적량 상승";
	tBrainInfo.szBrainEx[0] = L"드라이브 게이지 축적량 +20%";
	tBrainInfo.szBrainEx[1] = L"적을 쓰러뜨리거나 적에게 대미지를 받는 등";
	tBrainInfo.szBrainEx[2] = L"특정 행동으로 인한 축적량이 증가하여";
	tBrainInfo.szBrainEx[3] = L"드라이브 게이지가 쌓이기 쉬워진다.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"드라이브: 체력 자동 회복";
	tBrainInfo.szBrainEx[0] = L"드라이브 중 체력이 서서히 회복된다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"드라이브: 상태이상 회복";
	tBrainInfo.szBrainEx[0] = L"드라이브 발동하면 상태이상을 회복한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"드라이브: 연속 어설트 비전";
	tBrainInfo.szBrainEx[0] = L"브라이브 중 『어설트 비전』 이";
	tBrainInfo.szBrainEx[1] = L"연속으로 발생하게 된다.";
	tBrainInfo.szBrainEx[2] = L"※『어설트 비전』은 유대 레벨이 상승하면 발생";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"드라이브: 드라이브 게이지 회복1";
	tBrainInfo.szBrainEx[0] = L"드라이브 중 브레인  크러시로 적을 쓰러뜨리면";
	tBrainInfo.szBrainEx[1] = L"브라이브 게이지가 조금 회복된다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"드라이브: 보너스 배율 증가량 상승";
	tBrainInfo.szBrainEx[0] = L"드라이브 중 획득 EXP의 보너스 배율의";
	tBrainInfo.szBrainEx[1] = L"증가량이 상승한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"드라이브: 공격 크러시 효과 상승";
	tBrainInfo.szBrainEx[0] = L"드라이브 중 공격으로 크러시 게이지를";
	tBrainInfo.szBrainEx[1] = L"많이 깎을 수 있다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 6;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"드라이브: 드라이브 게이지 회복 2";
	tBrainInfo.szBrainEx[0] = L"드라이브 중 브레인 크러시로 적을 쓰러뜨리면";
	tBrainInfo.szBrainEx[1] = L"드라이브 게이지가 조금 회복된다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"드라이브: 스테이터스 상승";
	tBrainInfo.szBrainEx[0] = L"드라이브 중 초뇌능력, 공격력, 방어력 +10%";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"드라이브: SAS 게이지 회복";
	tBrainInfo.szBrainEx[0] = L"드라이브를 발동하면";
	tBrainInfo.szBrainEx[1] = L"모든 SAS 게이지가 모두 회복된다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 강화
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"염력 게이지 회복량 상승(공중 공격)";
	tBrainInfo.szBrainEx[0] = L"공중 공격으로 인한 염력 게이지의 회복량이 증가한다.";
	tBrainInfo.szBrainEx[1] = L"※[스페이스바]+[왼쪽 마루스] 올려베기 공격은 포함되지 않는다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"무기 공격 크러시 효과 상승2";
	tBrainInfo.szBrainEx[0] = L"무기 공격으로 크러시 게이지를 많이 깍을 수 있다.";
	tBrainInfo.szBrainEx[1] = L"크러시 효과가 5% 증가한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"염력 게이지 최대치 증가";
	tBrainInfo.szBrainEx[0] = L"같은 이름의 스킬이 2개 있으며, 1개를 습득하면 50%,";
	tBrainInfo.szBrainEx[1] = L"2개를 습득하면 100% 염력 게이지의 최대치가 증가한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"공격력 상승 1";
	tBrainInfo.szBrainEx[0] = L"공격력 +5%";
	tBrainInfo.szBrainEx[1] = L"무기 공격으로 주는 대미지가 증가한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"후속타";
	tBrainInfo.szBrainEx[0] = L"다운 중인 적에게 주는 대미지가 증가한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"소생 속도 상승";
	tBrainInfo.szBrainEx[0] = L"동료를 소생하는 속도가 상승한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"무기 공격 크러시 효과 상승 1";
	tBrainInfo.szBrainEx[0] = L"무기 공격으로 크러시 게이지를 많이 깍을 수 있다.";
	tBrainInfo.szBrainEx[1] = L"크러시 효과가 5% 증가한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"공격력 상승 2";
	tBrainInfo.szBrainEx[0] = L"공격력 +5%";
	tBrainInfo.szBrainEx[1] = L"무기 공격으로 주는 대미지가 증가한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"염력 게이지 최대치 증가";
	tBrainInfo.szBrainEx[0] = L"같은 이름의 스킬이 2개 있으며, 1개를 습득하면 50%,";
	tBrainInfo.szBrainEx[1] = L"2개를 습득하면 100% 염력 게이지의 최대치가 증가한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"초뇌능력 상승";
	tBrainInfo.szBrainEx[0] = L"초뇌능력 +5%";
	tBrainInfo.szBrainEx[1] = L"염력 공격으로 주는 대미지가 증가한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"염력 공격 크러시 효과 상승";
	tBrainInfo.szBrainEx[0] = L"염력 공격으로 크러시 게이지를 많이 깎을 수 있다.";
	tBrainInfo.szBrainEx[1] = L"크러시 효과가 5% 증가한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 확장
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"내려치기 공격";
	tBrainInfo.szBrainEx[0] = L"체공 중에 [C] 길게 누르기로";
	tBrainInfo.szBrainEx[1] = L"공중에서 하강하며 무기 공격을 한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"차지 공격";
	tBrainInfo.szBrainEx[0] = L"[C] 길게 누르기로 힘을 모으고, [C]를 떼면";
	tBrainInfo.szBrainEx[1] = L"넓은 범위에 위력을 높인 무기 공격을 한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"낙법";
	tBrainInfo.szBrainEx[0] = L"날려질 때 [위]로 자세를 바로 잡아 다운을 방지한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"저스트 회피 염력 게이지 회복";
	tBrainInfo.szBrainEx[0] = L"저스트 회피 성공 시 염력 게이지를 조금 회복한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"무기 콤보 4단";
	tBrainInfo.szBrainEx[0] = L"무기 공격을 4회까지 연속해서 할 수 있다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"2단 차지 공격";
	tBrainInfo.szBrainEx[0] = L"[C] 길게 누르기로 힘을 모은 후, 한 단계 더 힘을 모으고";
	tBrainInfo.szBrainEx[1] = L"[C] 를 떼면 『차지 공격』 의 위력이 높아진다. ";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"차지 공격 강화";
	tBrainInfo.szBrainEx[0] = L"『차지 공격』 과 『2단 차지 공격』의";
	tBrainInfo.szBrainEx[1] = L"공격 범위가 넓어지고 위력이 높아진다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"염력 후속타";
	tBrainInfo.szBrainEx[0] = L"다운된 적에게 [오름쪽 마우스]로 염력 공격을 하면";
	tBrainInfo.szBrainEx[1] = L"내리치듯이 염력 공격을 한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 6;
	tBrainInfo.iLevel = 6;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"무기 콤보 5탄";
	tBrainInfo.szBrainEx[0] = L"무기 공격을 5회까지 연속해서 할 수 있다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = GREEN;
	tBrainInfo.szBrainName = L"아이템 끌어당기기";
	tBrainInfo.szBrainEx[0] = L"근처에 떨어져있는 드롭 아이템을 자동으로 끌어당긴다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"저스트 회피 공격";
	tBrainInfo.szBrainEx[0] = L"저스트 회피 성공 시 [왼쪽 마우스]로 무기 공격을 한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"공궁 점프";
	tBrainInfo.szBrainEx[0] = L"공중 [스페이스바]로 한번 더 점프할 수 있다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"추가 공격 확장 1";
	tBrainInfo.szBrainEx[0] = L"염력 추가 공격과 러시 추가 공격을";
	tBrainInfo.szBrainEx[1] = L"3회까지 연속해서 할 수 있다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"회전 무빙 공격 확장";
	tBrainInfo.szBrainEx[0] = L"[C]로 이동하면서 넓은 범위에 휩쓰는";
	tBrainInfo.szBrainEx[1] = L"『회전 무빙 공격』을";
	tBrainInfo.szBrainEx[2] = L"한 콤보 중에 2번까지 할 수 있다.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"공중 추가 공격";
	tBrainInfo.szBrainEx[0] = L"공중 무기 콤보 중에 [오름쪽 마우스]로 공중 추가 공격을 한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 6;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"추가 공격 확장 2";
	tBrainInfo.szBrainEx[0] = L"염력 추가 공격과 러시 추가 공격을";
	tBrainInfo.szBrainEx[1] = L"4회까지 연속해서 할 수 있다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"공중 스텝";
	tBrainInfo.szBrainEx[0] = L"공중 [위]로 공중 스텝을 할 수 있다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"공중 대시 공격";
	tBrainInfo.szBrainEx[0] = L"『공중 스텝』 중 [왼쪽 마우스]로 전진하면서 무기 공격을 한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 보조
	tBrainInfo.iBP = 1;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"플러그인 장비 슬롯 추가 1";
	tBrainInfo.szBrainEx[0] = L"장비 화면에서 적용할 수 있는 플러그인이 하나 증가한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = GREEN;
	tBrainInfo.szBrainName = L"아이템 사용 제한 단축";
	tBrainInfo.szBrainEx[0] = L"아이템 사용 후의 사용 제한 시간을 단축시킨다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"브레인 크러시 SAS 회복";
	tBrainInfo.szBrainEx[0] = L"SAS 회과 중에 브레인 크러시로 적을 쓰러뜨리면";
	tBrainInfo.szBrainEx[1] = L"사용 중인 SAS의 SAS 게이지가 조금 회복한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"브레인 크러시 획득 금액 보너스";
	tBrainInfo.szBrainEx[0] = L"브레인 크러시로 적을 쓰러뜨리면";
	tBrainInfo.szBrainEx[1] = L"적을 쓰러뜨렸을 때 입수할 수 있는 돈이 증가한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"브레인 크러시 획득 EXP 보너스";
	tBrainInfo.szBrainEx[0] = L"브레인 크러시로 적을 쓰러뜨리면 획득 EXP가 증가한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"플러그인 장비 슬롯 추가 2";
	tBrainInfo.szBrainEx[0] = L"장비 화면에서 적용할 수 있는 플러그인이 하나 증가한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"SAS 동시 발동";
	tBrainInfo.szBrainEx[0] = L"같은 이름의 스킬이 2개 있으며,";
	tBrainInfo.szBrainEx[1] = L"하나를 습득하면, SAS를 동시에 2개까지 발동할 수 있으며,";
	tBrainInfo.szBrainEx[2] = L"2개를 습득하면, SAS를 동시에 4개까지 발동할 수 있다.";
	tBrainInfo.szBrainEx[3] = L"※SAS 발화와 SAS 방전은 동시에 발동할 수 없다.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"오버 킬";
	tBrainInfo.szBrainEx[0] = L"크러시 게이지가 다 깍여 활동을 정지한";
	tBrainInfo.szBrainEx[1] = L"적에게 공격을 명중시키면 EXP 보너스가 증가한다.";
	tBrainInfo.szBrainEx[2] = L"단, 브레인 크러시로밖에 쓰러뜨릴 수 없는 강적의 경우,";
	tBrainInfo.szBrainEx[3] = L"체력을 다 깍은 상황에서는 효과가 없다.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"브레인 크러시 찬스 연장";
	tBrainInfo.szBrainEx[0] = L"적의 크러시 게이지를 다 깎은 후";
	tBrainInfo.szBrainEx[1] = L"적이 더 오랜 시간 활동을 정지하게 되어";
	tBrainInfo.szBrainEx[2] = L"브레인 크러시 가능한 시간이 연장한다.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"SAS 동시 발동";
	tBrainInfo.szBrainEx[0] = L"같은 이름의 스킬이 2개 있으며,";
	tBrainInfo.szBrainEx[1] = L"하나를 습득하면, SAS를 동시에 2개까지 발동할 수 있으며,";
	tBrainInfo.szBrainEx[2] = L"2개를 습득하면, SAS를 동시에 4개까지 발동할 수 있다.";
	tBrainInfo.szBrainEx[3] = L"※SAS 발화와 SAS 방전은 동시에 발동할 수 없다.";
	m_vecBrain.push_back(tBrainInfo);
	// 잠시 다른 곳

	// 브레인 필드
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = BRAINFIELD;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"브레인 필드: 시간 연장 1";
	tBrainInfo.szBrainEx[0] = L"브레인 필드의 전개 시간을 5초 연장한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = BRAINFIELD;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"브레인 필드: 차지 투하 레벨 상승";
	tBrainInfo.szBrainEx[0] = L"자신이 브레인 필드를 전대하고 있을 때";
	tBrainInfo.szBrainEx[1] = L"[G] 길게 누르기로 모이는 오브젝트가 증가하여";
	tBrainInfo.szBrainEx[2] = L"오브젝트를 낙하시켜서 주는 대미지가 증가한다.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 보조 _2
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = GREEN;
	tBrainInfo.szBrainName = L"아이템 즉시 효과";
	tBrainInfo.szBrainEx[0] = L"아이템 사용 시의 동작을 생략해";
	tBrainInfo.szBrainEx[1] = L"즉시 효과를 얻을 수 있다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 6;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"체력 자동 회복";
	tBrainInfo.szBrainEx[0] = L"비전투 시 체력이 서서히 회복된다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 다시 브레인 필드
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"브레인 필드: 대미지 경감";
	tBrainInfo.szBrainEx[0] = L"자신이 브레인 필드를 전ㅐ하고 있을 때 받는 데미지가";
	tBrainInfo.szBrainEx[1] = L"경감된다. 같은 이름의 스킬이 2개 있으며";
	tBrainInfo.szBrainEx[2] = L"1개 습득하면 대미지가 반감,";
	tBrainInfo.szBrainEx[3] = L"2개 습득하면 대미자가 무효화된다.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"브레인 필드: 오버 킬";
	tBrainInfo.szBrainEx[0] = L"브레인 필드 전개 시, 크러시 게이지가 다 깎인";
	tBrainInfo.szBrainEx[1] = L"적에게 공격을 명중시키면 EXP 보너스가 증가한다.";
	tBrainInfo.szBrainEx[2] = L"단' 브레인 크러시로밖에 쓰러뜨릴 수 없는 강적의 경우";
	tBrainInfo.szBrainEx[3] = L"체력을 다 깎은 상황에서는 효과가 없다.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"브레인 필드: 타격 레벨 상승";
	tBrainInfo.szBrainEx[0] = L"자신의 브레인 필드를 전개하고 있을 때";
	tBrainInfo.szBrainEx[1] = L"[왼쪽 마우스]로 휘두르는 오브젝트가 증가하여";
	tBrainInfo.szBrainEx[2] = L"대미지를 줄 수 있는 범위가 넓어진다.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"브레인 필드: 드라이브 중 스톡 소비 없음";
	tBrainInfo.szBrainEx[0] = L"브레인 필드의 준비 아이콘이 표시된 상태에서";
	tBrainInfo.szBrainEx[1] = L"드라이브를 발동했을 경우, 드라이브 중 스톡을";
	tBrainInfo.szBrainEx[2] = L"소비하지 않고 브레인 필드를 전개할 수 있다.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"브레인 필드: 대미지 경감";
	tBrainInfo.szBrainEx[0] = L"자신이 브레인 필드를 전ㅐ하고 있을 때 받는 데미지가";
	tBrainInfo.szBrainEx[1] = L"경감된다. 같은 이름의 스킬이 2개 있으며";
	tBrainInfo.szBrainEx[2] = L"1개 습득하면 대미지가 반감,";
	tBrainInfo.szBrainEx[3] = L"2개 습득하면 대미자가 무효화된다.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = BRAINFIELD;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"브레인 필드: 발사 레벨 상승 1";
	tBrainInfo.szBrainEx[0] = L"자신이 브레인 필드를 전개하고 일을 때,";
	tBrainInfo.szBrainEx[1] = L"[왼쪽 마우스 2] [오른쪽 마우스] 나 ";
	tBrainInfo.szBrainEx[2] = L"[왼쪽 마우스 3] [오른쪽 마우스]의 [오른쪽 마우스]로";
	tBrainInfo.szBrainEx[3] = L"날리는 오브젝트가 증가한다.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"브레인 필드: 발사 레벨 상승 2";
	tBrainInfo.szBrainEx[0] = L"자신이 브레인 필드를 전개하고 있을 때";
	tBrainInfo.szBrainEx[1] = L"[왼쪽 마우스 3] [오른쪽 마우스]의";
	tBrainInfo.szBrainEx[2] = L"[오른쪽 마우스]로 날리는 오브젝트의 수가 증가한다.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"브레인 필드: 시간 연장 2";
	tBrainInfo.szBrainEx[0] = L"브레인 필드 전개 시간을 5초 연장한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);
}

void CItem_Manager::Free()
{
	m_vecItem.clear();
	m_vecBrain.clear();

}
