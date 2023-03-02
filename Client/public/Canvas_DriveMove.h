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
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

public:
	void	Set_OnDrive(const _float & fDriveFullTime);	// 드라이브가 진행될 전체 시간을 넣어주세요.

	void	Set_Circle(_bool bOn) {	
		m_bOnCircle = bOn;
	}

private:
	void	DriveGaugeFull_Tick();

private:
	_bool	m_bOnDrive = { false };
	_bool	m_bOnDriveStart_OneCheck = { false };	
	_bool	m_bOnDrive_OneCheck = { false };	// 한 번만 UI 정보를 Set 하거나, 출력을 Set 해주기 위해서

	_bool	m_bOnCircle = { false };	// UITODO : 'B' 를 사용하는 시점에 On

public:
	static CCanvas_DriveMove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END