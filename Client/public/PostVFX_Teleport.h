#pragma once
#include "PostProcess.h"
#include "PostVFX_WhiteOut.h"

BEGIN(Client)

class CPostVFX_Teleport : public CPostProcess
{
protected:
	CPostVFX_Teleport(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPostVFX_Teleport(const CPostVFX_Teleport& rhs);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;

public:
	DECLARE_CREATE(CPostVFX_Teleport)
	DECLARE_CLONE(CPostVFX_Teleport)
};

END