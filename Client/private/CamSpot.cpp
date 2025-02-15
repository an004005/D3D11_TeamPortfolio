#include "stdafx.h"
#include "..\public\CamSpot.h"
#include "GameInstance.h"
#include "Camera_Player.h"
#include "Controller.h"
#include "PlayerInfoManager.h"
#include "MathUtils.h"
#include "GameUtils.h"

CCamSpot::CCamSpot(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CCamSpot::CCamSpot(const CCamSpot & rhs)
	:CGameObject(rhs)
{
}

HRESULT CCamSpot::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamSpot::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	if (pArg)
	{
		m_pTargetObject = static_cast<CGameObject*>(pArg);
		Safe_AddRef(m_pTargetObject);
	}

	FAILED_CHECK(Setup_Components());

	CTransform::TRANSFORMDESC desc;
	desc.fSpeedPerSec = 2.f;
	desc.fRotationPerSec = XMConvertToRadians(90.0f);

	m_pTransformCom->SetTransformDesc(desc);

	return S_OK;
}

void CCamSpot::BeginTick()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, TEXT("Layer_Camera"))->GetGameObjects())
	{
		if (L"Prototype_GameObject_Camera_Player" == iter->GetPrototypeTag())
		{
			NULL_CHECK(m_pPlayerCamera = iter);
			Safe_AddRef(m_pPlayerCamera);
			break;
		}
	}

	_float4 vLookAt = m_pTargetObject->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) + m_pTargetObject->GetTransform()->Get_State(CTransform::STATE_LOOK);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, m_pTargetObject->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
	m_pTransformCom->LookAt_NonY(vLookAt);

	CPlayerInfoManager::GetInstance()->Set_CamSpot(this);
}

void CCamSpot::Tick(_double TimeDelta)
{
	MouseMove(TimeDelta);

	//CTransform::TRANSFORMDESC desc;
	//desc.fSpeedPerSec = 2.f;
	//desc.fRotationPerSec = XMConvertToRadians(90.0f);

	//m_pTransformCom->SetTransformDesc(desc);

	// CamSpot 이동 AfterPhsyX로 위치 이동

	//_vector vTargetPos = m_pTargetObject->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) + XMVectorSet(0.f, 1.5f, 0.f, 0.f);
	//_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//_vector vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	//_float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vMyPos));

	//if (0.01f >= fDistance)
	//	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vTargetPos);
	//else
	//	m_pTransformCom->Chase(vTargetPos, fDistance * 0.1f, 0.f);

	/*if (m_fLerpTime < 1.f)
	{
		m_fLerpTime += (_float)TimeDelta * 2.f;

		if (MOD_SYNC == m_eCamMod)
		{
			_vector vSyncPos = static_cast<CCamera_Player*>(m_pPlayerCamera)->Get_SyncPos(vMyPos, vMyLook, m_fCamHeight, TimeDelta);
			_vector vLerpPos = XMVectorLerp(m_AttachMatrix.r[3], vSyncPos, m_fLerpTime);

			_vector vSyncLook = static_cast<CCamera_Player*>(m_pPlayerCamera)->Get_SyncLook(vMyPos, vMyLook, m_fCamHeight, TimeDelta);
			_vector vLerpLook = XMVectorLerp(m_AttachMatrix.r[2], vSyncLook, m_fLerpTime);

			static_cast<CCamera_Player*>(m_pPlayerCamera)->Lerp_ActionPos(vLerpPos, vLerpLook);
		}
		else if (MOD_ATTACH == m_eCamMod)
		{
			_vector vSyncPos = static_cast<CCamera_Player*>(m_pPlayerCamera)->Get_SyncPos(vMyPos, vMyLook, m_fCamHeight, TimeDelta);
			_vector vLerpPos = XMVectorLerp(vSyncPos ,m_AttachMatrix.r[3], m_fLerpTime);

			_vector vSyncLook = static_cast<CCamera_Player*>(m_pPlayerCamera)->Get_SyncLook(vMyPos, vMyLook, m_fCamHeight, TimeDelta);
			_vector vLerpLook = XMVectorLerp(vSyncLook, m_AttachMatrix.r[2], m_fLerpTime);

			static_cast<CCamera_Player*>(m_pPlayerCamera)->Lerp_ActionPos(vLerpPos, vLerpLook);
		}
	}
	else
	{
		if (MOD_SYNC == m_eCamMod)
		{
			static_cast<CCamera_Player*>(m_pPlayerCamera)->Sync_Target(vMyPos, vMyLook, m_fCamHeight, TimeDelta);
		}
		else if (MOD_ATTACH == m_eCamMod)
		{
			static_cast<CCamera_Player*>(m_pPlayerCamera)->Attach_Target(m_AttachMatrix);
		}
	}*/
}

void CCamSpot::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CCamSpot::AfterPhysX()
{
	__super::AfterPhysX();

	_vector vTargetPos = m_pTargetObject->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) + XMVectorSet(0.f, 1.2f, 0.f, 0.f);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vMyPos));

	if (0.01f >= fDistance)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vTargetPos);
	else
		m_pTransformCom->Chase(vTargetPos, fDistance * 0.1f, 0.f);

	if (m_fLerpTime < 1.f)
	{
		m_fLerpTime += (_float)g_fTimeDelta * 4.f;

		if (MOD_SYNC == m_eCamMod)
		{
			m_fLerpTime = 1.f;

			if (m_bCheckActionEnd != MOD_SYNC)
			{
				m_CheckMatrix = m_AttachMatrix;
			}

			_vector vSyncPos = static_cast<CCamera_Player*>(m_pPlayerCamera)->Get_SyncPos(vMyPos, vMyLook, m_fCamHeight, g_fTimeDelta);
			_vector vLerpPos = XMVectorLerp(m_CheckMatrix.r[3], vSyncPos, m_fLerpTime);

			_vector vSyncLook = static_cast<CCamera_Player*>(m_pPlayerCamera)->Get_SyncLook(vMyPos, vMyLook, m_fCamHeight, g_fTimeDelta);
			_vector vLerpLook = XMVectorLerp(m_CheckMatrix.r[2], vSyncLook, m_fLerpTime);

			//if (m_fLerpTime < 1.f)
			//	CGameInstance::GetInstance()->SetCameraFov(70.f - (10.f * m_fLerpTime));
			//else
			//	CGameInstance::GetInstance()->SetCameraFov(60.f);

			static_cast<CCamera_Player*>(m_pPlayerCamera)->Lerp_ActionPos(vLerpPos, vLerpLook);
		}
		else if (MOD_ATTACH == m_eCamMod)
		{
			_vector vSyncPos = static_cast<CCamera_Player*>(m_pPlayerCamera)->Get_SyncPos(vMyPos, vMyLook, m_fCamHeight, g_fTimeDelta);
			_vector vLerpPos = XMVectorLerp(vSyncPos, m_AttachMatrix.r[3], m_fLerpTime);

			_vector vSyncLook = static_cast<CCamera_Player*>(m_pPlayerCamera)->Get_SyncLook(vMyPos, vMyLook, m_fCamHeight, g_fTimeDelta);
			_vector vLerpLook = XMVectorLerp(vSyncLook, m_AttachMatrix.r[2], m_fLerpTime);

			//if (m_fLerpTime < 1.f)
			//	CGameInstance::GetInstance()->SetCameraFov(60.f + (10.f * m_fLerpTime));
			//else
			//	CGameInstance::GetInstance()->SetCameraFov(70.f);

			static_cast<CCamera_Player*>(m_pPlayerCamera)->Lerp_ActionPos(vLerpPos, vLerpLook);
		}
	}
	else
	{
		if (MOD_SYNC == m_eCamMod)
		{
			static_cast<CCamera_Player*>(m_pPlayerCamera)->Sync_Target(vMyPos, vMyLook, m_fCamHeight, g_fTimeDelta);
		}
		else if (MOD_ATTACH == m_eCamMod)
		{
			static_cast<CCamera_Player*>(m_pPlayerCamera)->Attach_Target(m_AttachMatrix);
		}
	}

	m_bCheckActionEnd = m_eCamMod;
}

void CCamSpot::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
}

void CCamSpot::MouseMove(_double TimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	m_pController->Tick(TimeDelta);

	m_vMouseMovement = m_pController->GetMouseAxis();
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	if (m_vMouseMovement.x)
	{
		_matrix matRotation = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f),
			XMConvertToRadians(90.f) * m_fSensitivity * g_fTimeDelta * m_vMouseMovement.x);
		vLook = XMVector3TransformNormal(vLook, matRotation);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	}
	if (m_vMouseMovement.y)
	{
		m_fCamHeight += static_cast<_float>(m_vMouseMovement.y) * m_fSensitivity * 0.1f;

		if (m_fCamHeight >= 3.f)
			m_fCamHeight = 3.f;
		else if (m_fCamHeight <= -6.f)
			m_fCamHeight = -6.f;
	}
}

void CCamSpot::Switch_CamMod()
{
	if (MOD_SYNC == m_eCamMod)
	{
		m_eCamMod = MOD_ATTACH;
		m_fLerpTime = 0.f;
	}
}

void CCamSpot::Reset_CamMod()
{
	if (MOD_ATTACH == m_eCamMod)
	{
		m_eCamMod = MOD_SYNC;
		m_fLerpTime = 0.f;
	}
}

void CCamSpot::SetUp_BoneMatrix(CModel * pModel, _fmatrix Transform)
{
	m_AttachMatrix = XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationZ(XMConvertToRadians(-90.f)) * pModel->GetBoneMatrix("CameraPos") * Transform;
}

void CCamSpot::Arrange_Cam()
{
	_float4 vLookAt = m_pTargetObject->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) + m_pTargetObject->GetTransform()->Get_State(CTransform::STATE_LOOK);

	_float4 vTarget = m_pTargetObject->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) + XMVectorSet(0.f, 1.2f, 0.f, 0.f);

	m_fCamHeight = 0.f;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vTarget);
	m_pTransformCom->LookAt_NonY(vLookAt);
}

_bool CCamSpot::Cam_Closer(_double TimeDelta, _float fRatio, _float fLimit)
{
	if (0.f == fRatio) fRatio = TimeDelta;
	_float fMag = static_cast<CCamera_Player*>(m_pPlayerCamera)->Get_Magnification() - (TimeDelta / fRatio);
	fMag = max(fMag, fLimit);

	static_cast<CCamera_Player*>(m_pPlayerCamera)->Set_Magnification(fMag);

	if (fLimit == fMag)
	{
		// 상한치까지 다가갔으면 true 반환
		return true;
	}

	return false;
}

_bool CCamSpot::Cam_Away(_double TimeDelta, _float fRatio, _float fLimit)
{
	if (0.f == fRatio) fRatio = TimeDelta;
	_float fMag = static_cast<CCamera_Player*>(m_pPlayerCamera)->Get_Magnification() + (TimeDelta / fRatio);
	fMag = min(fMag, fLimit);

	static_cast<CCamera_Player*>(m_pPlayerCamera)->Set_Magnification(fMag);

	if (fLimit == fMag)
	{
		// 상한치까지 다가갔으면 true 반환
		return true;
	}

	return false;
}

void CCamSpot::Random_Shaking(_float fShakePower)
{
	// 함수가 실행되는 동안 계속 흔듦

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_vector vShakeDir = XMVectorSet(
		CMathUtils::RandomFloat(-fShakePower, fShakePower),
		CMathUtils::RandomFloat(-fShakePower, fShakePower),
		CMathUtils::RandomFloat(-fShakePower, fShakePower),
		0.f);

	//_vector vShakeDir = XMVectorSet(
	//	CMathUtils::RandomFloat(fShakePower * 0.5f, fShakePower) * (rand() % 2 == 0 ? 1.f : -1.f),
	//	CMathUtils::RandomFloat(fShakePower * 0.5f, fShakePower) * (rand() % 2 == 0 ? 1.f : -1.f),
	//	CMathUtils::RandomFloat(fShakePower * 0.5f, fShakePower) * (rand() % 2 == 0 ? 1.f : -1.f),
	//	0.f);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos + vShakeDir);
}

void CCamSpot::Axis_Shaking(_float4 vAxis, _float fShakePower)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_float4 vInputAxis = XMVector3Normalize(vAxis);

	_matrix matTargetWorld = m_pTargetObject->GetTransform()->Get_WorldMatrix();
	_vector vScale, vRot, vTrans;
	XMMatrixDecompose(&vScale, &vRot, &vTrans, matTargetWorld);
	_matrix matRot = XMMatrixRotationQuaternion(vRot);
	vInputAxis = XMVector3TransformNormal(vInputAxis, matRot);

	_vector vShakeDir = XMVectorSet(
		CMathUtils::RandomFloat(-fShakePower, fShakePower) * vInputAxis.x,
		CMathUtils::RandomFloat(-fShakePower, fShakePower) * vInputAxis.y,
		CMathUtils::RandomFloat(-fShakePower, fShakePower) * vInputAxis.z,
		0.f);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos + vShakeDir);
}

void CCamSpot::Axis_Sliding(_float4 vAxis, _float fSlidePower)
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_float4 vInputAxis = XMVector3Normalize(vAxis);

	_matrix matTargetWorld = m_pTargetObject->GetTransform()->Get_WorldMatrix();
	_vector vScale, vRot, vTrans;
	XMMatrixDecompose(&vScale, &vRot, &vTrans, matTargetWorld);
	_matrix matRot = XMMatrixRotationQuaternion(vRot);
	vInputAxis = XMVector3TransformNormal(vInputAxis, matRot);

	_vector vShakeDir = XMVectorSet(
		fSlidePower * vInputAxis.x,
		fSlidePower * vInputAxis.y,
		fSlidePower * vInputAxis.z,
		0.f);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos + vShakeDir);
}

HRESULT CCamSpot::Setup_Components(void)
{
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Component_LocalController"), TEXT("Com_Controller"),
		(CComponent**)&m_pController));

	return S_OK;
}

CCamSpot * CCamSpot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCamSpot*		pInstance = new CCamSpot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamSpot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCamSpot::Clone(void * pArg)
{
	CCamSpot*		pInstance = new CCamSpot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamSpot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCamSpot::Free()
{
	__super::Free();
	Safe_Release(m_pTargetObject);
	Safe_Release(m_pPlayerCamera);
	Safe_Release(m_pController);
}
