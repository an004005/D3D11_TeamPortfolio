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
	_uint m_iHP;
	_uint m_iMaxHP;
	_uint m_iKineticEnergy;
	_uint m_iMaxKineticEnergy;
	_uint m_iKineticEnergyLevel;   // ���� �������� �� ä�� �� �ִ� �������� 3�ܰ谡 �����մϴ�. (0~2)
	_uint m_iKineticEnergyType;    // ���, ����, ����̺� ���¿� ���� ���� �������� �̹����� ���� �˴ϴ�. (0~2)

	_float m_fBaseAttackDamage;

	ESASType m_eAttack_SAS_Type;

	array<SAS_GAGE, SAS_CNT> Sasese{};

}	PLAYER_STAT;

typedef struct tagDamageDesc
{
	_int		m_iDamage;
	_vector		m_vHitDir;
	EAttackType	m_iDamageType;
	EBaseAxis	m_eHitDir;

}	DAMAGE_DESC;

enum CHANGETYPE { CHANGE_INCREASE, CHANGE_DECREASE, CHANGE_END };

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
	PLAYER_STAT		Get_PlayerStat() const { return m_tPlayerStat; }
	list<ESASType>	Get_PlayerSasList() const { return m_PlayerSasTypeList; }
	_bool			Get_isSasUsing(ESASType eType);
	CGameObject*	Get_KineticObject();
	CGameObject*	Get_TargetedMonster();
	CGameObject*	Get_SpecialObject();


public:	// Set
	void			Set_PlayerHP(_uint iHP) { m_tPlayerStat.m_iHP = iHP; }
	void			Change_PlayerHP(CHANGETYPE eType, _uint ChangeHP);

	void			Set_PlayerKineticEnergy(_uint iEnergy) { m_tPlayerStat.m_iKineticEnergy = iEnergy; }
	void			Change_PlayerKineticEnergy(CHANGETYPE eType, _uint ChangeEnergy);

	void			Set_KineticEnergyLevel(_uint iType) { m_tPlayerStat.m_iKineticEnergyType = iType; }
	void			Set_KineticEnetgyType(_uint iType) { m_tPlayerStat.m_iKineticEnergyLevel = iType; }

	void			Set_SasType(ESASType eType);
	void			Finish_SasType(ESASType eType);

	void			Change_SasEnergy(CHANGETYPE eChangeType, ESASType eSasType, _float iChangeEnergy);

	void			Set_PlayerWorldMatrix(_fmatrix worldmatrix);

	HRESULT			Set_KineticObject(CGameObject* pKineticObject);
	HRESULT			Set_TargetedMonster(CGameObject* pTargetedMonster);
	HRESULT			Set_SpecialObject(CGameObject* pSpecialObject);


public:
	HRESULT			Set_CamSpot(CGameObject* pCamSpot);
	void			Camera_Random_Shake(_float fForce);
	void			Camera_Axis_Shaking(_float4 vDir, _float fShakePower);
	void			Camera_Axis_Sliding(_float4 vDir, _float fShakePower);


private:	// ���� ���� ����
	PLAYER_STAT		m_tPlayerStat;
	list<ESASType>	m_PlayerSasTypeList;

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

private:	// ��� ���� �Լ�
	void			SAS_Checker();

public:
	virtual void Free() override;
};

END