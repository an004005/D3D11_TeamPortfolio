#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CMain_BrainMapIconPickUI final : public CUI
{
private:
	CMain_BrainMapIconPickUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMain_BrainMapIconPickUI(const CMain_BrainMapIconPickUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

	virtual void	Imgui_RenderProperty() override;

public:
	void	Set_Pick(const _bool bPick) {
		m_bPick = bPick;
	}

private:
	_float	m_fAlpha = { 0.0f };
	_bool	m_bAlphaDwon = { false };
	_bool	m_bPick = { false };

public:
	static CMain_BrainMapIconPickUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END