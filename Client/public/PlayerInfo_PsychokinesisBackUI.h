#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CPlayerInfo_PsychokinesisBackUI final : public CUI
{
private:
	CPlayerInfo_PsychokinesisBackUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerInfo_PsychokinesisBackUI(const CPlayerInfo_PsychokinesisBackUI& rhs);

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
	void	Set_PsychokinesisGauge(const _int iType, const _float & fGauge);

private:
	_float m_fPsychokinesisGauge = { 0.0f };	// 0~1
	_float m_fCurrentPsychokinesisGauge = { 0.0f };

	_bool	m_bMinus = { false };

public:
	static CPlayerInfo_PsychokinesisBackUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END