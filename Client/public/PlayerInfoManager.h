#pragma once

#include "Base.h"
#include "Client_Defines.h"

//�÷��̾ ������ ������ �̱������� ����

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
	_uint m_iKineticEnergyLevel = 0;   // ���� �������� �� ä�� �� �ִ� �������� 3�ܰ谡 �����մϴ�. (0~2)
	_uint m_iKineticEnergyType = 0;    // ���, ����, ����̺� ���¿� ���� ���� �������� �̹����� ���� �˴ϴ�. (0~2)

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
	void	Tick();	// �ǽð����� Ÿ�� ������ �����ϱ� ����

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


private:	// ���� ���� ����
	PLAYER_STAT		m_tPlayerStat;
	ESASType		m_ePlayerSasType;

	HANABI_STAT		m_tHanabiStat;
	TSUGUMI_STAT	m_tTsugumiStat;

private:	// ��ȣ�ۿ� ����
	CGameObject*	m_pKineticObject;
	CGameObject*	m_pTargetedMonster;


public:
	virtual void Free() override;
};

END