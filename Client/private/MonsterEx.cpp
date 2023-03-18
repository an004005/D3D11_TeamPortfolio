#include "stdafx.h"
#include "..\public\MonsterEx.h"
#include "Model.h"
#include "Animation.h"
#include "Renderer.h"

vector<wstring>			CMonsterEx::s_vecDefaultBlood{
	L"Default_Blood_00",
	L"Default_Blood_01",
	L"Default_Blood_02",
	L"Default_Blood_03",
	L"Default_Blood_04",
	L"Default_Blood_05",
	L"Default_Blood_06"
};
vector<wstring>			CMonsterEx::s_vecFireBlood{
	L"Fire_Blood_00",
	L"Fire_Blood_01",
	L"Fire_Blood_02",
	L"Fire_Blood_03",
	L"Fire_Blood_04",
	L"Fire_Blood_05",
	L"Fire_Blood_06"
};
vector<wstring>			CMonsterEx::s_vecElecBlood{
	L"Elec_Blood_00",
	L"Elec_Blood_01",
	L"Elec_Blood_02",
	L"Elec_Blood_03",
	L"Elec_Blood_04",
	L"Elec_Blood_05",
	L"Elec_Blood_06",
};

vector<wstring>			CMonsterEx::s_vecDefaultHit{
	L"Default_Attack_OnHit_00",
	L"Default_Attack_OnHit_01"
};
vector<wstring>			CMonsterEx::s_vecFireHit{
	L"Fire_Attack_OnHit_00",
	L"Fire_Attack_OnHit_01"
};
vector<wstring>			CMonsterEx::s_vecElecHit{
	L"Elec_Attack_OnHit_00",
	L"Elec_Attack_OnHit_01"
};

CMonsterEx::CMonsterEx(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CScarletCharacter(pDevice, pContext)
{
}

CMonsterEx::CMonsterEx(const CScarletCharacter& rhs)
	: CScarletCharacter(rhs)
{
}

HRESULT CMonsterEx::Initialize(void* pArg)
{
	FAILED_CHECK(CScarletCharacter::Initialize(pArg));
	m_DeathTimeline.SetFinishFunction([this]
	{
		m_bDelete = true;
	});
	m_DeathTimeline.SetCurve("Simple_Increase");

	SetUpComponents(pArg);
	SetupSound();


	return S_OK;
}

void CMonsterEx::Tick(_double TimeDelta)
{
	CScarletCharacter::Tick(TimeDelta);
}

void CMonsterEx::Late_Tick(_double TimeDelta)
{
	CScarletCharacter::Late_Tick(TimeDelta);
	if (m_bVisible)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
}

void CMonsterEx::Imgui_RenderProperty()
{
	CScarletCharacter::Imgui_RenderProperty();
	m_DeathTimeline.Imgui_RenderEditor();

	if (ImGui::CollapsingHeader("Anim Fast Modifier"))
	{
		static string szMonsterAnimName = "";
		if (ImGui::BeginListBox("Animation List"))
		{
		  static char szSeachAnim[MAX_PATH] = "";
		  ImGui::InputText("Anim Search", szSeachAnim, MAX_PATH);

		  const string strSearch = szSeachAnim;
		  const _bool bSearch = strSearch.empty() == false;

		  for (auto& Pair : m_pModelCom->Get_AnimList())
		  {
		     if (bSearch)
		     {
		        if (Pair.first.find(strSearch) == string::npos)
		           continue;
		     }

		     const bool bSelected = szMonsterAnimName == Pair.first;
		     if (bSelected)
		        ImGui::SetItemDefaultFocus();

		     if (ImGui::Selectable(Pair.first.c_str(), bSelected))
		        szMonsterAnimName = Pair.first;
		  }
		  ImGui::EndListBox();
		}

		if ("" != szMonsterAnimName)
		  m_pModelCom->Get_AnimList()[szMonsterAnimName]->Imgui_RenderProperty();
	}
}

HRESULT CMonsterEx::Render_ShadowDepth()
{
	m_pModelCom->Render_ShadowDepth(m_pTransformCom);
	return S_OK;
}

void CMonsterEx::SetUpComponents(void* pArg)
{
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));
}

void CMonsterEx::SetupSound()
{
	if (m_strDeathSoundTag.empty() == false)
		m_SoundStore.CloneSound(m_strDeathSoundTag);
	if (m_strImpactTag.empty() == false)
		m_SoundStore.CloneSound(m_strImpactTag);
	if (m_strImpactVoiceTag.empty() == false)
		m_SoundStore.CloneSound(m_strImpactVoiceTag);
}

void CMonsterEx::TakeDamage(DAMAGE_PARAM tDamageParams)
{
	wstring HitBloodName;
	wstring HitEffectName;

	switch (tDamageParams.eAttackSAS)
	{
	case ESASType::SAS_FIRE:
		HitBloodName = s_vecFireBlood[CMathUtils::RandomUInt(s_vecFireBlood.size() - 1)];
		HitEffectName = s_vecFireHit[CMathUtils::RandomUInt(s_vecFireHit.size() - 1)];
		break;

	case ESASType::SAS_ELETRIC:
		HitBloodName = s_vecElecBlood[CMathUtils::RandomUInt(s_vecElecBlood.size() - 1)];
		HitEffectName = s_vecElecHit[CMathUtils::RandomUInt(s_vecElecHit.size() - 1)];
		break;

	case ESASType::SAS_NOT:
		HitBloodName = s_vecDefaultBlood[CMathUtils::RandomUInt(s_vecDefaultBlood.size() - 1)];
		HitEffectName = s_vecDefaultHit[CMathUtils::RandomUInt(s_vecDefaultHit.size() - 1)];
		break;
	}
	

	_float4 vHitPos = _float4(tDamageParams.vHitPosition.x, tDamageParams.vHitPosition.y, tDamageParams.vHitPosition.z, 0.f);
	_float4 vEffectDir = _float4(tDamageParams.vSlashVector.x, tDamageParams.vSlashVector.y, tDamageParams.vSlashVector.z, 0.f);

	if (HitBloodName.empty() == false)
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_HIT, HitBloodName)->Start_AttachPosition(this, vHitPos, vEffectDir);
	if (HitEffectName.empty() == false)
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_HIT, HitEffectName)->Start_AttachPosition(this, vHitPos, vEffectDir);

	m_SoundStore.PlaySound(m_strImpactTag, &vHitPos);
	if (m_strImpactVoiceTag.empty() == false)
		m_SoundStore.PlaySound(m_strImpactVoiceTag, &vHitPos);
}

void CMonsterEx::Free()
{
	CScarletCharacter::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
