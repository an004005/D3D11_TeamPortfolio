#include "stdafx.h"
#include "..\public\GameManager_Tutorial.h"
#include "GameInstance.h"

CGameManager_Tutorial* CGameManager_Tutorial::s_GameManager = nullptr;

CGameManager_Tutorial::CGameManager_Tutorial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameManager(pDevice, pContext)
{
}

HRESULT CGameManager_Tutorial::Initialize()
{
	__super::Initialize();

	return S_OK;
}

void CGameManager_Tutorial::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

}

void CGameManager_Tutorial::ConsumeEnemyDamageReport(ENEMY_DAMAGE_REPORT tReport)
{
	__super::ConsumeEnemyDamageReport(tReport);

	if (m_bKineticAttackCheck == false)
	{
		if (tReport.eAttackType == EAttackType::ATK_HEAVY)
		{
			m_bKineticAttackCheck = true;
		}
	}

	if (m_bKineticAttackAndLockOn[1] == false)
	{
		if (m_bKineticAttackAndLockOn[0] == true)
		{
			if (tReport.eAttackType == EAttackType::ATK_HEAVY)
			{
				m_bKineticAttackAndLockOn[1] = true;
			}
		}
	}

	if (m_bFlatHit[1] == false)
	{
		if (m_bFlatHit[0] == true)
		{
			if (true == tReport.bDead)
			{
				m_bFlatHit[1] = true;
			}
		}
	}
	
	if (false == m_bHitAir)
	{
		if (tReport.eAttackType == EAttackType::ATK_TO_AIR)
		{
			m_bHitAir = true;
		}
	}

}

void CGameManager_Tutorial::ConsumePlayerDamageReport(PLAYER_DAMAGE_REPORT tReport)
{
}

CGameManager_Tutorial* CGameManager_Tutorial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CGameManager_Tutorial* pInstance = new CGameManager_Tutorial(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CGameManager_Tutorial");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGameManager_Tutorial::Free()
{
	__super::Free();
}
