#include "stdafx.h"
#include "..\public\Canvas_ItemMove.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "UI_Manager.h"

#include "Canvas_Item.h"
#include "Item_PushArrowUI.h"
#include "Item_GaugeUI.h"

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

	CUI_Manager::GetInstance()->Add_MoveCanvas(L"Canvas_ItemMove", this);
	m_vMaxDestination = { -7.0f, -7.0f };
	CCanvas::UIMove_FSM();

	return S_OK;
}

void CCanvas_ItemMove::BeginTick()
{
	CCanvas::BeginTick();
}

void CCanvas_ItemMove::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	m_pUIMoveFSM->Tick(TimeDelta);
	CCanvas::UIHit(TimeDelta);

	Key_Input();
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

void CCanvas_ItemMove::Key_Input()
{
	if (CGameInstance::GetInstance()->KeyDown(DIK_DOWN))
	{
		// 화살표 이동하기
		dynamic_cast<CItem_PushArrowUI*>(Find_ChildUI(L"Item_PushArrow"))->Set_Input();
		dynamic_cast<CItem_PushArrowUI*>(Find_ChildUI(L"Item_PushArrowBack"))->Set_Input();

		// Icon 사이즈 커지기, Icon 에 Light 깜박깜박
		dynamic_cast<CCanvas_Item*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Item"))->Set_CanvasItme();

		// 게이지 사용하기
		dynamic_cast<CItem_GaugeUI*>(Find_ChildUI(L"Item_Gauge"))->Set_CooldownTimeStart();
		dynamic_cast<CItem_GaugeUI*>(Find_ChildUI(L"Item_GaugeBack"))->Set_CooldownTimeStart();
		
	}
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
