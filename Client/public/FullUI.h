#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CFullUI final : public CUI
{
private:
	CFullUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFullUI(const CFullUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

public:
	void		Set_Alpha() {
		m_bAlpha = true;
	}
	_bool Get_Alpha() { return m_bAlpha; }

	void		Set_Float4sW(const _float& fFloat) {
		m_tParams.Float4s[0].w = fFloat;
	}
	_float Get_Float4sW() {
		return m_tParams.Float4s[0].w;
	}

private:
	_bool	m_bAlpha = { false };
	_bool	m_bReverse = { false };
	_float	m_fAlpha = { 0.0f };

public:
	static CFullUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END