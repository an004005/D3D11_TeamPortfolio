#pragma once
#include "PostProcess.h"
#include "PostVFX_WhiteOut.h"

BEGIN(Client)
class CPostVFX_Penetrate : public CPostProcess
{
protected:
	CPostVFX_Penetrate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPostVFX_Penetrate(const CPostVFX_Penetrate& rhs);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;

public:
	DECLARE_CREATE(CPostVFX_Penetrate)
	DECLARE_CLONE(CPostVFX_Penetrate)
};

END