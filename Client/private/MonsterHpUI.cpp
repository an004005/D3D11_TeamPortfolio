#include "stdafx.h"
#include "..\public\MonsterHpUI.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "EffectGroup.h"
#include "EffectSystem.h"
#include "VFX_Manager.h"
#include "Monster.h"
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

	/*Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_InGameDataGroup/MonsterHp.json");
	m_pGroup = dynamic_cast<CEffectGroup*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_NOW, L"Layer_MonsterHp", L"ProtoVFX_EffectGroup", &json));
	*/
	return S_OK;
}

void CMonsterHpUI::BeginTick()
{
	m_pGroup = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"MonsterHp", TEXT("Layer_MonsterUI"));
	Safe_AddRef(m_pGroup);
	Assert(m_pGroup != nullptr);

	//첫 인자에 넣어준 포인터의 뼈를 찾음.
	//m_pGroup->Start_Attach(m_pOwner, "Target_end", true);
	m_pGroup->Start_AttachPivot(m_pOwner, m_PivotMatrix, "Target", true, true);
}

void CMonsterHpUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_pOwner != nullptr)
	{
		_uint iHp = dynamic_cast<CMonster*>(m_pOwner)->GetHP();
		_uint iMaxHp = dynamic_cast<CMonster*>(m_pOwner)->GetMaxHP();

		if (iHp <= 0)
		{
			m_bDelete = true;
			return;
		}

		m_fRatio = (_float)iHp / (_float)iMaxHp;
		m_fHpBack = m_fRatio;
		m_pGroup->GetThirdEffect()->GetParams().Floats[0] = m_fRatio;
	}

	HpBack_Tick(TimeDelta);
}

void CMonsterHpUI::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	ImGui::DragFloat("Ratio", &m_fRatio);
	
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

	if (m_pGroup != nullptr)
		m_pGroup->SetDelete();

	Safe_Release(m_pGroup);
}