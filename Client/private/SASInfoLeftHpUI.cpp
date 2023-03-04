#include "stdafx.h"
#include "..\public\SASInfoLeftHpUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

CSASInfoLeftHpUI::CSASInfoLeftHpUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSASInfoLeftHpUI::CSASInfoLeftHpUI(const CSASInfoLeftHpUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSASInfoLeftHpUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASInfoLeftHpUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSASInfoLeftHpUI::BeginTick()
{


}

void CSASInfoLeftHpUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	//const _float2 canvaspos = m_pCanvas->Get_Position();

	//_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	//vPosition += XMLoadFloat2(&canvaspos);
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(vPosition, 1.f));


	
}

void CSASInfoLeftHpUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CSASInfoLeftHpUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASInfoLeftHpUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CSASInfoLeftHpUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CSASInfoLeftHpUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CSASInfoLeftHpUI * CSASInfoLeftHpUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASInfoLeftHpUI*		pInstance = new CSASInfoLeftHpUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASInfoLeftHpUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSASInfoLeftHpUI::Clone(void * pArg)
{
	CSASInfoLeftHpUI*		pInstance = new CSASInfoLeftHpUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASInfoLeftHpUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASInfoLeftHpUI::Free()
{
	CUI::Free();

}
