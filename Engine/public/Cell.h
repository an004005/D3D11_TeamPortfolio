#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum NEIGHBOR { NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END	};
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_float3 Get_Point(POINT ePoint) {
		return m_vPoints[ePoint];
	}

	void Set_Neighbor(NEIGHBOR eNeighbor, CCell* pNeighbor) {
		m_iNeighborIndices[eNeighbor] = pNeighbor->m_iIndex;
	}

public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex);
	void Imgui_Render();
	_int GetIdx() const { return m_iIndex; }
	_bool IsCliff() const { return m_bCliff; }
	void SetCliff(_bool bCliff) { m_bCliff = bCliff; }

public:
	_bool Compare_Points(const _float3& SourPoint, const _float3& DestPoint);
	_bool IsIn(_fvector vTargetPos, _int* pNeighborIndex);
	_float3 GetOutLineVector(_fvector vTargetPos);
	_float GetHeight(_fvector vPos);
	_int GetNeighborIdx(NEIGHBOR eNeighbor) const { return m_iNeighborIndices[eNeighbor]; }
	_vector GetNormal();

#ifdef _DEBUG
public:
	HRESULT Render(class CShader* pShader);
	void Set_Point(POINT ePoint, _float3 vNewPoint);

#endif

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

private:
	_int				m_iIndex = 0;
	_float3				m_vPoints[POINT_END];	
	_int				m_iNeighborIndices[NEIGHBOR_END];

	_bool				m_bCliff = false;

#ifdef _DEBUG
private:
	class CVIBuffer_Cell*			m_pVIBuffer = nullptr;
#endif // _DEBUG

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	virtual void Free();
};

END