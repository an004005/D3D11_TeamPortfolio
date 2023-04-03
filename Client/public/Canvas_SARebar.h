#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_SARebar : public CCanvas
{
protected:
	CCanvas_SARebar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_SARebar(const CCanvas_SARebar& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void   Tick(_double TimeDelta) override;

private:
	void	KeyInput();
	void	MouseLight_Tick(const _double& TimeDelta);
	void	Action_Tick(const _double& TimeDelta);

private:
	_uint	m_iInputCount = { 0 };
	_bool	m_bInput = { false };

	_bool m_fAlphaDown = { false };
	_float m_fMouseLightAlpha = { 0.0f };

public:
	static CCanvas_SARebar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END