#include "stdafx.h"
#include "..\public\VIBuffer_Point_Instancing.h"
#include "GameInstance.h"

CVIBuffer_Point_Instancing::CVIBuffer_Point_Instancing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{
}

CVIBuffer_Point_Instancing::CVIBuffer_Point_Instancing(const CVIBuffer_Point_Instancing & rhs)
	: CVIBuffer_Instancing(rhs)
	, m_iInitNumInstance(rhs.m_iInitNumInstance)
{

}

HRESULT CVIBuffer_Point_Instancing::Initialize_Prototype(_uint iNumInstance)
{
	CONTEXT_LOCK;

	FAILED_CHECK(__super::Initialize_Prototype());

	//if (FAILED(__super::Initialize_Prototype()))
	//	return E_FAIL;

	m_iInitNumInstance = iNumInstance;
	m_iNumInstance = iNumInstance;
	m_iIndexCountPerInstance = 1;
	m_iNumVertexBuffers = 2;
	m_iStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;
	m_iNumPrimitive = iNumInstance;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_iIndicesSizePerPrimitive = sizeof(_ushort);
	m_iNumIndicesPerPrimitive = 1;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXPOINT*			pVertices = new VTXPOINT;
	ZeroMemory(pVertices, sizeof(VTXPOINT));

	pVertices->vPosition = _float3(0.0f, 0.0f, 0.0f);
	pVertices->vPSize = _float2(0.2f, 0.2f);	

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	FAILED_CHECK(__super::Create_VertexBuffer());

	//if (FAILED(__super::Create_VertexBuffer()))
	//	return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	_ushort*		pIndices = new _ushort[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumPrimitive);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	FAILED_CHECK(__super::Create_IndexBuffer());

	//if (FAILED(__super::Create_IndexBuffer()))
	//	return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion


#pragma region INSTANCE_BUFFER

	m_iInstanceStride = sizeof(VTXMATRIX);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);	

	m_BufferDesc.ByteWidth = m_iInstanceStride * iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	VTXMATRIX*			pInstanceVertices = new VTXMATRIX[iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX));

	for (_uint i = 0; i < iNumInstance; ++i)
	{
		pInstanceVertices[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.0f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.0f, 0.f, 1.f, 0.f);
		pInstanceVertices[i].vPosition = _float4(0.f, 0.f, 0.f, 1.f);

		pInstanceVertices[i].vRotRight = _float4(1.0f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vRotUp = _float4(0.0f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vRotLook = _float4(0.0f, 0.f, 1.f, 0.f);
		pInstanceVertices[i].vRotPos = _float4(0.f, 0.f, 0.f, 1.f);
		
		pInstanceVertices[i].fGravityPower = 0.f;
		pInstanceVertices[i].NearestIndex = 0;

	}
	
	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pInstanceVertices;

	m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pInstanceBuffer);

	Safe_Delete_Array(pInstanceVertices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	/* 정점버퍼들을 장치에 바인딩한다.(복수를 바인딩한다.)  */

	ID3D11Buffer*			pVertexBuffers[] = {
		m_pVB,
		m_pInstanceBuffer
	};

	_uint					iStrides[] = {
		m_iStride,
		m_iInstanceStride,
	};

	_uint					iOffsets[] = {
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	/* 인덱스버퍼를 장치에 바인딩한다.(단일로 바인딩한다.)  */
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

	/*m_pContext->DrawIndexed(m_iNumIndices, 0, 0);*/
	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Point_Instancing::SetInstanceBuffer(VTXMATRIX* pBuffer, _uint iSize)
{
	m_iNumInstance = iSize;
	if (m_iNumInstance > m_iInitNumInstance)
		m_iNumInstance = m_iInitNumInstance;

	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	memcpy(SubResource.pData, pBuffer, sizeof(VTXMATRIX) * m_iNumInstance);
	m_pContext->Unmap(m_pInstanceBuffer, 0);
}

void CVIBuffer_Point_Instancing::SetInstanceBuffer(list<VTXMATRIX>* pBuffer)
{
	m_iNumInstance = (_uint)pBuffer->size();
	if (m_iNumInstance > m_iInitNumInstance)
		m_iNumInstance = m_iInitNumInstance;

	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	_uint iIdx = 0;
	for (auto& data : *pBuffer)
	{
		((VTXMATRIX*)SubResource.pData)[iIdx] = data;
		++iIdx;
		if (iIdx >= m_iNumInstance)
			break;
	}

	m_pContext->Unmap(m_pInstanceBuffer, 0);
}

void CVIBuffer_Point_Instancing::Tick(_double TimeDelta)
{
}

void CVIBuffer_Point_Instancing::Free()
{
	__super::Free();
}

CVIBuffer_Point_Instancing* CVIBuffer_Point_Instancing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	_uint iInstanceNum)
{
	CVIBuffer_Point_Instancing*		pInstance = new CVIBuffer_Point_Instancing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iInstanceNum)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Point_Instancing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Point_Instancing::Clone(void* pArg)
{
	CVIBuffer_Point_Instancing*		pInstance = new CVIBuffer_Point_Instancing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Point_Instancing");
		Safe_Release(pInstance);
	}
	return pInstance;
}
