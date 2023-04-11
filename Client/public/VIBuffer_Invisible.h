#pragma once
#include "VIBuffer.h"

BEGIN(Client)

class CVIBuffer_Invisible : public CVIBuffer
{
protected:
	CVIBuffer_Invisible(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Invisible(const CVIBuffer_Invisible& rhs);
	virtual ~CVIBuffer_Invisible() = default;

public:
	virtual HRESULT Initialize_Prototype(const vector<VTXNORTEX>& VtxData, const vector<FACEINDICES32>& IdxData);
	virtual HRESULT Initialize(void* pArg);

public:
	static CVIBuffer_Invisible* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const vector<VTXNORTEX>& VtxData, const vector<FACEINDICES32>& IdxData);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END