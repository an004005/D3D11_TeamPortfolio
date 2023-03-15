#include "stdafx.h"
#include "..\public\MonsterLockonUI.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "EffectGroup.h"
#include "EffectSystem.h"
#include "VFX_Manager.h"
#include "Player.h"
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
	//�÷��̾�� ���͸� ����ִµ� Ÿ���õ� �������� ������ ����
	//enum FINISHFUNC { FUNC_PLAYFROMSTART, FUNC_RESET, FUNC_STOP, FUNC_REVERSE, FUNC_END };

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//���⼭ �޴��� �׷쿡 �� ����Ʈ�� �־���.
	m_pTargetGroup = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"Lockon_Target", TEXT("Layer_UI"));
	//m_pTargetRhombusGroup = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"Lockon_TargetRhombus", TEXT("Layer_UI"));

	Safe_AddRef(m_pTargetGroup);
	//Safe_AddRef(m_pTargetRhombusGroup);

	Assert(m_pTargetGroup != nullptr);
	//Assert(m_pTargetRhombusGroup != nullptr);

	//TimeLine ������ ���� : 2
	m_pTargetGroup->Start_Attach(m_pOwner, "Target", true);

	//TimeLine ������ ���� : 4
	//m_pTargetRhombusGroup->Start_Attach(m_pOwner, "Target", true);
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

	if(m_pTargetGroup != nullptr)
		m_pTargetGroup->SetDelete();

	Safe_Release(m_pTargetGroup);

	//if (m_pTargetRhombusGroup != nullptr)
	//	m_pTargetRhombusGroup->SetDelete();

	//Safe_Release(m_pTargetRhombusGroup);
}