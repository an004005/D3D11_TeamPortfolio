#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CPlayerInfo_PsychokinesisUI final : public CUI
{
private:
	CPlayerInfo_PsychokinesisUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerInfo_PsychokinesisUI(const CPlayerInfo_PsychokinesisUI& rhs);

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
	void	Set_Type(const _uint iLevel);
	void	Set_Gauge(const _uint iType, const _float& fGauge);

public:
	_float2	Gauge_Position();

private:
	_float m_fPsychokinesisGauge = { 0.0f };	// 0~1
	_float m_fMaxLevelGauge = { 0.0f };

public:
	static CPlayerInfo_PsychokinesisUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END