#include "stdafx.h"
#include "..\public\Canvas_ItemMove.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "UI_Manager.h"

#include "Canvas_Item.h"
#include "Item_PushArrowUI.h"
#include "Item_LeftArrowUI.h"
#include "Item_RightArrowUI.h"
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

	//m_bInput = true;
	//Key_Input();

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

void CCanvas_ItemMove::Key_Input()
{
	// 아이템 Light 조절
	dynamic_cast<CCanvas_Item*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Item"))
		->	Set_IconLight(dynamic_cast<CItem_GaugeUI*>(Find_ChildUI(L"Item_Gauge"))->Get_Light());

	// 아이템 쿨 타임이 아직 돌지 않았다면 사용할 수 없다.
	if (false == dynamic_cast<CItem_GaugeUI*>(Find_ChildUI(L"Item_Gauge"))->Get_ItemUseStatuse()) return;

	// 조명 Reset 하기
	dynamic_cast<CCanvas_Item*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Item"))->Set_LightReset();

	if (CGameInstance::GetInstance()->KeyDown(DIK_DOWN))
	{
		// 예외 처리.. 할까..? 체력이 가득 찼을 경우 체력 아이템을 사용하지 못 하게 한다... 이렇게 되면 조건문이 너무 많아져서..ㅜ

		// 화살표 이동하기
		dynamic_cast<CItem_PushArrowUI*>(Find_ChildUI(L"Item_PushArrow"))->Set_Input();
		dynamic_cast<CItem_PushArrowUI*>(Find_ChildUI(L"Item_PushArrowBack"))->Set_Input();

		// 게이지 사용하기
		dynamic_cast<CItem_GaugeUI*>(Find_ChildUI(L"Item_Gauge"))->Set_CooldownTimeStart();
		dynamic_cast<CItem_GaugeUI*>(Find_ChildUI(L"Item_GaugeBack"))->Set_CooldownTimeStart();

		dynamic_cast<CCanvas_Item*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Item"))->Set_ItmeUse();
	}

	if (CGameInstance::GetInstance()->KeyDown(DIK_LEFT))
	{
		dynamic_cast<CItem_LeftArrowUI*>(Find_ChildUI(L"Item_LeftArrow"))->Set_Input();
		m_bInput = true;

	}

	if (CGameInstance::GetInstance()->KeyDown(DIK_RIGHT))
	{
		dynamic_cast<CItem_RightArrowUI*>(Find_ChildUI(L"Item_RightArrow"))->Set_Input();
		m_bInput = true;

	}

	if (m_bInput == true)
	{
		m_bInput = false;

		_float4 fColor = dynamic_cast<CCanvas_Item*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Item"))->ColorIndex(false);
		dynamic_cast<CItem_GaugeUI*>(Find_ChildUI(L"Item_Gauge"))->Set_Color(fColor);
		dynamic_cast<CItem_PushArrowUI*>(Find_ChildUI(L"Item_PushArrow"))->Set_Color(fColor);
		fColor = dynamic_cast<CCanvas_Item*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Item"))->ColorIndex(true);
		dynamic_cast<CItem_GaugeUI*>(Find_ChildUI(L"Item_GaugeBack"))->Set_Color(fColor);
		dynamic_cast<CItem_PushArrowUI*>(Find_ChildUI(L"Item_PushArrowBack"))->Set_Color(fColor);
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
