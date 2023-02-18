#include "stdafx.h"
#include "..\public\Frustum.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum()
{
}

HRESULT CFrustum::Initialize()
{
	/* 투영스페이스 공간에서의 여덟개 점을 정의한ㄷ. */
	m_vPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vPoints[3] = _float3(-1.f, -1.f, 0.f);

	m_vPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vPoints[7] = _float3(-1.f, -1.f, 1.f);	

	return S_OK;
}

void CFrustum::Transform_ToWorldSpace()
{
	/* why : 일반적인 절두체와 객체간의 위치비교는 월드스페이스에서 자주 일어날 수 있다.  */
	/* 월드스페이스까지의 역변환 작업은 한프레임내에서는 모두 동일 투영, 뷰 행렬의 역행르을 필요로하기때문에.  */
	/* 단, 정점(존내많으니까) 위치와의 절두체 비교를 위해서는 로컬스페이스에서의 충돌이 더 유리. 객체별로 사용하고 있는 월드행렬이 
	서로 다르기때문에 한번 변환시킨다라ㅡㄴㄴ 개념이 사용 불가능하다. 각개체안에서 로컬로 변환시켜 정점위치와 위치판단하겠다.  */
	CPipeLine*			pPipeLine = GET_INSTANCE(CPipeLine);

	_matrix			ProjMatrixInv = pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
	_matrix			ViewMatrixInv = pPipeLine->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);

	for (_uint i = 0; i < 8; ++i)
	{
		XMStoreFloat3(&m_vPointsInWorld[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), ProjMatrixInv));
		XMStoreFloat3(&m_vPointsInWorld[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPointsInWorld[i]), ViewMatrixInv));
	}

	Make_Planes(m_PlanesInWorld, m_vPointsInWorld);
	
	RELEASE_INSTANCE(CPipeLine);
}

void CFrustum::Transform_ToLocalSpace(_fmatrix WorldMatrix)
{
	_matrix			WorldMatrixInv = XMMatrixInverse(nullptr, WorldMatrix);

	_float3			vPoints[8];

	for (_uint i = 0; i < 8; ++i)
		XMStoreFloat3(&vPoints[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPointsInWorld[i]), WorldMatrixInv));
	
	Make_Planes(m_PlanesInLocal, vPoints);
}

_bool CFrustum::isInFrustum_WorldSpace(_fvector vWorldPos, _float fRange)
{
	for (_uint i = 0; i < 6; ++i)
	{
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_PlanesInWorld[i]), vWorldPos)))
			return false;
	}

	return true;
}

_bool CFrustum::isInFrustum_LocalSpace(_fvector vLocalPos, _float fRange)
{
	for (_uint i = 0; i < 6; ++i)
	{
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_PlanesInLocal[i]), vLocalPos)))
			return false;
	}

	return true;
}

void CFrustum::Make_Planes(_float4 * pPlanes, const _float3 * pPoints)
{
	XMStoreFloat4(&pPlanes[0], 
		XMPlaneFromPoints(XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[6])));
	XMStoreFloat4(&pPlanes[1],
		XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[3])));
	XMStoreFloat4(&pPlanes[2],
		XMPlaneFromPoints(XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[1])));
	XMStoreFloat4(&pPlanes[3],
		XMPlaneFromPoints(XMLoadFloat3(&pPoints[3]), XMLoadFloat3(&pPoints[2]), XMLoadFloat3(&pPoints[6])));
	XMStoreFloat4(&pPlanes[4],
		XMPlaneFromPoints(XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[7])));
	XMStoreFloat4(&pPlanes[5],
		XMPlaneFromPoints(XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[2])));
}

void CFrustum::Free()
{

}
