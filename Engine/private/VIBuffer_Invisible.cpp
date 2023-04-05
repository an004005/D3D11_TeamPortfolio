#include "stdafx.h"
#include "VIBuffer_Invisible.h"

CVIBuffer_Invisible::CVIBuffer_Invisible(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Invisible::CVIBuffer_Invisible(const CVIBuffer_Invisible& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Invisible::Initialize_Prototype(list<_float3> float3list, _float fHeight)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXPOS);
	m_iNumVertices = float3list.size() * 2 + 2;		// Á¡ °¹¼ö * 2 + 2
	m_iNumPrimitive = m_iNumVertices * 2;	// m_iNumVertices * 2
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
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

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOS));

	Set_VtxData(float3list, fHeight);

	_float3 vPos1, vPos2;
	_uint iCnt = 0;
	// ¡Ú 
	for (auto& iter : m_VtxPtList)
	{
		if (iCnt == (m_VtxPtList.size() - 2))
		{
			vPos1 = iter;
		}
		else if (iCnt == (m_VtxPtList.size() - 1))
		{
			vPos2 = iter;
		}

		iCnt++;
	}

	pVertices[0].vPosition = _float3(vPos1.x, _float(vPos1.y + m_fHeight), vPos1.z); //_float3(-0.5f, 0.f, 0.5f); 
	pVertices[1].vPosition = _float3(vPos2.x, _float(vPos2.y + m_fHeight), vPos2.z); //_float3(0.5f, 0.f, 0.5f); 
	pVertices[2].vPosition = vPos1;//_float3(0.5f, 0.f, -0.5f);
	pVertices[3].vPosition = vPos2;//_float3(-0.5f, 0.f, -0.5f);
	/*for (_int i = 0; i < float3list.size(); ++i)
	{
		pVertices[i].vPosition = ;
	}*/

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

	FACEINDICES32* pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);


#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Invisible::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Invisible::Set_VtxData(list<_float3> float3list, _float fHeight)
{
	m_VtxPtList = float3list;
	m_fHeight = fHeight;
}

CVIBuffer_Invisible* CVIBuffer_Invisible::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, list<_float3> float3list, _float fHeight)
{
	CVIBuffer_Invisible* pInstance = new CVIBuffer_Invisible(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(float3list, fHeight)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Invisible");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Invisible::Clone(void* pArg)
{
	CVIBuffer_Invisible* pInstance = new CVIBuffer_Invisible(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Invisible");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Invisible::Free()
{
	__super::Free();
}
