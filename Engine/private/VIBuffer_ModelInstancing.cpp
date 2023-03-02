#include "stdafx.h"
#include "VIBuffer_ModelInstancing.h"
#include "Bone.h"
#include "GameUtils.h"
#include "GameInstance.h"

CVIBuffer_ModelInstancing::CVIBuffer_ModelInstancing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{
}

CVIBuffer_ModelInstancing::CVIBuffer_ModelInstancing(const CVIBuffer_ModelInstancing & rhs)
	: CVIBuffer_Instancing(rhs)
	, m_strName(rhs.m_strName)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
{
}

HRESULT CVIBuffer_ModelInstancing::Initialize_Prototype(HANDLE hFile, CModel_Instancing* pModel, _uint iNumInstance)
{
	
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iNumInstance = iNumInstance;
	m_iIndexCountPerInstance = 3;

	DWORD dwByte = 0;
	m_strName = CGameUtils::ReadStrFile(hFile); /* Read */
	ReadFile(hFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr); /* Read */
	ReadFile(hFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr); /* Read */
	ReadFile(hFile, &m_iNumPrimitive, sizeof(_uint), &dwByte, nullptr); /* Read */

	m_iNumVertexBuffers = 2;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

#pragma region VERTEX_BUFFER

	FAILED_CHECK(Ready_VertexBuffer_NonAnimModel(hFile, pModel));
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

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		FACEINDICES32 buffer;
		ReadFile(hFile, &buffer, sizeof(FACEINDICES32), &dwByte, nullptr); /* Read */
		pIndices[i] = buffer;
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

#pragma region INSTANCE_BUFFER

#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_ModelInstancing::Initialize(void * pArg)
{

	m_iInstanceStride = sizeof(VTXMATRIX);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	VTXMATRIX*			pInstanceVertices = new VTXMATRIX[m_iNumInstance];
	ZeroMemory(pInstanceVertices, sizeof(VTXMATRIX) * m_iNumInstance);

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pInstanceVertices;
	
	FAILED_CHECK(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pInstanceBuffer));

	Safe_Delete_Array(pInstanceVertices);

	return S_OK;
}


void CVIBuffer_ModelInstancing::Tick(_double TimeDelta)
{
}

HRESULT CVIBuffer_ModelInstancing::Render()
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
	m_pContext->DrawIndexedInstanced(m_iNumIndices, m_WorldMatrix.size(), 0, 0, 0);

	return S_OK;
}

void CVIBuffer_ModelInstancing::Add_Instance(_float4x4 WorldMatrixf4x4)
{
	if (m_WorldMatrix.size() >= m_iNumInstance)
	{
		MSG_BOX("Full_Instance Buffer");
		return;
	}

	//월드 메트릭스 추가
	m_WorldMatrix.emplace_back(WorldMatrixf4x4);	
}

void CVIBuffer_ModelInstancing::Delete_Instance(_uint iIndex)
{
	if (m_WorldMatrix.empty())
	{
		MSG_BOX("Is Empty");
		return;
	}

	if (iIndex >= m_iNumInstance)
	{
		MSG_BOX("over index size");
		return;
	}

	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	//월드 메트릭스 삭제
	m_WorldMatrix.erase(m_WorldMatrix.begin() + iIndex);

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, m_WorldMatrix.data(), sizeof(_float4x4) * m_WorldMatrix.size());

	m_pContext->Unmap(m_pInstanceBuffer, 0);
}

void CVIBuffer_ModelInstancing::Modify_Matrix(_uint iIndex, _float4x4 WorldMatrixf4x4)
{
	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_WorldMatrix[iIndex] = WorldMatrixf4x4;

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, m_WorldMatrix.data(), sizeof(_float4x4) * m_WorldMatrix.size());

	m_pContext->Unmap(m_pInstanceBuffer, 0);
}

void CVIBuffer_ModelInstancing::Map_Mesh()
{
	//쓰레드 충돌나서 ADD_INSTANCE 끝나고 마지막에 한번만 실행
	CONTEXT_LOCK;

	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, m_WorldMatrix.data(), sizeof(_float4x4) * m_WorldMatrix.size());

	m_pContext->Unmap(m_pInstanceBuffer, 0);
}


HRESULT CVIBuffer_ModelInstancing::Ready_VertexBuffer_NonAnimModel(HANDLE hFile, CModel_Instancing* pModel)
{
	m_iStride = sizeof(VTXMODEL);
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	VTXMODEL* pNonAnimVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pNonAnimVertices, sizeof(VTXMODEL) * m_iNumVertices);

	const _matrix PivotMatrix = pModel->GetPivotMatrix();
	DWORD dwByte;
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		VTXMODEL buffer;
		ReadFile(hFile, &buffer, sizeof(VTXMODEL), &dwByte, nullptr); /* Read */

		XMStoreFloat3(&buffer.vPosition, XMVector3TransformCoord(XMLoadFloat3(&buffer.vPosition), PivotMatrix));
		XMStoreFloat3(&buffer.vNormal, XMVector3TransformNormal(XMLoadFloat3(&buffer.vNormal), PivotMatrix));
		pNonAnimVertices[i] = buffer;
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pNonAnimVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pNonAnimVertices);

	return S_OK;
}


CVIBuffer_ModelInstancing * CVIBuffer_ModelInstancing::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, HANDLE hFile, CModel_Instancing* pModel, _uint iNumInstance)
{
	CVIBuffer_ModelInstancing*		pInstance = new CVIBuffer_ModelInstancing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(hFile, pModel, iNumInstance)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_ModelInstancing");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_ModelInstancing::Clone(void * pArg)
{
	CVIBuffer_ModelInstancing*		pInstance = new CVIBuffer_ModelInstancing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_ModelInstancing");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CVIBuffer_ModelInstancing::Free()
{
	__super::Free();
}
