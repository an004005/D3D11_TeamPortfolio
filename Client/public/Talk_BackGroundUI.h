#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Timeline.h"

BEGIN(Client)

class CTalk_BackGroundUI final : public CUI
{
private:
	CTalk_BackGroundUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTalk_BackGroundUI(const CTalk_BackGroundUI& rhs);

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
	void	Set_TalkUse() {
		m_bVisible = true;
		m_bUse = true;
		m_bTimeLine = true;
	}

private:
	CSimpleTimeline m_Timeline;

	_bool	m_bUse = { false };
	_bool	m_bTimeLine = { false };

public:
	static CTalk_BackGroundUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END