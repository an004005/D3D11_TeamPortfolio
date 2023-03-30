#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Canvas_BrainMap.h"

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
	_bool	Get_OnMouse() {
		return m_bOnMouse;
	}
	_bool	Get_OnButton() {
		return m_bOnButton;
	}
	void	Set_OnButton() {
		m_bOnButton = false;
	}

	CCanvas_BrainMap::BRAININFO	Get_BrainInfo() {
		return m_BrainInfo;
	}
	void	Set_BrainInfo(CCanvas_BrainMap::BRAININFO tBrainInfo);

	void	Set_IconIndex(const _float2 & fIndex);
	void	Set_BrainUse();

	_float2	Get_CurrentIconIndex() {
		return m_tParams.Float2s[0];
	}

	void	Set_OnIcon() {
		m_tParams.Float2s[0] = m_BrainInfo.vOnIconIndex;
	}

private:
	class CMain_OnMouseColorUI* m_pMouseColorUI = { nullptr };
	CCanvas_BrainMap::BRAININFO	m_BrainInfo;

	_bool	m_bOnMouse = { false };
	_bool	m_bOnButton = { false };

public:
	static CMain_OnMouseUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END