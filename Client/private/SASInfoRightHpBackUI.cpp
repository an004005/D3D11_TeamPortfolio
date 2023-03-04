#include "stdafx.h"
#include "..\public\SASInfoRightHpBackUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

CSASInfoRightHpBackUI::CSASInfoRightHpBackUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSASInfoRightHpBackUI::CSASInfoRightHpBackUI(const CSASInfoRightHpBackUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSASInfoRightHpBackUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASInfoRightHpBackUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSASInfoRightHpBackUI::BeginTick()
{


}

void CSASInfoRightHpBackUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	//const _float2 canvaspos = m_pCanvas->Get_Position();

	//_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	//vPosition += XMLoadFloat2(&canvaspos);
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(vPosition, 1.f));


	
}

void CSASInfoRightHpBackUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CSASInfoRightHpBackUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASInfoRightHpBackUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CSASInfoRightHpBackUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CSASInfoRightHpBackUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CSASInfoRightHpBackUI * CSASInfoRightHpBackUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASInfoRightHpBackUI*		pInstance = new CSASInfoRightHpBackUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASInfoRightHpBackUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSASInfoRightHpBackUI::Clone(void * pArg)
{
	CSASInfoRightHpBackUI*		pInstance = new CSASInfoRightHpBackUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASInfoRightHpBackUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASInfoRightHpBackUI::Free()
{
	CUI::Free();

}
