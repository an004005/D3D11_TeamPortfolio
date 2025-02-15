#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CButtonUI final : public CUI
{
private:
	CButtonUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CButtonUI(const CButtonUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;

public:
	_bool	Get_Input() {
		return m_bInput;
	}
	void	Set_Input() {
		m_bInput = false;
	}

private:
	_bool	m_bInput = { false };

public:
	static CButtonUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END