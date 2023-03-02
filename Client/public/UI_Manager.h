#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Canvas.h"

class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

public:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	map<wstring, CCanvas*> Get_Canvas() {
		return m_mapCanvass;
	}

	// Drive
	_float	Get_DriveGauge() {
		return m_fDriveGauge;
	}
	void	Set_DriveGaugePlus(const _float & DriveGauge) {
		if (100.0f < m_fDriveFullGauge + DriveGauge)
		{
			m_fDriveFullGauge = 1.0f;
			return;
		}
		m_fDriveGauge = DriveGauge / m_fDriveFullGauge;
	}
	//void	Set_DriveGaugeReset(const _float & DriveGauge) {	// 사용하는 용도가 아닌 초기화 또는 테스트를 위한 함수
	//	m_fDriveGauge = DriveGauge;
	//}

	_float	Get_DriveFullTime() {
		return m_fDriveFullTime;
	}
	void	Set_DriveFullTime(const _float & fDriveTime) {
		m_fDriveFullTime = fDriveTime;
	}

	// ---




public:
	CCanvas* Find_CCanvas(const wstring& pCanvasTag);
	void Add_CCanvas(const wstring& pCanvasTag, CCanvas* pCanvas);

private:
	map<wstring, CCanvas*> m_mapCanvass;

	_float m_fDriveGauge = { 0.0f };
	_float m_fDriveFullGauge = { 100.0f }; // UITODO : 나중에 드라이브 게이지의 전체 크기를 지정해 주세요.
	_float m_fDriveFullTime = { 0.0f };

public:
	virtual void Free() override;
};

