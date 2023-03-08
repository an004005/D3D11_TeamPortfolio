#pragma once
#include "PostProcess.h"
#include "Client_Defines.h"

BEGIN(Client)
class CPostVFX_ColorGrading : public CPostProcess
{
protected:
	CPostVFX_ColorGrading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPostVFX_ColorGrading(const CPostVFX_ColorGrading& rhs);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;


public:
	DECLARE_CREATE(CPostVFX_ColorGrading)
	DECLARE_CLONE(CPostVFX_ColorGrading)
};

END