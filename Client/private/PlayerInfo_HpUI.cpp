#include "stdafx.h"
#include "..\public\PlayerInfo_HpUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

CPlayerInfo_HpUI::CPlayerInfo_HpUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CPlayerInfo_HpUI::CPlayerInfo_HpUI(const CPlayerInfo_HpUI& rhs)
	: CUI(rhs)
{
}

HRESULT CPlayerInfo_HpUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerInfo_HpUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CPlayerInfo_HpUI::BeginTick()
{


}

void CPlayerInfo_HpUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	
	
}

void CPlayerInfo_HpUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CPlayerInfo_HpUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CPlayerInfo_HpUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CPlayerInfo_HpUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CPlayerInfo_HpUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CPlayerInfo_HpUI * CPlayerInfo_HpUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CPlayerInfo_HpUI*		pInstance = new CPlayerInfo_HpUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerInfo_HpUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CPlayerInfo_HpUI::Clone(void * pArg)
{
	CPlayerInfo_HpUI*		pInstance = new CPlayerInfo_HpUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayerInfo_HpUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerInfo_HpUI::Free()
{
	CUI::Free();

}
