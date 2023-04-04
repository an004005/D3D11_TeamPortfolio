#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Invisible : public CVIBuffer
{
protected:
	CVIBuffer_Invisible(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Invisible(const CVIBuffer_Invisible& rhs);
	virtual ~CVIBuffer_Invisible() = default;

public:
	virtual HRESULT Initialize_Prototype(list<_float3> float3list, _float fHeight);
	virtual HRESULT Initialize(void* pArg);

public:
	void Set_VtxData(list<_float3> float3list, _float fHeight);

private:
	_float3			m_vSourPos = { 0.f, 0.f, 0.f };
	_float3			m_vDestPos = { 0.f, 0.f, 0.f };
	_float			m_fHeight = 0.f;
	list<_float3>	m_VtxPtList;

public:
	static CVIBuffer_Invisible* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, list<_float3> float3list, _float fHeight);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END