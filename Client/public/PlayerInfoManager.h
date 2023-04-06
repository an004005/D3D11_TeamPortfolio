#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "Transform.h"

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
	_uint m_iHP = { 0 };
	_uint m_iMaxHP = { 0 };
	_uint m_iKineticEnergy = { 0 };
	_uint m_iMaxKineticEnergy = { 0 };
	_uint m_iKineticEnergyLevel = { 0 };   // 염력 게이지를 다 채울 수 있는 게이지가 3단계가 존재합니다. (0~2)
	_uint m_iKineticEnergyType = { 0 };    // 평소(2), 공격(0), 드라이브(1) 상태에 따라 염력 게이지의 이미지가 변경 됩니다. (0~2)
	_uint iExp = { 0 };
	_uint iMaxExp = { 0 };
	_uint iLevel = { 0 };
	_uint iSprbrPower = { 0 };
	_uint iAttack = { 0 };
	_uint iDefense = { 0 };
	_uint iBP = { 0 };
	_uint iCoin = { 0 };

	_bool bBattle = false;

	_float m_fBaseAttackDamage;

	ESASType m_eAttack_SAS_Type;

	array<SAS_GAGE, SAS_CNT> Sasese{};

}	PLAYER_STAT;

typedef struct tagHanabiStatus
{
	_bool bMember = { false };

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
	_bool bMember = { false };

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

typedef struct tagDamageDesc
{
	_int		m_iDamage;
	_vector		m_vHitDir;
	EAttackType	m_iDamageType;
	EBaseAxis	m_eHitDir;

}	DAMAGE_DESC;

enum CHANGETYPE { CHANGE_INCREASE, CHANGE_DECREASE, CHANGE_END };
enum SASMEET { KYOTO, LUCA, SEEDEN, ARASHI, SASMEMBER_END };

class CPlayerInfoManager final : public CBase
{
	DECLARE_SINGLETON(CPlayerInfoManager)

private:
	CPlayerInfoManager();
	virtual ~CPlayerInfoManager() = default;

public:
	HRESULT	Initialize();
	void	Tick(_double TimeDelta);	// 실시간으로 타겟 정보를 갱신하기 위함

public:	// Get
	PLAYER_STAT&	Get_PlayerStat() { 
		return m_tPlayerStat;
	}
	list<ESASType>		Get_PlayerSasList() const { return m_PlayerSasTypeList; }
	_bool					Get_isSasUsing(ESASType eType);
	CGameObject*	Get_KineticObject();
	CGameObject*	Get_TargetedMonster();
	CGameObject*	Get_SpecialObject();

	HANABI_STAT		Get_HanabiStat() const { return m_tHanabiStat; }
	TSUGUMI_STAT	Get_TsugumiStat() const { return m_tTsugumiStat; }
	_bool					Get_SASMember(const SASMEET eSAS) { return m_bSASMember[eSAS]; }

public:	// Set
	void			Set_PlayerHP(_uint iHP) { m_tPlayerStat.m_iHP = iHP; }
	void			Change_PlayerHP(CHANGETYPE eType, _uint ChangeHP);

	void			Set_PlayerKineticEnergy(_uint iEnergy) { m_tPlayerStat.m_iKineticEnergy = iEnergy; }
	void			Change_PlayerKineticEnergy(CHANGETYPE eType, _uint ChangeEnergy);

	void			Set_KineticEnergyLevel(_uint iType) { m_tPlayerStat.m_iKineticEnergyLevel = iType; }
	void			Set_KineticEnetgyType(_uint iType) { m_tPlayerStat.m_iKineticEnergyType = iType; }

	void			Set_SasType(ESASType eType);
	void			Finish_SasType(ESASType eType);

	void			Change_SasEnergy(CHANGETYPE eChangeType, ESASType eSasType, _float iChangeEnergy);

	void			Set_BattleState(_bool bBattle) { m_tPlayerStat.bBattle = bBattle; }

	void			Set_PlayerWorldMatrix(_fmatrix worldmatrix);

	HRESULT	Set_KineticObject(CGameObject* pKineticObject);
	HRESULT	Set_TargetedMonster(CGameObject* pTargetedMonster);
	HRESULT	Set_SpecialObject(CGameObject* pSpecialObject);

	void			Set_BP(const _uint iBP) { m_tPlayerStat.iBP = iBP;	}
	
	// SAS
	void			Set_HanabiMember() { m_tHanabiStat.bMember = true; }
	void			Set_TsugumiMember() { m_tTsugumiStat.bMember = true; }
	void			Set_SASMember(const SASMEET eSAS) { m_bSASMember[eSAS] = true; }

public:
	HRESULT	Set_CamSpot(CGameObject* pCamSpot);
	void			Camera_Random_Shake(_float fForce);
	void			Camera_Axis_Shaking(_float4 vDir, _float fShakePower);
	void			Camera_Axis_Sliding(_float4 vDir, _float fShakePower);

private:	// 스탯 정보 관련
	PLAYER_STAT		m_tPlayerStat;
	list<ESASType>		m_PlayerSasTypeList;

	HANABI_STAT		m_tHanabiStat;
	TSUGUMI_STAT	m_tTsugumiStat;

private:	// 상호작용 관련
	CGameObject*	m_pKineticObject;
	CGameObject*	m_pTargetedMonster;
	CGameObject*	m_pSpecialObject;

private:
	CGameObject*	m_pCamSpot = nullptr;

private:
	_matrix			m_PlayerWorldMatrix = XMMatrixIdentity();

private:
	_float			m_fBaseAttackDamage;

private:
	_bool	m_bSASMember[SASMEET::SASMEMBER_END] = { false, false, false, false };

private:	// 기능 정리 함수
	void			SAS_Checker();

public:
	virtual void Free() override;
};

END