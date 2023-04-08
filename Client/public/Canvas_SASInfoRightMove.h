#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_SASInfoRightMove : public CCanvas
{
protected:
	CCanvas_SASInfoRightMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_SASInfoRightMove(const CCanvas_SASInfoRightMove& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	BeginTick() override;
	virtual void	Tick(_double TimeDelta) override;

	virtual void	Imgui_RenderProperty() override;

public:
	void	Set_HillBar();

private:
	void	SASRightHp_Tick();
	void	RendomTexture_Tick(const _double & dTimeDelta);
	void	HillBar_Tick(const _double& TimeDelta);

private:
	_float	m_fHPRatio = { 0.0f };
	_double	m_dRendomTexture_TimeAcc = { 0.0 };

	_bool	m_bHill = { false };
	_bool	m_bHpHill = { false };
	_double	m_dMaxHillChake_TimeAcc = { 0.0 };

public:
	static CCanvas_SASInfoRightMove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END