#include "stdafx.h"
#include "VIBuffer_Mesh_Instance.h"

#include "GameUtils.h"

CVIBuffer_Mesh_Instancing::CVIBuffer_Mesh_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instancing(pDevice, pContext)
{
}

CVIBuffer_Mesh_Instancing::CVIBuffer_Mesh_Instancing(const CVIBuffer_Mesh_Instancing& rhs)
	: CVIBuffer_Instancing(rhs)
	, m_iInitNumInstance(rhs.m_iInitNumInstance)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_strName(rhs.m_strName)
{
	m_bIsInstance = true;
}

HRESULT CVIBuffer_Mesh_Instancing::Initialize_Prototype(const char* pModelFilePath, _float4x4 TransformMatrix, _uint iNumInstance)
{
	FAILED_CHECK(__super::Initialize_Prototype());

	_tchar szFilePath[MAX_PATH];
	CGameUtils::c2wc(pModelFilePath, szFilePath);
	HANDLE hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if(INVALID_HANDLE_VALUE == hFile)
	{
		IM_ERROR("fail to open file %s", pModelFilePath);
		return E_FAIL;
	}
	DWORD dwByte = 0;

	m_strName = CGameUtils::ReadStrFile(hFile);

	_uint iNumMeshes = 0;
	ReadFile(hFile, &iNumMeshes, sizeof(_uint), &dwByte, nullptr); /* Read */

	for (int j = 0; j < iNumMeshes; ++j)
	{
		m_strName = CGameUtils::ReadStrFile(hFile); /* Read */
		ReadFile(hFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr); /* Read */
		ReadFile(hFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr); /* Read */
		ReadFile(hFile, &m_iNumPrimitive, sizeof(_uint), &dwByte, nullptr); /* Read */
	}

	m_iNumVertexBuffers = 2;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

#pragma region VERTEXBUFFER
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

		_matrix PivotMatrix = _float4x4::CreateScale({ 0.01f, 0.01f, 0.01f }) *_float4x4::CreateRotationY(XMConvertToRadians(-180.f));
		
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
	}
#pragma endregion



#pragma region INSTANCEBUFFER
	
	m_iInstanceStride = sizeof(VTXINSTANCE_DRAWDATA);
	m_iInitNumInstance = iNumInstance;
	m_iNumInstance = iNumInstance;

#pragma endregion

#pragma region INDEXBUFFER

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


	return S_OK;
}

HRESULT CVIBuffer_Mesh_Instancing::Initialize(void* pArg)
{
	VTXINSTANCE_DRAWDATA*		pInstance = new VTXINSTANCE_DRAWDATA[m_iNumInstance];

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iInstanceStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(pInstance, sizeof(VTXINSTANCE_DRAWDATA) * m_iNumInstance);

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pInstance;

	if (FAILED(m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pInstanceBuffer)))
		return E_FAIL;

	Safe_Delete_Array(pInstance);

	return S_OK;
}

void CVIBuffer_Mesh_Instancing::Tick(_double TimeDelta)
{
}

HRESULT CVIBuffer_Mesh_Instancing::Render()
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

void CVIBuffer_Mesh_Instancing::Start()
{
	m_bGenerate = true;
}





void CVIBuffer_Mesh_Instancing::GetRandomVector(_float2* out, _float2* min, _float2* max)
{
	out->x = GetRandomFloat(min->x, max->x);
	out->y = GetRandomFloat(min->y, max->y);
}

void CVIBuffer_Mesh_Instancing::GetRandomVector(_float3* out, _float3* min, _float3* max)
{
	out->x = GetRandomFloat(min->x, max->x);
	out->y = GetRandomFloat(min->y, max->y);
	out->z = GetRandomFloat(min->z, max->z);
}

void CVIBuffer_Mesh_Instancing::GetRandomVector(_float4* out, _float4* min, _float4* max)
{
	out->x = GetRandomFloat(min->x, max->x);
	out->y = GetRandomFloat(min->y, max->y);
	out->z = GetRandomFloat(min->z, max->z);
	out->w = GetRandomFloat(min->w, max->w);
}

float CVIBuffer_Mesh_Instancing::GetRandomFloat(float lowBound, float highBound)
{
	if (lowBound >= highBound)
		return lowBound;

	float f = (rand() % 10000) * 0.0001f;

	return (f * (highBound - lowBound)) + lowBound;
}

void CVIBuffer_Mesh_Instancing::SetInstanceBuffer(VTXINSTANCE* pBuffer, _uint iSize)
{
	// 안씀
	//m_iNumInstance = iSize;
	//if (m_iNumInstance > m_iInitNumInstance)
	//	m_iNumInstance = m_iInitNumInstance;

	//D3D11_MAPPED_SUBRESOURCE			SubResource;
	//ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	//m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	//memcpy(SubResource.pData, pBuffer, sizeof(VTXINSTANCE_DRAWDATA) * m_iNumInstance);
	//m_pContext->Unmap(m_pInstanceBuffer, 0);
}

void CVIBuffer_Mesh_Instancing::SetInstanceBuffer(list<VTXINSTANCE>* pBuffer)
{
	m_iNumInstance = (_uint)pBuffer->size();
	if (m_iNumInstance > m_iInitNumInstance)
		m_iNumInstance = m_iInitNumInstance;
	IM_LOG("%d", m_iNumInstance);

	D3D11_MAPPED_SUBRESOURCE			SubResource;
	ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	VTXINSTANCE_DRAWDATA tmp;

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	_uint iIdx = 0;
	for (auto& data : *pBuffer)
	{
		tmp.vRight = data.vRight;
		tmp.vUp = data.vUp;
		tmp.vLook = data.vLook;
		tmp.vPosition = data.vPosition;
		//tmp.vColor = data.vColor;

		//memcpy(&tmp, &data, sizeof(VTXINSTANCE_DRAWDATA));
		((VTXINSTANCE_DRAWDATA*)SubResource.pData)[iIdx] = tmp;
		++iIdx;
		if (iIdx >= m_iNumInstance)
			break;
	}

	m_pContext->Unmap(m_pInstanceBuffer, 0);
}

CVIBuffer_Mesh_Instancing* CVIBuffer_Mesh_Instancing::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char* pModelFilePath, _float4x4 TransformMatrix, _uint iNumInstance)
{
	CVIBuffer_Mesh_Instancing*		pInstance = new CVIBuffer_Mesh_Instancing(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, TransformMatrix, iNumInstance)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Mesh_Instancing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Mesh_Instancing::Clone(void* pArg)
{
	CVIBuffer_Mesh_Instancing*		pInstance = new CVIBuffer_Mesh_Instancing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Mesh_Instancing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Mesh_Instancing::Free()
{
	__super::Free();

	Safe_Release(m_pInstanceBuffer);

}
