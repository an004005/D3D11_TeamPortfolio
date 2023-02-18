#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_PosRect : public CVIBuffer
{
protected:
	CVIBuffer_PosRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_PosRect(const CVIBuffer_PosRect& rhs);
	virtual ~CVIBuffer_PosRect() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);


public:
	static CVIBuffer_PosRect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END

