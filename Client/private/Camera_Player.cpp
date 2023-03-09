#include "stdafx.h"
#include "..\public\Camera_Player.h"
#include "GameInstance.h"

CCamera_Player::CCamera_Player(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Player::CCamera_Player(const CCamera_Player & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Player::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Player::Initialize(void * pArg)
{
	if (FAILED(CCamera::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CTransform::TRANSFORMDESC desc;
	desc.fSpeedPerSec = 0.f;
	desc.fRotationPerSec = XMConvertToRadians(90.0f);

	m_pTransformCom->SetTransformDesc(desc);

	//SetMainCamera(this);

	return S_OK;
}

void CCamera_Player::BeginTick()
{
}

void CCamera_Player::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	if (IsMainCamera() == false)
		return;
}

void CCamera_Player::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CCamera_Player::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCamera_Player::Imgui_RenderProperty()
{
	CCamera::Imgui_RenderProperty();

	if (ImGui::Button("MainPlayerCam"))
	{
		SetMainCamera(this);
	}
}

void CCamera_Player::Sync_Target(_fvector TargetPos, _fvector TargetLook, _float CamHeight, _double TimeDelta)
{
	_vector vLook = TargetLook;

	vLook = XMVector3Normalize(TargetLook) * -3.f;

	_vector vCamPos = TargetPos + (XMVector3Normalize(vLook + XMVectorSet(0.f, CamHeight + 3.f, 0.f, 0.f)) * 5.f);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vCamPos);

	m_pTransformCom->LookAt(TargetPos + XMVectorSet(0.f, 0.5f, 0.f, 0.f));
}

HRESULT CCamera_Player::SetUp_Components()
{
	return S_OK;
}

CCamera_Player * CCamera_Player::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCamera_Player*		pInstance = new CCamera_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCamera_Player::Clone(void * pArg)
{
	CCamera_Player*		pInstance = new CCamera_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamera_Player::Free()
{
	__super::Free();
}
