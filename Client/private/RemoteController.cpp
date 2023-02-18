#include "stdafx.h"
#include "..\public\RemoteController.h"
#include "GameInstance.h"
#include "ClientServiceMgr.h"

CRemoteController::CRemoteController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CController(pDevice, pContext)
{
}

CRemoteController::CRemoteController(const CRemoteController& rhs)
	: CController(rhs)
{
	m_eType = EControllerType::REMOTE;
}


void CRemoteController::Tick(_double TimeDelta)
{
	// auto pGameInstance = CGameInstance::GetInstance();
	m_vMouseAxis = _float2::Zero;
	m_vMoveAxis = _float3::Zero;

	{
		READ_LOCK;
		if (m_NextInputs.empty() == false)
		{
			const INPUT_SYNC_DATA tData = m_NextInputs.front();
			m_NextInputs.pop();

			m_vMouseAxis = tData.vMouseAxis;
			m_iInputMask = tData.iInputMask;
		}

		if (m_NextWorldMatrices.empty() == false)
		{
			const _float4x4 World = m_NextWorldMatrices.front();
			m_NextWorldMatrices.pop();

			// m_pOwner->GetComponentChecked<CTransform>(CGameObject::m_pTransformComTag)
				// ->Set_WorldMatrix(World);
		}
	}

	if (m_bInputLock)
	{
		Invalidate();
		return;
	}

	{
		if (m_iInputMask & MASK_W)
		{
			m_vMoveAxis.z += 1.f;
		}
		if (m_iInputMask & MASK_S)
		{
			m_vMoveAxis.z -= 1.f;
		}
		if (m_iInputMask & MASK_A)
		{
			m_vMoveAxis.x -= 1.f;
		}
		if (m_iInputMask & MASK_D)
		{
			m_vMoveAxis.x += 1.f;
		}
	}

	UpdateRemoteInputState(MASK_SPACE, SPACE);
	UpdateRemoteInputState(MASK_LCTRL, CTRL);
	UpdateRemoteInputState(MASK_LSHIFT, SHIFT);
	UpdateRemoteInputState(MASK_C, C);
	UpdateRemoteInputState(MASK_Q, Q);
	UpdateRemoteInputState(MASK_E, E);
	UpdateRemoteInputState(MASK_X, X);
	UpdateRemoteInputState(MASK_R, R);
	// UpdateRemoteInputState(MASK_G, G)
	UpdateRemoteInputState(MASK_NUM_1, NUM_1);
	UpdateRemoteInputState(MASK_NUM_2, NUM_2);
	UpdateRemoteInputState(MASK_NUM_3, NUM_3);
	UpdateRemoteInputState(MASK_NUM_4, NUM_4);
	UpdateRemoteInputState(MASK_MOUSELB, MOUSE_LB);
	UpdateRemoteInputState(MASK_MOUSERB, MOUSE_RB);
}

void CRemoteController::AddNextWorldMatrix(const _float4x4& WorldMatrix)
{
	WRITE_LOCK;
	m_NextWorldMatrices.push(WorldMatrix);
}

void CRemoteController::AddNextInputSyncData(INPUT_SYNC_DATA tInputSyncData)
{
	WRITE_LOCK;
	m_NextInputs.push(tInputSyncData);
}

void CRemoteController::UpdateRemoteInputState(EInputMask eMask, EHandleInput eInput)
{
	if (m_iInputMask & eMask)
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

CRemoteController* CRemoteController::Clone(void* pArg)
{
	CRemoteController*		pInstance = new CRemoteController(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRemoteController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRemoteController* CRemoteController::Create()
{
	CRemoteController*		pInstance = new CRemoteController(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRemoteController");
		Safe_Release(pInstance);
	}
	return pInstance;
}
