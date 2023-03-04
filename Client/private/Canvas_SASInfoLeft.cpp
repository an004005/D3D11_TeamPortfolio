#include "stdafx.h"
#include "..\public\Canvas_SASInfoLeft.h"
#include "GameInstance.h"

CCanvas_SASInfoLeft::CCanvas_SASInfoLeft(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SASInfoLeft::CCanvas_SASInfoLeft(const CCanvas_SASInfoLeft& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SASInfoLeft::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_SASInfoLeft::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCanvas_SASInfoLeft::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

}

void CCanvas_SASInfoLeft::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();
}

void CCanvas_SASInfoLeft::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_SASInfoLeft::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_SASInfoLeft * CCanvas_SASInfoLeft::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_SASInfoLeft*		pInstance = new CCanvas_SASInfoLeft(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SASInfoLeft");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_SASInfoLeft::Clone(void * pArg)
{
	CCanvas_SASInfoLeft*		pInstance = new CCanvas_SASInfoLeft(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SASInfoLeft");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SASInfoLeft::Free()
{
	CCanvas::Free();
}
