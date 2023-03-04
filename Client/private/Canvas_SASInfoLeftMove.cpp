#include "stdafx.h"
#include "..\public\Canvas_SASInfoLeftMove.h"
#include "GameInstance.h"

CCanvas_SASInfoLeftMove::CCanvas_SASInfoLeftMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SASInfoLeftMove::CCanvas_SASInfoLeftMove(const CCanvas_SASInfoLeftMove& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SASInfoLeftMove::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_SASInfoLeftMove::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCanvas_SASInfoLeftMove::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

}

void CCanvas_SASInfoLeftMove::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();
}

void CCanvas_SASInfoLeftMove::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_SASInfoLeftMove::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_SASInfoLeftMove * CCanvas_SASInfoLeftMove::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_SASInfoLeftMove*		pInstance = new CCanvas_SASInfoLeftMove(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SASInfoLeftMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_SASInfoLeftMove::Clone(void * pArg)
{
	CCanvas_SASInfoLeftMove*		pInstance = new CCanvas_SASInfoLeftMove(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SASInfoLeftMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SASInfoLeftMove::Free()
{
	CCanvas::Free();
}
