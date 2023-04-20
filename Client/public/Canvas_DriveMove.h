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
	void	Set_OnDrive(const _float & fDriveFullTime);	// 드라이브가 진행될 전체 시간을 넣어주세요. 드라이브 사용이 시작 됩니다. (단' 게이지가 다 찼을 때)

	void	Set_DriveGauge(const _float & fMinGauge, const _float & fMaxGauge) {
		m_fDriveGauge = fMinGauge / fMaxGauge;
	}

private:
	void	DriveGauge_Tikc();

	void	DriveGaugeFull_Tick();
	void	Drive_Tick();	// 드라이브를 사용하게 되면 계속 실행

private:
	_float	m_fDriveGauge = { 0.0f };	

	_bool	m_bOnDrive = { false };		// 내부에서 게이지가 가득 찬 경우에 true 로 변경합니다.
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