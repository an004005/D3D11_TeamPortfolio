#pragma once
#include "Client_Defines.h"
#include "GameManager.h"

BEGIN(Client)

class CGameManager_Tutorial : public CGameManager
{
private:
	static CGameManager_Tutorial* s_GameManager;
protected:
	CGameManager_Tutorial(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	virtual ~CGameManager_Tutorial() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void ConsumeEnemyDamageReport(ENEMY_DAMAGE_REPORT tReport) override;
	virtual void ConsumePlayerDamageReport(PLAYER_DAMAGE_REPORT tReport) override;

public:
	_bool	Get_KineticAttackCheck() {
		return m_bKineticAttackCheck;
	}

	_bool	Get_KineticAttackAndLockOn() {
		return m_bKineticAttackAndLockOn[1];
	}
	void	Set_KineticAttackAndLockOn() {
		m_bKineticAttackAndLockOn[0] = true;
	}

	_bool	Get_FlatHit() {
		return m_bFlatHit[1];
	}
	void	Set_FlatHit() {
		m_bFlatHit[0] = true;
	}

	_bool	Get_HitAir() {
		return m_bHitAir;
	}

private:
	_bool m_bKineticAttackCheck = false;
	_bool m_bKineticAttackAndLockOn[2] = { false, false };
	_bool m_bFlatHit[2] = { false, false };
	_bool m_bHitAir = { false };

public:
	static CGameManager_Tutorial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END