#include "stdafx.h"
#include "..\public\MonsterHpUI.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "EffectGroup.h"
#include "EffectSystem.h"
#include "VFX_Manager.h"
#include "Monster.h"

// Monster Name : m_pGroup->GetThirdEffect()->GetParams().Float2s[0]
// x : 1~20 레벨 입력할 때 (-1) 해서 넣어주면 된다. ex) 레벨 1을 주고 싶다면 1을 넣고, 내부에서는 x에 (-1) = 0을 넣어주면 결과로 레벨 1 출력
// y : [0] 브론욘 [1] 스커미 팡뒤 [2] 바일 풀 [3] 버디 러미 [4] 바스 포즈 [5] 경건 페리

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
	//m_pMonsterName = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"MonsterName", TEXT("Layer_MonsterUI"));

	Safe_AddRef(m_pGroup);
	//Safe_AddRef(m_pMonsterName);

	Assert(m_pGroup != nullptr);
	//Assert(m_pMonsterName != nullptr);
	//첫 인자에 넣어준 포인터의 뼈를 찾음.
	m_pGroup->Start_AttachPivot(m_pOwner, m_PivotMatrix, "Target", true, true);
	//m_pMonsterName->Start_AttachPivot(m_pOwner, m_PivotMatrix, "Target", true, true);

}

void CMonsterHpUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	if (m_pOwner != nullptr)
	{
		if (dynamic_cast<CMonster*>(m_pOwner)->IsDead())
		{
			m_bDelete = true;
			return;
		}

		_uint iHp = dynamic_cast<CMonster*>(m_pOwner)->GetHP();
		_uint iMaxHp = dynamic_cast<CMonster*>(m_pOwner)->GetMaxHP();

		m_fRatio = (_float)iHp / (_float)iMaxHp;
		m_fHpBack = m_fRatio;
		m_pGroup->GetThirdEffect()->GetParams().Floats[0] = m_fRatio;

		HpBack_Tick(TimeDelta);
	}
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
	{
		m_pGroup->SetDelete();
		Safe_Release(m_pGroup);
	}
	

	if (m_pMonsterName != nullptr)
	{
		m_pMonsterName->SetDelete();
		Safe_Release(m_pMonsterName);
	}
	
}