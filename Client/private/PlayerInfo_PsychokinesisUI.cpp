#include "stdafx.h"
#include "..\public\PlayerInfo_PsychokinesisUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

CPlayerInfo_PsychokinesisUI::CPlayerInfo_PsychokinesisUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CPlayerInfo_PsychokinesisUI::CPlayerInfo_PsychokinesisUI(const CPlayerInfo_PsychokinesisUI& rhs)
	: CUI(rhs)
{
}

HRESULT CPlayerInfo_PsychokinesisUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerInfo_PsychokinesisUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayerInfo_PsychokinesisUI::BeginTick()
{


}

void CPlayerInfo_PsychokinesisUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	//const _float2 canvaspos = m_pCanvas->Get_Position();

	//_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	//vPosition += XMLoadFloat2(&canvaspos);
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSetW(vPosition, 1.f));


	
}

void CPlayerInfo_PsychokinesisUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CPlayerInfo_PsychokinesisUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CPlayerInfo_PsychokinesisUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CPlayerInfo_PsychokinesisUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CPlayerInfo_PsychokinesisUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CPlayerInfo_PsychokinesisUI * CPlayerInfo_PsychokinesisUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CPlayerInfo_PsychokinesisUI*		pInstance = new CPlayerInfo_PsychokinesisUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerInfo_PsychokinesisUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CPlayerInfo_PsychokinesisUI::Clone(void * pArg)
{
	CPlayerInfo_PsychokinesisUI*		pInstance = new CPlayerInfo_PsychokinesisUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayerInfo_PsychokinesisUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerInfo_PsychokinesisUI::Free()
{
	CUI::Free();

}
