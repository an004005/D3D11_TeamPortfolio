#pragma once
#include "VIBuffer_Instancing.h"
#include "Client_Defines.h"

BEGIN(Client)

class CVIBuffer_Line_Instancing : public CVIBuffer_Instancing
{
public:
	CVIBuffer_Line_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Line_Instancing(const CVIBuffer_Line_Instancing& rhs);
	virtual ~CVIBuffer_Line_Instancing() = default;

public:
	virtual HRESULT Initialize_Prototype(const vector<VTXLINE_POS_INSTANCE>& InstanceData);
	virtual void Tick(_double TimeDelta) override {}
	virtual HRESULT Render() override;

	// void ReCreate(_uint iNumInstance);

public:
	virtual void Free() override;
   static CVIBuffer_Line_Instancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const vector<VTXLINE_POS_INSTANCE>& InstanceData);
   virtual CComponent* Clone(void* pArg = nullptr);

};

END