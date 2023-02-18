#include "stdafx.h"
#include "..\public\Cell.h"
#include "VIBuffer_Cell.h"
#include "Shader.h"
#include "PipeLine.h"

CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_iIndex { 0 }
	, m_vPoints { _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f) }
	, m_iNeighborIndices { -1, -1, -1 }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3 * pPoints, _int iIndex)
{
	m_iIndex = iIndex;
	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);


#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
#endif

	return S_OK;
}

void CCell::Imgui_Render()
{
	ImGui::Text("Cell Mode Select");
	if (ImGui::RadioButton("Floor Mode", m_bCliff == false))
	{
		m_bCliff = false;
	}

	if (ImGui::RadioButton("Cliff Mode", m_bCliff == true))
	{
		m_bCliff = true;
	}
}

/* 내 세개의 점들 중, 전달받은 두개의 점과 같은 점이 있는지를 체크. */
_bool CCell::Compare_Points(const _float3 & SourPoint, const _float3 & DestPoint)
{
	const _vector vEpsilon = XMVectorSet(0.00001f, 0.00001f, 0.00001f, 0.00001f);

	const _vector vSource = XMLoadFloat3(&SourPoint);
	const _vector vDest = XMLoadFloat3(&DestPoint);

	const _vector vPointA = XMLoadFloat3(&m_vPoints[POINT_A]);
	const _vector vPointB = XMLoadFloat3(&m_vPoints[POINT_B]);
	const _vector vPointC = XMLoadFloat3(&m_vPoints[POINT_C]);

	if (true == XMVector3NearEqual(vPointA, vSource, vEpsilon))
	{
		if (true == XMVector3NearEqual(vPointB, vDest, vEpsilon))
			return true;		

		if (true == XMVector3NearEqual(vPointC, vDest, vEpsilon))
			return true;
	}

	if (true == XMVector3NearEqual(vPointB, vSource, vEpsilon))
	{
		if (true == XMVector3NearEqual(vPointC, vDest, vEpsilon))
			return true;

		if (true == XMVector3NearEqual(vPointA, vDest, vEpsilon))
			return true;
	}

	if (true == XMVector3NearEqual(vPointC, vSource, vEpsilon))
	{
		if (true == XMVector3NearEqual(vPointA, vDest, vEpsilon))
			return true;

		if (true == XMVector3NearEqual(vPointB, vDest, vEpsilon))
			return true;
	}

	return false;
}

_bool CCell::IsIn(_fvector vTargetPos, _int* pNeighborIndex)
{
	for (_uint i = 0; i < NEIGHBOR_END; ++i)
	{
		_vector		vLine = XMLoadFloat3(&m_vPoints[(i + 1) % NEIGHBOR_END]) - XMLoadFloat3(&m_vPoints[i]);
		_vector		vNormal = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f));
		_vector		vDir = XMVector3Normalize(vTargetPos - XMLoadFloat3(&m_vPoints[i]));

		if (0 < XMVectorGetX(XMVector3Dot(vNormal, vDir)))
		{
			*pNeighborIndex = m_iNeighborIndices[i];
			return false;	
		}
	}

	return true;
}

_float3 CCell::GetOutLineVector(_fvector vTargetPos)
{
	for (_uint i = 0; i < NEIGHBOR_END; ++i)
	{
		_vector		vLine = XMLoadFloat3(&m_vPoints[(i + 1) % NEIGHBOR_END]) - XMLoadFloat3(&m_vPoints[i]);
		_vector		vNormal = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f));
		_vector		vDir = XMVector3Normalize(vTargetPos - XMLoadFloat3(&m_vPoints[i]));

		if (0 < XMVectorGetX(XMVector3Dot(vNormal, vDir)))
		{
			return XMVector3Normalize(vLine);
		}
	}

	return _float3::Zero;
}

_float CCell::GetHeight(_fvector vPos)
{
	_vector vRayOrigin = XMVectorSetY(vPos, 2000.f);
	_vector vRayDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);

	_float fDist;
	if (TriangleTests::Intersects(vRayOrigin, vRayDir, m_vPoints[POINT_A], m_vPoints[POINT_B], m_vPoints[POINT_C], fDist))
	{
		return XMVectorGetY(vRayOrigin + vRayDir * fDist);
	}

	return -1000.f;
}

_vector CCell::GetNormal()
{
	const _vector vA = m_vPoints[POINT_A];
	const _vector vB = m_vPoints[POINT_B];
	const _vector vC = m_vPoints[POINT_C];
	const _vector vAB = vB - vA;
	const _vector vBC = vC - vB;
	return XMVector3Normalize(XMVector3Cross(vAB, vBC));
}

#ifdef _DEBUG
HRESULT CCell::Render(CShader * pShader)
{
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	pShader->Set_Matrix("g_WorldMatrix", &_float4x4::Identity);

	CPipeLine*		pPipeLine = CPipeLine::GetInstance();

	pShader->Set_Matrix("g_ViewMatrix", &pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW));
	pShader->Set_Matrix("g_ProjMatrix", &pPipeLine->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ));
	
	pShader->Begin(0);

	m_pVIBuffer->Render();

	return S_OK;
}

void CCell::Set_Point(POINT ePoint, _float3 vNewPoint)
{
	m_vPoints[ePoint] = vNewPoint;
	m_pVIBuffer->Update_VertexBuffer({m_vPoints[0], m_vPoints[1], m_vPoints[2]});
}
#endif // _DEBUG

CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3 * pPoints, _int iIndex)
{
	CCell*		pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX("Failed to Created : CCell");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCell::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif
}
