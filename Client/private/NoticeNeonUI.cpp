#include "stdafx.h"
#include "..\public\NoticeNeonUI.h"
#include "GameInstance.h"
#include "JsonStorage.h"

#include "EffectSystem.h"
#include "VFX_Manager.h"

CNoticeNeonUI::CNoticeNeonUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CNoticeNeonUI::CNoticeNeonUI(const CNoticeNeonUI & rhs)
	: CGameObject(rhs)
{
}

HRESULT CNoticeNeonUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNoticeNeonUI::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	

	return S_OK;
}

void CNoticeNeonUI::BeginTick()
{
	m_dDead_TimeAcc = 0.0;

	m_pNoticeNeonGroup = CVFX_Manager::GetInstance()->GetEffect(EF_UI, s2ws(m_strNoticeNeon), TEXT("Layer_NoticeNeonUI"));
	Safe_AddRef(m_pNoticeNeonGroup);
	Assert(m_pNoticeNeonGroup != nullptr);
	m_pNoticeNeonGroup->Start_Attach(m_pOwner, "Target", true, true);
}

void CNoticeNeonUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	Dead_Tick(TimeDelta);
}

void CNoticeNeonUI::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	
}

void CNoticeNeonUI::LoadFromJson(const Json & json)
{
	if (json.contains("NoticeNeon") == false)
		Assert(!"No NoticeNeon");

	m_strNoticeNeon = json["NoticeNeon"];


}

void CNoticeNeonUI::Dead_Tick(const _double & TimeDelta)
{
	if (true == m_dKill)
		return;

	m_dDead_TimeAcc += TimeDelta;
	if (3.0 < m_dDead_TimeAcc)
	{
		m_dKill = true;
	}
}

CNoticeNeonUI * CNoticeNeonUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNoticeNeonUI*      pInstance = new CNoticeNeonUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNoticeNeonUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CNoticeNeonUI::Clone(void * pArg)
{
	CNoticeNeonUI*      pInstance = new CNoticeNeonUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNoticeNeonUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNoticeNeonUI::Free()
{
	__super::Free();

	if (m_pNoticeNeonGroup != nullptr)
	{
		m_pNoticeNeonGroup->SetDelete();
		Safe_Release(m_pNoticeNeonGroup);
	}


}