#include "stdafx.h"
#include "..\public\Camera_Dynamic.h"
#include "GameInstance.h"

CCamera_Dynamic::CCamera_Dynamic(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Dynamic::CCamera_Dynamic(const CCamera_Dynamic & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Dynamic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CCamera_Dynamic::Initialize(void * pArg)
{
	if (FAILED(CCamera::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CTransform::TRANSFORMDESC desc;
	desc.fSpeedPerSec = m_fSpeedPerSec;
	desc.fRotationPerSec = XMConvertToRadians(90.0f);

	m_pTransformCom->SetTransformDesc(desc);

	SetMainCamera(this);

	return S_OK;
}

void CCamera_Dynamic::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_bActive == false)
		return;

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	if (IsMainCamera() == false)
		return;

	
	if (pGameInstance->KeyPressing(DIK_W))
	{
		m_pTransformCom->Go_Straight(TimeDelta);
	}	

	if (pGameInstance->KeyPressing(DIK_S))
	{
		m_pTransformCom->Go_Backward(TimeDelta);
	}

	if (LEVEL_NOW != LEVEL_EFFECT)
	{
		if (pGameInstance->KeyPressing(DIK_SPACE))
		{
			_float3 vVelocity = m_pTransformCom->Get_State(CTransform::STATE_UP) * m_fSpeedPerSec;
			m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
		}
		if (pGameInstance->KeyPressing(DIK_LCONTROL))
		{
			_float3 vVelocity = -m_pTransformCom->Get_State(CTransform::STATE_UP) * m_fSpeedPerSec;
			m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
		}
	}

	if (pGameInstance->KeyPressing(DIK_A))
	{
		m_pTransformCom->Go_Left(TimeDelta);
	}

	if (pGameInstance->KeyPressing(DIK_D))
	{
		m_pTransformCom->Go_Right(TimeDelta);
	}

	if (pGameInstance->Get_DIMouseState(CInput_Device::DIM_RB) & 0x80)
	{
		_long			MouseMove = 0;

		if (!pGameInstance->KeyDown(CInput_Device::DIM_RB))
		{
			if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_X))
			{
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * m_fMouseSpeed);
			}

			if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_Y))
			{
				m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta * MouseMove * m_fMouseSpeed);
			}
		}
	}

	m_pTransformCom->SetSpeed(m_fSpeedPerSec);
}

void CCamera_Dynamic::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);	
}

HRESULT CCamera_Dynamic::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCamera_Dynamic::Imgui_RenderProperty()
{
	CCamera::Imgui_RenderProperty();

	ImGui::SliderFloat("MouseSpeed", &m_fMouseSpeed, 0.01f, 0.1f);
	ImGui::SliderFloat("KeyBoardSpeed", &m_fSpeedPerSec, 0.1f, 50.f);
}

HRESULT CCamera_Dynamic::SetUp_Components()
{
	return S_OK;
}


CCamera_Dynamic * CCamera_Dynamic::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CCamera_Dynamic*		pInstance = new CCamera_Dynamic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Dynamic");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCamera_Dynamic::Clone(void * pArg)
{
	CCamera_Dynamic*		pInstance = new CCamera_Dynamic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Dynamic");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Dynamic::Free()
{
	__super::Free();
}
