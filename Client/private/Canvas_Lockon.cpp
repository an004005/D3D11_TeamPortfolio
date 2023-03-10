#include "stdafx.h"
#include "..\public\Canvas_Drive.h"
#include "Canvas_Lockon.h"

CCanvas_Lockon::CCanvas_Lockon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_Lockon::CCanvas_Lockon(const CCanvas_Lockon& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_Lockon::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_Lockon::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCanvas_Lockon::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

}

void CCanvas_Lockon::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_Lockon::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_Lockon::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_Lockon * CCanvas_Lockon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_Lockon*		pInstance = new CCanvas_Lockon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_Lockon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_Lockon::Clone(void * pArg)
{
	CCanvas_Lockon*		pInstance = new CCanvas_Lockon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_Lockon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_Lockon::Free()
{
	CCanvas::Free();
}
