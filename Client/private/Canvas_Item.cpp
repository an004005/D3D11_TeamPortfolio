#include "stdafx.h"
#include "..\public\Canvas_Item.h"
#include "GameInstance.h"

CCanvas_Item::CCanvas_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_Item::CCanvas_Item(const CCanvas_Item& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_Item::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_Item::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCanvas_Item::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

}

void CCanvas_Item::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();
}

void CCanvas_Item::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_Item::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_Item * CCanvas_Item::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_Item*		pInstance = new CCanvas_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_Item");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_Item::Clone(void * pArg)
{
	CCanvas_Item*		pInstance = new CCanvas_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_Item");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_Item::Free()
{
	CCanvas::Free();
}
