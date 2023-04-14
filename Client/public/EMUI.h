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
	void TurnEyesOut();
	void Create_UIInfo();
	void Delete_UIInfo();

	void Update_UIInfo();
	void Create_BossUI();
	void Create_DamageFont(DAMAGE_PARAM& tDamageParams);
	void Create_CGUI();
	void Update_NoticeNeon();

	void SetWeakBoneName(const string& strWeakBoneName) {m_strWeakBoneName = strWeakBoneName; }
	void SetShieldUIPivotBoneName(const string& BoneName) {m_strShieldUIPivotBoneName = BoneName; }

private:
	class CEnemy* m_pOwner = nullptr;
	class CMonsterShildUI* m_pShieldUI = nullptr;
	class CMonsterHpUI* m_pHPUI = nullptr;
	class CCanvas_BossHpMove* m_BossHp = nullptr;
	class CEffectGroup* m_pCGEffect = nullptr;

	string m_strWeakBoneName = "Weak01";
	string m_strShieldUIPivotBoneName = "Target";

	pair<class CEffectGroup*, class CParticleGroup*> m_pNoticNeon = { nullptr ,nullptr };
	
public:
	static CEMUI* Create(class CEnemy* pEnemy);
	virtual void Free() override;
};

END