#include "stdafx.h"
#include "..\public\Canvas_DriveMove.h"
#include "GameInstance.h"
#include "FSMComponent.h"

CCanvas_DriveMove::CCanvas_DriveMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_DriveMove::CCanvas_DriveMove(const CCanvas_DriveMove& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_DriveMove::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_DriveMove::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CCanvas::UIMove_FSM();

	return S_OK;
}

void CCanvas_DriveMove::BeginTick()
{

}

void CCanvas_DriveMove::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	m_pUIMoveFSM->Tick(TimeDelta);

}

void CCanvas_DriveMove::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_DriveMove::Render()
{
	if (FAILED(CCanvas::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_DriveMove::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();
}

void CCanvas_DriveMove::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);

}

void CCanvas_DriveMove::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);

}

CCanvas_DriveMove * CCanvas_DriveMove::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_DriveMove*		pInstance = new CCanvas_DriveMove(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_DriveMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_DriveMove::Clone(void * pArg)
{
	CCanvas_DriveMove*		pInstance = new CCanvas_DriveMove(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_DriveMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_DriveMove::Free()
{
	CCanvas::Free();

}
