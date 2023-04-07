#include "stdafx.h"
#include "..\public\MonsterHpUI.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "EffectSystem.h"
#include "VFX_Manager.h"
#include "Enemy.h"

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
	if (m_eMonsterName == EEnemyName::EM0320 || m_eMonsterName == EEnemyName::EM0800 || 
		m_eMonsterName == EEnemyName::EM1100 || m_eMonsterName == EEnemyName::EM1200 ||
		m_eMonsterName == EEnemyName::EM8210)
	{
		m_pGroup = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"BossHp", PLAYERTEST_LAYER_FRONTUI);
		m_pMonsterName = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"BossName", PLAYERTEST_LAYER_FRONTUI);
	}
	else
	{
		m_pGroup = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"MonsterHp", PLAYERTEST_LAYER_FRONTUI);
		m_pMonsterName = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"MonsterName", PLAYERTEST_LAYER_FRONTUI);
	}

	Safe_AddRef(m_pGroup);
	Safe_AddRef(m_pMonsterName);

	Assert(m_pGroup != nullptr);
	Assert(m_pMonsterName != nullptr);

	//첫 인자에 넣어준 포인터의 뼈를 찾음.
	m_pGroup->Start_AttachPivot(m_pOwner, m_PivotMatrix, "Target", true, true);
	m_pMonsterName->Start_AttachPivot(m_pOwner, m_PivotMatrix, "Target", true, true);

	m_pMonsterName->GetSecondEffect()->GetParams().Float2s[0] = { _float(m_iMonsterLevel - 1), _float(m_eMonsterName) };
}

void CMonsterHpUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	if (m_pGroup == nullptr) return;
	HpBack_Tick(TimeDelta);
}

void CMonsterHpUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

}

HRESULT CMonsterHpUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CMonsterHpUI::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	//ImGui::DragFloat("Ratio", &m_fRatio);
}

void CMonsterHpUI::Set_MonsterInfo(const _int iLevel, const EEnemyName eName)
{
	m_iMonsterLevel = iLevel;
	m_eMonsterName = eName;
}

void CMonsterHpUI::Set_HpRatio(_float fHpRatio)
{
	m_pGroup->GetThirdEffect()->GetParams().Floats[0] = fHpRatio;
	m_fHpBack = fHpRatio;
}

void CMonsterHpUI::HpBack_Tick(const _double & TimeDelta)
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

	if (m_bCloned)
	{
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
	
}