#pragma once
#include <Input_Device.h>
#include "Client_Defines.h"
#include "Component.h"
#include "TimerHelper.h"
#include "HelperClasses.h"

BEGIN(Client)

typedef struct tagInputSyncData
{
	_uint iInputMask;
	_float2 vMouseAxis;
} INPUT_SYNC_DATA;

enum EInputMask
{
	MASK_W = 1,
	MASK_A = 1 << 1,
	MASK_S = 1 << 2,
	MASK_D = 1 << 3,

	MASK_LSHIFT = 1 << 4,
	MASK_SPACE = 1 << 5,
	MASK_LCTRL = 1 << 6,
	MASK_NUM_1 = 1 << 7,
	MASK_NUM_2 = 1 << 8,
	MASK_NUM_3 = 1 << 9,
	MASK_R = 1 << 10,

	MASK_Q = 1 << 11,
	MASK_E = 1 << 12,
	MASK_C = 1 << 13,
	MASK_X = 1 << 14,
	
	MASK_MOUSELB = 1 << 15,
	MASK_MOUSERB = 1 << 16,
	MASK_NUM_4 = 1 << 17,
};

enum class EControllerType
{
	LOCAL,
	REMOTE,
	AI,
	DUMMY,
	CONTROLLER_END
};

class CController : public CComponent
{
public:
	enum EHandleAxis
	{
		MOVE_AXIS,
		MOUSE_AXIS,
		AXIS_END
	};

	enum EHandleInput
	{
		SPACE,
		CTRL,
		SHIFT,
		MOUSE_LB,
		MOUSE_RB,
		NUM_1,
		NUM_2,
		NUM_3,
		NUM_4,
		C,
		Q,
		E,
		R,
		X,
		G,
		B,
		T,
		W,
		S,
		L,
		F,
		M,
		HANDLE_END
	};
protected:
	CController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CController(const CController& rhs);
	virtual ~CController() override = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta);

	EControllerType GetType() const { return m_eType; }

	_bool KeyDown(EHandleInput eInput) const { return m_InputState[eInput] == KEY_STATE::DOWN; }
	_bool KeyUp(EHandleInput eInput) const { return m_InputState[eInput] == KEY_STATE::UP; }
	_bool KeyPress(EHandleInput eInput) const { 
		return m_InputState[eInput] == KEY_STATE::PRESS;
	}

	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_float2 GetMouseAxis() const { return m_vMouseAxis; }
	// _uint GetInputMask() const { return m_iInputMask; }

	virtual void Invalidate();
	void SetInputLock(_bool bLock) { m_bInputLock = bLock; }
	_bool IsCursorLock() const { return m_bCursorLock; }

protected:
	void UpdateInputState(_ubyte iDIK, EHandleInput eInput);
	void UpdateInputState(CInput_Device::MOUSEKEYSTATE eMouse, EHandleInput eInput);

protected:
	EControllerType m_eType = EControllerType::CONTROLLER_END;

	array<KEY_STATE, HANDLE_END> m_InputState{};
	_float3 m_vMoveAxis;
	_float2 m_vMouseAxis;

	// _uint m_iPreInputMask = 0;
	// _uint m_iInputMask = 0;

	// CCoolTimeHelper m_WorldCoolTime;

	_bool m_bCursorLock = false;
	_bool m_bInputLock = false;

	_bool m_bRejector = true;

public:
	virtual void Free() override;
	virtual CComponent* Clone(void* pArg) override;
	static CController* Create();
};


/*************************
 *     CDummyController
 ***************************/
class CDummyController : public CController
{
private:
	CDummyController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDummyController(const CDummyController& rhs);

public:
	virtual void Tick(_double TimeDelta);

public:
	virtual CComponent* Clone(void* pArg) override;
	static CDummyController* Create();
};

END