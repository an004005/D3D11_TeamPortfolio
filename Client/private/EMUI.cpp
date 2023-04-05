#include "stdafx.h"
#include "EMUI.h"
#include "Enemy.h"
#include "MonsterHpUI.h"
#include "MonsterShildUI.h"
#include "Canvas_BossHpMove.h"
#include "JsonStorage.h"
#include "EffectGroup.h"
#include "VFX_Manager.h"
#include "EffectSystem.h"
#include "MathUtils.h"

CEMUI::CEMUI()
{
}

HRESULT CEMUI::Initialize_Prototype(CEnemy* pEnemy)
{
	assert(pEnemy != nullptr);
	m_pOwner = pEnemy;

	return S_OK;
}

void CEMUI::TurnEyesOut()
{
	CEffectGroup* pEffectGroup = nullptr;
	pEffectGroup = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"Lockon_Find", PLAYERTEST_LAYER_FRONTUI);
	assert(pEffectGroup != nullptr);

	//TimeLine 끝나고 삭제
	pEffectGroup->Start_AttachPivot(m_pOwner, m_EnemyFindEyesPivot, "Target", true, true, true);
}

void CEMUI::Create_UIInfo()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (m_pShieldUI != nullptr || m_pHPUI != nullptr) return;

	if (m_pOwner->HasCrushGauge())
	{
		m_pShieldUI = dynamic_cast<CMonsterShildUI*>(pGameInstance->Clone_GameObject_Get(TEXT("Layer_FrontUI"), TEXT("Prototype_GameObject_MonsterShield")));
		assert(m_pShieldUI != nullptr);

		m_pShieldUI->Set_Owner(m_pOwner);
		m_pShieldUI->SetPivotMatrix(m_EnemyInfoBarPivot);
		m_pShieldUI->Set_MonsterInfo(m_pOwner->GetEnemyLevel(), m_pOwner->GetEnemyName());
	}
	else
	{
		m_pHPUI = dynamic_cast<CMonsterHpUI*>(pGameInstance->Clone_GameObject_Get(TEXT("Layer_FrontUI"), TEXT("Prototype_GameObject_MonsterHP")));
		assert(m_pHPUI != nullptr);

		m_pHPUI->Set_Owner(m_pOwner);
		m_pHPUI->SetPivotMatrix(m_EnemyInfoBarPivot);
		m_pHPUI->Set_MonsterInfo(m_pOwner->GetEnemyLevel(), m_pOwner->GetEnemyName());
	}
}

void CEMUI::Update_UIInfo()
{
	//if (m_bDead == true) return;

	if (m_pShieldUI != nullptr)
		m_pShieldUI->SetShild(m_pOwner->GetHpRatio(), m_pOwner->GetCrushGageRatio());
	else if (m_pHPUI != nullptr)
		m_pHPUI->Set_HpRatio(m_pOwner->GetHpRatio());

	if(m_BossHp != nullptr)
		m_BossHp->Set_BossHp(m_pOwner->GetHpRatio());
}

void CEMUI::Create_BossUI()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_BossHpMove.json");

	m_BossHp = dynamic_cast<CCanvas_BossHpMove*>(pGameInstance->Clone_GameObject_Get(TEXT("Layer_UI"), L"Canvas_BossHpMove", &json));
	assert(m_BossHp != nullptr && "Failed to Clone : CCanvas_BossHpMove");

}

void CEMUI::Create_DamageFont(DAMAGE_PARAM& tDamageParams)
{
	_int iDamage = tDamageParams.iDamage;
	array<_int, 4> SaveNum;

	//10을 넣어주면 숫자가 안뜬다고 함
	SaveNum.fill(10);

	_int iCount = 0;
	while (iDamage != 0)
	{
		SaveNum[iCount++] = iDamage % 10;
		iDamage = iDamage / 10;
	}

	_float4 vHitPosition = tDamageParams.vHitPosition;
	
	vHitPosition.x += CMathUtils::RandomFloat(-1.f, 1.f);
	vHitPosition.y += CMathUtils::RandomFloat(-0.5f, 1.f);
	vHitPosition.z += CMathUtils::RandomFloat(-1.f, 1.f);

	CEffectGroup* pFont = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_UI, L"AttackNum");
	pFont->Start_AttachOnlyPos(vHitPosition);
	
	pFont->GetFirstEffect()->GetParams().Float2s[0] = { _float(SaveNum[3]), 0.0f };
	pFont->GetSecondEffect()->GetParams().Float2s[0] = { _float(SaveNum[2]), 0.0f };
	pFont->GetThirdEffect()->GetParams().Float2s[0] = { _float(SaveNum[1]), 0.0f };
	pFont->GetFourthEffect()->GetParams().Float2s[0] = { _float(SaveNum[0]), 0.0f };
}

CEMUI* CEMUI::Create(CEnemy* pEnemy)
{
	CEMUI* pInstance = new CEMUI;
	
	if (FAILED(pInstance->Initialize_Prototype(pEnemy)))
	{
		MSG_BOX("Failed to Created : CEMUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEMUI::Free()
{
	if(m_pShieldUI != nullptr)
		m_pShieldUI->SetDelete();

	if (m_pHPUI != nullptr)
		m_pHPUI->SetDelete();

	if(m_BossHp != nullptr)
		m_BossHp->SetDelete();
}
