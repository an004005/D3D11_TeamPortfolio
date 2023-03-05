#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CDrive_RightDotUI final : public CUI
{
public:
	enum DRIVECOUNT {ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, ELEVEN, TWELVE, DRIVECOUNT_END };

private:
	CDrive_RightDotUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDrive_RightDotUI(const CDrive_RightDotUI& rhs);

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
	void	Set_DriveCount();
	void	Reset_DriveRightDot() {
		m_iDriveCount = 0;
		m_eDriveCount = ONE;
	}

private:
	void	DriveCount_Tick();

private:
	DRIVECOUNT	m_eDriveCount = ONE;
	_uint		m_iDriveCount = { 0 };

public:
	static CDrive_RightDotUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END