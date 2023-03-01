#include "stdafx.h"
#include "..\public\Canvas_PlayerInfoMove.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "UI_Manager.h"

CCanvas_PlayerInfoMove::CCanvas_PlayerInfoMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_PlayerInfoMove::CCanvas_PlayerInfoMove(const CCanvas_PlayerInfoMove& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_PlayerInfoMove::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_PlayerInfoMove::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_CCanvas(L"Canvas_PlayerInfoMove", this);
	CCanvas::UIMove_FSM();

	return S_OK;
}

void CCanvas_PlayerInfoMove::BeginTick()
{

}

void CCanvas_PlayerInfoMove::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	m_pUIMoveFSM->Tick(TimeDelta);

	if (CGameInstance::GetInstance()->KeyDown(DIK_0))
		Set_UIMove();
}

void CCanvas_PlayerInfoMove::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_PlayerInfoMove::Render()
{
	if (FAILED(CCanvas::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_PlayerInfoMove::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();
}

void CCanvas_PlayerInfoMove::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);

}

void CCanvas_PlayerInfoMove::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);

}

CCanvas_PlayerInfoMove * CCanvas_PlayerInfoMove::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_PlayerInfoMove*		pInstance = new CCanvas_PlayerInfoMove(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_PlayerInfoMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_PlayerInfoMove::Clone(void * pArg)
{
	CCanvas_PlayerInfoMove*		pInstance = new CCanvas_PlayerInfoMove(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_PlayerInfoMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_PlayerInfoMove::Free()
{
	CCanvas::Free();

}
