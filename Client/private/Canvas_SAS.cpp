#include "stdafx.h"
#include "..\public\Canvas_SAS.h"
#include "GameInstance.h"

CCanvas_SAS::CCanvas_SAS(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SAS::CCanvas_SAS(const CCanvas_SAS& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SAS::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_SAS::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCanvas_SAS::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

}

void CCanvas_SAS::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();
}

void CCanvas_SAS::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_SAS::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_SAS * CCanvas_SAS::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_SAS*		pInstance = new CCanvas_SAS(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SAS");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_SAS::Clone(void * pArg)
{
	CCanvas_SAS*		pInstance = new CCanvas_SAS(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SAS");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SAS::Free()
{
	CCanvas::Free();
}
