#pragma once
#include "Client_Defines.h"
#include "PostProcess.h"

BEGIN(Client)

class CPostVFX_Distortion :	public CPostProcess
{
public:
	CPostVFX_Distortion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPostVFX_Distortion(const CPostVFX_Distortion& rhs);

public:
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					Tick(_double TimeDelta) override;
	// void							SetWhiteOutTime(_float fWhiteOutTime);

public:
	static CPostVFX_Distortion*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*			Clone(void* pArg) override;
};

END