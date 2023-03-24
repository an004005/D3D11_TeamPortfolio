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

void CItem_Manager::Brain_Intiialize()
{
	BRAININFO	tBrainInfo;

	// ����̺�
	tBrainInfo.iBP = 1;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����̺� ������ ������ ���";
	tBrainInfo.szBrainEx[0] = L"����̺� ������ ������ +20%";
	tBrainInfo.szBrainEx[1] = L"���� �����߸��ų� ������ ������� �޴� ��";
	tBrainInfo.szBrainEx[2] = L"Ư�� �ൿ���� ���� �������� �����Ͽ�";
	tBrainInfo.szBrainEx[3] = L"����̺� �������� ���̱� ��������.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����̺�: ü�� �ڵ� ȸ��";
	tBrainInfo.szBrainEx[0] = L"����̺� �� ü���� ������ ȸ���ȴ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����̺�: �����̻� ȸ��";
	tBrainInfo.szBrainEx[0] = L"����̺� �ߵ��ϸ� �����̻��� ȸ���Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����̺�: ���� �Ʈ ����";
	tBrainInfo.szBrainEx[0] = L"����̺� �� ���Ʈ ������ ��";
	tBrainInfo.szBrainEx[1] = L"�������� �߻��ϰ� �ȴ�.";
	tBrainInfo.szBrainEx[2] = L"�ء��Ʈ �������� ���� ������ ����ϸ� �߻�";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����̺�: ����̺� ������ ȸ��1";
	tBrainInfo.szBrainEx[0] = L"����̺� �� �극��  ũ���÷� ���� �����߸���";
	tBrainInfo.szBrainEx[1] = L"����̺� �������� ���� ȸ���ȴ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����̺�: ���ʽ� ���� ������ ���";
	tBrainInfo.szBrainEx[0] = L"����̺� �� ȹ�� EXP�� ���ʽ� ������";
	tBrainInfo.szBrainEx[1] = L"�������� ����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"����̺�: ���� ũ���� ȿ�� ���";
	tBrainInfo.szBrainEx[0] = L"����̺� �� �������� ũ���� ��������";
	tBrainInfo.szBrainEx[1] = L"���� ���� �� �ִ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 6;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����̺�: ����̺� ������ ȸ�� 2";
	tBrainInfo.szBrainEx[0] = L"����̺� �� �극�� ũ���÷� ���� �����߸���";
	tBrainInfo.szBrainEx[1] = L"����̺� �������� ���� ȸ���ȴ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����̺�: �������ͽ� ���";
	tBrainInfo.szBrainEx[0] = L"����̺� �� �ʳ��ɷ�, ���ݷ�, ���� +10%";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����̺�: SAS ������ ȸ��";
	tBrainInfo.szBrainEx[0] = L"����̺긦 �ߵ��ϸ�";
	tBrainInfo.szBrainEx[1] = L"��� SAS �������� ��� ȸ���ȴ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// ��ȭ
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� ������ ȸ���� ���(���� ����)";
	tBrainInfo.szBrainEx[0] = L"���� �������� ���� ���� �������� ȸ������ �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"��[�����̽���]+[���� ���罺] �÷����� ������ ���Ե��� �ʴ´�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"���� ���� ũ���� ȿ�� ���2";
	tBrainInfo.szBrainEx[0] = L"���� �������� ũ���� �������� ���� ���� �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"ũ���� ȿ���� 5% �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� ������ �ִ�ġ ����";
	tBrainInfo.szBrainEx[0] = L"���� �̸��� ��ų�� 2�� ������, 1���� �����ϸ� 50%,";
	tBrainInfo.szBrainEx[1] = L"2���� �����ϸ� 100% ���� �������� �ִ�ġ�� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���ݷ� ��� 1";
	tBrainInfo.szBrainEx[0] = L"���ݷ� +5%";
	tBrainInfo.szBrainEx[1] = L"���� �������� �ִ� ������� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"�ļ�Ÿ";
	tBrainInfo.szBrainEx[0] = L"�ٿ� ���� ������ �ִ� ������� �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"�һ� �ӵ� ���";
	tBrainInfo.szBrainEx[0] = L"���Ḧ �һ��ϴ� �ӵ��� ����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� ���� ũ���� ȿ�� ��� 1";
	tBrainInfo.szBrainEx[0] = L"���� �������� ũ���� �������� ���� ���� �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"ũ���� ȿ���� 5% �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���ݷ� ��� 2";
	tBrainInfo.szBrainEx[0] = L"���ݷ� +5%";
	tBrainInfo.szBrainEx[1] = L"���� �������� �ִ� ������� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� ������ �ִ�ġ ����";
	tBrainInfo.szBrainEx[0] = L"���� �̸��� ��ų�� 2�� ������, 1���� �����ϸ� 50%,";
	tBrainInfo.szBrainEx[1] = L"2���� �����ϸ� 100% ���� �������� �ִ�ġ�� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"�ʳ��ɷ� ���";
	tBrainInfo.szBrainEx[0] = L"�ʳ��ɷ� +5%";
	tBrainInfo.szBrainEx[1] = L"���� �������� �ִ� ������� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"���� ���� ũ���� ȿ�� ���";
	tBrainInfo.szBrainEx[0] = L"���� �������� ũ���� �������� ���� ���� �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"ũ���� ȿ���� 5% �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// Ȯ��
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����ġ�� ����";
	tBrainInfo.szBrainEx[0] = L"ü�� �߿� [C] ��� �������";
	tBrainInfo.szBrainEx[1] = L"���߿��� �ϰ��ϸ� ���� ������ �Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� ����";
	tBrainInfo.szBrainEx[0] = L"[C] ��� ������� ���� ������, [C]�� ����";
	tBrainInfo.szBrainEx[1] = L"���� ������ ������ ���� ���� ������ �Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����";
	tBrainInfo.szBrainEx[0] = L"������ �� [��]�� �ڼ��� �ٷ� ��� �ٿ��� �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����Ʈ ȸ�� ���� ������ ȸ��";
	tBrainInfo.szBrainEx[0] = L"����Ʈ ȸ�� ���� �� ���� �������� ���� ȸ���Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� �޺� 4��";
	tBrainInfo.szBrainEx[0] = L"���� ������ 4ȸ���� �����ؼ� �� �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"2�� ���� ����";
	tBrainInfo.szBrainEx[0] = L"[C] ��� ������� ���� ���� ��, �� �ܰ� �� ���� ������";
	tBrainInfo.szBrainEx[1] = L"[C] �� ���� ������ ���ݡ� �� ������ ��������. ";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� ���� ��ȭ";
	tBrainInfo.szBrainEx[0] = L"������ ���ݡ� �� ��2�� ���� ���ݡ���";
	tBrainInfo.szBrainEx[1] = L"���� ������ �о����� ������ ��������.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"���� �ļ�Ÿ";
	tBrainInfo.szBrainEx[0] = L"�ٿ�� ������ [������ ���콺]�� ���� ������ �ϸ�";
	tBrainInfo.szBrainEx[1] = L"����ġ���� ���� ������ �Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 6;
	tBrainInfo.iLevel = 6;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� �޺� 5ź";
	tBrainInfo.szBrainEx[0] = L"���� ������ 5ȸ���� �����ؼ� �� �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = GREEN;
	tBrainInfo.szBrainName = L"������ �������";
	tBrainInfo.szBrainEx[0] = L"��ó�� �������ִ� ��� �������� �ڵ����� �������.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����Ʈ ȸ�� ����";
	tBrainInfo.szBrainEx[0] = L"����Ʈ ȸ�� ���� �� [���� ���콺]�� ���� ������ �Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� ����";
	tBrainInfo.szBrainEx[0] = L"���� [�����̽���]�� �ѹ� �� ������ �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"�߰� ���� Ȯ�� 1";
	tBrainInfo.szBrainEx[0] = L"���� �߰� ���ݰ� ���� �߰� ������";
	tBrainInfo.szBrainEx[1] = L"3ȸ���� �����ؼ� �� �� �ִ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"ȸ�� ���� ���� Ȯ��";
	tBrainInfo.szBrainEx[0] = L"[C]�� �̵��ϸ鼭 ���� ������ �۾���";
	tBrainInfo.szBrainEx[1] = L"��ȸ�� ���� ���ݡ���";
	tBrainInfo.szBrainEx[2] = L"�� �޺� �߿� 2������ �� �� �ִ�.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"���� �߰� ����";
	tBrainInfo.szBrainEx[0] = L"���� ���� �޺� �߿� [������ ���콺]�� ���� �߰� ������ �Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 6;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"�߰� ���� Ȯ�� 2";
	tBrainInfo.szBrainEx[0] = L"���� �߰� ���ݰ� ���� �߰� ������";
	tBrainInfo.szBrainEx[1] = L"4ȸ���� �����ؼ� �� �� �ִ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� ����";
	tBrainInfo.szBrainEx[0] = L"���� [��]�� ���� ������ �� �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� ��� ����";
	tBrainInfo.szBrainEx[0] = L"������ ���ܡ� �� [���� ���콺]�� �����ϸ鼭 ���� ������ �Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// ����
	tBrainInfo.iBP = 1;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"�÷����� ��� ���� �߰� 1";
	tBrainInfo.szBrainEx[0] = L"��� ȭ�鿡�� ������ �� �ִ� �÷������� �ϳ� �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = GREEN;
	tBrainInfo.szBrainName = L"������ ��� ���� ����";
	tBrainInfo.szBrainEx[0] = L"������ ��� ���� ��� ���� �ð��� �����Ų��.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"�극�� ũ���� SAS ȸ��";
	tBrainInfo.szBrainEx[0] = L"SAS ȸ�� �߿� �극�� ũ���÷� ���� �����߸���";
	tBrainInfo.szBrainEx[1] = L"��� ���� SAS�� SAS �������� ���� ȸ���Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"�극�� ũ���� ȹ�� �ݾ� ���ʽ�";
	tBrainInfo.szBrainEx[0] = L"�극�� ũ���÷� ���� �����߸���";
	tBrainInfo.szBrainEx[1] = L"���� �����߷��� �� �Լ��� �� �ִ� ���� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"�극�� ũ���� ȹ�� EXP ���ʽ�";
	tBrainInfo.szBrainEx[0] = L"�극�� ũ���÷� ���� �����߸��� ȹ�� EXP�� �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"�÷����� ��� ���� �߰� 2";
	tBrainInfo.szBrainEx[0] = L"��� ȭ�鿡�� ������ �� �ִ� �÷������� �ϳ� �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"SAS ���� �ߵ�";
	tBrainInfo.szBrainEx[0] = L"���� �̸��� ��ų�� 2�� ������,";
	tBrainInfo.szBrainEx[1] = L"�ϳ��� �����ϸ�, SAS�� ���ÿ� 2������ �ߵ��� �� ������,";
	tBrainInfo.szBrainEx[2] = L"2���� �����ϸ�, SAS�� ���ÿ� 4������ �ߵ��� �� �ִ�.";
	tBrainInfo.szBrainEx[3] = L"��SAS ��ȭ�� SAS ������ ���ÿ� �ߵ��� �� ����.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� ų";
	tBrainInfo.szBrainEx[0] = L"ũ���� �������� �� �￩ Ȱ���� ������";
	tBrainInfo.szBrainEx[1] = L"������ ������ ���߽�Ű�� EXP ���ʽ��� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"��, �극�� ũ���÷ιۿ� �����߸� �� ���� ������ ���,";
	tBrainInfo.szBrainEx[3] = L"ü���� �� ���� ��Ȳ������ ȿ���� ����.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"�극�� ũ���� ���� ����";
	tBrainInfo.szBrainEx[0] = L"���� ũ���� �������� �� ���� ��";
	tBrainInfo.szBrainEx[1] = L"���� �� ���� �ð� Ȱ���� �����ϰ� �Ǿ�";
	tBrainInfo.szBrainEx[2] = L"�극�� ũ���� ������ �ð��� �����Ѵ�.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"SAS ���� �ߵ�";
	tBrainInfo.szBrainEx[0] = L"���� �̸��� ��ų�� 2�� ������,";
	tBrainInfo.szBrainEx[1] = L"�ϳ��� �����ϸ�, SAS�� ���ÿ� 2������ �ߵ��� �� ������,";
	tBrainInfo.szBrainEx[2] = L"2���� �����ϸ�, SAS�� ���ÿ� 4������ �ߵ��� �� �ִ�.";
	tBrainInfo.szBrainEx[3] = L"��SAS ��ȭ�� SAS ������ ���ÿ� �ߵ��� �� ����.";
	m_vecBrain.push_back(tBrainInfo);
	// ��� �ٸ� ��

	// �극�� �ʵ�
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = BRAINFIELD;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"�극�� �ʵ�: �ð� ���� 1";
	tBrainInfo.szBrainEx[0] = L"�극�� �ʵ��� ���� �ð��� 5�� �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = BRAINFIELD;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"�극�� �ʵ�: ���� ���� ���� ���";
	tBrainInfo.szBrainEx[0] = L"�ڽ��� �극�� �ʵ带 �����ϰ� ���� ��";
	tBrainInfo.szBrainEx[1] = L"[G] ��� ������� ���̴� ������Ʈ�� �����Ͽ�";
	tBrainInfo.szBrainEx[2] = L"������Ʈ�� ���Ͻ��Ѽ� �ִ� ������� �����Ѵ�.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// ���� _2
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = GREEN;
	tBrainInfo.szBrainName = L"������ ��� ȿ��";
	tBrainInfo.szBrainEx[0] = L"������ ��� ���� ������ ������";
	tBrainInfo.szBrainEx[1] = L"��� ȿ���� ���� �� �ִ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 6;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"ü�� �ڵ� ȸ��";
	tBrainInfo.szBrainEx[0] = L"������ �� ü���� ������ ȸ���ȴ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// �ٽ� �극�� �ʵ�
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"�극�� �ʵ�: ����� �氨";
	tBrainInfo.szBrainEx[0] = L"�ڽ��� �극�� �ʵ带 �����ϰ� ���� �� �޴� ��������";
	tBrainInfo.szBrainEx[1] = L"�氨�ȴ�. ���� �̸��� ��ų�� 2�� ������";
	tBrainInfo.szBrainEx[2] = L"1�� �����ϸ� ������� �ݰ�,";
	tBrainInfo.szBrainEx[3] = L"2�� �����ϸ� ����ڰ� ��ȿȭ�ȴ�.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"�극�� �ʵ�: ���� ų";
	tBrainInfo.szBrainEx[0] = L"�극�� �ʵ� ���� ��, ũ���� �������� �� ����";
	tBrainInfo.szBrainEx[1] = L"������ ������ ���߽�Ű�� EXP ���ʽ��� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"��' �극�� ũ���÷ιۿ� �����߸� �� ���� ������ ���";
	tBrainInfo.szBrainEx[3] = L"ü���� �� ���� ��Ȳ������ ȿ���� ����.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"�극�� �ʵ�: Ÿ�� ���� ���";
	tBrainInfo.szBrainEx[0] = L"�ڽ��� �극�� �ʵ带 �����ϰ� ���� ��";
	tBrainInfo.szBrainEx[1] = L"[���� ���콺]�� �ֵθ��� ������Ʈ�� �����Ͽ�";
	tBrainInfo.szBrainEx[2] = L"������� �� �� �ִ� ������ �о�����.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"�극�� �ʵ�: ����̺� �� ���� �Һ� ����";
	tBrainInfo.szBrainEx[0] = L"�극�� �ʵ��� �غ� �������� ǥ�õ� ���¿���";
	tBrainInfo.szBrainEx[1] = L"����̺긦 �ߵ����� ���, ����̺� �� ������";
	tBrainInfo.szBrainEx[2] = L"�Һ����� �ʰ� �극�� �ʵ带 ������ �� �ִ�.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"�극�� �ʵ�: ����� �氨";
	tBrainInfo.szBrainEx[0] = L"�ڽ��� �극�� �ʵ带 �����ϰ� ���� �� �޴� ��������";
	tBrainInfo.szBrainEx[1] = L"�氨�ȴ�. ���� �̸��� ��ų�� 2�� ������";
	tBrainInfo.szBrainEx[2] = L"1�� �����ϸ� ������� �ݰ�,";
	tBrainInfo.szBrainEx[3] = L"2�� �����ϸ� ����ڰ� ��ȿȭ�ȴ�.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = BRAINFIELD;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"�극�� �ʵ�: �߻� ���� ��� 1";
	tBrainInfo.szBrainEx[0] = L"�ڽ��� �극�� �ʵ带 �����ϰ� ���� ��,";
	tBrainInfo.szBrainEx[1] = L"[���� ���콺 2] [������ ���콺] �� ";
	tBrainInfo.szBrainEx[2] = L"[���� ���콺 3] [������ ���콺]�� [������ ���콺]��";
	tBrainInfo.szBrainEx[3] = L"������ ������Ʈ�� �����Ѵ�.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"�극�� �ʵ�: �߻� ���� ��� 2";
	tBrainInfo.szBrainEx[0] = L"�ڽ��� �극�� �ʵ带 �����ϰ� ���� ��";
	tBrainInfo.szBrainEx[1] = L"[���� ���콺 3] [������ ���콺]��";
	tBrainInfo.szBrainEx[2] = L"[������ ���콺]�� ������ ������Ʈ�� ���� �����Ѵ�.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"�극�� �ʵ�: �ð� ���� 2";
	tBrainInfo.szBrainEx[0] = L"�극�� �ʵ� ���� �ð��� 5�� �����Ѵ�.";
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
