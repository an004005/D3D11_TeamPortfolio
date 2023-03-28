#include "stdafx.h"
#include "..\public\GameManager_Tutorial.h"
#include "GameInstance.h"
#include "JsonStorage.h"

CGameManager_Tutorial* CGameManager_Tutorial::s_GameManager = nullptr;

CGameManager_Tutorial::CGameManager_Tutorial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameManager(pDevice, pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CGameManager_Tutorial::Initialize()
{
	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Tutorial.json");
	m_pCanvas_Tutorial = dynamic_cast<CCanvas_Tutorial*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_NOW, PLAYERTEST_LAYER_FRONTUI, L"Canvas_Tutorial", &json));
	Assert(m_pCanvas_Tutorial != nullptr, "Failed to Clone : m_pCanvas_Tutorial");

	return S_OK;
}

void CGameManager_Tutorial::Tick(_double TimeDelta)
{

}

void CGameManager_Tutorial::ConsumeEnemyDamageReport(ENEMY_DAMAGE_REPORT tReport)
{
	__super::ConsumeEnemyDamageReport(tReport);

	if (m_bKineticAttackCheck == false)
	{
		if (tReport.eAttackType == EAttackType::ATK_HEAVY)
		{
			m_pCanvas_Tutorial->Set_Success();
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

	if (m_bKineticAttackAndLockOn[2] == false)
	{
		if (m_bKineticAttackAndLockOn[2] == true)
		{
			m_pCanvas_Tutorial->Set_Success();
		}
	}

	if (m_bFlatHit[1] == false)
	{
		if (m_bFlatHit[0] == true)
		{
			if (true == tReport.bDead)
			{
				m_pCanvas_Tutorial->Set_Success();
				m_bFlatHit[1] = true;
			}
		}
	}
	
	if (false == m_bHitAir)
	{
		if (tReport.eAttackType == EAttackType::ATK_TO_AIR)
		{
 			m_pCanvas_Tutorial->Set_Success();
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
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
