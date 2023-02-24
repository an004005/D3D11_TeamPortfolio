#include "stdafx.h"
#include "..\public\Canvas_SASInfo.h"
#include "GameInstance.h"

CCanvas_SASInfo::CCanvas_SASInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SASInfo::CCanvas_SASInfo(const CCanvas_SASInfo& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SASInfo::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_SASInfo::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCanvas_SASInfo::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

}

void CCanvas_SASInfo::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();
}

void CCanvas_SASInfo::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_SASInfo::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_SASInfo * CCanvas_SASInfo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_SASInfo*		pInstance = new CCanvas_SASInfo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SASInfo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_SASInfo::Clone(void * pArg)
{
	CCanvas_SASInfo*		pInstance = new CCanvas_SASInfo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SASInfo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SASInfo::Free()
{
	CCanvas::Free();
}
