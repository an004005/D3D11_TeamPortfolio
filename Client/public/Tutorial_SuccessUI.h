#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CTutorial_SuccessUI final : public CUI
{
private:
	CTutorial_SuccessUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTutorial_SuccessUI(const CTutorial_SuccessUI& rhs);

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
	_bool	Get_SuccessEnd() {
		return m_bSuccessEnd;
	}

private:
	_bool	m_bSuccessEnd = { false };
	_int	m_iDeleteCount = { 0 };
	_double	m_dChange_TimcAcc = { 0.0 };

public:
	static CTutorial_SuccessUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END