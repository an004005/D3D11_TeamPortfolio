#include "stdafx.h"
#include "..\public\VIBuffer_Line.h"

CVIBuffer_Line::CVIBuffer_Line(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Line::CVIBuffer_Line(const CVIBuffer_Line& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Line::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXLINE);
	m_iNumVertices = 2000;
	// m_iNumPrimitive = 2;	
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES16);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	VTXLINE vtxLine;
	ZeroMemory(&vtxLine, sizeof(VTXLINE));

	vector<VTXLINE> tmp(1000, vtxLine);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = tmp.data();
	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;
#pragma endregion


	return S_OK;
}

HRESULT CVIBuffer_Line::Initialize(void* pArg)
{
	return CVIBuffer::Initialize(pArg);
}

HRESULT CVIBuffer_Line::Render()
{

	ID3D11Buffer*			pVertexBuffers[] = {
		m_pVB, 			
	};

	_uint					iStrides[] = {
		m_iStride,
	};

	_uint					iOffsets[] = {
		0, 
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

 	m_pContext->Draw(m_iLineCnt, 0);
	return S_OK;
}

void CVIBuffer_Line::SetLines(const vector<VTXLINE>& lines)
{
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_iLineCnt = lines.size();
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	memcpy(SubResource.pData, lines.data(), sizeof(VTXLINE) * m_iLineCnt);
	m_pContext->Unmap(m_pVB, 0);
}

CVIBuffer_Line* CVIBuffer_Line::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Line*		pInstance = new CVIBuffer_Line(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Line");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Line::Clone(void* pArg)
{
	CVIBuffer_Line*		pInstance = new CVIBuffer_Line(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Line");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Line::Free()
{
	CVIBuffer::Free();
}
