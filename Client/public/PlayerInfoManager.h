#pragma once

#include "Base.h"
#include "Client_Defines.h"

//플레이어가 가지는 정보를 싱글톤으로 관리

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CMapKinetic_Object;
class CMonster;

typedef struct tagSasGage
{
	_float Energy;
	_float MaxEnergy;
	_float MinEnergy;

	_float RecoveryRate;
	_float UseRate;

	_bool bUsable = false;

}	SAS_GAGE;

typedef struct tagPlayerStatus
{
	_uint m_iHP = 1000;
	_uint m_iMaxHP = 1000;
	_uint m_iKineticEnergy = 100;
	_uint m_iMaxKineticEnergy = 100;
	_uint m_iKineticEnergyLevel = 0;   // 염력 게이지를 다 채울 수 있는 게이지가 3단계가 존재합니다. (0~2)
	_uint m_iKineticEnergyType = 0;    // 평소, 공격, 드라이브 상태에 따라 염력 게이지의 이미지가 변경 됩니다. (0~2)

	array<SAS_GAGE, SAS_CNT> Sasese{};
	
	_uint iExp = { 0 };
	_uint iMaxExp = { 0 };
	_uint iLevel = { 0 };
	_uint iSprbrPower = { 0 };
	_uint iAttack = { 0 };
	_uint iDefense = { 0 };

}	PLAYER_STAT;

typedef struct tagHanabiStatus
{
	_bool m_bMember = { false };

	_uint iHP = { 0 };
	_uint iMaxHP = { 0 };
	_uint iExp = { 0 };
	_uint iMaxExp = { 0 };
	_uint iLevel = { 0 };
	_uint iBondLevel = { 0 };
	_uint iSprbrPower = { 0 };
	_uint iAttack = { 0 };
	_uint iDefense = { 0 };

}	HANABI_STAT;

typedef struct tagTsugumiStatus
{
	_bool m_bMember = { false };

	_uint iHP = { 0 };
	_uint iMaxHP = { 0 };
	_uint iExp = { 0 };
	_uint iMaxExp = { 0 };
	_uint iLevel = { 0 };
	_uint iBondLevel = { 0 };
	_uint iSprbrPower = { 0 };
	_uint iAttack = { 0 };
	_uint iDefense = { 0 };

}	TSUGUMI_STAT;

class CPlayerInfoManager final : public CBase
{
	DECLARE_SINGLETON(CPlayerInfoManager)

private:
	CPlayerInfoManager();
	virtual ~CPlayerInfoManager() = default;

public:
	HRESULT	Initialize();
	void	Tick();	// 실시간으로 타겟 정보를 갱신하기 위함

public:	// Get
	PLAYER_STAT		Get_PlayerStat() const { return m_tPlayerStat; }
	CGameObject*	Get_KineticObject();
	CGameObject*	Get_TargetedMonster();

	HANABI_STAT		Get_HanabiStat() const { return m_tHanabiStat; }
	TSUGUMI_STAT	Get_TsugumiStat() const { return m_tTsugumiStat; }

public:	// Set
	//void			Change_PlayerHP(_int)

	HRESULT			Set_KineticObject(CGameObject* pKineticObject);
	HRESULT			Set_TargetedMonster(CGameObject* pTargetedMonster);


private:	// 스탯 정보 관련
	PLAYER_STAT		m_tPlayerStat;
	ESASType		m_ePlayerSasType;

	HANABI_STAT		m_tHanabiStat;
	TSUGUMI_STAT	m_tTsugumiStat;

private:	// 상호작용 관련
	CGameObject*	m_pKineticObject;
	CGameObject*	m_pTargetedMonster;


public:
	virtual void Free() override;
};

END