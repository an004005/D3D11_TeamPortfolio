#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CEMUI : public CBase
{
private:
	CEMUI();
	virtual ~CEMUI() = default;

public:
	HRESULT	Initialize_Prototype(class CEnemy* pEnemy);

public:
	void SetUpInfoBarPivot(_float4x4 Infobar) {
		m_EnemyInfoBarPivot = Infobar;
	}

	void SetUpFindEyesPivot(_float4x4 Findeyes) {
		m_EnemyFindEyesPivot = Findeyes;
	}

	void SetUpPivots(_float4x4 Infobar, _float4x4 Findeyes) {
		m_EnemyInfoBarPivot = Infobar;
		m_EnemyFindEyesPivot = Findeyes;
	}

public:
	const _float4x4 GetUpInfoBarPivot() const { return m_EnemyInfoBarPivot; }
	const _float4x4 GetUpFindEyesPivot() const { return m_EnemyFindEyesPivot; }

public:
	void TurnEyesOut();
	void Create_UIInfo();
	void Delete_UIInfo();

	void Update_UIInfo();
	void Create_BossUI();
	void Create_DamageFont(DAMAGE_PARAM& tDamageParams);
	void Create_CGUI();
	void Update_NoticeNeon();


private:
	class CEnemy* m_pOwner = nullptr;
	class CMonsterShildUI* m_pShieldUI = nullptr;
	class CMonsterHpUI* m_pHPUI = nullptr;
	class CCanvas_BossHpMove* m_BossHp = nullptr;
	class CEffectGroup* m_pCGEffect = nullptr;

	pair<class CEffectGroup*, class CParticleGroup*> m_pNoticNeon = { nullptr ,nullptr };
	
private:
	_float4x4	m_EnemyInfoBarPivot;
	_float4x4	m_EnemyFindEyesPivot;

public:
	static CEMUI* Create(class CEnemy* pEnemy);
	virtual void Free() override;
};

END