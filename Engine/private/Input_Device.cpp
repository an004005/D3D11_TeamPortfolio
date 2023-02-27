#include "stdafx.h"
#include "Input_Device.h"

IMPLEMENT_SINGLETON(CInput_Device)

CInput_Device::CInput_Device()
	: m_pInputSDK(nullptr), m_pKeyBoard(nullptr), m_pMouse(nullptr)
{
	ZeroMemory(m_byKeyState, sizeof(m_byKeyState));
	ZeroMemory(&m_MouseState, sizeof(m_MouseState));
	m_MouseLeftState = KEY_STATE::E_END;
	m_MouseRightState = KEY_STATE::E_END;
}

HRESULT CInput_Device::Ready_Input_Device(HINSTANCE hInst, HWND hWnd)
{
	m_hWnd = hWnd;
	_states.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);

	// DInput 컴객체를 생성하는 함수
	if (FAILED(DirectInput8Create(hInst,DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
		return E_FAIL;
											

	// 키보드 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard,&m_pKeyBoard, nullptr)))
		return E_FAIL;

	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();

	
	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;		

	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pMouse->Acquire();

	return S_OK;
}

void CInput_Device::Invalidate_Input_Device(void)
{
	HWND hwnd = ::GetActiveWindow(); //활성창이 아니면 입력 안함
	if (m_hWnd != hwnd)
	{
		for (_uint key = 0; key < KEY_TYPE_COUNT; key++)
			_states[key] = KEY_STATE::NONE;
		return;
	}

	m_pKeyBoard->GetDeviceState(256, m_byKeyState);

	for (_uint key = 0; key < KEY_TYPE_COUNT; key++)
	{
		// 키가 눌려 있으면 true
		if (m_byKeyState[key] & 0x80)
		{
			KEY_STATE& state = _states[key];

			// 이전 프레임에 키를 누른 상태라면 PRESS
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::PRESS;
			else
				state = KEY_STATE::DOWN;
		}
		else
		{
			KEY_STATE& state = _states[key];

			// 이전 프레임에 키를 누른 상태라면 UP
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::UP;
			else
				state = KEY_STATE::NONE;
		}
	}

	m_pMouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState);

	if (Get_DIMouseState(DIM_LB) & 0x8000)
	{
		KEY_STATE& state = m_MouseLeftState;

		// 이전 프레임에 키를 누른 상태라면 PRESS
		if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
			state = KEY_STATE::PRESS;
		else
			state = KEY_STATE::DOWN;
	}
	else
	{
		KEY_STATE& state = m_MouseLeftState;

		// 이전 프레임에 키를 누른 상태라면 UP
		if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
			state = KEY_STATE::UP;
		else
			state = KEY_STATE::NONE;
	}


	if (Get_DIMouseState(DIM_RB) & 0x8000)
	{
		KEY_STATE& state = m_MouseRightState;

		// 이전 프레임에 키를 누른 상태라면 PRESS
		if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
			state = KEY_STATE::PRESS;
		else
			state = KEY_STATE::DOWN;
	}
	else
	{
		KEY_STATE& state = m_MouseRightState;

		// 이전 프레임에 키를 누른 상태라면 UP
		if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
			state = KEY_STATE::UP;
		else
			state = KEY_STATE::NONE;
	}
}

void CInput_Device::Invalidate()
{
	for (_uint key = 0; key < KEY_TYPE_COUNT; key++)
		_states[key] = KEY_STATE::NONE;

	m_MouseLeftState = KEY_STATE::NONE;
	m_MouseRightState = KEY_STATE::NONE;
}


void Engine::CInput_Device::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}
