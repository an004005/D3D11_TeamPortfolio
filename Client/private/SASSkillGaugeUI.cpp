#include "stdafx.h"
#include "..\public\SASSkillGaugeUI.h"
#include "GameInstance.h"

CSASSkillGaugeUI::CSASSkillGaugeUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSASSkillGaugeUI::CSASSkillGaugeUI(const CSASSkillGaugeUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSASSkillGaugeUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASSkillGaugeUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	///* For.Com_VIBuffer */
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_CircleRect"), TEXT("Com_VIBuffer_CRect"),
	//	(CComponent**)&m_pVIBufferCom)))
	//	return E_FAIL;

	return S_OK;
}

void CSASSkillGaugeUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

}

void CSASSkillGaugeUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CSASSkillGaugeUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASSkillGaugeUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CSASSkillGaugeUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CSASSkillGaugeUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CSASSkillGaugeUI * CSASSkillGaugeUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASSkillGaugeUI*		pInstance = new CSASSkillGaugeUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASSkillGaugeUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSASSkillGaugeUI::Clone(void * pArg)
{
	CSASSkillGaugeUI*		pInstance = new CSASSkillGaugeUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASSkillGaugeUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASSkillGaugeUI::Free()
{
	__super::Free();

}
