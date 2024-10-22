#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

typedef struct tagEnemyDamageReport
{
	class CScarletCharacter* pCauser = nullptr;
	class CEnemy* pTaker = nullptr;
	EEnemyName eName;
	ENEMY_STAT eStat;
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

public:
	void		Set_FullItem(const wstring szItemName);
	void		Set_AddlItem(const wstring szItemName);
	void		Set_LeftTalk(const _int iIndex, const _int iQuest = -1);

	void		Set_SuccessQuest(const _uint iCoin);

public:
	list<ENEMY_DAMAGE_REPORT>	GetEnemyReport() { return m_EneymyReports; }
	void						ResetEnemyReport() { m_EneymyReports.clear(); }

public:
	_bool	Get_EM0320Dead() {
		return m_bEM0320Dead;
	}
	void		Set_EM0320Dead() {
		m_bEM0320Dead = true;
	}

	_bool	Get_Flower() {
		return m_bFlower;
	}
	void	Set_Flower() {
		m_bFlower = true;
	}

protected:
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

protected:
	class CCanvas_Acquisition*	m_pCanvas_Acquisition = { nullptr };
	class CCanvas_LeftTalk*		m_pCanvas_LeftTalk = { nullptr };
	class CDistanceUI*					m_pDistanceUI = { nullptr };

private:
	_bool	m_bQuest = { false };
	_bool	m_bSuccessQuest = { false };

	_bool	m_bEM0320Dead = { false };
	_bool	m_bEM0110Dead = { false };
	_bool	m_bEM1200Dead = { false };
	_bool	m_bFlower = { false };

	list<ENEMY_DAMAGE_REPORT> m_EneymyReports;

public:
	static CGameManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END