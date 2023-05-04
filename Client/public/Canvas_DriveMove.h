#pragma once

#include "Client_Defines.h"
#include "Canvas.h"

BEGIN(Client)

class CCanvas_DriveMove : public CCanvas
{
protected:
	CCanvas_DriveMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_DriveMove(const CCanvas_DriveMove& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	BeginTick() override;
	virtual void	Tick(_double TimeDelta) override;

	virtual void	Imgui_RenderProperty() override;

public:
	void	Set_OnDrive(const _float & fDriveFullTime);	// ����̺갡 ����� ��ü �ð��� �־��ּ���. ����̺� ����� ���� �˴ϴ�. (��' �������� �� á�� ��)

	void	Set_DriveGauge(const _float & fMinGauge, const _float & fMaxGauge) {
		m_fDriveGauge = fMinGauge / fMaxGauge;
	}

private:
	void	DriveGauge_Tikc();

	void	DriveGaugeFull_Tick();
	void	Drive_Tick();	// ����̺긦 ����ϰ� �Ǹ� ��� ����

private:
	_float	m_fDriveGauge = { 0.0f };	

	_bool	m_bOnDrive = { false };		// ���ο��� �������� ���� �� ��쿡 true �� �����մϴ�.
	_bool	m_bStartBrainField= { false };		

	_float	m_fRightDotCount = { 1.0f };
	_float m_fShaderRightDot = { 0.0f };

	_bool	m_bRightFullDot = { false };

public:
	static CCanvas_DriveMove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END