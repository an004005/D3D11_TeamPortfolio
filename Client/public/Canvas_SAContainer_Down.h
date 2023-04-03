#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_SAContainer_Down : public CCanvas
{
protected:
	CCanvas_SAContainer_Down(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_SAContainer_Down(const CCanvas_SAContainer_Down& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void   Tick(_double TimeDelta) override;

private:
	void	KeyInput();
	void	MouseLight_Tick(const _double& TimeDelta);
	void	Action_Tick(const _double& TimeDelta);

private:
	_uint	m_iAllCount = { 0 };
	_uint	m_iInputCount = { 0 };
	_bool	m_bInput = { false };
	_bool	m_bKeyDown = { false };

	_bool m_fAlphaDown = { false };
	_float m_fMouseLightAlpha = { 0.0f };

	_double m_dMuseMove_TimeAcc = { 0.0 };

public:
	static CCanvas_SAContainer_Down* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END