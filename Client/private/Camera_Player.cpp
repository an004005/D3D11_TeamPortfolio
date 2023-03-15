#include "stdafx.h"
#include "..\public\Camera_Player.h"
#include "GameInstance.h"
#include "ControlledRigidBody.h"
#include "PhysX_Manager.h"

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

	//FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_ControlledRigidBody",
	//	L"Collider", (CComponent**)&m_pCollider, pArg));
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

	if (pGameInstance->KeyDown(DIK_F11))
	{
		SetMainCamera(this);
	}

	if (IsMainCamera() == false)
		return;
}

void CCamera_Player::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	//const _vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//const _vector vPrePos = m_vPrePos;
	//const _vector vMoveDelta = vPos - vPrePos;

	//// transform 변동사항 적용
	//m_pCollider->MoveDisp(vMoveDelta, (_float)TimeDelta);

}

void CCamera_Player::AfterPhysX()
{
	//CGameObject::AfterPhysX();
	//const _float4 vColliderFootPos = m_pCollider->GetPosition();
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vColliderFootPos);
	//m_vPrePos = vColliderFootPos;

	//_float4 vCamPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//IM_LOG("AfterPhysX : %f %f %f", vCamPos.x, vCamPos.y, vCamPos.z);
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

	_vector vSyncDir = (XMVector3Normalize(vLook + XMVectorSet(0.f, CamHeight + 3.f, 0.f, 0.f)) * 4.f);
	_float fStdDistance = XMVectorGetX(XMVector3Length(vSyncDir));
	_float fDistance = fStdDistance;

	physx::PxRaycastHit hitBuffer[1];
	physx::PxRaycastBuffer rayOut(hitBuffer, 1);

	RayCastParams param;
	param.rayOut = &rayOut;
	param.vOrigin = TargetPos;
	param.vDir = vSyncDir;
	param.fDistance = fDistance;
	param.iTargetType = CTB_STATIC;
	param.bSingle = true;

	if (CGameInstance::GetInstance()->RayCast(param))
	{
		for (int i = 0; i < rayOut.getNbAnyHits(); ++i)
		{
			auto pHit = rayOut.getAnyHit(i);
			fDistance = pHit.distance * 0.9f;
		}
	}

	// 처음 한번
	if (-1.f == m_fBeforeCamDistance) { m_fBeforeCamDistance = fDistance; }
	else if (m_fBeforeCamDistance < fDistance)
	{
		fDistance = min(m_fBeforeCamDistance + (TimeDelta * 10.f * ((fStdDistance - m_fBeforeCamDistance) / fStdDistance)), fDistance);
	}

	m_fBeforeCamDistance = fDistance;
	
	_vector vCamPos = TargetPos + XMVector3Normalize(vSyncDir) * fDistance;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vCamPos);

	m_pTransformCom->LookAt(TargetPos + XMVectorSet(0.f, 0.5f, 0.f, 0.f));
}

_vector CCamera_Player::Get_SyncPos(_fvector TargetPos, _fvector TargetLook, _float CamHeight, _double TimeDelta)
{
	_vector vLook = TargetLook;

	vLook = XMVector3Normalize(TargetLook) * -3.f;

	_vector vCamPos = TargetPos + (XMVector3Normalize(vLook + XMVectorSet(0.f, CamHeight + 3.f, 0.f, 0.f)) * 4.f);

	return vCamPos;
}

_vector CCamera_Player::Get_SyncLook(_fvector TargetPos, _fvector TargetLook, _float CamHeight, _double TimeDelta)
{
	_vector vLook = TargetLook;

	vLook = XMVector3Normalize(TargetLook) * -3.f;

	_vector vCamPos = TargetPos + (XMVector3Normalize(vLook + XMVectorSet(0.f, CamHeight + 3.f, 0.f, 0.f)) * 4.f);

	return (TargetPos + XMVectorSet(0.f, 0.5f, 0.f, 0.f)) - vCamPos;
}

void CCamera_Player::Lerp_ActionPos(_fvector vPos, _fvector vLook)
{
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);

	m_pTransformCom->LookAt(vPos + XMVector3Normalize(vLook));
}

void CCamera_Player::Attach_Target(_fmatrix AttachMatrix)
{
	m_pTransformCom->Set_WorldMatrix(AttachMatrix);
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
