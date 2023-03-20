#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CMain_PickUI final : public CUI
{
private:
	CMain_PickUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMain_PickUI(const CMain_PickUI& rhs);

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
	_bool	Get_OnButton() {
		return m_vOnButton;
	}
	void	Set_OnButton() {
		m_vOnButton = false;
	}

private:
	void	CurrentPick(const _double & TimeDelta);

private:
	_bool	m_vOnButton = { false };

	_tchar*	m_szText = { L"" };
	_float2 m_vPosition = { 0.0f, 0.0f };
	_float2 m_vSize = { 0.0f, 0.0f };


public:
	static CMain_PickUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END