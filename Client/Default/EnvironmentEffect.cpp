#include "stdafx.h"
#include "EnvironmentEffect.h"
#include "JsonLib.h"
#include "EffectGroup.h"
#include "ParticleGroup.h"
#include "VFX_Manager.h"

CEnvironmentEffect::CEnvironmentEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEnvironmentEffect::CEnvironmentEffect(const CEnvironmentEffect& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnvironmentEffect::Initialize(void* pArg)
{
	FAILED_CHECK(CGameObject::Initialize(pArg));
	SpawnEffect();

	return S_OK;
}

void CEnvironmentEffect::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();

    static array<const char*, ENVEFFECT_END> EnvEffectTypeNames{
        "Env_Crush_1_1", "Env_Crush_1_2", "Env_Spark_Particle_10Sec", "Boss_WaterFall"
    };
    if (ImGui::BeginCombo("Env Effect Seletor", EnvEffectTypeNames[m_eType]))
    {
        for (int i = 0; i < ENVEFFECT_END; ++i)
        {
            const bool bSelected = false;
            if (ImGui::Selectable(EnvEffectTypeNames[i], bSelected))
                m_eType = static_cast<EEnvironmentEffectType>(i);
        }
        ImGui::EndCombo();
    }
	if (ImGui::Button("Re SpawnEffect"))
	{
		SpawnEffect();
	}
}

void CEnvironmentEffect::SaveToJson(Json& json)
{
	CGameObject::SaveToJson(json);
	json["Type"] = m_eType;
}

void CEnvironmentEffect::LoadFromJson(const Json& json)
{
	CGameObject::LoadFromJson(json);
	m_eType = json["Type"];
}

void CEnvironmentEffect::SpawnEffect()
{
	Clear();

	switch (m_eType)
	{
	case Env_Crush_1_1:
		m_pEffectGroup = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ENVIRENMENT, L"Env_Crush_1_1");
		m_pEffectGroup->Start_NoAttach(this, true);
		Safe_AddRef(m_pEffectGroup);
		break;
	case Env_Crush_1_2: 
		m_pEffectGroup = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_ENVIRENMENT, L"Env_Crush_1_2");
		m_pEffectGroup->Start_NoAttach(this, true);
		Safe_AddRef(m_pEffectGroup);
		break;
	case Env_Spark_Particle_10Sec:
		m_pParticleGroup = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_ENVIRENMENT, L"Env_Spark_Particle_10Sec");
		m_pParticleGroup->Start_NoAttach(this, true);
		Safe_AddRef(m_pParticleGroup);
		break;
	case Boss_WaterFall:
		m_pEffectGroup = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"Boss_WaterFall");
		m_pEffectGroup->Start_NoAttach(this, true);
		Safe_AddRef(m_pEffectGroup);
		break;
	case ENVEFFECT_END: 
		FALLTHROUGH;
	default:
		break;
	}
}

void CEnvironmentEffect::Clear()
{
	if (m_pEffectGroup)
	{
		m_pEffectGroup->SetDelete();
		Safe_Release(m_pEffectGroup);
		m_pEffectGroup = nullptr;
	}

	if (m_pParticleGroup)
	{
		m_pParticleGroup->Delete_Particles();
		Safe_Release(m_pParticleGroup);
		m_pParticleGroup = nullptr;
	}
}

void CEnvironmentEffect::Free()
{
	CGameObject::Free();
	Clear();
}

CEnvironmentEffect* CEnvironmentEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnvironmentEffect* pInstance = new CEnvironmentEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEnvironmentEffect");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CEnvironmentEffect::Clone(void* pArg)
{
	CEnvironmentEffect* pInstance = new CEnvironmentEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEnvironmentEffect");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}
