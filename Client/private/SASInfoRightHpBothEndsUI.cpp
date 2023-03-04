#include "stdafx.h"
#include "..\public\SASInfoRightHpBothEndsUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

CSASInfoRightHpBothEndsUI::CSASInfoRightHpBothEndsUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSASInfoRightHpBothEndsUI::CSASInfoRightHpBothEndsUI(const CSASInfoRightHpBothEndsUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSASInfoRightHpBothEndsUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASInfoRightHpBothEndsUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSASInfoRightHpBothEndsUI::BeginTick()
{


}

void CSASInfoRightHpBothEndsUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	//const _float2 canvaspos = m_pCanvas->Get_Position();

	//_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	//vPosition += XMLoadFloat2(&canvaspos);
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(vPosition, 1.f));


	
}

void CSASInfoRightHpBothEndsUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CSASInfoRightHpBothEndsUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASInfoRightHpBothEndsUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CSASInfoRightHpBothEndsUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CSASInfoRightHpBothEndsUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CSASInfoRightHpBothEndsUI * CSASInfoRightHpBothEndsUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASInfoRightHpBothEndsUI*		pInstance = new CSASInfoRightHpBothEndsUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASInfoRightHpBothEndsUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSASInfoRightHpBothEndsUI::Clone(void * pArg)
{
	CSASInfoRightHpBothEndsUI*		pInstance = new CSASInfoRightHpBothEndsUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASInfoRightHpBothEndsUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASInfoRightHpBothEndsUI::Free()
{
	CUI::Free();

}
