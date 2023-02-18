#pragma once
#include "VIBuffer.h"
#include "Client_Defines.h"

BEGIN(Client)

class CVIBuffer_CircleRectLookOut : public CVIBuffer
{
protected:
   CVIBuffer_CircleRectLookOut(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
   CVIBuffer_CircleRectLookOut(const CVIBuffer_CircleRectLookOut& rhs);
   virtual ~CVIBuffer_CircleRectLookOut() = default;

public:
   virtual HRESULT Initialize_Prototype(_uint iSegmentCnt, _float fRingWidth, _uint iUVCnt);

public:
   static CVIBuffer_CircleRectLookOut* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSegmentCnt = 180, _float fRingWidth = 1.f, _uint iUVCnt = 8);
   virtual CComponent* Clone(void* pArg = nullptr);
   virtual void Free() override;
};

END