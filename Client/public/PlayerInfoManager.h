#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "Transform.h"

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
	_uint m_iHP = { 0 };
	_uint m_iMaxHP = { 0 };
	_uint m_iKineticEnergy = { 0 };
	_uint m_iMaxKineticEnergy = { 0 };
	_uint m_iKineticEnergyLevel = { 0 };   // ���� �������� �� ä�� �� �ִ� �������� 3�ܰ谡 �����մϴ�. (0~2)
	_uint m_iKineticEnergyType = { 0 };    // ���(2), ����(0), ����̺�(1) ���¿� ���� ���� �������� �̹����� ���� �˴ϴ�. (0~2)
	_uint iExp = { 0 };
	_uint iMaxExp = { 0 };
	_uint iLevel = { 0 };
	_uint iSprbrPower = { 0 };
	_uint iDefense = { 0 };
	_uint iBP = { 0 };
	_uint iCoin = { 0 };


	_bool bAttackEnable = false;

	
	_float fDriveEnergy = { 0 };
	_float fMaxDriveEnergy = { 0 };

	_float fBrainFieldMaintain = { 0 };
	_float fMaxBrainFieldMaintain = { 0 };

	_uint iWeaponType = { 0 };			// 0�� wp0106, 1�� wp0190

	_float fKineticCharge = { 0.f };	// ����ü ���� ����� (0~1)
	_float fSpecialCharge = { 0.f };

	_bool bBattle = false;
	_bool bDriveMode = false;
	_bool bBrainField = false;
	_bool bStartBrainField = false;

	_bool bCopy = false;

	_bool bAir = false;

	_float m_fBaseAttackDamage;

	ESASType m_eAttack_SAS_Type;

	array<SAS_GAGE, SAS_CNT> Sasese{};

}	PLAYER_STAT;

typedef struct tagHanabiStatus
{
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
enum SASMEET { HANABI, TSUGUMI, GEMMA, LUCA, SEEDEN, ARASHI, KYOTO, SASMEMBER_END };

class CPlayerInfoManager final : public CBase
{
	DECLARE_SINGLETON(CPlayerInfoManager)

private:
	CPlayerInfoManager();
	virtual ~CPlayerInfoManager() = default;

public:
	HRESULT	Initialize();
	void	Tick(_double TimeDelta);	// �ǽð����� Ÿ�� ������ �����ϱ� ����

public:	// Get
	PLAYER_STAT&	Get_PlayerStat() { 
		return m_tPlayerStat;
	}
	list<ESASType>		Get_PlayerSasList() const { return m_PlayerSasTypeList; }
	_bool					Get_isSasUsing(ESASType eType);
	CGameObject*	Get_KineticObject();
	CGameObject*	Get_TargetedMonster();
	CGameObject*	Get_SpecialObject();

	// SAS
	HANABI_STAT&		Get_HanabiStat() { 
		return m_tHanabiStat;
	}
	TSUGUMI_STAT&	Get_TsugumiStat() { 
		return m_tTsugumiStat;
	}
	_bool					Get_SASMember(const SASMEET eSAS) { 
		return m_bSASMember[eSAS];
	}

public:	// Set
	void			Set_PlayerHP(_uint iHP) { m_tPlayerStat.m_iHP = iHP; }
	void			Change_PlayerHP(CHANGETYPE eType, _uint ChangeHP);

	void			Set_PlayerKineticEnergy(_uint iEnergy) { m_tPlayerStat.m_iKineticEnergy = iEnergy; }
	void			Change_PlayerKineticEnergy(CHANGETYPE eType, _uint ChangeEnergy);

	void			Set_KineticEnergyLevel(_uint iType) { m_tPlayerStat.m_iKineticEnergyLevel = iType; }
	void			Set_KineticEnetgyType(_uint iType) { m_tPlayerStat.m_iKineticEnergyType = iType; }

	void			Set_DriveEnergy(_float fDrive) { m_tPlayerStat.fDriveEnergy = fDrive; }
	void			Change_DriveEnergy(CHANGETYPE eType, _float ChangeDrive);

	void			Set_BrainFieldMaintain(_float fBrain) { m_tPlayerStat.fBrainFieldMaintain = fBrain; }
	void			Change_BrainFieldMaintain(CHANGETYPE eType, _float ChangeBrain);

	void			Set_SasType(ESASType eType);
	void			Finish_SasType(ESASType eType);

	void			Change_SasEnergy(CHANGETYPE eChangeType, ESASType eSasType, _float iChangeEnergy);
	void			Set_SasEnergy(ESASType eSasType, _float iSetEnergy);
	void			Release_SasEnergy_All();

	void			Set_BattleState(_bool bBattle) { m_tPlayerStat.bBattle = bBattle; }

	void			Set_PlayerWorldMatrix(_fmatrix worldmatrix);
	_matrix			Get_PlayerWorldMatrix();

	void			Set_KineticCharge(_float fCharge) { m_tPlayerStat.fKineticCharge = fCharge; }
	_float			Get_KineticCharge() { return m_tPlayerStat.fKineticCharge; }

	void			Set_SpecialCharge(_float fCharge) { m_tPlayerStat.fSpecialCharge = fCharge; }
	_float			Get_SpecialCharge() { return m_tPlayerStat.fSpecialCharge; }

	void			Set_PlayerWeapon(_uint iWeaponType);
	_uint			Get_PlayerWeapon() { return m_tPlayerStat.iWeaponType; }

	void			Set_DriveMode(_bool bDrive) { m_tPlayerStat.bDriveMode = bDrive; }
	void			Set_BrainField(_bool bBrain) { m_tPlayerStat.bBrainField = bBrain; }

	void			Set_Copy(_bool bCopy) { m_tPlayerStat.bCopy = bCopy; }
	_bool			Get_Copy() { return m_tPlayerStat.bCopy; }

	void			Set_Air(_bool bAir) { m_tPlayerStat.bAir = bAir; }
	_bool			Get_Air() { return m_tPlayerStat.bAir; }

	HRESULT	Set_KineticObject(CGameObject* pKineticObject);
	HRESULT	Set_TargetedMonster(CGameObject* pTargetedMonster);
	HRESULT	Set_SpecialObject(CGameObject* pSpecialObject);
	void			Set_PlayerAttackEnable(_bool bEnable) { m_tPlayerStat.bAttackEnable = bEnable; }
	_bool			Get_PlayerAttackEnable() { return m_tPlayerStat.bAttackEnable; }

	// HRESULT			Set_KineticObject(CGameObject* pKineticObject);
	// HRESULT			Set_TargetedMonster(CGameObject* pTargetedMonster);
	// HRESULT			Set_SpecialObject(CGameObject* pSpecialObject);

	void			Set_BP(const _uint iBP) { m_tPlayerStat.iBP = iBP;	}

	void			Set_Exp(const _uint iExp);
	void			Set_StartBrainField();
	
	// SAS
	void			Set_SASMember(const SASMEET eSAS) { 
		m_bSASMember[eSAS] = true; 
	}

public:
	HRESULT			Set_CamSpot(CGameObject* pCamSpot);
	void			Camera_Random_Shake(_float fForce);
	void			Camera_Random_Shake_Maintain(_float fForce, _float fMaintain);
	void			Camera_Axis_Shaking(_float4 vDir, _float fShakePower);
	void			Camera_Axis_Sliding(_float4 vDir, _float fShakePower);

private:	// ���� ���� ����
	PLAYER_STAT		m_tPlayerStat;
	list<ESASType>		m_PlayerSasTypeList;

	HANABI_STAT		m_tHanabiStat;
	TSUGUMI_STAT	m_tTsugumiStat;

private:	// ��ȣ�ۿ� ����
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
	_bool	m_bSASMember[SASMEET::SASMEMBER_END] = { false, false, false, false, false, false, false };

private:	// ��� ���� �Լ�
	void			SAS_Checker();

private:	// ī�޶� ����ũ ����
	_float	m_fRandomShakeMaintain = 0.f;
	_float	m_fRandomShakeForce = 0.f;

public:
	virtual void Free() override;
};

END