#pragma once
#include "Client_Defines.h"
#include "VIBuffer_Point_Instancing.h"

BEGIN(Client)
class CVIBuffer_Trail_Instancing :	public CVIBuffer_Point_Instancing
{
public:
	CVIBuffer_Trail_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Trail_Instancing(const CVIBuffer_Trail_Instancing& rhs);
	virtual ~CVIBuffer_Trail_Instancing() = default;

public:
	virtual void Tick(_double TimeDelta) override;
	void AddData(_float4x4 Matrix);
	vector<VTXMATRIX>* GetDatas() { return &m_PointData; }
	_bool IsEmpty() const { return m_PointData.empty(); }
	void ClearBuffer() { m_PointData.clear(); }
	void SetLifeCount(_bool bLife) { m_bLife = bLife; }

private:
	vector<VTXMATRIX> m_PointData;
	_bool m_bLife = true;

public:
	static CVIBuffer_Trail_Instancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance = 1);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END