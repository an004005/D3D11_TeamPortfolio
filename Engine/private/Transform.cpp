#include "stdafx.h"
#include "..\public\Transform.h"
#include "Shader.h"
#include "ImguiUtils.h"
#include "JsonLib.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
	
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
{
	m_WorldMatrix = _float4x4::Identity;
}

_matrix CTransform::Get_WorldMatrix_Inverse()
{
	return XMMatrixInverse(nullptr, Get_WorldMatrix());
}

_matrix CTransform::Get_WorldMatrix()
{
	if (m_pParent)
		return XMLoadFloat4x4(&m_WorldMatrix) * m_pParent->Get_WorldMatrix();
	else
		return XMLoadFloat4x4(&m_WorldMatrix); 
}

_float4x4 CTransform::Get_WorldMatrix_f4x4()
{
	return Get_WorldMatrix();
}

_float CTransform::GetYaw_Radian()
{
	const _float3 vLook = _float3(m_WorldMatrix._31, 0.f, m_WorldMatrix._33);
	const _vector _vLook = XMVector3Normalize(vLook);

	_float fYaw;
	if (vLook.x < 0.f)
		fYaw = -acosf(XMVectorGetX(XMVector3Dot(_vLook, _float3::UnitZ)));
	else
		fYaw = acosf(XMVectorGetX(XMVector3Dot(_vLook, _float3::UnitZ)));

	return fYaw;
}

_float CTransform::GetPitch_Radian()
{
	return -asinf(XMVectorGetX(XMVector3Dot(Get_State(STATE_LOOK), XMVectorSet(0.f, 1.f, 0.f, 0.f))));
}

void CTransform::Set_Scaled(STATE eState, _float fScale)
{
	if (eState == STATE_TRANSLATION)
		return;

	_vector		vState = Get_State(eState);

	vState = XMVector3Normalize(vState) * fScale;

	Set_State(eState, vState);
}

void CTransform::Set_Scaled(_float3 vScale)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * vScale.z);
}

void CTransform::Scaling(STATE eState, _float fScale)
{
	if (eState == STATE_TRANSLATION)
		return;

	Set_State(eState, Get_State(eState) * fScale);
}

HRESULT CTransform::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		if (json.contains("Transform"))
		{
			json["Transform"]["WorldMatrix"].get_to(m_WorldMatrix);
		}
	}

	return S_OK;
}

void CTransform::Imgui_RenderProperty()
{
	static GUIZMO_INFO tInfo;
	CImguiUtils::Render_Guizmo(&m_WorldMatrix, tInfo, true, true);
}

void CTransform::SaveToJson(Json& json)
{
	json["Transform"]["WorldMatrix"] = m_WorldMatrix;
}

void CTransform::Go_Straight(_double TimeDelta)
{	
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	/* 이렇게 얻어온 VlOOK은 Z축 스케일을 포함하낟. */
	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * TimeDelta;

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Go_Backward(_double TimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vLook = Get_State(CTransform::STATE_LOOK);

	/* 이렇게 얻어온 VlOOK은 Z축 스케일을 포함하낟. */
	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * TimeDelta;

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Go_Left(_double TimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vRight = Get_State(CTransform::STATE_RIGHT);

	/* 이렇게 얻어온 VlOOK은 Z축 스케일을 포함하낟. */
	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * TimeDelta;

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Go_Right(_double TimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector	vRight = Get_State(CTransform::STATE_RIGHT);

	/* 이렇게 얻어온 VlOOK은 Z축 스케일을 포함하낟. */
	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * TimeDelta;

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Move(_double TimeDelta, _float3 vDir)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	vPosition += XMVector3Normalize(vDir) * m_TransformDesc.fSpeedPerSec * TimeDelta;
	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::MoveVelocity(_double TimeDelta, _float3 vVelocity)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	vPosition += XMLoadFloat3(&vVelocity) * static_cast<_float>(TimeDelta);
	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

_vector CTransform::MoveVelocity_Get(_double TimeDelta, _float3 vVelocity)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	vPosition += XMLoadFloat3(&vVelocity) * static_cast<_float>(TimeDelta);
	return vPosition;
}

void CTransform::MoveVelocity_Navi(_double TimeDelta, _float3 vVelocity, CNavigation* pNavi, _int& iCellIdx)
{
	_vector	vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector vNewPosition = vPosition + XMLoadFloat3(&vVelocity) * static_cast<_float>(TimeDelta);

	_float3 vFailedLineVector;
	if (false == pNavi->IsMove_OnNavigation(iCellIdx, vNewPosition, vFailedLineVector))
	{
		_float fYPos = XMVectorGetY(vNewPosition);
		_vector vMoveVector = XMLoadFloat3(&vVelocity) * static_cast<_float>(TimeDelta);

		_vector _vFailedLineVector = XMLoadFloat3(&vFailedLineVector);

		_float dot = XMVectorGetX(XMVector3Dot(_vFailedLineVector, vMoveVector));
		_float fLineSq = XMVectorGetX(XMVector3LengthSq(_vFailedLineVector));
		vMoveVector = _vFailedLineVector * dot / fLineSq;

		vNewPosition = XMVectorSetY(vPosition + vMoveVector, fYPos);
	}
	
	Set_State(CTransform::STATE_TRANSLATION, vNewPosition);
}

void CTransform::Turn(_fvector vAxis, _double TimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * TimeDelta);

	_vector		vRight = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp = Get_State(CTransform::STATE_UP);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);	

	Set_State(CTransform::STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(CTransform::STATE_LOOK , XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::TurnPitch(_double TimeDelta)
{
	if (m_bPitchLock)
	{
		const _float fPitch = GetPitch_Radian();
		const _float fNextPitch = fPitch + m_TransformDesc.fRotationPerSec * TimeDelta;
		if (fNextPitch > m_fMaxPitchRad)
		{
			_matrix		RotationMatrix = XMMatrixRotationAxis(Get_State(CTransform::STATE_RIGHT), m_fMaxPitchRad - fPitch);

			_vector		vRight = Get_State(CTransform::STATE_RIGHT);
			_vector		vUp = Get_State(CTransform::STATE_UP);
			_vector		vLook = Get_State(CTransform::STATE_LOOK);	

			Set_State(CTransform::STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
			Set_State(CTransform::STATE_UP, XMVector4Transform(vUp, RotationMatrix));
			Set_State(CTransform::STATE_LOOK , XMVector4Transform(vLook, RotationMatrix));
		}
		else if (fNextPitch < -m_fMaxPitchRad)
		{
			_matrix		RotationMatrix = XMMatrixRotationAxis(Get_State(CTransform::STATE_RIGHT), -m_fMaxPitchRad - fPitch);

			_vector		vRight = Get_State(CTransform::STATE_RIGHT);
			_vector		vUp = Get_State(CTransform::STATE_UP);
			_vector		vLook = Get_State(CTransform::STATE_LOOK);	

			Set_State(CTransform::STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
			Set_State(CTransform::STATE_UP, XMVector4Transform(vUp, RotationMatrix));
			Set_State(CTransform::STATE_LOOK , XMVector4Transform(vLook, RotationMatrix));
		}
		else
			Turn(Get_State(CTransform::STATE_RIGHT), TimeDelta);
	}
	else
	{
		Turn(Get_State(CTransform::STATE_RIGHT), TimeDelta);
	}
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	const _matrix RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	const _float3 vScale = Get_Scaled();
	const _vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	const _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	const _vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	Set_State(CTransform::STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::RemoveRotation()
{
	_matrix WorldMatrix = m_WorldMatrix;
	WorldMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(WorldMatrix.r[0]));
	WorldMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(WorldMatrix.r[1]));
	WorldMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(WorldMatrix.r[2]));
	m_WorldMatrix = WorldMatrix;
}

void CTransform::LookAt(_fvector vTargetPos)
{
	_float3		vScale = Get_Scaled();

	_vector		vLook = XMVector3Normalize(vTargetPos - Get_State(CTransform::STATE_TRANSLATION)) * vScale.z;
	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
	_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScale.y;

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_UP, vUp);
	Set_State(CTransform::STATE_LOOK, vLook);
}

void CTransform::Chase(_fvector vTargetPos, _double TimeDelta, _float fLimit)
{
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector		vDir = vTargetPos - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));

	if(fDistance > fLimit)
	{
		vPosition += XMVector3Normalize(vDir) * m_TransformDesc.fSpeedPerSec * TimeDelta;	
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
	}
}

void CTransform::BillBoard(_float4 vCamPos, _float3 vCamUp, _float3* vCamLook)
{
	XMVECTOR O = Get_State(STATE_TRANSLATION);
	XMVECTOR C = vCamPos;
    XMVECTOR Z = XMVectorSubtract(O, C);

    XMVECTOR N = XMVector3LengthSq(Z);
    if (XMVector3Less(N, g_XMEpsilon))
    {
        if (vCamLook)
        {
            XMVECTOR F = *vCamLook;
            Z = XMVectorNegate(F);
        }
        else
            Z = g_XMNegIdentityR2;
    }
    else
    {
        Z = XMVector3Normalize(Z);
    }

    XMVECTOR up = vCamUp;
    XMVECTOR X = XMVector3Cross(up, Z);
    X = XMVector3Normalize(X);

    XMVECTOR Y = XMVector3Cross(Z, X);

	_float3 vScale = Get_Scaled();

	Set_State(STATE_RIGHT, X * vScale.x);
	Set_State(STATE_UP, Y * vScale.y);
	Set_State(STATE_LOOK, Z * vScale.z);

    // XMMATRIX M;
    // M.r[0] = X;
    // M.r[1] = Y;
    // M.r[2] = Z;
    // M.r[3] = XMVectorSetW(O, 1.f);
}

void CTransform::ConstrainBillBoard(_float4 vCamPos, _float4 vRotateAxis, _float4* pCamLook, _float4* pLook)
{
    static const XMVECTORF32 s_minAngle = { { { 0.99825467075f, 0.99825467075f, 0.99825467075f, 0.99825467075f } } }; // 1.0 - XMConvertToRadians( 0.1f );

	XMVECTOR O = Get_State(STATE_TRANSLATION);
	XMVECTOR C = vCamPos;
    XMVECTOR faceDir = XMVectorSubtract(O, C);

    XMVECTOR N = XMVector3LengthSq(faceDir);
    if (XMVector3Less(N, g_XMEpsilon))
    {
        if (pCamLook)
        {
            XMVECTOR F = XMLoadFloat4(pCamLook);
            faceDir = XMVectorNegate(F);
        }
        else
            faceDir = g_XMNegIdentityR2;
    }
    else
    {
        faceDir = XMVector3Normalize(faceDir);
    }

    XMVECTOR Y = vRotateAxis;
    XMVECTOR X, Z;

    XMVECTOR dot = XMVectorAbs(XMVector3Dot(Y, faceDir));
    if (XMVector3Greater(dot, s_minAngle))
    {
        if (pLook)
        {
            Z = XMLoadFloat4(pLook);
            dot = XMVectorAbs(XMVector3Dot(Y, Z));
            if (XMVector3Greater(dot, s_minAngle))
            {
                dot = XMVectorAbs(XMVector3Dot(Y, g_XMNegIdentityR2));
                Z = (XMVector3Greater(dot, s_minAngle)) ? g_XMIdentityR0 : g_XMNegIdentityR2;
            }
        }
        else
        {
            dot = XMVectorAbs(XMVector3Dot(Y, g_XMNegIdentityR2));
            Z = (XMVector3Greater(dot, s_minAngle)) ? g_XMIdentityR0 : g_XMNegIdentityR2;
        }

        X = XMVector3Cross(Y, Z);
        X = XMVector3Normalize(X);

        Z = XMVector3Cross(X, Y);
        Z = XMVector3Normalize(Z);
    }
    else
    {
        X = XMVector3Cross(Y, faceDir);
        X = XMVector3Normalize(X);

        Z = XMVector3Cross(X, Y);
        Z = XMVector3Normalize(Z);
    }

	_float3 vScale = Get_Scaled();

	Set_State(STATE_RIGHT, X * vScale.x);
	Set_State(STATE_UP, Y * vScale.y);
	Set_State(STATE_LOOK, Z * vScale.z);
}

void CTransform::LookAxisLockBillBoard(_float4 vCamPos)
{
	_vector vLook = Get_State(STATE_LOOK);
	_float3 vScale = Get_Scaled();
	_vector vPos = Get_State(STATE_TRANSLATION);

	_vector vLookAtCam = XMLoadFloat4(&vCamPos) - vPos; // vLookAtCam
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLookAtCam, vLook));
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

	Set_State(STATE_RIGHT, vRight * vScale.x);
	Set_State(STATE_UP, vUp * vScale.y);
}

void CTransform::UpAxisLockBillBoard(_float4 vCamPos)
{
	_vector vUp = Get_State(STATE_UP);
	_float3 vScale = Get_Scaled();
	_vector vPos = Get_State(STATE_TRANSLATION);

	_vector vLookAtCam = XMLoadFloat4(&vCamPos) - vPos;
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLookAtCam));
	_vector vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));

	Set_State(STATE_RIGHT, vRight * vScale.x);
	Set_State(STATE_LOOK, vLook * vScale.z);
}

void CTransform::RightAxisLockBillBoard(_float4 vCamPos)
{
	_vector vRight = Get_State(STATE_RIGHT);
	_float3 vScale = Get_Scaled();
	_vector vPos = Get_State(STATE_TRANSLATION);

	_vector vLookAtCam = XMLoadFloat4(&vCamPos) - vPos;
	_vector vUp = XMVector3Normalize(XMVector3Cross(vRight, vLookAtCam));
	_vector vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));

	Set_State(STATE_UP, vUp * vScale.y);
	Set_State(STATE_LOOK, vLook * vScale.z);
}

void CTransform::CopyState(STATE eState, CTransform* pFrom)
{
	Set_State(eState, pFrom->Get_State(eState));
}

void CTransform::CopyWorld(CTransform* pFrom)
{
	m_WorldMatrix = pFrom->Get_WorldMatrix_f4x4();
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShaderCom, const char* pConstantName)
{
	if (nullptr == pShaderCom)
		return E_FAIL;

	return pShaderCom->Set_Matrix(pConstantName, &Get_WorldMatrix_f4x4());	
}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*		pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*		pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CTransform::Free()
{
	__super::Free();
	Safe_Release(m_pParent);

}
