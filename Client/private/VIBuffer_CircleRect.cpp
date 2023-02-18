#include "stdafx.h"
#include "..\public\VIBuffer_CircleRect.h"


CVIBuffer_CircleRect::CVIBuffer_CircleRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
   : CVIBuffer(pDevice,pContext)
{
}

CVIBuffer_CircleRect::CVIBuffer_CircleRect(const CVIBuffer_CircleRect& rhs)
   : CVIBuffer(rhs)
{
}


HRESULT CVIBuffer_CircleRect::Initialize_Prototype(_uint iSegmentCnt, _float fRingWidth)
{
	Assert(fRingWidth < 1.f);
	Assert(fRingWidth > 0.f);

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = iSegmentCnt * 2 + 2;
	m_iNumPrimitive = iSegmentCnt * 2;	
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES16);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

#pragma region VERTEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXTEX*			pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXTEX) * m_iNumVertices);

	_float fCurDegree = 0.f;
	_float fSegmentPerDeg = 360.f / static_cast<_float>(iSegmentCnt);
	_float fUpRadius = 0.5f;
	_float fDownRadius = fUpRadius - fRingWidth;
	_float fU = 0.f;

	for (_uint i = 0; i <= iSegmentCnt; ++i)
	{
		const _uint iUpIdx = i * 2;
		const _uint iDownIdx = iUpIdx + 1;

		const _float theta = (fCurDegree - 90.f) * XM_PI / 180.f;

		pVertices[iUpIdx].vPosition = _float3(cosf(theta) * fUpRadius, -sinf(theta) * fUpRadius, 0.f);
		pVertices[iUpIdx].vTexUV = _float2(fU, 0.f);

		pVertices[iDownIdx].vPosition = _float3(cosf(theta) * fDownRadius, -sinf(theta) * fDownRadius, 0.f);
		pVertices[iDownIdx].vTexUV = _float2(fU, 1.f);

		fCurDegree += fSegmentPerDeg;
		fU += 1.f / (_float)iSegmentCnt;
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

#pragma endregion

#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	FACEINDICES16*		pIndices = new FACEINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES16) * m_iNumPrimitive);

	for (_uint i = 0; i < iSegmentCnt; ++i)
	{
		_ushort iLeftTop = i * 2;
		_ushort iLeftBot = iLeftTop + 1;
		_ushort iRightTop = (i + 1) * 2;
		_ushort iRightBot = iRightTop + 1;

		pIndices[i * 2]._0 = iLeftTop;
		pIndices[i * 2]._1 = iRightTop;
		pIndices[i * 2]._2 = iRightBot;

		pIndices[i * 2 + 1]._0 = iLeftTop;
		pIndices[i * 2 + 1]._1 = iRightBot;
		pIndices[i * 2 + 1]._2 = iLeftBot;
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;


	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

CVIBuffer_CircleRect* CVIBuffer_CircleRect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
                                                   _uint iSegmentCnt, _float fRingWidth)
{
	CVIBuffer_CircleRect*		pInstance = new CVIBuffer_CircleRect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(iSegmentCnt, fRingWidth)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_CircleRect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_CircleRect::Clone(void* pArg)
{
	CVIBuffer_CircleRect*		pInstance = new CVIBuffer_CircleRect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_CircleRect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_CircleRect::Free()
{
	CVIBuffer::Free();
}
