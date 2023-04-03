#include "stdafx.h"
#include "VIBuffer_Line_Instancing.h"

typedef struct tagLineIndices16
{
	unsigned short			_0, _1;
}LINEINDICES16;

CVIBuffer_Line_Instancing::CVIBuffer_Line_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{
}

CVIBuffer_Line_Instancing::CVIBuffer_Line_Instancing(const CVIBuffer_Line_Instancing& rhs)
	: CVIBuffer_Instancing(rhs)
{
}

HRESULT CVIBuffer_Line_Instancing::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iIndexCountPerInstance = 1;
	m_iNumVertexBuffers = 2;
	m_iStride = sizeof(VTXLINE_POS);
	m_iNumVertices = 2;
	m_iNumPrimitive = 1;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_iIndicesSizePerPrimitive = sizeof(LINEINDICES16);
	m_iNumIndicesPerPrimitive = 2;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXLINE_POS*			pVertices = new VTXLINE_POS[2];
	ZeroMemory(pVertices, sizeof(VTXLINE_POS) * 2);

	pVertices[0].vPosition = _float3(0.0f, 0.0f, 0.0f);
	pVertices[1].vPosition = _float3(0.0f, 0.0f, 1.0f);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

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

	LINEINDICES16*		pIndices = new LINEINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(LINEINDICES16) * m_iNumPrimitive);

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		pIndices[i]._0 = 0;
		pIndices[i]._1 = 1;
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion



	return S_OK;
}

HRESULT CVIBuffer_Line_Instancing::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer*			pVertexBuffers[] = {
		m_pVB,
		m_pInstanceBuffer
	};

	_uint		iStrides[] = {
		m_iStride,
		m_iInstanceStride
	};

	_uint		iOffsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	/* 6 : 하나의 도형을 그리기위해 사용하는 인덱스의 갯수. 네모라서 여섯개.  */

	m_pContext->DrawIndexedInstanced(m_iNumIndices, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Line_Instancing::CreateInstanceBuffer(const vector<VTXLINE_POS_INSTANCE>& InstanceData)
{
	m_iNumInstance = (_uint)InstanceData.size();;

#pragma region INSTANCE_BUFFER
	m_iInstanceStride = sizeof(VTXLINE_POS_INSTANCE);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);	

	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;
	// m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	// VTXLINE_POS_INSTANCE*			pInstanceVertices = new VTXLINE_POS_INSTANCE[iNumInstance];
	// ZeroMemory(pInstanceVertices, sizeof(VTXLINE_POS_INSTANCE) * iNumInstance);
	//
	// for (_uint i = 0; i < iNumInstance; ++i)
	// {
	// 	pInstanceVertices[i].vRight = _float4(1.0f, 0.f, 0.f, 0.f);
	// 	pInstanceVertices[i].vUp = _float4(0.0f, 1.f, 0.f, 0.f);
	// 	pInstanceVertices[i].vLook = _float4(0.0f, 0.f, 1.f, 0.f);
	// 	pInstanceVertices[i].vPosition = _float4(0.f, 0.f, 0.f, 1.f);
	// 	pInstanceVertices[i].vStartTangent = _float3::Zero;
	// 	pInstanceVertices[i].vEndTangent = _float3::Zero;
	//
	// }
	
	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = InstanceData.data();

	m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pInstanceBuffer);

	// Safe_Delete_Array(pInstanceVertices);
#pragma endregion
}

void CVIBuffer_Line_Instancing::Free()
{
	CVIBuffer_Instancing::Free();
}

CVIBuffer_Line_Instancing* CVIBuffer_Line_Instancing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Line_Instancing*		pInstance = new CVIBuffer_Line_Instancing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Line_Instancing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Line_Instancing::Clone(void* pArg)
{
	CVIBuffer_Line_Instancing*		pInstance = new CVIBuffer_Line_Instancing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Line_Instancing");
		Safe_Release(pInstance);
	}
	return pInstance;
}
