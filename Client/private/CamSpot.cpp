#include "stdafx.h"
#include "..\public\CamSpot.h"
#include "GameInstance.h"
#include "Camera_Player.h"
#include "Controller.h"

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
	desc.fSpeedPerSec = 1.f;
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

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, m_pTargetObject->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
}

void CCamSpot::Tick(_double TimeDelta)
{
	MouseMove(TimeDelta);

	_vector vTargetPos = m_pTargetObject->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) + XMVectorSet(0.f, 1.5f, 0.f, 0.f);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vMyPos));

	if (0.01f >= fDistance)
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vTargetPos);
	else
		m_pTransformCom->Chase(vTargetPos, fDistance * 0.1f, 0.f);

	static_cast<CCamera_Player*>(m_pPlayerCamera)->Sync_Target(vMyPos, vMyLook, m_fCamHeight,TimeDelta);
}

void CCamSpot::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CCamSpot::MouseMove(_double TimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	m_pController->Tick(TimeDelta);

	_float2 vMouseMovement = m_pController->GetMouseAxis();
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	if (vMouseMovement.x)
	{
		_matrix matRotation = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f),
			XMConvertToRadians(90.f) * m_fSensitivity * (_float)TimeDelta * vMouseMovement.x);
		vLook = XMVector3TransformNormal(vLook, matRotation);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	}
	if (vMouseMovement.y)
	{
		m_fCamHeight += static_cast<_float>(vMouseMovement.y) * m_fSensitivity * 0.1f;

		if (m_fCamHeight >= 3.f)
			m_fCamHeight = 3.f;
		else if (m_fCamHeight <= -3.f)
			m_fCamHeight = -3.f;
	}
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
