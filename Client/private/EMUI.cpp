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

	_float4x4	EyesPivot = XMMatrixTranslation(0.f, 1.5f, 0.f);
	pEffectGroup->Start_AttachPivot(m_pOwner, EyesPivot, "Target", true, true, true);
}

void CEMUI::Create_UIInfo()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (m_pShieldUI != nullptr || m_pHPUI != nullptr) return;

	if (m_pOwner->HasCrushGauge())
	{
		m_pShieldUI = dynamic_cast<CMonsterShildUI*>(pGameInstance->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, TEXT("Prototype_GameObject_MonsterShield")));
		assert(m_pShieldUI != nullptr);

		m_pShieldUI->Set_Owner(m_pOwner);
		m_pShieldUI->Set_MonsterInfo(m_pOwner->GetEnemyLevel(), m_pOwner->GetEnemyName());
	}
	else
	{
		m_pHPUI = dynamic_cast<CMonsterHpUI*>(pGameInstance->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, TEXT("Prototype_GameObject_MonsterHP")));
		assert(m_pHPUI != nullptr);

		m_pHPUI->Set_Owner(m_pOwner);
		m_pHPUI->Set_MonsterInfo(m_pOwner->GetEnemyLevel(), m_pOwner->GetEnemyName());
	}
}

void CEMUI::Delete_UIInfo()
{
	if (m_pShieldUI != nullptr)
	{
		m_pShieldUI->SetDelete();
		m_pShieldUI = nullptr;
	}

	if (m_pHPUI != nullptr)
	{
		m_pHPUI->SetDelete();
		m_pHPUI = nullptr;
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

	json["Level"] = m_pOwner->GetEnemyLevel();
	json["Name"] = m_pOwner->GetEnemyName();
	m_BossHp = dynamic_cast<CCanvas_BossHpMove*>(pGameInstance->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"Canvas_BossHpMove", &json));
	assert(m_BossHp != nullptr && "Failed to Clone : CCanvas_BossHpMove");
}

void CEMUI::Create_DamageFont(DAMAGE_PARAM& tDamageParams)
{
	_int iDamage = tDamageParams.iDamage;
	_float4 vHitPosition = tDamageParams.vHitPosition;
	
	if (tDamageParams.pCauser == nullptr)
	{
		vHitPosition = m_pOwner->GetKineticTargetPos();
	}
	
	vHitPosition.x += CMathUtils::RandomFloat(-1.f, 1.f);
	vHitPosition.y += CMathUtils::RandomFloat(-0.5f, 1.f);
	vHitPosition.z += CMathUtils::RandomFloat(-1.f, 1.f);

	if (iDamage == 0)
	{
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_UI, L"NoDamage")
		->Start_AttachOnlyPos(vHitPosition);
	}
	else
	{
		array<_int, 4> SaveNum;

		//10을 넣어주면 숫자가 안뜬다고 함
		SaveNum.fill(10);

		_int iCount = 0;
		while (iDamage != 0)
		{
			SaveNum[iCount++] = iDamage % 10;
			iDamage = iDamage / 10;
		}

		CEffectGroup* pFont = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_UI, L"AttackNum");
		pFont->Start_AttachOnlyPos(vHitPosition);

		pFont->GetFirstEffect()->GetParams().Float2s[0] = { _float(SaveNum[3]), 0.0f };
		pFont->GetSecondEffect()->GetParams().Float2s[0] = { _float(SaveNum[2]), 0.0f };
		pFont->GetThirdEffect()->GetParams().Float2s[0] = { _float(SaveNum[1]), 0.0f };
		pFont->GetFourthEffect()->GetParams().Float2s[0] = { _float(SaveNum[0]), 0.0f };
	}
	
}

void CEMUI::Create_CGUI()
{
//	_float4x4	pivot = XMMatrixIdentity();
	m_pCGEffect = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_UI, L"G_Monster");
	m_pCGEffect->Start_Attach(m_pOwner,  "Weak01", true, true);
	Safe_AddRef(m_pCGEffect);
}

void CEMUI::Update_NoticeNeon()
{
	_float4x4	NoticeNeonPivot = XMMatrixTranslation(0.f, 2.f, 0.f);

	if (m_pNoticNeon.first != nullptr && m_pNoticNeon.second != nullptr)
	{
		m_pNoticNeon.first->SetDelete();
		m_pNoticNeon.second->Delete_Particles();

		Safe_Release(m_pNoticNeon.first);
		Safe_Release(m_pNoticNeon.second);

		m_pNoticNeon.first = nullptr;
		m_pNoticNeon.second = nullptr;
	}

	switch (m_pOwner->GetDeBuffType())
	{
	case Client::EDeBuffType::DEBUFF_FIRE:
		m_pNoticNeon.first = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_UI, L"NoticeNeon_fire");
		m_pNoticNeon.second = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_HIT, L"NoticeNeon_Fire_Particle");
		break;

	case Client::EDeBuffType::DEBUFF_OIL:
		m_pNoticNeon.first = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_UI, L"NoticeNeon_oil");
		m_pNoticNeon.second = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_HIT, L"Notice_Neon_Oil_Particle");
		break;

	case Client::EDeBuffType::DEBUFF_THUNDER:
		m_pNoticNeon.first = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_UI, L"NoticeNeon_thunder");
		m_pNoticNeon.second = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_HIT, L"Notice_Neon_Thunder_Particle");
		break;

	case Client::EDeBuffType::DEBUFF_WATER:
		m_pNoticNeon.first = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_UI, L"NoticeNeon_water");
		m_pNoticNeon.second = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_HIT, L"Notice_Neon_Water_Particle");
		break;

	case Client::EDeBuffType::DEBUFF_END:
		break;
	default:
		break;
	}

	if (m_pNoticNeon.first != nullptr && m_pNoticNeon.second != nullptr)
	{
		m_pNoticNeon.first->Start_AttachPivot(m_pOwner, NoticeNeonPivot, "Target", true, true);
		m_pNoticNeon.second->Start_NoAttach(m_pNoticNeon.first->GetFirstEffect(), true, true);
		Safe_AddRef(m_pNoticNeon.first);
		Safe_AddRef(m_pNoticNeon.second);
	}

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
	if (m_pShieldUI != nullptr)
	{
		m_pShieldUI->SetDelete();
		m_pShieldUI = nullptr;
	}

	if (m_pHPUI != nullptr)
	{
		m_pHPUI->SetDelete();
		m_pHPUI = nullptr;
	}

	if (m_BossHp != nullptr)
	{
		m_BossHp->SetDelete();
		m_BossHp = nullptr;
	}

	if (m_pCGEffect != nullptr)
	{
		m_pCGEffect->SetDelete();
		Safe_Release(m_pCGEffect);
		m_pCGEffect = nullptr;
	}

	if (m_pNoticNeon.first != nullptr && m_pNoticNeon.second != nullptr)
	{
		m_pNoticNeon.first->SetDelete();
		m_pNoticNeon.second->Delete_Particles();

		Safe_Release(m_pNoticNeon.first);
		Safe_Release(m_pNoticNeon.second);

		m_pNoticNeon.first = nullptr;
		m_pNoticNeon.second = nullptr;
	}
}
