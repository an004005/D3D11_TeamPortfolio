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

public:
	_bool	Get_GaugeFull() {	// Full UI �� ���� ���ؼ� �����ϴ� �Լ�
		return m_bGaugeFull;
	}

	void	Set_DriveGauge_Use(const _float & fDriveFullTime) {
		m_bDriveGauge_Use = true;
		m_fDriveFullTime = fDriveFullTime;
	}

	_float	Get_CurrentDriveGauge() {
		return m_fCurrentDriveGauge;
	}

	void	Set_DriveGauge(const _float & fGauge) {
		m_fDriveGauge = fGauge;
	}

private:
	void	DrivaGaugePlus_Tick(const _double & dTimeDelta);
	void	DirveRightDot_Tick(const _double & dTimeDelta);
	void	DrivaGaugeUse_Tick(const _double & dTimeDelta);

private:
	_float	m_fDriveGauge = { 0.0f };			// Canvas ���� 0~1 �� �Ѱ��ش�.
	_float	m_fCurrentDriveGauge = { 0.0f };	// �� Ŭ���� �� ���� ����ϴ� �뵵
	_float	m_fDriveFullTime = { 0.0f };		// Drive �� ����� �ϴ� ��ü�ð�
	_double m_dCurrentDriveTime = { 0.0 };

	_bool	m_bDriveGauge_Use = { false };

	_bool	m_bGaugeFull = { false };
	_double	m_dGaugeFull_TimeAcc = { 0.0 };

public:
	static CDrive_GaugeUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END