#pragma once
#include "PostProcess.h"
#include "Client_Defines.h"
#include "Timeline.h"

BEGIN(Client)

class CPostVFX_SuperSpeed : public CPostProcess
{
protected:
	CPostVFX_SuperSpeed(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPostVFX_SuperSpeed(const CPostVFX_SuperSpeed& rhs);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;

	void Active(_bool bActive);

private:
	_bool m_bActive = false;

public:
	DECLARE_CREATE(CPostVFX_SuperSpeed)
	DECLARE_CLONE(CPostVFX_SuperSpeed)
};

END