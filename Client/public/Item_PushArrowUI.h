#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CItem_PushArrowUI final : public CUI
{
private:
	CItem_PushArrowUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_PushArrowUI(const CItem_PushArrowUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	BeginTick() override;
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

	void	Set_Color(const _float4 & fColor) {
		m_tParams.Float4s[0] = fColor;
	}

private:
	_bool	m_bInput = { false };
	_float	m_fStartY = { 0.0f };
	_double m_fMoveTimeAcc = { 0.0f };

public:
	static CItem_PushArrowUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END