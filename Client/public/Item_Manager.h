#pragma once

#include "Client_Defines.h"
#include "Base.h"

class CItem_Manager final : public CBase
{
	DECLARE_SINGLETON(CItem_Manager)

public:
	enum MAINITEM { ALL, BATTLE, WEAPON, ETC, MAINITEM_END };

	typedef struct tagItemInfo
	{
		MAINITEM	eType = { MAINITEM_END };
		_bool		bNew = { false };
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
	
public:	// Get
	vector<pair<wstring, ITEMINFO>>	Get_ItmeInfo() {
		return m_vecItem;
	}

public:	// Set
	void	Set_ItemInfo(const wstring & wszName, const ITEMINFO & tItemInfo) {
		m_vecItem.emplace_back(wszName, tItemInfo);
	}

private:
	MAINITEM	m_eMainItem = { MAINITEM_END };

	vector<pair<wstring, ITEMINFO>>	m_vecItem;

public:
	virtual void Free() override;
};
