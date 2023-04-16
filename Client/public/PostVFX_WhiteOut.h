#pragma once
#include "PostProcess.h"
#include "Client_Defines.h"
#include "Timeline.h"

BEGIN(Client)
class CPostVFX_WhiteOut : public CPostProcess
{
protected:
	CPostVFX_WhiteOut(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPostVFX_WhiteOut(const CPostVFX_WhiteOut& rhs);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	void SetWhiteOutTime(_float fWhiteOutTime);

	void BlackInOut();

private:
	CSimpleTimeline m_Timeline;

public:
	DECLARE_CREATE(CPostVFX_WhiteOut)
	DECLARE_CLONE(CPostVFX_WhiteOut)
};

END