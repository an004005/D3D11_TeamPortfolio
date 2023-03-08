#pragma once
#include "Client_Defines.h"
#include "PostProcess.h"

BEGIN(Client)
class CPostVFX_HitDecal : public CPostProcess
{
public:
	CPostVFX_HitDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPostVFX_HitDecal(const CPostVFX_HitDecal& rhs);

public:
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					Tick(_double TimeDelta) override;

public:
	static CPostVFX_HitDecal*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
};

END