#include "stdafx.h"
#include "..\public\Frustum.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum()
{
}

HRESULT CFrustum::Initialize()
{
	/* ���������̽� ���������� ������ ���� �����Ѥ�. */
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
	/* why : �Ϲ����� ����ü�� ��ü���� ��ġ�񱳴� ���彺���̽����� ���� �Ͼ �� �ִ�.  */
	/* ���彺���̽������� ����ȯ �۾��� �������ӳ������� ��� ���� ����, �� ����� ���ร�� �ʿ���ϱ⶧����.  */
	/* ��, ����(���������ϱ�) ��ġ���� ����ü �񱳸� ���ؼ��� ���ý����̽������� �浹�� �� ����. ��ü���� ����ϰ� �ִ� ��������� 
	���� �ٸ��⶧���� �ѹ� ��ȯ��Ų�ٶ�Ѥ��� ������ ��� �Ұ����ϴ�. ����ü�ȿ��� ���÷� ��ȯ���� ������ġ�� ��ġ�Ǵ��ϰڴ�.  */
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
