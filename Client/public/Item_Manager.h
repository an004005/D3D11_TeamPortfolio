#pragma once

#include "Client_Defines.h"
#include "Base.h"

//#define STORAGE 17

class CItem_Manager final : public CBase
{
	DECLARE_SINGLETON(CItem_Manager)

public:
	enum class MAINITEM { ALL, BATTLE, WEAPON, ETC, MAINITEM_END };

	typedef struct tagItemInfo
	{
		MAINITEM	eType = { MAINITEM::MAINITEM_END };
		_bool		bNew = { false };
		_bool		bPick = { false };
		_uint		iCount = { 0 };
		_uint		iMaxCount = { 0 };
		_uint		iWeaponNum = { 0 };		// [0] Weapon 이 아닌 [1] 현재 사용 중인 무기 [2] 변경 하고자 하는 무기 [3] 그 외 무기
		_uint		iWeaponAttack = { 0 };
		_float2		vIconIndex = { 0.0f, 0.0f };
		_tchar*	szItemEx[4] = {};

	}ITEMINFO;

	typedef struct tagBattleItem
	{
		wstring	wsName = L"아이템 초기화\n";
		_uint		iCount = { 0 };
		_float2		vIconIndex = { 0.0f, 0.0f };

	}BATTLEITEM;

public:
	CItem_Manager();
	virtual ~CItem_Manager() = default;

public:
	HRESULT	Initialize();
	
public:
	vector<BATTLEITEM> 	Get_BattleItem() {
		return m_vecBattleItem;
	}

	vector<pair<wstring, ITEMINFO>>	Get_ItmeInfo() {
		return m_vecItem;
	}
	void	Set_ItemInfo(const size_t iIndex, const ITEMINFO & tItemInfo) {
		m_vecItem[iIndex].second = tItemInfo;
	}
	void	Set_ItemCount(const wstring & wszName, const _uint iCount);


	_float2	Get_ItemIndexPos(const size_t iIndex) {
		return m_arrItemIndexPos[iIndex];
	}
	_float2	Get_IconIndexPos(const size_t iIndex) {
		return m_arrIconIndexPos[iIndex];
	}

private:
	void	ItemPosition_Intiialize();
	void	ItemInfo_Intiialize();

private:
	MAINITEM	m_eMainItem = { MAINITEM::MAINITEM_END };

	array<_float2, 17>	m_arrItemIndexPos;
	array<_float2, 17>	m_arrIconIndexPos;
	vector<pair<wstring, ITEMINFO>>	m_vecItem;
	vector<BATTLEITEM> m_vecBattleItem;	// Front UI에 띄울 용도

	//list< BATTLEITEM>	m_BarrleIemList;		// Front UI에 띄울 용도
	//array<BATTLEITEM, ITEMINDEX_END> m_arrBattleItem;	// Front UI에 띄울 용도
	//enum ITEMINDEX { ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, ITEMINDEX_END };

public:
	virtual void Free() override;
};
