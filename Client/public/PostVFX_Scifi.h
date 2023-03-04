#pragma once
#include "Client_Defines.h"
#include "PostProcess.h"

class CPostVFX_Scifi :	public CPostProcess
{
	CPostVFX_Scifi(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPostVFX_Scifi(const CPostVFX_Scifi& rhs);

public:
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					Tick(_double TimeDelta) override;

public:
	static CPostVFX_Scifi*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
};

