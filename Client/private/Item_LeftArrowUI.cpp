#include "stdafx.h"
#include "..\public\Item_LeftArrowUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

CItem_LeftArrowUI::CItem_LeftArrowUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CItem_LeftArrowUI::CItem_LeftArrowUI(const CItem_LeftArrowUI& rhs)
	: CUI(rhs)
{
}

HRESULT CItem_LeftArrowUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_LeftArrowUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CItem_LeftArrowUI::BeginTick()
{


}

void CItem_LeftArrowUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	//const _float2 canvaspos = m_pCanvas->Get_Position();

	//_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	//vPosition += XMLoadFloat2(&canvaspos);
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(vPosition, 1.f));


	
}

void CItem_LeftArrowUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CItem_LeftArrowUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CItem_LeftArrowUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CItem_LeftArrowUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CItem_LeftArrowUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CItem_LeftArrowUI * CItem_LeftArrowUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CItem_LeftArrowUI*		pInstance = new CItem_LeftArrowUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItem_LeftArrowUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CItem_LeftArrowUI::Clone(void * pArg)
{
	CItem_LeftArrowUI*		pInstance = new CItem_LeftArrowUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CItem_LeftArrowUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CItem_LeftArrowUI::Free()
{
	CUI::Free();

}
