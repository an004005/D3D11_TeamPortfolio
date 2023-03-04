#include "stdafx.h"
#include "..\public\Canvas_SASInfoRight.h"
#include "GameInstance.h"

CCanvas_SASInfoRight::CCanvas_SASInfoRight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SASInfoRight::CCanvas_SASInfoRight(const CCanvas_SASInfoRight& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SASInfoRight::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_SASInfoRight::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCanvas_SASInfoRight::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

}

void CCanvas_SASInfoRight::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();
}

void CCanvas_SASInfoRight::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_SASInfoRight::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_SASInfoRight * CCanvas_SASInfoRight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_SASInfoRight*		pInstance = new CCanvas_SASInfoRight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SASInfoRight");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_SASInfoRight::Clone(void * pArg)
{
	CCanvas_SASInfoRight*		pInstance = new CCanvas_SASInfoRight(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SASInfoRight");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SASInfoRight::Free()
{
	CCanvas::Free();
}
