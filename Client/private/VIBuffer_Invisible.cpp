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

HRESULT CVIBuffer_Invisible::Initialize_Prototype(const vector<VTXNORTEX>& VtxData, const vector<FACEINDICES32>& IdxData)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXNORTEX);
	m_iNumVertices = (_uint)VtxData.size();
	m_iNumPrimitive = (_uint)IdxData.size();
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


	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = VtxData.data();

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

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = IdxData.data();

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Invisible::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Invisible* CVIBuffer_Invisible::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const vector<VTXNORTEX>& VtxData, const vector<FACEINDICES32>& IdxData)
{
	CVIBuffer_Invisible* pInstance = new CVIBuffer_Invisible(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(VtxData, IdxData)))
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
