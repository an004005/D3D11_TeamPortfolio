#include "stdafx.h"
#include "..\public\Canvas_PlayerInfo.h"
#include <ImguiUtils.h>
#include "GameInstance.h"
#include "JsonLib.h"

CCanvas_PlayerInfo::CCanvas_PlayerInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_PlayerInfo::CCanvas_PlayerInfo(const CCanvas_PlayerInfo& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_PlayerInfo::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_PlayerInfo::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCanvas_PlayerInfo::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

}

void CCanvas_PlayerInfo::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();
}

void CCanvas_PlayerInfo::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_PlayerInfo::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_PlayerInfo * CCanvas_PlayerInfo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_PlayerInfo*		pInstance = new CCanvas_PlayerInfo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_PlayerInfo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_PlayerInfo::Clone(void * pArg)
{
	CCanvas_PlayerInfo*		pInstance = new CCanvas_PlayerInfo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_PlayerInfo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_PlayerInfo::Free()
{
	CCanvas::Free();
}
