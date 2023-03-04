#include "stdafx.h"
#include "..\public\SASInfoLeftHpBothEndsUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

CSASInfoLeftHpBothEndsUI::CSASInfoLeftHpBothEndsUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSASInfoLeftHpBothEndsUI::CSASInfoLeftHpBothEndsUI(const CSASInfoLeftHpBothEndsUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSASInfoLeftHpBothEndsUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASInfoLeftHpBothEndsUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSASInfoLeftHpBothEndsUI::BeginTick()
{


}

void CSASInfoLeftHpBothEndsUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	//const _float2 canvaspos = m_pCanvas->Get_Position();

	//_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	//vPosition += XMLoadFloat2(&canvaspos);
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(vPosition, 1.f));


	
}

void CSASInfoLeftHpBothEndsUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CSASInfoLeftHpBothEndsUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASInfoLeftHpBothEndsUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CSASInfoLeftHpBothEndsUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CSASInfoLeftHpBothEndsUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CSASInfoLeftHpBothEndsUI * CSASInfoLeftHpBothEndsUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASInfoLeftHpBothEndsUI*		pInstance = new CSASInfoLeftHpBothEndsUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASInfoLeftHpBothEndsUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSASInfoLeftHpBothEndsUI::Clone(void * pArg)
{
	CSASInfoLeftHpBothEndsUI*		pInstance = new CSASInfoLeftHpBothEndsUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASInfoLeftHpBothEndsUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASInfoLeftHpBothEndsUI::Free()
{
	CUI::Free();

}
