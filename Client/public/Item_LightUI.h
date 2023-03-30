#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CItem_LightUI final : public CUI
{
private:
	CItem_LightUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_LightUI(const CItem_LightUI& rhs);

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
	void	Set_ItemLightUse() {
		m_bUse = true;
		m_fAlphaDown = false;
		m_dAlpha_TimeAcc = 0.0;
	}

	void	Set_LightColor(const _float3 fColor) {
		m_tParams.Float4s[0].x = fColor.x;
		m_tParams.Float4s[0].y = fColor.y;
		m_tParams.Float4s[0].z = fColor.z;
	}

public:
	_bool	m_bUse = { false };
	_bool	m_fAlphaDown = { false };
	_float	m_fAlpha = { 0.0f };
	_double	m_dAlpha_TimeAcc = { 0.0 };

public:
	static CItem_LightUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END