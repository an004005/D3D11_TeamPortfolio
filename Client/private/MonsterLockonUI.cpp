#include "stdafx.h"
#include "..\public\MonsterLockonUI.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "EffectGroup.h"
#include "EffectSystem.h"
#include "VFX_Manager.h"
#include "Player.h"

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

	//플레이어에서 몬스터를 들고있는데 타겟팅된 몬스터의의 정보를 들고와서 거기서 붙일 뼈를 셋팅해주게한다.

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	m_pTargetGroup = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"Lockon_Target", TEXT("Layer_MonsterUI"));
	m_pTargetRhombusGroup = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"Lockon_Target", TEXT("Lockon_TargetRhombus"));

	Safe_AddRef(m_pTargetGroup);
	Safe_AddRef(m_pTargetRhombusGroup);

	Assert(m_pTargetGroup != nullptr);
	Assert(m_pTargetRhombusGroup != nullptr);

	//m_pTargetGroup->Start_Attach(this, "Target", true);
	//m_pTargetRhombusGroup->Start_Attach(this, "Target", true);
	return S_OK;
}

void CMonsterLockonUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_pTargetGroup->GetTransform()->CopyState(CTransform::STATE_TRANSLATION, m_pTransformCom);
	//m_pTargetRhombusGroup->GetTransform()->CopyState(CTransform::STATE_TRANSLATION, m_pTransformCom);
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