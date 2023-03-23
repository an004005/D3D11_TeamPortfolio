#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CMain_OnMouseUI final : public CUI
{
private:
	CMain_OnMouseUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMain_OnMouseUI(const CMain_OnMouseUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

public:


private:
	void	CurrentOnMouse(const _double & TimeDelta);

private:
	_bool	m_bOnMouse = { false };
	_bool	m_bOnButton = { false };

public:
	static CMain_OnMouseUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END