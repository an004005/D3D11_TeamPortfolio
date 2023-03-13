#include "stdafx.h"
#include "..\public\AttackNumUI.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "EffectGroup.h"
#include "EffectSystem.h"

CAttackNumUI::CAttackNumUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CAttackNumUI::CAttackNumUI(const CAttackNumUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CAttackNumUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAttackNumUI::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_InGameDataGroup/PsychokinesisGauge_G.json");
	m_pGroup = dynamic_cast<CEffectGroup*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_NOW, L"Layer_GravikenisisG", L"ProtoVFX_EffectGroup", &json));

	Safe_AddRef(m_pGroup);
	Assert(m_pGroup != nullptr);

	return S_OK;
}

void CAttackNumUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_pGroup->GetTransform()->CopyState(CTransform::STATE_TRANSLATION, m_pTransformCom);
}

void CAttackNumUI::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	static _float fRatio;
	ImGui::DragFloat("Ratio", &fRatio);
	SetfRatio(fRatio);
}

void CAttackNumUI::SetfRatio(const _float & fRatio)
{
	m_pGroup->GetSecondEffect()->GetParams().Floats[0] = fRatio;
	m_pGroup->GetThirdEffect()->GetParams().Floats[0] = fRatio;
}

CAttackNumUI * CAttackNumUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAttackNumUI*      pInstance = new CAttackNumUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAttackNumUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CAttackNumUI::Clone(void * pArg)
{
	CAttackNumUI*      pInstance = new CAttackNumUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAttackNumUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAttackNumUI::Free()
{
	__super::Free();

	m_pGroup->SetDelete();
	Safe_Release(m_pGroup);
}