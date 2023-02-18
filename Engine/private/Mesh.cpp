#include "stdafx.h"
#include "..\public\Mesh.h"
#include "Model.h"
#include "Bone.h"
#include "GameUtils.h"

CMesh::CMesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh & rhs)
	: CVIBuffer(rhs)
	, m_eType(rhs.m_eType)
	, m_strName(rhs.m_strName)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_BoneNames(rhs.m_BoneNames)
{
}

HRESULT CMesh::Initialize_Prototype(CModel::TYPE eType, HANDLE hFile, CModel* pModel)
{
	m_eType = eType;

	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	DWORD dwByte = 0;
	m_strName = CGameUtils::ReadStrFile(hFile); /* Read */
	ReadFile(hFile, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr); /* Read */
	ReadFile(hFile, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr); /* Read */
	ReadFile(hFile, &m_iNumPrimitive, sizeof(_uint), &dwByte, nullptr); /* Read */
	m_iNumVertexBuffers = 1;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

#pragma region VERTEX_BUFFER

	HRESULT			hr = 0;

	if (CModel::TYPE_NONANIM == m_eType)
	{
		hr = Ready_VertexBuffer_NonAnimModel(hFile, pModel);
	}
	else
	{
		hr = Ready_VertexBuffer_AnimModel(hFile, pModel);
	}

	if (FAILED(hr))
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

void CMesh::SetUp_MeshBones(CModel* pModel)
{
	if (m_BoneNames.empty())
	{
		if (CBone* pBone = pModel->Get_BonePtr(m_strName))
		{
			m_BoneNames.push_back(m_strName);
		}
	}

	for (auto& boneName : m_BoneNames)
	{
		CBone* pBone = pModel->Get_BonePtr(boneName);
		Assert(pBone != nullptr);

		m_Bones.push_back(pBone);	
		Safe_AddRef(pBone);
	}
}

void CMesh::SetUp_BoneMatrices(_float4x4 * pBoneMatrices, _fmatrix PivotMatrix)
{	
	_uint		iNumBones = 0;

	if (m_BoneNames.empty())
		XMStoreFloat4x4(&pBoneMatrices[0], PivotMatrix);

	for (const auto& pBone : m_Bones)
	{
		// BoneMatrix = 오프셋매트릭스 * 컴바인드매트릭스;
		XMStoreFloat4x4(&pBoneMatrices[iNumBones++], 
			pBone->Get_OffsetMatrix()
			* pBone->Get_CombindMatrix()
			* PivotMatrix);
	}
}

HRESULT CMesh::Ready_VertexBuffer_NonAnimModel(HANDLE hFile, CModel* pModel)
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

HRESULT CMesh::Ready_VertexBuffer_AnimModel(HANDLE hFile, CModel* pModel)
{
	m_iStride = sizeof(VTXANIMMODEL);
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	DWORD dwByte = 0;

	_uint iNumBones = 0;
	ReadFile(hFile, &iNumBones, sizeof(_uint), &dwByte, nullptr); /* Read */
	m_BoneNames.reserve(iNumBones);

	for (_uint i = 0; i < iNumBones; ++i)
	{
		m_BoneNames.push_back(CGameUtils::ReadStrFile(hFile));  /* Read */
	}


	VTXANIMMODEL* pAnimVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pAnimVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		VTXANIMMODEL buffer;
		ReadFile(hFile, &buffer, sizeof(VTXANIMMODEL), &dwByte, nullptr); /* Read */
		pAnimVertices[i] = buffer;
	}

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pAnimVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pAnimVertices);

	return S_OK;
}

CMesh * CMesh::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::TYPE eType, HANDLE hFile, CModel* pModel)
{
	CMesh*		pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, hFile, pModel)))
	{
		MSG_BOX("Failed to Created : CMesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CMesh::Clone(void * pArg)
{
	CMesh*		pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CMesh::Free()
{
	__super::Free();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();
}
