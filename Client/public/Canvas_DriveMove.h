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
	void	Set_OnDrive(_bool bOn) {
		m_bOnDrive = bOn;
	}
	void	Set_LeftDot(_bool bOn) {
		m_bLeftDot = bOn;
	}
	void	Set_RightDot(_bool bOn) {
		m_bRightDot = bOn;
	}
	void	Set_RightDotFull(_bool bOn) {
		m_bRightDotFull = bOn;
	}


private:
	_bool	m_bOnDrive = { false };
	_bool	m_bLeftDot = { false };
	_bool	m_bRightDot = { false };
	_bool	m_bRightDotFull = { false };

public:
	static CCanvas_DriveMove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END