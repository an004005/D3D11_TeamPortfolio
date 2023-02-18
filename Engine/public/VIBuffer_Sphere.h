#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Sphere  final : public CVIBuffer
{
protected:
   CVIBuffer_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
   CVIBuffer_Sphere(const CVIBuffer_Sphere& rhs);
   virtual ~CVIBuffer_Sphere() = default;

public:
   virtual HRESULT Initialize_Prototype(_float _radius);
   virtual HRESULT Initialize(void* pArg);

public:
   static CVIBuffer_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float _radius);
   virtual CComponent* Clone(void* pArg = nullptr);
   virtual void Free() override;
};

END