#include "stdafx.h"
#include "../public/PlayerInfoManager.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MapKinetic_Object.h"
#include "Monster.h"

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

	m_tPlayerStat.m_iKineticEnergyLevel = 2;
	m_tPlayerStat.m_iKineticEnergyType = 2;

	m_tHanabiStat.iExp = 0;
	m_tHanabiStat.iMaxExp = 100;
	m_tHanabiStat.iLevel = 1;
	m_tHanabiStat.iSprbrPower = 110;
	m_tHanabiStat.iAttack = 30;
	m_tHanabiStat.iDefense = 15;

#pragma endregion 플레이어 기본 스탯 초기화

#pragma region	멤버 기본 스탯 초기화

	m_tHanabiStat.m_bMember = false;
	m_tHanabiStat.iHP = 1700;
	m_tHanabiStat.iMaxHP = 1700;
	m_tHanabiStat.iExp = 0;
	m_tHanabiStat.iMaxExp = 100;
	m_tHanabiStat.iLevel = 1;
	m_tHanabiStat.iBondLevel = 1;
	m_tHanabiStat.iSprbrPower = 276;
	m_tHanabiStat.iAttack = 50;
	m_tHanabiStat.iDefense = 20;

	m_tTsugumiStat.m_bMember = false;
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

	m_ePlayerSasType = ESASType::SAS_END;

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

void CPlayerInfoManager::Tick()
{
	// 사용이 불가능한 객체를 바로 null로 채워주기 위해 매 틱 작동한다.
	// 플레이어에서 값을 사용할 것이므로 플레이어의 가장 첫 틱에서 동작하게 하자

	if (nullptr == m_pKineticObject) m_pKineticObject = nullptr;
	if (true == m_pKineticObject->IsDeleted()) m_pKineticObject = nullptr;
	if (false == CGameInstance::GetInstance()->Check_ObjectAlive(m_pKineticObject)) m_pKineticObject = nullptr;
	if (false == static_cast<CMapKinetic_Object*>(m_pKineticObject)->Usable()) m_pKineticObject = nullptr;
	if (true == static_cast<CMapKinetic_Object*>(m_pKineticObject)->GetThrow()) m_pKineticObject = nullptr;

	if (nullptr == m_pTargetedMonster) m_pTargetedMonster = nullptr;
	if (true == m_pTargetedMonster->IsDeleted()) m_pTargetedMonster = nullptr;
	if (false == CGameInstance::GetInstance()->Check_ObjectAlive(m_pTargetedMonster)) m_pTargetedMonster = nullptr;
	if (true == static_cast<CMonster*>(m_pTargetedMonster)->IsDead()) m_pTargetedMonster = nullptr;
}

CGameObject * CPlayerInfoManager::Get_KineticObject()
{
	if (nullptr == m_pKineticObject) return nullptr;	// null이면
	if (true == m_pKineticObject->IsDeleted()) return nullptr;	// 지워졌으면
	if (false == CGameInstance::GetInstance()->Check_ObjectAlive(m_pKineticObject)) return nullptr;	// 유효하지 않은 주소이면
	if (false == static_cast<CMapKinetic_Object*>(m_pKineticObject)->Usable()) return nullptr;		// 이미 사용한 객체이면
	if (true == static_cast<CMapKinetic_Object*>(m_pKineticObject)->GetThrow()) return nullptr;		// 던져진 객체이면

	return m_pKineticObject;
}

CGameObject * CPlayerInfoManager::Get_TargetedMonster()
{
	if (nullptr == m_pTargetedMonster) return nullptr;	// null이면
	if (true == m_pTargetedMonster->IsDeleted()) return nullptr;	// 지워졌으면
	if (false == CGameInstance::GetInstance()->Check_ObjectAlive(m_pTargetedMonster)) return nullptr;	// 유효하지 않은 주소이면
	if (true == static_cast<CMonster*>(m_pTargetedMonster)->IsDead()) return nullptr;	// 죽은 상태이면

	return m_pTargetedMonster;
}

HRESULT CPlayerInfoManager::Set_KineticObject(CGameObject * pKineticObject)
{
	NULL_CHECK(pKineticObject);

	if (true == m_pKineticObject->IsDeleted()) return E_FAIL;										// 지워졌으면
	if (false == CGameInstance::GetInstance()->Check_ObjectAlive(m_pKineticObject)) return E_FAIL;	// 유효하지 않은 주소이면
	if (false == static_cast<CMapKinetic_Object*>(m_pKineticObject)->Usable()) return E_FAIL;		// 이미 사용한 객체이면
	if (true == static_cast<CMapKinetic_Object*>(m_pKineticObject)->GetThrow()) return E_FAIL;		// 던져진 객체이면
	
	m_pKineticObject = pKineticObject;

	return S_OK;
}

HRESULT CPlayerInfoManager::Set_TargetedMonster(CGameObject * pTargetedMonster)
{
	NULL_CHECK(pTargetedMonster);

	if (true == m_pTargetedMonster->IsDeleted()) return E_FAIL;	// 지워졌으면
	if (false == CGameInstance::GetInstance()->Check_ObjectAlive(m_pTargetedMonster)) return E_FAIL;	// 유효하지 않은 주소이면
	if (true == static_cast<CMonster*>(m_pTargetedMonster)->IsDead()) return E_FAIL;	// 죽은 상태이면

	m_pTargetedMonster = pTargetedMonster;

	return S_OK;
}

void CPlayerInfoManager::Free()
{
}
