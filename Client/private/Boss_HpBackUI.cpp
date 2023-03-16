#include "stdafx.h"
#include "..\public\Boss_HpBackUI.h"
#include "GameInstance.h"

CBoss_HpBackUI::CBoss_HpBackUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CBoss_HpBackUI::CBoss_HpBackUI(const CBoss_HpBackUI& rhs)
	: CUI(rhs)
{
}

HRESULT CBoss_HpBackUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_HpBackUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBoss_HpBackUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	
	if (m_fCurrentHp > m_fHp)
	{
		m_fCurrentHp -= _float(TimeDelta) * 0.1f;
		m_tParams.Floats[0] = m_fCurrentHp;
	}
	else
	{
		m_fCurrentHp = m_fHp;
	}
}

void CBoss_HpBackUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CBoss_HpBackUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CBoss_HpBackUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CBoss_HpBackUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CBoss_HpBackUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CBoss_HpBackUI::Set_BossHp(const _float & fHp)
{
	if (0.0f >= fHp)
	{
		m_bVisible = false;
		return;
	}

	m_fHp = fHp;
}

CBoss_HpBackUI * CBoss_HpBackUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CBoss_HpBackUI*		pInstance = new CBoss_HpBackUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBoss_HpBackUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CBoss_HpBackUI::Clone(void * pArg)
{
	CBoss_HpBackUI*		pInstance = new CBoss_HpBackUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss_HpBackUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss_HpBackUI::Free()
{
	__super::Free();

}
