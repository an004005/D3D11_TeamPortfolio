#include "stdafx.h"
#include "..\public\SASInfoRightHpUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

CSASInfoRightHpUI::CSASInfoRightHpUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSASInfoRightHpUI::CSASInfoRightHpUI(const CSASInfoRightHpUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSASInfoRightHpUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASInfoRightHpUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSASInfoRightHpUI::BeginTick()
{


}

void CSASInfoRightHpUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	//const _float2 canvaspos = m_pCanvas->Get_Position();

	//_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	//vPosition += XMLoadFloat2(&canvaspos);
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(vPosition, 1.f));


	
}

void CSASInfoRightHpUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CSASInfoRightHpUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASInfoRightHpUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CSASInfoRightHpUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CSASInfoRightHpUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CSASInfoRightHpUI * CSASInfoRightHpUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASInfoRightHpUI*		pInstance = new CSASInfoRightHpUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASInfoRightHpUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSASInfoRightHpUI::Clone(void * pArg)
{
	CSASInfoRightHpUI*		pInstance = new CSASInfoRightHpUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASInfoRightHpUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASInfoRightHpUI::Free()
{
	CUI::Free();

}
