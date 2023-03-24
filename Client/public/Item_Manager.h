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
		_uint		iWeaponNum = { 0 };		// [0] Weapont 이 아닌 [1] 현재 사용 중인 무기 [2] 변경 하고자 하는 무기 [3] 그 외 무기
		_uint		iWeaponAttack = { 0 };
		_float2		vIconIndex = { 0.0f, 0.0f };
		_tchar*		szItemEx[4] = {};

	}ITEMINFO;
	
	enum BRAINTYPE { DRIVE, ENHANCE, EXPANSION, ASSISTANCE, BRAINFIELD, BRAINTYPE_END };
	enum BRAINCOLOR { GREEN, YELLOW, RAD, PURPLE, BRAINCOLOR_END };

	typedef struct tagBrainInfo
	{
		BRAINTYPE	eType = { BRAINTYPE_END };
		BRAINCOLOR	eColor = { BRAINCOLOR_END };

		_uint		iBP = { 0 };
		_uint		iLevel = { 0 };
		_tchar*		szBrainName = {};
		_tchar*		szBrainEx[4] = {};

	}BRAININFO;

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

	vector<BRAININFO>	Get_BrainInfo() {
		return m_vecBrain;
	}

private:
	void	ItemIndex_Intiialize();
	void	ItemInfo_Intiialize();
	void	Brain_Intiialize();

private:
	MAINITEM	m_eMainItem = { MAINITEM_END };

	array<_float2, 17>	m_arrItemIndexPos;
	array<_float2, 17>	m_arrIconIndexPos;
	vector<pair<wstring, ITEMINFO>>	m_vecItem;
	
	vector<BRAININFO>	m_vecBrain;

public:
	virtual void Free() override;
};
