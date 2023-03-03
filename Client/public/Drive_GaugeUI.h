#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CDrive_GaugeUI final : public CUI
{
private:
	CDrive_GaugeUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDrive_GaugeUI(const CDrive_GaugeUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

public:
	_bool	Get_GaugeFull() {	// Full UI 를 띄우기 위해서 제공하는 함수
		return m_bGaugeFull;
	}

	void	Set_DriveGauge_Plus() {	// 외부에서 드라이브 게이지를 증가 시킬 떄 호출한다.
		m_bDriveGauge_Plus = true;
	}
	void	Set_DriveGauge_Minus() {
		m_bDriveGauge_Minus = true;
	}

private:
	void	DrivaGaugePlus_Tick(const _double & dTimeDelta);
	void	DrivaGaugeMinus_Tick(const _double & dTimeDelta);

	void	DirveRightDot_Tick(const _double & dTimeDelta);

private:
	_bool	m_bDriveGauge_Plus = { false };	
	_bool	m_bDriveGauge_Minus = { false };

	_bool	m_bGaugeFull = { false };
	_double	m_dGaugeFull_TimeAcc = { 0.0 };

public:
	static CDrive_GaugeUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END