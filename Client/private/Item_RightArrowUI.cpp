#include "stdafx.h"
#include "..\public\Item_RightArrowUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

CItem_RightArrowUI::CItem_RightArrowUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CItem_RightArrowUI::CItem_RightArrowUI(const CItem_RightArrowUI& rhs)
	: CUI(rhs)
{
}

HRESULT CItem_RightArrowUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_RightArrowUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CItem_RightArrowUI::BeginTick()
{


}

void CItem_RightArrowUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	//const _float2 canvaspos = m_pCanvas->Get_Position();

	//_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	//vPosition += XMLoadFloat2(&canvaspos);
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(vPosition, 1.f));


	
}

void CItem_RightArrowUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CItem_RightArrowUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CItem_RightArrowUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CItem_RightArrowUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CItem_RightArrowUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CItem_RightArrowUI * CItem_RightArrowUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CItem_RightArrowUI*		pInstance = new CItem_RightArrowUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItem_RightArrowUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CItem_RightArrowUI::Clone(void * pArg)
{
	CItem_RightArrowUI*		pInstance = new CItem_RightArrowUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CItem_RightArrowUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CItem_RightArrowUI::Free()
{
	CUI::Free();

}
