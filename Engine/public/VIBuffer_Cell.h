#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cell final : public CVIBuffer
{
protected:
	CVIBuffer_Cell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Cell(const CVIBuffer_Cell& rhs);
	virtual ~CVIBuffer_Cell() = default;

public:
	virtual HRESULT Initialize_Prototype(const _float3* pPoints);
	virtual HRESULT Initialize(void* pArg);
	void Update_VertexBuffer(array<_float3, 3> Points);

public:
	static CVIBuffer_Cell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;

};

END