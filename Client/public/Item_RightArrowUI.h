#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CItem_RightArrowUI final : public CUI
{
private:
	CItem_RightArrowUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_RightArrowUI(const CItem_RightArrowUI& rhs);

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
	void	Set_Input() {
		m_bInput = true;
	}

	void	Set_MouseInput() {	
		m_bMouse = true;
	}

	_bool		Get_KeyDown() {
		return m_bCursorKeyDown;
	}
	void		Set_KeyDown() {
		m_bCursorKeyDown = false;
	}

private:
	_bool	m_bInput = { false };
	_float	m_fStartX = { 0.0f };
	_double m_fMoveTimeAcc = { 0.0f };
	_bool		m_bMouse = { false };
	_bool		m_bCursorKeyDown = { false };

public:
	static CItem_RightArrowUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END