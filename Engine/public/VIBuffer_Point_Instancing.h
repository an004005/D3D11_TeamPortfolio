#pragma once
#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instancing : public CVIBuffer_Instancing
{
public:
	CVIBuffer_Point_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instancing(const CVIBuffer_Point_Instancing& rhs);
	virtual ~CVIBuffer_Point_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumInstance);
	virtual HRESULT Render();
	virtual void Tick(_double TimeDelta) override;

	void SetInstanceBuffer(VTXMATRIX* pBuffer, _uint iSize);
	void SetInstanceBuffer(list<VTXMATRIX>* pBuffer);


protected:
	_uint m_iInitNumInstance;

public:
	virtual void Free() override;
   static CVIBuffer_Point_Instancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iInstanceNum);
   virtual CComponent* Clone(void* pArg = nullptr);
};

END