#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
protected:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pHeightMapFilePath);
	virtual HRESULT Initialize(void* pArg);

	bool PickTerrainVtx(OUT _float4& vPickPos, _fmatrix WorldMatrixInv);

	_uint GetNumVtxX() const { return m_iNumVerticesX; }
	_uint GetNumVtxZ() const { return m_iNumVerticesZ; }

private:
	

private:
	_uint			m_iNumVerticesX = 0;
	_uint			m_iNumVerticesZ = 0;

	VTXNORTEX*			m_pVertices = nullptr;
	FACEINDICES32*		m_pIndices = nullptr;

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;

};

END