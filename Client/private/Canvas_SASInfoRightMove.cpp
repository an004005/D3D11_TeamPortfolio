#include "stdafx.h"
#include "..\public\Canvas_SASInfoRightMove.h"
#include "GameInstance.h"

CCanvas_SASInfoRightMove::CCanvas_SASInfoRightMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SASInfoRightMove::CCanvas_SASInfoRightMove(const CCanvas_SASInfoRightMove& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SASInfoRightMove::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_SASInfoRightMove::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCanvas_SASInfoRightMove::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

}

void CCanvas_SASInfoRightMove::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();
}

void CCanvas_SASInfoRightMove::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_SASInfoRightMove::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_SASInfoRightMove * CCanvas_SASInfoRightMove::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_SASInfoRightMove*		pInstance = new CCanvas_SASInfoRightMove(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SASInfoRightMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_SASInfoRightMove::Clone(void * pArg)
{
	CCanvas_SASInfoRightMove*		pInstance = new CCanvas_SASInfoRightMove(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SASInfoRightMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SASInfoRightMove::Free()
{
	CCanvas::Free();
}
