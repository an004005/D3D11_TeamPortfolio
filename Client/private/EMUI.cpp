#include "stdafx.h"
#include "EMUI.h"
#include "Enemy.h"
#include "MonsterHpUI.h"
#include "MonsterShildUI.h"

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
}
