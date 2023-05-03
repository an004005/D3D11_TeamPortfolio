#include "stdafx.h"
#include "..\public\Controller.h"
#include "GameInstance.h"
#include "GameUtils.h"
#include "UI_Manager.h"

CController::CController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CController::CController(const CController& rhs)
	: CComponent(rhs)
{
	m_eType = EControllerType::LOCAL;
	m_InputState.fill(KEY_STATE::NONE);
	// m_WorldCoolTime = { 0.2, false };

#ifdef _DEBUG
	m_bCursorLock = true;
#endif
}

HRESULT CController::Initialize(void* pArg)
{
	return CComponent::Initialize(pArg);
}

void CController::Tick(_double TimeDelta)
{
	auto pGameInstance = CGameInstance::GetInstance();

	if (m_bRejector)
	{
		if (pGameInstance->KeyDown(DIK_F5))
		{
			m_bRejector = false;
		}

		Invalidate();
		return;
	}

	m_vMouseAxis = _float2::Zero;
	m_vMoveAxis = _float3::Zero;

	// 임시
	// if (pGameInstance->KeyDown(DIK_T))
	// {
	// 	m_bCursorLock = !m_bCursorLock;
	// 	if (m_bCursorLock)
	// 	{
	// 		CGameUtils::ShowCursor();
	// 		m_bInputLock = true;
	// 	}
	// 	else
	// 	{
	// 		CGameUtils::HideCursor();
	// 		m_bInputLock = false;
	// 	}
	// }
	// UI매지너로 기능 옮김

	if (CUI_Manager::GetInstance()->IsMouseActive())
	{
		m_bCursorLock = true;
		m_bInputLock = true;
	}
	else
	{
		m_bCursorLock = false;
		m_bInputLock = false;
	}


	if (m_bInputLock)
	{
		Invalidate();
		return;
	}

	// 움직임
	{
		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
		{
			m_vMoveAxis.z += 1.f;
		}
		if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
		{
			m_vMoveAxis.z -= 1.f;
		}
		if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
		{
			m_vMoveAxis.x -= 1.f;
		}
		if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
		{
			m_vMoveAxis.x += 1.f;
		}
	}

	// 마우스 움직임
	if (m_bCursorLock == false)
	{
		_long MouseMove = 0;
		if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_X))
		{
			m_vMouseAxis.x += static_cast<_float>(MouseMove);
		}

		if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_Y))
		{
			m_vMouseAxis.y += static_cast<_float>(MouseMove);
		}

		POINT cursor{ g_iWinSizeX / 2, g_iWinSizeY / 2 };
		ClientToScreen(g_hWnd, &cursor);
		SetCursorPos(cursor.x, cursor.y);
	}


	UpdateInputState(DIK_SPACE, SPACE);
	UpdateInputState(DIK_LCONTROL, CTRL);
	UpdateInputState(DIK_LSHIFT, SHIFT);
	UpdateInputState(DIK_C, C);
	UpdateInputState(DIK_Q, Q);
	UpdateInputState(DIK_E, E);
	UpdateInputState(DIK_X, X);
	UpdateInputState(DIK_F, F);
	UpdateInputState(DIK_B, B);

	UpdateInputState(DIK_R, R);
	UpdateInputState(DIK_G, G);

	UpdateInputState(DIK_M, M);

	UpdateInputState(DIK_1, NUM_1);
	UpdateInputState(DIK_2, NUM_2);
	UpdateInputState(DIK_3, NUM_3);
	UpdateInputState(DIK_4, NUM_4);

	UpdateInputState(CInput_Device::DIM_LB, MOUSE_LB);
	UpdateInputState(CInput_Device::DIM_RB, MOUSE_RB);

	//
	// {
	// 	m_iPreInputMask = m_iInputMask;
	// 	m_iInputMask = 0;
	//
	// 	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
	// 		m_iInputMask |= MASK_W;
	// 	if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
	// 		m_iInputMask |= MASK_A;
	// 	if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
	// 		m_iInputMask |= MASK_S;
	// 	if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
	// 		m_iInputMask |= MASK_D;
	//
	// 	if (pGameInstance->Get_DIKeyState(DIK_LSHIFT) & 0x80)
	// 		m_iInputMask |= MASK_LSHIFT;
	// 	if (pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
	// 		m_iInputMask |= MASK_SPACE;
	// 	if (pGameInstance->Get_DIKeyState(DIK_LCONTROL) & 0x80)
	// 		m_iInputMask |= MASK_LCTRL;
	// 	if (pGameInstance->Get_DIKeyState(DIK_1) & 0x80)
	// 		m_iInputMask |= MASK_NUM_1;
	// 	if (pGameInstance->Get_DIKeyState(DIK_2) & 0x80)
	// 		m_iInputMask |= MASK_NUM_2;
	// 	if (pGameInstance->Get_DIKeyState(DIK_3) & 0x80)
	// 		m_iInputMask |= MASK_NUM_3;
	// 	if (pGameInstance->Get_DIKeyState(DIK_4) & 0x80)
	// 		m_iInputMask |= MASK_NUM_4;
	// 	if (pGameInstance->Get_DIKeyState(DIK_R) & 0x80)
	// 		m_iInputMask |= MASK_R;
	//
	// 	if (pGameInstance->Get_DIKeyState(DIK_Q) & 0x80)
	// 		m_iInputMask |= MASK_Q;
	// 	if (pGameInstance->Get_DIKeyState(DIK_E) & 0x80)
	// 		m_iInputMask |= MASK_E;
	// 	if (pGameInstance->Get_DIKeyState(DIK_C) & 0x80)
	// 		m_iInputMask |= MASK_C;
	// 	if (pGameInstance->Get_DIKeyState(DIK_X) & 0x80)
	// 		m_iInputMask |= MASK_X;
	//
	// 	if (pGameInstance->Get_DIMouseState(CInput_Device::DIM_LB) & 0x8000)
	// 		m_iInputMask |= MASK_MOUSELB;
	// 	if (pGameInstance->Get_DIMouseState(CInput_Device::DIM_RB) & 0x8000)
	// 		m_iInputMask |= MASK_MOUSERB;
	// }

}

void CController::Invalidate()
{
	m_vMouseAxis = _float2::Zero;
	m_vMoveAxis = _float3::Zero;
	for (auto& state : m_InputState)
		state = KEY_STATE::NONE;
	// m_iInputMask = 0;
	// m_iPreInputMask = 0;
}

void CController::UpdateInputState(_ubyte iDIK, EHandleInput eInput)
{
	if (CGameInstance::GetInstance()->Get_DIKeyState(iDIK) & 0x80)
	{
		KEY_STATE& state = m_InputState[eInput];

		// 이전 프레임에 키를 누른 상태라면 PRESS
		if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
			state = KEY_STATE::PRESS;
		else
			state = KEY_STATE::DOWN;
	}
	else
	{
		KEY_STATE& state = m_InputState[eInput];

		// 이전 프레임에 키를 누른 상태라면 UP
		if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
			state = KEY_STATE::UP;
		else
			state = KEY_STATE::NONE;
	}
}

void CController::UpdateInputState(CInput_Device::MOUSEKEYSTATE eMouse, EHandleInput eInput)
{
	if (CGameInstance::GetInstance()->Get_DIMouseState(eMouse) & 0x80)
	{
		KEY_STATE& state = m_InputState[eInput];

		// 이전 프레임에 키를 누른 상태라면 PRESS
		if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
			state = KEY_STATE::PRESS;
		else
			state = KEY_STATE::DOWN;
	}
	else
	{
		KEY_STATE& state = m_InputState[eInput];

		// 이전 프레임에 키를 누른 상태라면 UP
		if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
			state = KEY_STATE::UP;
		else
			state = KEY_STATE::NONE;
	}
}

void CController::Free()
{
	__super::Free();
}

CComponent* CController::Clone(void* pArg)
{
	CController*		pInstance = new CController(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CController* CController::Create()
{
	CController*		pInstance = new CController(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CController");
		Safe_Release(pInstance);
	}
	return pInstance;
}


/*************************
 *     CDummyController
 ***************************/
CDummyController::CDummyController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CController(pDevice, pContext)
{
}

CDummyController::CDummyController(const CDummyController& rhs)
	: CController(rhs)
{
	m_eType = EControllerType::DUMMY;
}

void CDummyController::Tick(_double TimeDelta)
{
}

CComponent* CDummyController::Clone(void* pArg)
{
	CController*		pInstance = new CDummyController(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummyController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CDummyController* CDummyController::Create()
{
	CDummyController*		pInstance = new CDummyController(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDummyController");
		Safe_Release(pInstance);
	}
	return pInstance;
}
