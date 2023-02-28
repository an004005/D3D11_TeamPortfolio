#include "stdafx.h"
#include "..\public\VIBuffer_Trail_Instancing.h"


CVIBuffer_Trail_Instancing::CVIBuffer_Trail_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Point_Instancing(pDevice, pContext)
{
}

CVIBuffer_Trail_Instancing::CVIBuffer_Trail_Instancing(const CVIBuffer_Trail_Instancing& rhs)
	: CVIBuffer_Point_Instancing(rhs)
{
}

void CVIBuffer_Trail_Instancing::Tick(_double TimeDelta)
{
	if (m_bLife)
	{
		for (auto& data : m_PointData)
		{
			data.vPosition.w -= (_float)TimeDelta;
			// data.vUp.w -= (_float)TimeDelta;
			// m_fLifeTime -= (_float)TimeDelta;
		}

		m_PointData.erase(remove_if(m_PointData.begin(), m_PointData.end(), [](const VTXMATRIX& Matrix)
		{
			// return Matrix.vUp.w <= 0.f;
			return Matrix.vPosition.w <= 0.f;
		}), m_PointData.end());


		D3D11_MAPPED_SUBRESOURCE			SubResource;
		ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
		memcpy(SubResource.pData, m_PointData.data(), sizeof(VTXMATRIX) * m_PointData.size());
		m_pContext->Unmap(m_pInstanceBuffer, 0);

		m_iNumInstance = (_uint)m_PointData.size();
	}
}

void CVIBuffer_Trail_Instancing::AddData(_float4x4 Matrix)
{
	if (m_PointData.size() >= m_iInitNumInstance)
		return;

	VTXMATRIX data;
	memcpy(&data, &Matrix, sizeof(_float4x4));
	m_PointData.push_back(data);

	if (m_bLife == false)
	{
		D3D11_MAPPED_SUBRESOURCE			SubResource;
		ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
		memcpy(SubResource.pData, m_PointData.data(), sizeof(VTXMATRIX) * m_PointData.size());
		m_pContext->Unmap(m_pInstanceBuffer, 0);

		m_iNumInstance = (_uint)m_PointData.size();
	}
}

CVIBuffer_Trail_Instancing* CVIBuffer_Trail_Instancing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance)
{
	CVIBuffer_Trail_Instancing*		pInstance = new CVIBuffer_Trail_Instancing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iNumInstance)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Trail_Instancing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Trail_Instancing::Clone(void* pArg)
{
	CVIBuffer_Trail_Instancing*		pInstance = new CVIBuffer_Trail_Instancing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Trail_Instancing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Trail_Instancing::Free()
{
	CVIBuffer_Point_Instancing::Free();
}