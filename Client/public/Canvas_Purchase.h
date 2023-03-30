#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_Purchase : public CCanvas
{
public:
	enum MAINITEM { ALL, BATTLE, WEAPON, MAINITEM_END };

protected:
	CCanvas_Purchase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_Purchase(const CCanvas_Purchase& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;

private:
	HRESULT	Add_MainCanvas();

	void	Menu_Tick();
	void	Canvas_Visible();

private:
	CCanvas* m_arrCanvass[MAINITEM::MAINITEM_END] = { false, false, false };
	MAINITEM	m_eMainItem = { MAINITEM::ALL };
	
	_float2		m_vPosssss = { 0.0f, 0.0f };

public:
	static CCanvas_Purchase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END