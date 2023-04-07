#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CBoss_AppearBackUI final : public CUI
{
private:
	CBoss_AppearBackUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_AppearBackUI(const CBoss_AppearBackUI& rhs);

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
	void	Set_AppearBackGround() {
		m_bVisible = true;
		m_bAppearBackGround = true;
	}

	void	Set_AppearEnd() {
		m_bAppearEnd = true;
	}

	//void	Set_Value(const _float fSpeed, const _float fAlpha) {
	//	m_fSpeed = fSpeed;
	//	m_fAlpha = fAlpha;
	//}

private:
	_bool	m_bAppearEnd = { false };
	_bool	m_bAppearBackGround = { false };

	_float	m_fSpeed = { 0.0f };
	_float	m_fAlpha = { 0.0f };

public:
	static CBoss_AppearBackUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END