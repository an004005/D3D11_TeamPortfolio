#include "stdafx.h"
#include "..\public\SASInfoLeftHpBackUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

CSASInfoLeftHpBackUI::CSASInfoLeftHpBackUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSASInfoLeftHpBackUI::CSASInfoLeftHpBackUI(const CSASInfoLeftHpBackUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSASInfoLeftHpBackUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASInfoLeftHpBackUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CSASInfoLeftHpBackUI::BeginTick()
{


}

void CSASInfoLeftHpBackUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	//const _float2 canvaspos = m_pCanvas->Get_Position();

	//_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	//vPosition += XMLoadFloat2(&canvaspos);
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(vPosition, 1.f));


	
}

void CSASInfoLeftHpBackUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CSASInfoLeftHpBackUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASInfoLeftHpBackUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CSASInfoLeftHpBackUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CSASInfoLeftHpBackUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CSASInfoLeftHpBackUI * CSASInfoLeftHpBackUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASInfoLeftHpBackUI*		pInstance = new CSASInfoLeftHpBackUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASInfoLeftHpBackUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSASInfoLeftHpBackUI::Clone(void * pArg)
{
	CSASInfoLeftHpBackUI*		pInstance = new CSASInfoLeftHpBackUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASInfoLeftHpBackUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASInfoLeftHpBackUI::Free()
{
	CUI::Free();

}
