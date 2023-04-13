#include "stdafx.h"
#include "../public/PlayerInfoManager.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MapKinetic_Object.h"
#include "Monster.h"
#include "Enemy.h"
#include "CamSpot.h"
#include "JsonStorage.h"
#include "Canvas_Alarm.h"
#include "Camera.h"

IMPLEMENT_SINGLETON(CPlayerInfoManager)

CPlayerInfoManager::CPlayerInfoManager()
{
}

HRESULT CPlayerInfoManager::Initialize()
{
	ZeroMemory(&m_tPlayerStat, sizeof(PLAYER_STAT));

#pragma region 플레이어 기본 스탯 초기화

	m_tPlayerStat.m_iHP = 1000;
	m_tPlayerStat.m_iMaxHP = 1000;

	m_tPlayerStat.m_iKineticEnergy = 100;
	m_tPlayerStat.m_iMaxKineticEnergy = 100;

	m_tPlayerStat.m_iKineticEnergyLevel = 0;
	m_tPlayerStat.m_iKineticEnergyType = 2;

	m_tPlayerStat.m_fBaseAttackDamage = 100.f;

	m_tPlayerStat.fDriveEnergy = 0.f;
	m_tPlayerStat.fMaxDriveEnergy = 100.f;

	m_tPlayerStat.fBrainFieldMaintain = 0.f;
	m_tPlayerStat.fMaxBrainFieldMaintain = 60.0f;

	m_tPlayerStat.iExp = 50;
	m_tPlayerStat.iMaxExp = 100;
	m_tPlayerStat.iLevel = 1;
	m_tPlayerStat.iSprbrPower = 110;
	m_tPlayerStat.iDefense = 15;
	m_tPlayerStat.iBP = 50;
	m_tPlayerStat.iCoin = 5000;

	m_tPlayerStat.bCopy = false;

	m_tPlayerStat.bAir = false;
		
#pragma endregion 플레이어 기본 스탯 초기화

	ZeroMemory(&m_tHanabiStat, sizeof(HANABI_STAT));
	ZeroMemory(&m_tTsugumiStat, sizeof(TSUGUMI_STAT));

#pragma region	멤버 기본 스탯 초기화

	m_tHanabiStat.iHP = 1700;
	m_tHanabiStat.iMaxHP = 1700;
	m_tHanabiStat.iExp = 50;
	m_tHanabiStat.iMaxExp = 100;
	m_tHanabiStat.iLevel = 1;
	m_tHanabiStat.iBondLevel = 1;
	m_tHanabiStat.iSprbrPower = 276;
	m_tHanabiStat.iAttack = 50;
	m_tHanabiStat.iDefense = 20;

	m_tTsugumiStat.iHP = 1300;
	m_tTsugumiStat.iMaxHP = 1300;
	m_tTsugumiStat.iExp = 0;
	m_tTsugumiStat.iMaxExp = 100;
	m_tTsugumiStat.iLevel = 1;
	m_tTsugumiStat.iBondLevel = 1;
	m_tTsugumiStat.iSprbrPower = 325;
	m_tTsugumiStat.iAttack = 60;
	m_tTsugumiStat.iDefense = 10;

#pragma endregion	멤버 기본 스탯 초기화

#pragma region SAS정보 초기화

	m_tPlayerStat.m_eAttack_SAS_Type = ESASType::SAS_NOT;

	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_FIRE)].Energy = 60.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_FIRE)].MaxEnergy = 60.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_FIRE)].MinEnergy = 20.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_FIRE)].bUsable = true;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_FIRE)].RecoveryRate = 1.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_FIRE)].UseRate = 1.f;

	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_PENETRATE)].MaxEnergy = 60.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_PENETRATE)].MinEnergy = 20.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_PENETRATE)].bUsable = true;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_PENETRATE)].Energy = 60.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_PENETRATE)].RecoveryRate = 1.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_PENETRATE)].UseRate = 1.f;

	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_COPY)].MaxEnergy = 60.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_COPY)].MinEnergy = 20.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_COPY)].bUsable = true;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_COPY)].Energy = 60.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_COPY)].RecoveryRate = 1.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_COPY)].UseRate = 1.f;

	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_ELETRIC)].MaxEnergy = 60.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_ELETRIC)].MinEnergy = 20.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_ELETRIC)].bUsable = true;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_ELETRIC)].Energy = 60.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_ELETRIC)].RecoveryRate = 1.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_ELETRIC)].UseRate = 1.f;

	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_GRAVIKENISIS)].MaxEnergy = 60.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_GRAVIKENISIS)].MinEnergy = 20.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_GRAVIKENISIS)].bUsable = true;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_GRAVIKENISIS)].Energy = 60.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_GRAVIKENISIS)].RecoveryRate = 1.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_GRAVIKENISIS)].UseRate = 1.f;

	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_HARDBODY)].MaxEnergy = 60.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_HARDBODY)].MinEnergy = 20.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_HARDBODY)].bUsable = true;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_HARDBODY)].Energy = 60.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_HARDBODY)].RecoveryRate = 1.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_HARDBODY)].UseRate = 1.f;

	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_INVISIBLE)].MaxEnergy = 60.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_INVISIBLE)].MinEnergy = 20.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_INVISIBLE)].bUsable = true;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_INVISIBLE)].Energy = 60.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_INVISIBLE)].RecoveryRate = 1.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_INVISIBLE)].UseRate = 1.f;

	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_SUPERSPEED)].MaxEnergy = 60.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_SUPERSPEED)].MinEnergy = 20.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_SUPERSPEED)].bUsable = true;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_SUPERSPEED)].Energy = 60.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_SUPERSPEED)].RecoveryRate = 1.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_SUPERSPEED)].UseRate = 1.f;

	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_TELEPORT)].MaxEnergy = 60.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_TELEPORT)].MinEnergy = 20.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_TELEPORT)].bUsable = true;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_TELEPORT)].Energy = 60.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_TELEPORT)].RecoveryRate = 1.f;
	m_tPlayerStat.Sasese[static_cast<_uint>(ESASType::SAS_TELEPORT)].UseRate = 1.f;

#pragma endregion SAS정보 초기화

	return S_OK;
}

void CPlayerInfoManager::Tick(_double TimeDelta)
{
	// 사용이 불가능한 객체를 바로 null로 채워주기 위해 매 틱 작동한다.
	// 플레이어에서 값을 사용할 것이므로 플레이어의 가장 첫 틱에서 동작하게 하자

	if (nullptr == m_pKineticObject) m_pKineticObject = nullptr;
	else if (false == CGameInstance::GetInstance()->Check_ObjectAlive(m_pKineticObject)) m_pKineticObject = nullptr;
	else if (true == m_pKineticObject->IsDeleted()) m_pKineticObject = nullptr;
	else if (false == static_cast<CMapKinetic_Object*>(m_pKineticObject)->Usable()) m_pKineticObject = nullptr;
	else if (true == static_cast<CMapKinetic_Object*>(m_pKineticObject)->GetThrow()) m_pKineticObject = nullptr;

	if (nullptr == m_pTargetedMonster) m_pTargetedMonster = nullptr;
	else if (false == CGameInstance::GetInstance()->Check_ObjectAlive(m_pTargetedMonster)) m_pTargetedMonster = nullptr;
	else if (true == m_pTargetedMonster->IsDeleted()) m_pTargetedMonster = nullptr;
	//else if (true == static_cast<CMonster*>(m_pTargetedMonster)->IsDead()) m_pTargetedMonster = nullptr;
	else if (true == static_cast<CEnemy*>(m_pTargetedMonster)->IsDead()) m_pTargetedMonster = nullptr;

	if (nullptr == m_pSpecialObject) m_pSpecialObject = nullptr;
	else if (false == CGameInstance::GetInstance()->Check_ObjectAlive(m_pSpecialObject)) m_pSpecialObject = nullptr;
	else if (true == m_pSpecialObject->IsDeleted()) m_pSpecialObject = nullptr;

	SAS_Checker();

	if (0.f < m_fRandomShakeMaintain)
	{
		m_fRandomShakeMaintain -= (_float)TimeDelta;
		Camera_Random_Shake(m_fRandomShakeForce);
	}
	else
	{
		m_fRandomShakeMaintain = 0.f;
		m_fRandomShakeForce = 0.f;
	}
}

_bool CPlayerInfoManager::Get_isSasUsing(ESASType eType)
{
	for (auto& iter : m_PlayerSasTypeList)
	{
		if (iter == eType)
			return true;
	}
	return false;
}

CGameObject * CPlayerInfoManager::Get_KineticObject()
{
	if (nullptr == m_pKineticObject) return nullptr;	// null이면
	else if (false == CGameInstance::GetInstance()->Check_ObjectAlive(m_pKineticObject)) return nullptr;	// 유효하지 않은 주소이면
	else if (true == m_pKineticObject->IsDeleted()) return nullptr;	// 지워졌으면
	else if (false == static_cast<CMapKinetic_Object*>(m_pKineticObject)->Usable()) return nullptr;		// 이미 사용한 객체이면
	else if (true == static_cast<CMapKinetic_Object*>(m_pKineticObject)->GetThrow()) return nullptr;		// 던져진 객체이면

	return m_pKineticObject;
}

CGameObject * CPlayerInfoManager::Get_TargetedMonster()
{
	if (nullptr == m_pTargetedMonster) return nullptr;	// null이면
	else if (false == CGameInstance::GetInstance()->Check_ObjectAlive(m_pTargetedMonster)) return nullptr;	// 유효하지 않은 주소이면
	else if (true == m_pTargetedMonster->IsDeleted()) return nullptr;	// 지워졌으면
	//else if (true == static_cast<CMonster*>(m_pTargetedMonster)->IsDead()) return nullptr;	// 죽은 상태이면
	else if (true == static_cast<CEnemy*>(m_pTargetedMonster)->IsDead()) return nullptr;

	return m_pTargetedMonster;
}

CGameObject * CPlayerInfoManager::Get_SpecialObject()
{
	if (nullptr == m_pSpecialObject) return nullptr;	// null이면
	else if (false == CGameInstance::GetInstance()->Check_ObjectAlive(m_pSpecialObject)) return nullptr;	// 유효하지 않은 주소이면
	else if (true == m_pSpecialObject->IsDeleted()) return nullptr;	// 지워졌으면

	return m_pSpecialObject;
}

void CPlayerInfoManager::Change_PlayerHP(CHANGETYPE eType, _uint ChangeHP)
{
	if (CHANGE_INCREASE == eType)
	{
		m_tPlayerStat.m_iHP += ChangeHP;
	}
	else if (CHANGE_DECREASE == eType)
	{
		if (m_tPlayerStat.m_iHP < ChangeHP)
			ChangeHP = m_tPlayerStat.m_iHP;

		m_tPlayerStat.m_iHP -= ChangeHP;
	}

	if (m_tPlayerStat.m_iHP > m_tPlayerStat.m_iMaxHP)	m_tPlayerStat.m_iHP = m_tPlayerStat.m_iMaxHP;
	if (m_tPlayerStat.m_iHP < 0)						m_tPlayerStat.m_iHP = 0;
}

void CPlayerInfoManager::Change_HanabiHP(CHANGETYPE eType, _uint ChangeHP)
{
	if (CHANGE_INCREASE == eType)
	{
		m_tHanabiStat.iHP += ChangeHP;
	}
	else if (CHANGE_DECREASE == eType)
	{
		if (m_tHanabiStat.iHP < ChangeHP)
			ChangeHP = m_tHanabiStat.iHP;

		m_tHanabiStat.iHP -= ChangeHP;
	}

	if (m_tHanabiStat.iHP > m_tHanabiStat.iMaxHP)	m_tHanabiStat.iHP = m_tHanabiStat.iMaxHP;
	if (m_tHanabiStat.iHP < 0)						m_tHanabiStat.iHP = 0;
}

void CPlayerInfoManager::Change_TsugumiHP(CHANGETYPE eType, _uint ChangeHP)
{
	if (CHANGE_INCREASE == eType)
	{
		m_tTsugumiStat.iHP += ChangeHP;
	}
	else if (CHANGE_DECREASE == eType)
	{
		if (m_tTsugumiStat.iHP < ChangeHP)
			ChangeHP = m_tTsugumiStat.iHP;

		m_tTsugumiStat.iHP -= ChangeHP;
	}

	if (m_tTsugumiStat.iHP > m_tTsugumiStat.iMaxHP)	m_tTsugumiStat.iHP = m_tTsugumiStat.iMaxHP;
	if (m_tTsugumiStat.iHP < 0)						m_tTsugumiStat.iHP = 0;
}

void CPlayerInfoManager::Change_PlayerKineticEnergy(CHANGETYPE eType, _uint ChangeEnergy)
{
	if (CHANGE_INCREASE == eType) 
	{
		m_tPlayerStat.m_iKineticEnergy += ChangeEnergy;
	}
	else if (CHANGE_DECREASE == eType) 
	{
		if (m_tPlayerStat.m_iKineticEnergy < ChangeEnergy)
			ChangeEnergy = m_tPlayerStat.m_iKineticEnergy;

		m_tPlayerStat.m_iKineticEnergy -= ChangeEnergy;
	}	

	if (m_tPlayerStat.m_iKineticEnergy > m_tPlayerStat.m_iMaxKineticEnergy)	m_tPlayerStat.m_iKineticEnergy = m_tPlayerStat.m_iMaxKineticEnergy;
	if (m_tPlayerStat.m_iKineticEnergy < 0)									m_tPlayerStat.m_iKineticEnergy = 0;
}

void CPlayerInfoManager::Change_DriveEnergy(CHANGETYPE eType, _float ChangeDrive)
{
	if (CHANGE_INCREASE == eType)
	{
		m_tPlayerStat.fDriveEnergy += ChangeDrive;
	}
	else if (CHANGE_DECREASE == eType)
	{
		if (m_tPlayerStat.fDriveEnergy < ChangeDrive)
			ChangeDrive = m_tPlayerStat.fDriveEnergy;

		m_tPlayerStat.fDriveEnergy -= ChangeDrive;
	}

	if (m_tPlayerStat.fDriveEnergy > m_tPlayerStat.fMaxDriveEnergy)	m_tPlayerStat.fDriveEnergy = m_tPlayerStat.m_iMaxKineticEnergy;
	if (m_tPlayerStat.fDriveEnergy < 0)								m_tPlayerStat.fDriveEnergy = 0;
}

void CPlayerInfoManager::Change_BrainFieldMaintain(CHANGETYPE eType, _float ChangeBrain)
{
	if (CHANGE_INCREASE == eType)
	{
		m_tPlayerStat.fBrainFieldMaintain += ChangeBrain;
	}
	else if (CHANGE_DECREASE == eType)
	{
		if (m_tPlayerStat.fBrainFieldMaintain < ChangeBrain)
			ChangeBrain = m_tPlayerStat.fBrainFieldMaintain;

		m_tPlayerStat.fBrainFieldMaintain -= ChangeBrain;
	}

	if (m_tPlayerStat.fBrainFieldMaintain > m_tPlayerStat.fMaxBrainFieldMaintain)	
		m_tPlayerStat.fBrainFieldMaintain = m_tPlayerStat.fMaxBrainFieldMaintain;
	if (m_tPlayerStat.fBrainFieldMaintain < 0)
		m_tPlayerStat.fBrainFieldMaintain = 0;
}

void CPlayerInfoManager::Set_SasType(ESASType eType)
{
	// SAS 추가 -> 겹치는건 빼고, 넣을건 넣고, 총 3개까지
	// SASList가 비어있으면 SAS_NOT으로 적용되게 하자

	switch (eType)
	{
		case ESASType::SAS_FIRE:
		{
			for (auto SAS = m_PlayerSasTypeList.begin(); SAS != m_PlayerSasTypeList.end();)
			{
				if ((*SAS) == ESASType::SAS_ELETRIC)
					SAS = m_PlayerSasTypeList.erase(SAS);
				else
					++SAS;
			}

			m_PlayerSasTypeList.push_back(eType);

			break;
		}
		case ESASType::SAS_ELETRIC:
		{
			for (auto SAS = m_PlayerSasTypeList.begin(); SAS != m_PlayerSasTypeList.end();)
			{
				if ((*SAS) == ESASType::SAS_FIRE)
					SAS = m_PlayerSasTypeList.erase(SAS);
				else
					++SAS;
			}

			m_PlayerSasTypeList.push_back(eType);

			break;
		}
		case ESASType::SAS_HARDBODY:
		case ESASType::SAS_PENETRATE:
		case ESASType::SAS_SUPERSPEED:
		case ESASType::SAS_TELEPORT:
		case ESASType::SAS_COPY:
		{
			m_PlayerSasTypeList.push_back(eType);
			break;
		}
		default:
		{
			break;
		}
	}

	if (3 < m_PlayerSasTypeList.size())
	{
		m_PlayerSasTypeList.pop_front();
	}
}

void CPlayerInfoManager::Finish_SasType(ESASType eType)
{
	for (auto SAS = m_PlayerSasTypeList.begin(); SAS != m_PlayerSasTypeList.end();)
	{
		if ((*SAS) == eType) 
		{ 
			if (m_tPlayerStat.Sasese[static_cast<_uint>(*SAS)].MinEnergy >
				m_tPlayerStat.Sasese[static_cast<_uint>(*SAS)].Energy)
			{
				m_tPlayerStat.Sasese[static_cast<_uint>(*SAS)].bUsable = false;
			}

			SAS = m_PlayerSasTypeList.erase(SAS);

			break;
		}
		else 
		{
			++SAS;
		}
	}
}

void CPlayerInfoManager::Change_SasEnergy(CHANGETYPE eChangeType, ESASType eSasType, _float iChangeEnergy)
{
	if (CHANGE_INCREASE == eChangeType)				m_tPlayerStat.Sasese[static_cast<_uint>(eSasType)].Energy += iChangeEnergy;
	else if (CHANGE_DECREASE == eChangeType)		m_tPlayerStat.Sasese[static_cast<_uint>(eSasType)].Energy -= iChangeEnergy;

	if (m_tPlayerStat.Sasese[static_cast<_uint>(eSasType)].Energy > m_tPlayerStat.Sasese[static_cast<_uint>(eSasType)].MaxEnergy)
		m_tPlayerStat.Sasese[static_cast<_uint>(eSasType)].Energy = m_tPlayerStat.Sasese[static_cast<_uint>(eSasType)].MaxEnergy;
	if (m_tPlayerStat.Sasese[static_cast<_uint>(eSasType)].Energy < 0.f)
		m_tPlayerStat.Sasese[static_cast<_uint>(eSasType)].Energy = 0.f;
}

void CPlayerInfoManager::Set_SasEnergy(ESASType eSasType, _float iSetEnergy)
{
	m_tPlayerStat.Sasese[static_cast<_uint>(eSasType)].Energy = iSetEnergy;
}

void CPlayerInfoManager::Release_SasEnergy_All()
{
	for (_uint i = 0; i < SAS_CNT; ++i)
	{
		if (Get_isSasUsing(ESASType(i)))
		{
			m_tPlayerStat.Sasese[i].Energy = 0.1f;
		}
	}
}

void CPlayerInfoManager::Set_PlayerWorldMatrix(_fmatrix worldmatrix)
{
	m_PlayerWorldMatrix = worldmatrix;
}

_matrix CPlayerInfoManager::Get_PlayerWorldMatrix()
{
	return m_PlayerWorldMatrix;
}

void CPlayerInfoManager::Set_PlayerWeapon(_uint iWeaponType)
{
	m_tPlayerStat.iWeaponType = iWeaponType;
}

HRESULT CPlayerInfoManager::Set_KineticObject(CGameObject * pKineticObject)
{
	if (nullptr == pKineticObject) { m_pKineticObject = nullptr; return S_OK; }
	else if (false == CGameInstance::GetInstance()->Check_ObjectAlive(pKineticObject)) return E_FAIL;	// 유효하지 않은 주소이면
	else if (true == pKineticObject->IsDeleted()) return E_FAIL;																	// 지워졌으면
	else if (false == static_cast<CMapKinetic_Object*>(pKineticObject)->Usable()) return E_FAIL;				// 이미 사용한 객체이면
	else if (true == static_cast<CMapKinetic_Object*>(pKineticObject)->GetThrow()) return E_FAIL;			// 던져진 객체이면
	
	m_pKineticObject = pKineticObject;
	return S_OK;
}

HRESULT CPlayerInfoManager::Set_TargetedMonster(CGameObject * pTargetedMonster)
{
	if (nullptr == pTargetedMonster) { m_pTargetedMonster = nullptr; return S_OK; }
	else if (false == CGameInstance::GetInstance()->Check_ObjectAlive(pTargetedMonster)) return E_FAIL;	// 유효하지 않은 주소이면
	else if (true == pTargetedMonster->IsDeleted()) return E_FAIL;	// 지워졌으면
	//else if (true == static_cast<CMonster*>(pTargetedMonster)->IsDead()) return E_FAIL;	// 죽은 상태이면
	else if (true == static_cast<CEnemy*>(pTargetedMonster)->IsDead()) return E_FAIL;	// 죽은 상태이면

	m_pTargetedMonster = pTargetedMonster;
	return S_OK;
}

HRESULT CPlayerInfoManager::Set_SpecialObject(CGameObject * pSpecialObject)
{
	if (nullptr == pSpecialObject) { m_pSpecialObject = nullptr; return S_OK; }
	else if (false == CGameInstance::GetInstance()->Check_ObjectAlive(pSpecialObject)) return E_FAIL;	// 유효하지 않은 주소이면
	else if (true == pSpecialObject->IsDeleted()) return E_FAIL;										// 지워졌으면

	m_pSpecialObject = pSpecialObject;
	return S_OK;
}

void CPlayerInfoManager::Set_Exp(const _uint iExp)
{
	_uint iAllExp = m_tPlayerStat.iExp + iExp;
	if (m_tPlayerStat.iMaxExp > iAllExp)
		m_tPlayerStat.iExp += iExp;
	else
	{
		_uint iOverExp = iAllExp - m_tPlayerStat.iMaxExp;

		++m_tPlayerStat.iLevel;
		m_tPlayerStat.iMaxExp += 100;
		m_tPlayerStat.iExp = 0;
		m_tPlayerStat.m_iMaxHP += 50;
		m_tPlayerStat.m_iHP = m_tPlayerStat.m_iMaxHP;
		m_tPlayerStat.m_fBaseAttackDamage += static_cast<_float>(m_tPlayerStat.iLevel);
		m_tPlayerStat.iBP = m_tPlayerStat.iLevel * 2;

		m_tPlayerStat.iExp = iOverExp;

		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Alarm.json");
		CCanvas_Alarm* pUI_Alarm = dynamic_cast<CCanvas_Alarm*>(CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_UI"), L"Canvas_Alarm", &json));
		pUI_Alarm->Set_LevelUp(m_tPlayerStat.iLevel);
	}

	if (false == m_bSASMember[HANABI]) return;

	iAllExp = m_tHanabiStat.iExp + iExp;
	if (m_tHanabiStat.iMaxExp > iAllExp)
		m_tHanabiStat.iExp += iExp;
	else
	{
		_uint iOverExp = iAllExp - m_tPlayerStat.iMaxExp;

		++m_tHanabiStat.iLevel;
		m_tHanabiStat.iMaxExp += 100;
		m_tHanabiStat.iExp = 0;
		m_tHanabiStat.iMaxHP += 50;
		m_tHanabiStat.iHP = m_tHanabiStat.iMaxHP;
		m_tHanabiStat.iAttack += m_tHanabiStat.iLevel;

		m_tHanabiStat.iExp = iOverExp;
	}

	iAllExp = m_tTsugumiStat.iExp + iExp;
	if (m_tTsugumiStat.iMaxExp > iAllExp)
		m_tTsugumiStat.iExp += iExp;
	else
	{
		_uint iOverExp = iAllExp - m_tPlayerStat.iMaxExp;

		++m_tTsugumiStat.iLevel;
		m_tTsugumiStat.iMaxExp += 100;
		m_tTsugumiStat.iExp = 0;
		m_tTsugumiStat.iMaxHP += 50;
		m_tTsugumiStat.iHP = m_tTsugumiStat.iMaxHP;
		m_tTsugumiStat.iAttack += m_tTsugumiStat.iLevel;

		m_tTsugumiStat.iExp = iOverExp;
	}
}

HRESULT CPlayerInfoManager::Set_CamSpot(CGameObject * pCamSpot)
{
	m_pCamSpot = pCamSpot;

	return S_OK;
}

void CPlayerInfoManager::Camera_Random_Shake(_float fForce)
{
	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pCamSpot))
	{
		static_cast<CCamSpot*>(m_pCamSpot)->Random_Shaking(fForce);
	}
}

void CPlayerInfoManager::Camera_Random_Shake_Maintain(_float fForce, _float fMaintain)
{
	if (0.f != m_fRandomShakeMaintain || 0.f != m_fRandomShakeForce) return;

	m_fRandomShakeMaintain = fMaintain;
	m_fRandomShakeForce = fForce;
}

void CPlayerInfoManager::Camera_Axis_Shaking(_float4 vDir, _float fShakePower)
{
	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pCamSpot))
	{
		static_cast<CCamSpot*>(m_pCamSpot)->Axis_Shaking(vDir, fShakePower);
	}
}

void CPlayerInfoManager::Camera_Axis_Sliding(_float4 vDir, _float fShakePower)
{
	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pCamSpot))
	{
		static_cast<CCamSpot*>(m_pCamSpot)->Axis_Sliding(vDir, fShakePower);
	}
}

void CPlayerInfoManager::Camera_Arrange()
{
	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pCamSpot))
	{
		static_cast<CCamSpot*>(m_pCamSpot)->Arrange_Cam();
	}
}

HRESULT CPlayerInfoManager::Set_PlayerCam(CCamera* pCam)
{
	if (CGameInstance::GetInstance()->Check_ObjectAlive(pCam))
	{
		m_pPlayerCam = pCam;
	}

	return S_OK;
}

CCamera* CPlayerInfoManager::Get_PlayerCam()
{
	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pPlayerCam))
	{
		return m_pPlayerCam;
	}

	return nullptr;
}

void CPlayerInfoManager::SAS_Checker()
{
	for (_uint i = 0; i < SAS_CNT; ++i)
	{
		const auto Result = find(m_PlayerSasTypeList.begin(), m_PlayerSasTypeList.end(), static_cast<ESASType>(i));

		if (Result == m_PlayerSasTypeList.end())
		{
			// 사용중이지 않음, 회복
			m_tPlayerStat.Sasese[i].Energy += (g_fTimeDelta * m_tPlayerStat.Sasese[i].RecoveryRate);

			if (m_tPlayerStat.Sasese[i].MaxEnergy <= m_tPlayerStat.Sasese[i].Energy)
			{
				m_tPlayerStat.Sasese[i].Energy = m_tPlayerStat.Sasese[i].MaxEnergy;
			}

			if (m_tPlayerStat.Sasese[i].MinEnergy <= m_tPlayerStat.Sasese[i].Energy)
			{
				m_tPlayerStat.Sasese[i].bUsable = true;
			}
		}
		else
		{
			// 사용중임, 감소
			m_tPlayerStat.Sasese[i].Energy -= (g_fTimeDelta * m_tPlayerStat.Sasese[i].UseRate);

			if (0.f >= m_tPlayerStat.Sasese[i].Energy)
			{

				for (auto SAS = m_PlayerSasTypeList.begin(); SAS != m_PlayerSasTypeList.end();)
				{
					if ((*SAS) == static_cast<ESASType>(i))
					{
						SAS = m_PlayerSasTypeList.erase(SAS);
						m_tPlayerStat.Sasese[i].bUsable = false;

						break;
					}
					else
						++SAS;
				}

			}
		}
	}

	m_tPlayerStat.m_eAttack_SAS_Type = ESASType::SAS_NOT;

	const auto FireCheck = find(m_PlayerSasTypeList.begin(), m_PlayerSasTypeList.end(), ESASType::SAS_FIRE);
	if (FireCheck != m_PlayerSasTypeList.end())
		m_tPlayerStat.m_eAttack_SAS_Type = ESASType::SAS_FIRE;

	const auto ElecCheck = find(m_PlayerSasTypeList.begin(), m_PlayerSasTypeList.end(), ESASType::SAS_ELETRIC);
	if (ElecCheck != m_PlayerSasTypeList.end())
		m_tPlayerStat.m_eAttack_SAS_Type = ESASType::SAS_ELETRIC;
}

void CPlayerInfoManager::Free()
{
}
