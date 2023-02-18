#include "stdafx.h"
#include "..\public\FPSCamera.h"
#include "GameInstance.h"
#include "GameUtils.h"

CFPSCamera::CFPSCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CFPSCamera::CFPSCamera(const CFPSCamera& rhs)
	: CCamera(rhs)
{
}

HRESULT CFPSCamera::Initialize(void* pArg)
{
	// SetMainCamera(this);

	m_DeathTimeline.SetTimelineLength(2.5);
	m_DeathTimeline.SetCurve(this, &CFPSCamera::Tick_DeathCamMove, CCurveFloatSmoothStepRatio::Create(2.5f));

	return __super::Initialize(pArg);
}

void CFPSCamera::Tick(_double TimeDelta)
{
	m_DeathTimeline.Tick(TimeDelta);

	if (m_fShakeTime > m_fCurShakeTime)
	{
		m_fCurShakeTime += (_float)TimeDelta;

		const _float fX = CGameUtils::GetRandFloat(-1.f, 1.f) * m_fMagnitude;
		const _float fY = CGameUtils::GetRandFloat(-1.f, 1.f) * m_fMagnitude;

		const _matrix matLocal = XMMatrixTranslation(fX, fY, 0.f);
		m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix() * matLocal);
	}

	__super::Tick(TimeDelta);
}

void CFPSCamera::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

void CFPSCamera::SetWorldMatrix(_fmatrix WorldMatrix)
{
	_float4x4 TmpWorldMatrix;
	XMStoreFloat4x4(&TmpWorldMatrix, WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(TmpWorldMatrix);
}

void CFPSCamera::StartDeathCamMove(_float4 vLookAt)
{
	m_DeathTimeline.PlayFromStart();
	m_vLookAt = vLookAt;

	// pitch 제거
	m_vDeathCamMoveStartPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	m_vDeathCamMoveEndPos = m_vDeathCamMoveStartPos;

	// 뒤 2.5
	m_vDeathCamMoveEndPos -= 2.5f * XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	// 위로 1.5
	m_vDeathCamMoveEndPos += (_float4::UnitY * 1.5f);
}

void CFPSCamera::SetRespawn()
{
	m_DeathTimeline.Stop();
}

void CFPSCamera::Tick_DeathCamMove(_float fValue)
{
	_vector vPositionOut = XMVectorLerp(m_vDeathCamMoveStartPos, m_vDeathCamMoveEndPos, fValue);
	vPositionOut = XMVectorSetW(vPositionOut, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPositionOut);

	m_pTransformCom->LookAt(m_vLookAt);
}

void CFPSCamera::PlayShake(_float fDuration, _float fMagnitude)
{
	m_fCurShakeTime = 0.f;
	m_fShakeTime = fDuration;
	m_fMagnitude = fMagnitude;
}

CFPSCamera* CFPSCamera::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
 	CFPSCamera*		pInstance = new CFPSCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFPSCamera");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CFPSCamera::Clone(void* pArg)
{
	CFPSCamera*		pInstance = new CFPSCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Dynamic");
		Safe_Release(pInstance);
	}
	return pInstance;
}
