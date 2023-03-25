#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

typedef struct tagEnemyDamageReport
{
	class CScarletCharacter* pCauser = nullptr;
	class CEnemy* pTaker = nullptr;
	_uint iTakeDamage = 0;
	ESASType eAttackSAS = ESASType::SAS_END;
	EDeBuffType eBeDeBuff = EDeBuffType::DEBUFF_END; // 걸린 상태이상
	EAttackType eAttackType = EAttackType::ATK_END;
	EKineticAttackType eKineticAtkType = EKineticAttackType::KINETIC_ATTACK_END;
	_bool bHitWeak = false;
	_bool bDead = false;
} ENEMY_DAMAGE_REPORT;

typedef struct tagPlayerDamageReport
{
	class CEnemy* pCauser = nullptr;
	class CPlayer* pTaker = nullptr;
	_uint iTakeDamage = 0;
	EDeBuffType eBeDeBuff = EDeBuffType::DEBUFF_END; // 걸린 상태이상
	EAttackType eAttackType = EAttackType::ATK_END;
	_bool bDead = false;
} PLAYER_DAMAGE_REPORT;

class CGameManager : public CBase
{
private:
	static CGameManager* s_GameManager;
protected:
	CGameManager(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	virtual ~CGameManager() = default;

public:
	static CGameManager* GetInstance() { return s_GameManager; }
	static void SetGameManager(CGameManager* pGameManager);
	static _uint DestroyInstance();

	virtual HRESULT Initialize();
	virtual void Tick(_double TimeDelta);
	virtual void ConsumeEnemyDamageReport(ENEMY_DAMAGE_REPORT tReport);
	virtual void ConsumePlayerDamageReport(PLAYER_DAMAGE_REPORT tReport);

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

public:
	static CGameManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END