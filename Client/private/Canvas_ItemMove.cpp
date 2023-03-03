#include "stdafx.h"
#include "..\public\Canvas_ItemMove.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "UI_Manager.h"

CCanvas_ItemMove::CCanvas_ItemMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_ItemMove::CCanvas_ItemMove(const CCanvas_ItemMove& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_ItemMove::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_ItemMove::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_CCanvas(L"Canvas_PlayerInfoMove", this);
	CCanvas::UIMove_FSM();

	return S_OK;
}

void CCanvas_ItemMove::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	m_pUIMoveFSM->Tick(TimeDelta);

}

void CCanvas_ItemMove::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();
}

void CCanvas_ItemMove::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_ItemMove::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_ItemMove * CCanvas_ItemMove::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_ItemMove*		pInstance = new CCanvas_ItemMove(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_ItemMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_ItemMove::Clone(void * pArg)
{
	CCanvas_ItemMove*		pInstance = new CCanvas_ItemMove(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_ItemMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_ItemMove::Free()
{
	CCanvas::Free();
}
