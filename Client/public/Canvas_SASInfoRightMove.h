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
	virtual void	Tick(_double TimeDelta) override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

public:
	void	Set_SASRightHp(const _float & fCurrentHp, const _float & fMaxHp) {	// UITODO : BeginTick() 에서 호출
		m_fHp = fCurrentHp / fMaxHp;
	}

private:
	void	ChildHp_Tick();
	void	RendomTexture(const _double & dTimeDelta);

private:
	_float	m_fHp = { 0.0f };
	_double	m_dRendomTexture_TimeAcc = { 0.0 };

public:
	static CCanvas_SASInfoRightMove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END