#pragma once

#include "Base.h"

BEGIN(Engine)

enum class KEY_STATE
{
	NONE,
	PRESS,
	DOWN,
	UP,
	E_END
};

enum
{
	KEY_TYPE_COUNT = static_cast<_int>(UINT8_MAX + 1),
	KEY_STATE_COUNT = static_cast<_int>(KEY_STATE::E_END),
};

class CInput_Device :	public CBase
{
	DECLARE_SINGLETON(CInput_Device)

public:
	enum MOUSEKEYSTATE { DIM_LB, DIM_RB, DIM_MB, DIM_END };
	enum MOUSEMOVESTATE { DIMS_X, DIMS_Y, DIMS_Z, DIMS_END };

private:
	CInput_Device();
	virtual ~CInput_Device() = default;

public:
	_byte		Get_DIKeyState(_ubyte byKeyID) { return m_byKeyState[byKeyID]; }
	_byte		Get_DIMouseState(MOUSEKEYSTATE byMouseID)
	{
		return m_MouseState.rgbButtons[byMouseID];
	}
	_long		Get_DIMouseMove(MOUSEMOVESTATE eMoveState)
	{
		return *(((long*)&m_MouseState) + eMoveState);
	}

	// 누르고 있을 때
	bool KeyPressing(_ubyte byKeyID) { return _states[byKeyID] == KEY_STATE::PRESS; }
	// 맨 처음 눌렀을 때
	bool KeyDown(_ubyte byKeyID) { return _states[byKeyID] == KEY_STATE::DOWN; }
	// 맨 처음 눌렀다 뗐을 때
	bool KeyUp(_ubyte byKeyID) { return _states[byKeyID] == KEY_STATE::UP; }

	bool KeyDown(MOUSEKEYSTATE byMouseID)
	{
		if (byMouseID == DIM_LB)
			return m_MouseLeftState == KEY_STATE::DOWN;
		if (byMouseID == DIM_RB)
			return m_MouseRightState == KEY_STATE::DOWN;
		return false;
	}

	bool KeyUp(MOUSEKEYSTATE byMouseID)
	{
		if (byMouseID == DIM_LB)
			return m_MouseLeftState == KEY_STATE::UP;
		if (byMouseID == DIM_RB)
			return m_MouseRightState == KEY_STATE::UP;
		return false;
	}

	bool KeyPressing(MOUSEKEYSTATE byMouseID)
	{
		if (byMouseID == DIM_LB)
			return m_MouseLeftState == KEY_STATE::PRESS || m_MouseLeftState == KEY_STATE::DOWN;
		if (byMouseID == DIM_RB)
			return m_MouseRightState == KEY_STATE::PRESS || m_MouseRightState == KEY_STATE::DOWN;
		return false;
	}

public:
	HRESULT			Ready_Input_Device(HINSTANCE hInst, HWND hWnd);
	void			Invalidate_Input_Device(void);
	void			Invalidate();

private:
	_byte			m_byKeyState[256];
	DIMOUSESTATE	m_MouseState;

	LPDIRECTINPUTDEVICE8		m_pKeyBoard;
	LPDIRECTINPUTDEVICE8		m_pMouse;

	LPDIRECTINPUT8				m_pInputSDK;

	HWND m_hWnd;
	vector<KEY_STATE> _states;
	KEY_STATE m_MouseLeftState;
	KEY_STATE m_MouseRightState;

public:
	virtual void		Free(void);
};

END