#include "stdafx.h"
#include "..\public\Item_NameUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

CItem_NameUI::CItem_NameUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CItem_NameUI::CItem_NameUI(const CItem_NameUI& rhs)
	: CUI(rhs)
{
}

HRESULT CItem_NameUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_NameUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CItem_NameUI::BeginTick()
{


}

void CItem_NameUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	//const _float2 canvaspos = m_pCanvas->Get_Position();

	//_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	//vPosition += XMLoadFloat2(&canvaspos);
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(vPosition, 1.f));


	
}

void CItem_NameUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CItem_NameUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CItem_NameUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CItem_NameUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CItem_NameUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CItem_NameUI * CItem_NameUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CItem_NameUI*		pInstance = new CItem_NameUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItem_NameUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CItem_NameUI::Clone(void * pArg)
{
	CItem_NameUI*		pInstance = new CItem_NameUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CItem_NameUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CItem_NameUI::Free()
{
	CUI::Free();

}
