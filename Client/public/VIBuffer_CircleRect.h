#pragma once
#include "VIBuffer.h"
#include "Client_Defines.h"

BEGIN(Client)

class CVIBuffer_CircleRect : public CVIBuffer
{
protected:
   CVIBuffer_CircleRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
   CVIBuffer_CircleRect(const CVIBuffer_CircleRect& rhs);
   virtual ~CVIBuffer_CircleRect() = default;

public:
   virtual HRESULT Initialize_Prototype(_uint iSegmentCnt, _float fRingWidth);

public:
   static CVIBuffer_CircleRect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSegmentCnt = 180, _float fRingWidth = 0.1f);
   virtual CComponent* Clone(void* pArg = nullptr);
   virtual void Free() override;
};

END