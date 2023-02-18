#include "stdafx.h"
#include "..\public\VIBuffer_Terrain.h"
#include "GameUtils.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
	: CVIBuffer(rhs)
	  , m_pVertices(rhs.m_pVertices)
	  , m_pIndices(rhs.m_pIndices)
	  , m_iNumVerticesX(rhs.m_iNumVerticesX)
	  , m_iNumVerticesZ(rhs.m_iNumVerticesZ)
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* pHeightMapFilePath)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	_ulong dwByte = 0;
	HANDLE hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	BITMAPFILEHEADER fh;
	BITMAPINFOHEADER ih;
	_ulong* pPixel = nullptr;

	ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	pPixel = new _ulong[m_iNumVertices];
	ZeroMemory(pPixel, sizeof(_ulong) * m_iNumVertices);

	ReadFile(hFile, pPixel, sizeof(_ulong) * m_iNumVertices, &dwByte, nullptr);

	CloseHandle(hFile);

	m_iNumVertexBuffers = 1;
	m_iStride = sizeof(VTXNORTEX);

	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_iIndicesSizePerPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesPerPrimitive = 3;
	m_iNumIndices = m_iNumIndicesPerPrimitive * m_iNumPrimitive;

#pragma region VERTEX_BUFFER
	m_pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXNORTEX));

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			//	11111111 11111011 11111011 11111011
			//& 00000000 00000000 00000000 11111111

			// m_pVertices[iIndex].vPosition = _float3(j, (pPixel[iIndex] & 0x000000ff) / 10.f, i);
			m_pVertices[iIndex].vPosition = _float3(j, 0.f, i);
			m_pVertices[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			m_pVertices[iIndex].vTexUV = _float2(j / (m_iNumVerticesX - 1.0f), i / (m_iNumVerticesZ - 1.0f));
		}
	}

	Safe_Delete_Array(pPixel);

#pragma endregion


#pragma region INDEX_BUFFER
	m_pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_uint iNumFaces = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			_uint iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector vSour, vDest, vNormal;

			m_pIndices[iNumFaces]._0 = iIndices[0];
			m_pIndices[iNumFaces]._1 = iIndices[1];
			m_pIndices[iNumFaces]._2 = iIndices[2];

			vSour = XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(
				&m_pVertices[m_pIndices[iNumFaces]._0].vPosition);
			vDest = XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(
				&m_pVertices[m_pIndices[iNumFaces]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vNormal,
			              XMVector3Normalize(XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vNormal) + vNormal));
			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vNormal,
			              XMVector3Normalize(XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vNormal) + vNormal));
			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vNormal,
			              XMVector3Normalize(XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vNormal) + vNormal));

			++iNumFaces;

			m_pIndices[iNumFaces]._0 = iIndices[0];
			m_pIndices[iNumFaces]._1 = iIndices[2];
			m_pIndices[iNumFaces]._2 = iIndices[3];

			vSour = XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vPosition) - XMLoadFloat3(
				&m_pVertices[m_pIndices[iNumFaces]._0].vPosition);
			vDest = XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vPosition) - XMLoadFloat3(
				&m_pVertices[m_pIndices[iNumFaces]._1].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vNormal,
			              XMVector3Normalize(XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._0].vNormal) + vNormal));
			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vNormal,
			              XMVector3Normalize(XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._1].vNormal) + vNormal));
			XMStoreFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vNormal,
			              XMVector3Normalize(XMLoadFloat3(&m_pVertices[m_pIndices[iNumFaces]._2].vNormal) + vNormal));

			++iNumFaces;
		}
	}

#pragma endregion


	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = m_pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;


	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndicesSizePerPrimitive * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = m_pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	// Safe_Delete_Array(m_pVertices);
	// Safe_Delete_Array(m_pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

bool CVIBuffer_Terrain::PickTerrainVtx(_float4& vPickPos, _fmatrix WorldMatrixInv)
{
	_float4 vRayOrigin, vRayDir;
	CGameUtils::GetPickingRay(vRayOrigin, vRayDir);

	_vector _vRayOrigin = XMLoadFloat4(&vRayOrigin);
	_vector _vRayDir = XMLoadFloat4(&vRayDir);

	_vRayOrigin = XMVector4Transform(_vRayOrigin, WorldMatrixInv);
	_vRayDir = XMVector4Normalize(XMVector4Transform(_vRayDir, WorldMatrixInv));

	list<pair<_float, _uint>> XCandidateList;
	list<pair<_uint, _uint>> stack;
	stack.push_back({0, m_iNumVerticesX - 1});


	_uint iVtxX = 0;

	// x ¸ÕÀú
	while (!stack.empty())
	{
		_uint iMinX = stack.back().first;
		_uint iMaxX = stack.back().second;
		stack.pop_back();

		_uint iIndexes[4]
		{
			iMinX,
			(m_iNumVerticesZ - 1) * m_iNumVerticesX + iMinX,
			(m_iNumVerticesZ - 1) * m_iNumVerticesX + iMaxX,
			iMaxX
		};
		_float fDistance;

		_float4 v0 = _float4(m_pVertices[iIndexes[0]].vPosition.x, m_pVertices[iIndexes[0]].vPosition.y, m_pVertices[iIndexes[0]].vPosition.z, 1.f);
		_float4 v1 = _float4(m_pVertices[iIndexes[1]].vPosition.x, m_pVertices[iIndexes[1]].vPosition.y, m_pVertices[iIndexes[1]].vPosition.z, 1.f);
		_float4 v2 = _float4(m_pVertices[iIndexes[2]].vPosition.x, m_pVertices[iIndexes[2]].vPosition.y, m_pVertices[iIndexes[2]].vPosition.z, 1.f);
		_vector _v0 = XMLoadFloat4(&v0);
		_vector _v1 = XMLoadFloat4(&v1);
		_vector _v2 = XMLoadFloat4(&v2);

		if (TriangleTests::Intersects(_vRayOrigin, _vRayDir, _v0, _v1, _v2, fDistance))
		{
			if (iMaxX - iMinX == 1)
				XCandidateList.push_back({fDistance, iMinX});
			else
			{
				stack.push_back({iMinX, (iMaxX + iMinX) / 2});
				stack.push_back({(iMaxX + iMinX) / 2, iMaxX});
			}
			continue;
		}

		v0 = _float4(m_pVertices[iIndexes[0]].vPosition.x, m_pVertices[iIndexes[0]].vPosition.y, m_pVertices[iIndexes[0]].vPosition.z, 1.f);
		v1 = _float4(m_pVertices[iIndexes[2]].vPosition.x, m_pVertices[iIndexes[2]].vPosition.y, m_pVertices[iIndexes[2]].vPosition.z, 1.f);
		v2 = _float4(m_pVertices[iIndexes[3]].vPosition.x, m_pVertices[iIndexes[3]].vPosition.y, m_pVertices[iIndexes[3]].vPosition.z, 1.f);
		_v0 = XMLoadFloat4(&v0);
		_v1 = XMLoadFloat4(&v1);
		_v2 = XMLoadFloat4(&v2);

		if (TriangleTests::Intersects(_vRayOrigin, _vRayDir, _v0, _v1, _v2, fDistance))
		{
			if (iMaxX - iMinX == 1)
				XCandidateList.push_back({fDistance, iMinX});
			else
			{
				stack.push_back({iMinX, (iMaxX + iMinX) / 2});
				stack.push_back({(iMaxX + iMinX) / 2, iMaxX});
			}
		}
	}

	if (XCandidateList.empty())
		return false;

	XCandidateList.sort();
	iVtxX = XCandidateList.front().second;
	stack.clear();

	_float fMinDistance = FLT_MAX;
	stack.push_back({0, m_iNumVerticesZ - 1});

	while (!stack.empty())
	{
		_uint iMinZ = stack.back().first;
		_uint iMaxZ = stack.back().second;
		stack.pop_back();

		_uint iIndexes[4]
		{
			iMinZ * m_iNumVerticesX + iVtxX,
			iMaxZ * m_iNumVerticesX + iVtxX,
			iMaxZ * m_iNumVerticesX + iVtxX + 1,
			iMinZ * m_iNumVerticesX + iVtxX + 1
		};

		_float fDistance;

		_float4 v0 = _float4(m_pVertices[iIndexes[0]].vPosition.x, m_pVertices[iIndexes[0]].vPosition.y, m_pVertices[iIndexes[0]].vPosition.z, 1.f);
		_float4 v1 = _float4(m_pVertices[iIndexes[1]].vPosition.x, m_pVertices[iIndexes[1]].vPosition.y, m_pVertices[iIndexes[1]].vPosition.z, 1.f);
		_float4 v2 = _float4(m_pVertices[iIndexes[2]].vPosition.x, m_pVertices[iIndexes[2]].vPosition.y, m_pVertices[iIndexes[2]].vPosition.z, 1.f);
		_vector _v0 = XMLoadFloat4(&v0);
		_vector _v1 = XMLoadFloat4(&v1);
		_vector _v2 = XMLoadFloat4(&v2);

		if (TriangleTests::Intersects(_vRayOrigin, _vRayDir, _v0, _v1, _v2, fDistance))
		{
			if (iMaxZ - iMinZ == 1 && fMinDistance > fDistance)
			{
				fMinDistance = fDistance;
				_vRayOrigin += _vRayDir * fDistance;
				XMStoreFloat4(&vPickPos, _vRayOrigin);
			}
			else
			{
				stack.push_back({iMinZ, (iMaxZ + iMinZ) / 2});
				stack.push_back({(iMaxZ + iMinZ) / 2, iMaxZ});
				continue;
			}
		}

		v0 = _float4(m_pVertices[iIndexes[0]].vPosition.x, m_pVertices[iIndexes[0]].vPosition.y, m_pVertices[iIndexes[0]].vPosition.z, 1.f);
		v1 = _float4(m_pVertices[iIndexes[2]].vPosition.x, m_pVertices[iIndexes[2]].vPosition.y, m_pVertices[iIndexes[2]].vPosition.z, 1.f);
		v2 = _float4(m_pVertices[iIndexes[3]].vPosition.x, m_pVertices[iIndexes[3]].vPosition.y, m_pVertices[iIndexes[3]].vPosition.z, 1.f);
		_v0 = XMLoadFloat4(&v0);
		_v1 = XMLoadFloat4(&v1);
		_v2 = XMLoadFloat4(&v2);

		if (TriangleTests::Intersects(_vRayOrigin, _vRayDir, _v0, _v1, _v2, fDistance))
		{
			if (iMaxZ - iMinZ == 1 && fMinDistance > fDistance)
			{
				fMinDistance = fDistance;
				_vRayOrigin += _vRayDir * fDistance;
				XMStoreFloat4(&vPickPos, _vRayOrigin);
			}
			else
			{
				stack.push_back({iMinZ, (iMaxZ + iMinZ) / 2});
				stack.push_back({(iMaxZ + iMinZ) / 2, iMaxZ});
			}
		}
	}


	return fMinDistance < FLT_MAX;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
                                             const _tchar* pHeightMapFilePath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pHeightMapFilePath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CVIBuffer_Terrain::Free()
{
	if (!IsCloned())
	{
		Safe_Delete_Array(m_pVertices);
		Safe_Delete_Array(m_pIndices);
	}
	m_pVertices = nullptr;
	m_pIndices = nullptr;
	__super::Free();
}
