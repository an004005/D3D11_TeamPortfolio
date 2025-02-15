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
		_uint		iPrice = { 0 };
		_uint		iSellingPrice = { 0 };
		_uint		iCount = { 0 };
		_uint		iMaxCount = { 0 };
		_bool		bWeapon = { 0 };
		_uint		iWeaponNum = { 0 };		// [0] Weapon 이 아닌 [1] 현재 사용 중인 무기 [2] 변경 하고자 하는 무기 [3] 그 외 무기
		_uint		iWeaponAttack = { 0 };
		_float2		vIconIndex = { 0.0f, 0.0f };
		_tchar*	szItemEx[4] = {};

	}ITEMINFO;

	typedef struct tagBattleItem
	{
		wstring	wsName = L"한글";
		_uint		iCount = { 0 };
		_float2		vIconIndex = { 0.0f, 0.0f };

	}BATTLEITEM;

public:
	CItem_Manager();
	virtual ~CItem_Manager() = default;

public:
	HRESULT	Initialize();
	
public:
	// FrontUI에 연동하기 위해
	vector<BATTLEITEM> 	Get_BattleItem() {
		return m_vecBattleItem;
	}

	// 아이템의 정보를 가져오기 위해
	vector<pair<wstring, ITEMINFO>>	Get_ItmeInfo() {
		return m_vecItem;
	}
	void	Set_ItemInfo(const size_t iIndex, const ITEMINFO & tItemInfo) {
		m_vecItem[iIndex].second = tItemInfo;
	}
	void	Set_ItemCount(const wstring & wszName, const _int iCount);
	void	Set_Weapon(const wstring& wszName, const _bool bCurrent);

	// 위치를 가져오기 위해
	_float2	Get_ItemIndexPos(const size_t iIndex) {
		return m_arrItemIndexPos[iIndex];
	}
	_float2	Get_IconIndexPos(const size_t iIndex) {
		return m_arrIconIndexPos[iIndex];
	}
	_float2	Get_ShopListPos(const size_t iIndex) {
		return m_arrShopListPos[iIndex];
	}
	_float2	Get_ShopIconPos(const size_t iIndex) {
		return m_arrShopIconPos[iIndex];
	}

	// 이전에 브레인 맵을 찍어 뒀는지
	_bool	Get_Brain(const size_t iIndex) {
		return m_arrBrain[iIndex];
	}
	void		Set_Brain(const size_t iIndex) {
		m_arrBrain[iIndex] = true;
	}

	void Save(const string& path);
	void Load(const string& path);


private:
	void	ItemListPos_Initialize();
	void	ItemInfo_Initialize();
	void	ShopListPos_Initialize();

private:
	MAINITEM	m_eMainItem = { MAINITEM::MAINITEM_END };

	array<_float2, 14>	m_arrItemIndexPos;
	array<_float2, 14>	m_arrIconIndexPos;
	vector<pair<wstring, ITEMINFO>>	m_vecItem;
	vector<BATTLEITEM> m_vecBattleItem;	// Front UI에 띄울 용도

	array<_float2, 10>	m_arrShopListPos;
	array<_float2, 10>	m_arrShopIconPos;

	array<_bool, 61 >		m_arrBrain;

public:
	virtual void Free() override;
};


