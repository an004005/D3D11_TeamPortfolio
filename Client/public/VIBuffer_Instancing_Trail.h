#pragma once
#include "VIBuffer_Point_Instancing.h"
#include "Client_Defines.h"

BEGIN(Client)

class CVIBuffer_Instancing_Trail : public CVIBuffer_Point_Instancing
{
public:
	CVIBuffer_Instancing_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instancing_Trail(const CVIBuffer_Instancing_Trail& rhs);
	virtual ~CVIBuffer_Instancing_Trail() = default;

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
	virtual void Free() override;
	static CVIBuffer_Instancing_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance = 1);
	virtual CComponent* Clone(void* pArg = nullptr) override;
};

END