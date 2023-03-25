#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CMain_BrainUI final : public CUI
{
private:
	CMain_BrainUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMain_BrainUI(const CMain_BrainUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

	virtual void	Imgui_RenderProperty() override;

private:
	_float	m_fValue = { 0.0f };
	_bool	m_bValueDwon = { false };

public:
	static CMain_BrainUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END