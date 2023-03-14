#include "stdafx.h"
#include "..\public\MonsterHpUI.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "EffectGroup.h"
#include "EffectSystem.h"

CMonsterHpUI::CMonsterHpUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMonsterHpUI::CMonsterHpUI(const CMonsterHpUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonsterHpUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterHpUI::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_InGameDataGroup/MonsterHp.json");
	m_pGroup = dynamic_cast<CEffectGroup*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_NOW, L"Layer_MonsterHp", L"ProtoVFX_EffectGroup", &json));

	Safe_AddRef(m_pGroup);
	Assert(m_pGroup != nullptr);

	return S_OK;
}

void CMonsterHpUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_pGroup->GetTransform()->CopyState(CTransform::STATE_TRANSLATION, m_pTransformCom);

	HpBack_Tick(TimeDelta);
}

void CMonsterHpUI::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	static _float fRatio;
	ImGui::DragFloat("Ratio", &fRatio);
	SetHP(fRatio);
}

void CMonsterHpUI::SetHP(_float fRatio)
{
	// 2: HpBack 3: Hp
	m_fHpBack = fRatio;
	//m_pGroup->GetSecondEffect()->GetParams().Floats[0] = fRatio;
	m_pGroup->GetThirdEffect()->GetParams().Floats[0] = fRatio;
}

void CMonsterHpUI::HpBack_Tick(const _double & TimeDelta)
{
	if (m_fCurrentHpBack > m_fHpBack)
	{
		m_fCurrentHpBack -= _float(TimeDelta) * 0.3f;
	}
	else
	{
		m_fCurrentHpBack = m_fHpBack;
	}

	m_pGroup->GetSecondEffect()->GetParams().Floats[0] = m_fCurrentHpBack;
}

CMonsterHpUI * CMonsterHpUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonsterHpUI*      pInstance = new CMonsterHpUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonsterHpUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMonsterHpUI::Clone(void * pArg)
{
	CMonsterHpUI*      pInstance = new CMonsterHpUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonsterHpUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonsterHpUI::Free()
{
	__super::Free();


	if (m_pGroup != nullptr && m_pGroup->IsDeleted() == false)
		m_pGroup->SetDelete();

	Safe_Release(m_pGroup);
}