#pragma once

#include "Client_Defines.h"
#include "Base.h"

//#define STORAGE 17

class CItem_Manager final : public CBase
{
	DECLARE_SINGLETON(CItem_Manager)

public:
	enum MAINITEM { ALL, BATTLE, WEAPON, ETC, MAINITEM_END };

	typedef struct tagItemInfo
	{
		MAINITEM	eType = { MAINITEM_END };
		_bool		bNew = { false };
		_bool		bPick = { false };
		_uint		iCount = { 0 };
		_uint		iMaxCount = { 0 };
		_uint		iWeaponAttack = { 0 };
		_float2		vIconIndex = { 0.0f, 0.0f };
		_tchar*		szItemEx[4] = {};

	}ITEMINFO;

public:
	CItem_Manager();
	virtual ~CItem_Manager() = default;

public:
	HRESULT	Initialize();
	
public:
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
	MAINITEM	m_eMainItem = { MAINITEM_END };

	vector<pair<wstring, ITEMINFO>>	m_vecItem;
	array<_float2, 17>	m_arrItemIndexPos;
	array<_float2, 17>	m_arrIconIndexPos;

public:
	virtual void Free() override;
};
