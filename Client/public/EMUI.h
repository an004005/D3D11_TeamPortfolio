#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "SoundStore.h"

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
	void Delete_BossUI();
	void Create_DamageFont(DAMAGE_PARAM& tDamageParams);
	void Create_CGUI();
	void Update_NoticeNeon();

	void SetWeakBoneName(const string& strWeakBoneName) {m_strWeakBoneName = strWeakBoneName; }
	void SetShieldUIPivotBoneName(const string& BoneName) {m_strShieldUIPivotBoneName = BoneName; }
	void SetShieldUIPivot(_float4x4 Pivot) {m_ShieldUIPivot = Pivot; }
	void SetTargetBone(const string& BoneName) { m_strTargetBoneName = BoneName; }

	const string& GetTargetBoneName() { return m_strTargetBoneName; }

private:
	class CEnemy* m_pOwner = nullptr;
	class CMonsterShildUI* m_pShieldUI = nullptr;
	class CMonsterHpUI* m_pHPUI = nullptr;
	class CCanvas_BossHpMove* m_BossHp = nullptr;
	class CEffectGroup* m_pCGEffect = nullptr;

	class CEffectGroup*	 m_pNoticeNeon = nullptr;
	string m_strWeakBoneName = "Weak01";

	string m_strShieldUIPivotBoneName = "Target";
	_float4x4 m_ShieldUIPivot = XMMatrixTranslation(0.f, 0.5f, 0.f);

	string m_strTargetBoneName = "Target";

	CSoundStore		m_SoundStore;
public:
	static CEMUI* Create(class CEnemy* pEnemy);
	virtual void Free() override;
};

END