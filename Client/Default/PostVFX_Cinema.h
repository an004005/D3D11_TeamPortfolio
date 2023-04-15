#pragma once
#include "PostProcess.h"
#include "Client_Defines.h"

BEGIN(Client)

class CPostVFX_Cinema : public CPostProcess
{
protected:
	CPostVFX_Cinema(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPostVFX_Cinema(const CPostVFX_Cinema& rhs);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	void Active(_bool bActive);

private:
	_bool m_bActive = false;

public:
	DECLARE_CREATE(CPostVFX_Cinema)
	DECLARE_CLONE(CPostVFX_Cinema)
};

class CPostVFX_Loading : public CPostProcess
{
protected:
	CPostVFX_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPostVFX_Loading(const CPostVFX_Loading& rhs);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;

public:
	DECLARE_CREATE(CPostVFX_Loading)
	DECLARE_CLONE(CPostVFX_Loading)
};

END