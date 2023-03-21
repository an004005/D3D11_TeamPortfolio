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

#pragma region �÷��̾� �⺻ ���� �ʱ�ȭ

	m_tPlayerStat.m_iHP = 1000;
	m_tPlayerStat.m_iMaxHP = 1000;

	m_tPlayerStat.m_iKineticEnergy = 100;
	m_tPlayerStat.m_iMaxKineticEnergy = 100;

	m_tPlayerStat.m_iKineticEnergyLevel = 0;
	m_tPlayerStat.m_iKineticEnergyType = 2;

	m_tPlayerStat.iExp = 0;
	m_tPlayerStat.iMaxExp = 100;
	m_tPlayerStat.iLevel = 1;
	m_tPlayerStat.iSprbrPower = 110;
	m_tPlayerStat.iAttack = 30;
	m_tPlayerStat.iDefense = 15;

#pragma endregion �÷��̾� �⺻ ���� �ʱ�ȭ

	ZeroMemory(&m_tHanabiStat, sizeof(HANABI_STAT));
	ZeroMemory(&m_tTsugumiStat, sizeof(TSUGUMI_STAT));

#pragma region	��� �⺻ ���� �ʱ�ȭ

	m_tHanabiStat.bMember = false;
	m_tHanabiStat.iHP = 1700;
	m_tHanabiStat.iMaxHP = 1700;
	m_tHanabiStat.iExp = 50;
	m_tHanabiStat.iMaxExp = 100;
	m_tHanabiStat.iLevel = 1;
	m_tHanabiStat.iBondLevel = 1;
	m_tHanabiStat.iSprbrPower = 276;
	m_tHanabiStat.iAttack = 50;
	m_tHanabiStat.iDefense = 20;

	m_tTsugumiStat.bMember = false;
	m_tTsugumiStat.iHP = 1300;
	m_tTsugumiStat.iMaxHP = 1300;
	m_tTsugumiStat.iExp = 0;
	m_tTsugumiStat.iMaxExp = 100;
	m_tTsugumiStat.iLevel = 1;
	m_tTsugumiStat.iBondLevel = 1;
	m_tTsugumiStat.iSprbrPower = 325;
	m_tTsugumiStat.iAttack = 60;
	m_tTsugumiStat.iDefense = 10;

#pragma endregion	��� �⺻ ���� �ʱ�ȭ

#pragma region SAS���� �ʱ�ȭ

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

#pragma endregion SAS���� �ʱ�ȭ


	return S_OK;
}

void CPlayerInfoManager::Tick(_double TimeDelta)
{
	// ����� �Ұ����� ��ü�� �ٷ� null�� ä���ֱ� ���� �� ƽ �۵��Ѵ�.
	// �÷��̾�� ���� ����� ���̹Ƿ� �÷��̾��� ���� ù ƽ���� �����ϰ� ����

	if (nullptr == m_pKineticObject) m_pKineticObject = nullptr;
	else if (false == CGameInstance::GetInstance()->Check_ObjectAlive(m_pKineticObject)) m_pKineticObject = nullptr;
	else if (true == m_pKineticObject->IsDeleted()) m_pKineticObject = nullptr;
	else if (false == static_cast<CMapKinetic_Object*>(m_pKineticObject)->Usable()) m_pKineticObject = nullptr;
	else if (true == static_cast<CMapKinetic_Object*>(m_pKineticObject)->GetThrow()) m_pKineticObject = nullptr;

	if (nullptr == m_pTargetedMonster) m_pTargetedMonster = nullptr;
	else if (false == CGameInstance::GetInstance()->Check_ObjectAlive(m_pTargetedMonster)) m_pTargetedMonster = nullptr;
	else if (true == m_pTargetedMonster->IsDeleted()) m_pTargetedMonster = nullptr;
	else if (true == static_cast<CMonster*>(m_pTargetedMonster)->IsDead()) m_pTargetedMonster = nullptr;

	SAS_Checker();
}

CGameObject * CPlayerInfoManager::Get_KineticObject()
{
	if (nullptr == m_pKineticObject) return nullptr;	// null�̸�
	else if (false == CGameInstance::GetInstance()->Check_ObjectAlive(m_pKineticObject)) return nullptr;	// ��ȿ���� ���� �ּ��̸�
	else if (true == m_pKineticObject->IsDeleted()) return nullptr;	// ����������
	else if (false == static_cast<CMapKinetic_Object*>(m_pKineticObject)->Usable()) return nullptr;		// �̹� ����� ��ü�̸�
	else if (true == static_cast<CMapKinetic_Object*>(m_pKineticObject)->GetThrow()) return nullptr;		// ������ ��ü�̸�

	return m_pKineticObject;
}

CGameObject * CPlayerInfoManager::Get_TargetedMonster()
{
	if (nullptr == m_pTargetedMonster) return nullptr;	// null�̸�
	else if (false == CGameInstance::GetInstance()->Check_ObjectAlive(m_pTargetedMonster)) return nullptr;	// ��ȿ���� ���� �ּ��̸�
	else if (true == m_pTargetedMonster->IsDeleted()) return nullptr;	// ����������
	else if (true == static_cast<CMonster*>(m_pTargetedMonster)->IsDead()) return nullptr;	// ���� �����̸�

	return m_pTargetedMonster;
}

void CPlayerInfoManager::Change_PlayerHP(CHANGETYPE eType, _uint ChangeHP)
{
	if		(CHANGE_INCREASE == eType)		m_tPlayerStat.m_iHP += ChangeHP;
	else if (CHANGE_DECREASE == eType)		m_tPlayerStat.m_iHP -= ChangeHP;

	if (m_tPlayerStat.m_iHP > m_tPlayerStat.m_iMaxHP)	m_tPlayerStat.m_iHP = m_tPlayerStat.m_iMaxHP;
	if (m_tPlayerStat.m_iHP < 0)						m_tPlayerStat.m_iHP = 0;
}

void CPlayerInfoManager::Change_PlayerKineticEnergy(CHANGETYPE eType, _uint ChangeEnergy)
{
	if		(CHANGE_INCREASE == eType)		m_tPlayerStat.m_iKineticEnergy += ChangeEnergy;
	else if (CHANGE_DECREASE == eType)		m_tPlayerStat.m_iKineticEnergy -= ChangeEnergy;

	if (m_tPlayerStat.m_iKineticEnergy > m_tPlayerStat.m_iMaxKineticEnergy)	m_tPlayerStat.m_iKineticEnergy = m_tPlayerStat.m_iMaxKineticEnergy;
	if (m_tPlayerStat.m_iKineticEnergy < 0)									m_tPlayerStat.m_iKineticEnergy = 0;
}

void CPlayerInfoManager::Set_SasType(ESASType eType)
{
	// SAS �߰� -> ��ġ�°� ����, ������ �ְ�, �� 3������
	// SASList�� ��������� SAS_NOT���� ����ǰ� ����

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

HRESULT CPlayerInfoManager::Set_KineticObject(CGameObject * pKineticObject)
{
	if (nullptr == pKineticObject) { m_pKineticObject = nullptr; return S_OK; }
	else if (false == CGameInstance::GetInstance()->Check_ObjectAlive(pKineticObject)) return E_FAIL;	// ��ȿ���� ���� �ּ��̸�
	else if (true == pKineticObject->IsDeleted()) return E_FAIL;										// ����������
	else if (false == static_cast<CMapKinetic_Object*>(pKineticObject)->Usable()) return E_FAIL;		// �̹� ����� ��ü�̸�
	else if (true == static_cast<CMapKinetic_Object*>(pKineticObject)->GetThrow()) return E_FAIL;		// ������ ��ü�̸�
	
	m_pKineticObject = pKineticObject;
	return S_OK;
}

HRESULT CPlayerInfoManager::Set_TargetedMonster(CGameObject * pTargetedMonster)
{
	if (nullptr == pTargetedMonster) { m_pTargetedMonster = nullptr; return S_OK; }
	else if (false == CGameInstance::GetInstance()->Check_ObjectAlive(pTargetedMonster)) return E_FAIL;	// ��ȿ���� ���� �ּ��̸�
	else if (true == pTargetedMonster->IsDeleted()) return E_FAIL;	// ����������
	else if (true == static_cast<CMonster*>(pTargetedMonster)->IsDead()) return E_FAIL;	// ���� �����̸�

	m_pTargetedMonster = pTargetedMonster;
	return S_OK;
}

void CPlayerInfoManager::SAS_Checker()
{
	for (_uint i = 0; i < SAS_CNT; ++i)
	{
		const auto Result = find(m_PlayerSasTypeList.begin(), m_PlayerSasTypeList.end(), static_cast<ESASType>(i));

		if (Result == m_PlayerSasTypeList.end())
		{
			// ��������� ����, ȸ��
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
			// �������, ����
//			m_tPlayerStat.Sasese[i].Energy -= (g_fTimeDelta * m_tPlayerStat.Sasese[i].UseRate);

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
