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

	tItemInfo.szItemEx[0] = L"ü���� �ִ�ġ 30%��ŭ ȸ���Ѵ�.";
	tItemInfo.szItemEx[1] = L"�ణ ������ �ָ��� ���� ������ ����.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"ȸ��(��) ����", tItemInfo);

	tItemInfo.szItemEx[0] = L"ü���� �ִ�ġ 60%��ŭ ȸ���Ѵ�.";
	tItemInfo.szItemEx[1] = L"������ ������ ������ ���� ������ ����.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"ȸ��(��) ����", tItemInfo);

	tItemInfo.szItemEx[0] = L"ü���� ������ ȸ���Ѵ�.";
	tItemInfo.szItemEx[1] = L"��û ������ ����� ���� ������ ����.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"ȸ��(��) ����", tItemInfo);

	tItemInfo.szItemEx[0] = L"�Ʊ� ��ü�� ü���� �ִ�ġ 30%��ŭ ȸ���Ѵ�.";
	tItemInfo.szItemEx[1] = L"�� ���� ���� �Դ� �йи� ��..";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"��ü: ȸ��(��) ����", tItemInfo);

	tItemInfo.szItemEx[0] = L"�Ʊ� ��ü�� ü���� �ִ�ġ 60%��ŭ ȸ���Ѵ�.";
	tItemInfo.szItemEx[1] = L"�� ���� ���� �Դ� �йи� ��.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"��ü: ȸ��(��) ����", tItemInfo);

	tItemInfo.szItemEx[0] = L"�Ʊ� ��ü�� ü���� ������ ȸ���Ѵ�.";
	tItemInfo.szItemEx[1] = L"�� ���� ���� �Դ� �йи� ��.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"��ü: ȸ��(��) ����", tItemInfo);

	tItemInfo.szItemEx[0] = L"SAS �������� �ִ���� ȸ���Ѵ�.";
	tItemInfo.szItemEx[1] = L"�Ӹ��� ���������� �ÿ��� ����.";
	tItemInfo.szItemEx[2] = L" ";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"SAS ���޼�", tItemInfo);

	////////////////////////////////////////////////////////////////////////////////
	tItemInfo.eType = WEAPON;
	tItemInfo.iMaxCount = 1;
	tItemInfo.iWeaponAttack = 27;

	tItemInfo.szItemEx[0] = L"�����䰡 �Ʒû� �������� �ֿ��ϴ� ��.";
	tItemInfo.szItemEx[1] = L"Į���� ��� ������, �ʱ޿��� �߱� ������ ��������ε�";
	tItemInfo.szItemEx[2] = L"�ٷ�� ����. ��� ����� �̸��� �پ� �ִ�.";
	tItemInfo.szItemEx[3] = L" ";
	m_vecItem.emplace_back(L"��ȣ ���󸶻�", tItemInfo);

	////////////////////////////////////////////////////////////////////////////////
	tItemInfo.eType = ETC;
	tItemInfo.iMaxCount = 1;
	tItemInfo.iWeaponAttack = 0;

	tItemInfo.szItemEx[0] = L"�� ������������� �ֿ� �ؽ�Ʈ ������.";
	tItemInfo.szItemEx[1] = L"�������� �������� �޸� ����. ������ �������� ����";
	tItemInfo.szItemEx[2] = L"������ �����ϸ鼭 ȸ������ �ʰ� �״�� ������ ���.";
	tItemInfo.szItemEx[3] = L" "; // ���빰�� �극�� �޽��� ȭ�鿡�� Ȯ���� �� �ִ�.
	m_vecItem.emplace_back(L"���������� �ؽ�Ʈ ������", tItemInfo);


	return S_OK;
}

void CItem_Manager::Free()
{


}
