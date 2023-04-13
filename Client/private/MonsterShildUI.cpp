#include "stdafx.h"
#include "..\public\MonsterShildUI.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "EffectSystem.h"
#include "VFX_Manager.h"

CMonsterShildUI::CMonsterShildUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMonsterShildUI::CMonsterShildUI(const CMonsterShildUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonsterShildUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterShildUI::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	//Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_InGameDataGroup/MonsterShild.json");
	//m_pGroup = dynamic_cast<CEffectGroup*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_NOW, L"Layer_MonsterShild", L"ProtoVFX_EffectGroup", &json));

	//Safe_AddRef(m_pGroup);
	//Assert(m_pGroup != nullptr);

	return S_OK;
}

void CMonsterShildUI::BeginTick()
{
	m_pGroup = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"MonsterShild", PLAYERTEST_LAYER_FRONTUI);
	m_pMonsterName = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"BossName", PLAYERTEST_LAYER_FRONTUI);

	Safe_AddRef(m_pGroup);
	Safe_AddRef(m_pMonsterName);

	Assert(m_pGroup != nullptr);
	Assert(m_pMonsterName != nullptr);

	_float4x4	EyesPivot = XMMatrixTranslation(0.f, 0.5f, 0.f);
	m_pGroup->Start_AttachPivot(m_pOwner, EyesPivot, "Target", true, true);
	m_pMonsterName->Start_AttachPivot(m_pOwner, EyesPivot, "Target", true, true);

	m_pMonsterName->GetSecondEffect()->GetParams().Float2s[0] = { _float(m_iMonsterLevel - 1), _float(m_eMonsterName) };
}

void CMonsterShildUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	//m_pGroup->GetTransform()->CopyState(CTransform::STATE_TRANSLATION, m_pTransformCom);

	if (m_pGroup == nullptr) return;

	HpBack_Tick(TimeDelta);
	//Broken_Tick(TimeDelta);
}

void CMonsterShildUI::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	static _float fHp;
	static _float fShild;
	ImGui::DragFloat("Hp", &fHp);
	ImGui::DragFloat("Shild", &fShild);

	if (ImGui::Button("Set")) {
		SetShild(fHp, fShild);
	}
}

void CMonsterShildUI::Set_MonsterInfo(const _int iLevel, const EEnemyName eName)
{
	m_iMonsterLevel = iLevel;
	m_eMonsterName = eName;
}

void CMonsterShildUI::SetShild(const _float & fHP, const _float & fShild)
{
	// 2: Hp, 3: HpBack, 4: Shild
	//m_pGroup->GetSecondEffect()->GetParams().Floats[0] = fHP;
	//m_fHpBack = fHP;
	if (m_pGroup == nullptr) return;

	m_pGroup->GetThirdEffect()->GetParams().Floats[0] = fHP;
	m_pGroup->GetFourthEffect()->GetParams().Floats[0] = fShild;
	
	m_fHpBack = fHP;
	//m_fShild = fShild;
}

void CMonsterShildUI::Broken_Tick(const _double & TimeDelta)
{
	if (0.0f > m_fShild)
		m_bBroken = true;

	if (false == m_bBroken) return;

	if (0 == m_dBroken_TimeAcc)
		m_pGroup->GetFifthEffect()->GetParams().Ints[0] = 3;

	m_dBroken_TimeAcc += TimeDelta;
	if (1.0 < m_dBroken_TimeAcc)
	{
		m_bBroken = false;
		m_dBroken_TimeAcc = 0.0;
		m_pGroup->GetFifthEffect()->GetParams().Ints[0] = 1;
	}
}

void CMonsterShildUI::HpBack_Tick(const _double & TimeDelta)
{
	if (m_fCurrentHpBack > m_fHpBack)
	{
		m_fCurrentHpBack -= _float(TimeDelta) * 0.2f;
	}
	else
	{
		m_fCurrentHpBack = m_fHpBack;
	}

	m_pGroup->GetSecondEffect()->GetParams().Floats[0] = m_fCurrentHpBack;
}

CMonsterShildUI * CMonsterShildUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonsterShildUI*      pInstance = new CMonsterShildUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonsterShildUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMonsterShildUI::Clone(void * pArg)
{
	CMonsterShildUI*      pInstance = new CMonsterShildUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonsterShildUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonsterShildUI::Free()
{
	__super::Free();

	if (m_bCloned)
	{
		if (m_pGroup != nullptr)
		{
			m_pGroup->SetDelete();
			Safe_Release(m_pGroup);
			m_pGroup = nullptr;
		}

		if (m_pMonsterName != nullptr)
		{
			m_pMonsterName->SetDelete();
			Safe_Release(m_pMonsterName);
			m_pMonsterName = nullptr;
		}
	}
}