#include "stdafx.h"
#include "..\public\MonsterLockonUI.h"
#include "GameInstance.h"
#include "VFX_Manager.h"
#include "Monster.h"

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


	return S_OK;
}

void CMonsterLockonUI::BeginTick()
{
	//플레이어에서 몬스터를 들고있는데 타겟팅된 몬스터의의 정보를 들고옴
	//enum FINISHFUNC { FUNC_PLAYFROMSTART, FUNC_RESET, FUNC_STOP, FUNC_REVERSE, FUNC_END };

	//여기서 메니저 그룹에 내 이펙트를 넣어줌.
	m_pTargetGroup = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"Lockon_Target", PLAYERTEST_LAYER_FRONTUI);

	Safe_AddRef(m_pTargetGroup);

	Assert(m_pTargetGroup != nullptr);

	//TimeLine 끝나고 유지 : 2
	m_pTargetGroup->Start_Attach(m_pOwner, m_strBoneName, true, true);

	//TimeLine 끝나고 삭제 : 4
}

void CMonsterLockonUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

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

	if(m_pTargetGroup != nullptr && m_pTargetGroup->IsDeleted() == false)
		m_pTargetGroup->SetDelete();

	if (m_bCloned)
	{
		if (m_pTargetGroup != nullptr)
		{
			m_pTargetGroup->SetDelete();
			Safe_Release(m_pTargetGroup);
			m_pTargetGroup = nullptr;
		}
	}
	
}