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

		//���Ͱ� �ƴ� �迭�� �����ؾ� �ҵ�?
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
	tItemInfo.szItemEx[0] = L"ü���� �ִ�ġ 30%��ŭ ȸ���Ѵ�.";
	tItemInfo.szItemEx[1] = L"�ణ ������ �ָ��� ���� ������ ����.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"ȸ��(��) ����", tItemInfo);

	tItemInfo.vIconIndex = { 0.0f, 5.0f };
	tItemInfo.szItemEx[0] = L"ü���� �ִ�ġ 60%��ŭ ȸ���Ѵ�.";
	tItemInfo.szItemEx[1] = L"������ ������ ������ ���� ������ ����.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"ȸ��(��) ����", tItemInfo);

	tItemInfo.vIconIndex = { 3.0f, 0.0f };
	tItemInfo.szItemEx[0] = L"ü���� ������ ȸ���Ѵ�.";
	tItemInfo.szItemEx[1] = L"��û ������ ����� ���� ������ ����.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"ȸ��(��) ����", tItemInfo);

	tItemInfo.vIconIndex = { 2.0f, 0.0f };
	tItemInfo.szItemEx[0] = L"�Ʊ� ��ü�� ü���� �ִ�ġ 30%��ŭ ȸ���Ѵ�.";
	tItemInfo.szItemEx[1] = L"�� ���� ���� �Դ� �йи� ��..";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"��ü: ȸ��(��) ����", tItemInfo);

	tItemInfo.vIconIndex = { 4.0f, 0.0f };
	tItemInfo.szItemEx[0] = L"�Ʊ� ��ü�� ü���� �ִ�ġ 60%��ŭ ȸ���Ѵ�.";
	tItemInfo.szItemEx[1] = L"�� ���� ���� �Դ� �йи� ��.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"��ü: ȸ��(��) ����", tItemInfo);

	tItemInfo.vIconIndex = { 1.0f, 5.0f };
	tItemInfo.szItemEx[0] = L"�Ʊ� ��ü�� ü���� ������ ȸ���Ѵ�.";
	tItemInfo.szItemEx[1] = L"�� ���� ���� �Դ� �йи� ��.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"��ü: ȸ��(��) ����", tItemInfo);

	tItemInfo.vIconIndex = { 3.0f, 1.0f };
	tItemInfo.szItemEx[0] = L"SAS �������� �ִ���� ȸ���Ѵ�.";
	tItemInfo.szItemEx[1] = L"�Ӹ��� ���������� �ÿ��� ����.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"SAS ���޼�", tItemInfo);

	////////////////////////////////////////////////////////////////////////////////
	tItemInfo.eType = WEAPON;
	tItemInfo.iMaxCount = 1;
	tItemInfo.iWeaponAttack = 27;
	tItemInfo.vIconIndex = { 4.0f, 2.0f };

	tItemInfo.iWeaponNum = 1;
	tItemInfo.szItemEx[0] = L"�����䰡 �Ʒû� �������� �ֿ��ϴ� ��.";
	tItemInfo.szItemEx[1] = L"Į���� ��� ������, �ʱ޿��� �߱� ������ ��������ε�";
	tItemInfo.szItemEx[2] = L"�ٷ�� ����. ��� ����� �̸��� �پ� �ִ�.";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"��ȣ ���󸶻�", tItemInfo);

	tItemInfo.iWeaponNum = 2;
	tItemInfo.szItemEx[0] = L"���� �׽�Ʈ.";
	tItemInfo.szItemEx[1] = L"Į���� ��� ������, �ʱ޿��� �߱� ������ ��������ε�";
	tItemInfo.szItemEx[2] = L"�ٷ�� ����. ��� ����� �̸��� �پ� �ִ�.";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"���� �׽�Ʈ", tItemInfo);

	tItemInfo.iWeaponNum = 3;
	tItemInfo.szItemEx[0] = L"���� �׽�Ʈ2.";
	tItemInfo.szItemEx[1] = L"Į���� ��� ������, �ʱ޿��� �߱� ������ ��������ε�";
	tItemInfo.szItemEx[2] = L"�ٷ�� ����. ��� ����� �̸��� �پ� �ִ�.";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"���� �׽�Ʈ2", tItemInfo);

	////////////////////////////////////////////////////////////////////////////////
	tItemInfo.eType = ETC;
	tItemInfo.iMaxCount = 1;
	tItemInfo.iWeaponAttack = 0;

	tItemInfo.vIconIndex = { 1.0f, 2.0f };
	tItemInfo.szItemEx[0] = L"�� ������������� �ֿ� �ؽ�Ʈ ������.";
	tItemInfo.szItemEx[1] = L"�������� �������� �޸� ����. ������ �������� ����";
	tItemInfo.szItemEx[2] = L"������ �����ϸ鼭 ȸ������ �ʰ� �״�� ������ ���.";
	tItemInfo.szItemEx[3] = L" "; // ���빰�� �극�� �޽��� ȭ�鿡�� Ȯ���� �� �ִ�.
	m_vecItem.emplace_back(L"���������� �ؽ�Ʈ ������", tItemInfo);

	tItemInfo.szItemEx[0] = L"�� ������������� �ֿ� �ؽ�Ʈ ������.";
	tItemInfo.szItemEx[1] = L"�������� �������� �޸� ����. ������ �������� ����";
	tItemInfo.szItemEx[2] = L"������ �����ϸ鼭 ȸ������ �ʰ� �״�� ������ ���.";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"��Ÿ �׽�Ʈ", tItemInfo);
}

void CItem_Manager::Free()
{
	m_vecItem.clear();

}
