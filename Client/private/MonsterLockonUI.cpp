#include "stdafx.h"
#include "..\public\MonsterLockonUI.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "EffectGroup.h"
#include "EffectSystem.h"

CMonsterLockonUI::CMonsterLockonUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMonsterLockonUI::CMonsterLockonUI(const CMonsterLockonUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonsterLockonUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterLockonUI::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_InGameDataGroup/Lockon_Target.json");
	m_pTargetGroup = dynamic_cast<CEffectGroup*>(pGameInstance->Clone_GameObject_Get(LEVEL_NOW, L"Layer_Lockon", L"ProtoVFX_EffectGroup", &json));

	json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_InGameDataGroup/Lockon_TargetRhombus.json");
	m_pTargetRhombusGroup = dynamic_cast<CEffectGroup*>(pGameInstance->Clone_GameObject_Get(LEVEL_NOW, L"Layer_Lockon", L"ProtoVFX_EffectGroup", &json));

	Safe_AddRef(m_pTargetGroup);
	Assert(m_pTargetGroup != nullptr);
	Safe_AddRef(m_pTargetRhombusGroup);
	Assert(m_pTargetRhombusGroup != nullptr);

	return S_OK;
}

void CMonsterLockonUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_pTargetGroup->GetTransform()->CopyState(CTransform::STATE_TRANSLATION, m_pTransformCom);
	m_pTargetRhombusGroup->GetTransform()->CopyState(CTransform::STATE_TRANSLATION, m_pTransformCom);
}

void CMonsterLockonUI::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

}

CMonsterLockonUI * CMonsterLockonUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonsterLockonUI*      pInstance = new CMonsterLockonUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMonsterLockonUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMonsterLockonUI::Clone(void * pArg)
{
	CMonsterLockonUI*      pInstance = new CMonsterLockonUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMonsterLockonUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonsterLockonUI::Free()
{
	__super::Free();

	m_pTargetGroup->SetDelete();
	Safe_Release(m_pTargetGroup);
	m_pTargetRhombusGroup->SetDelete();
	Safe_Release(m_pTargetRhombusGroup);
}