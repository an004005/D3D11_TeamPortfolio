#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_Sale : public CCanvas
{
public:
	enum SHOPMENU { ALL, BATTLE, WEAPON, MAINITEM_END };

protected:
	CCanvas_Sale(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_Sale(const CCanvas_Sale& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;

private:
	HRESULT	Add_ShopCanvas();

	void	MenuPcik_Tick();
	void	Canvas_Visible();

private:
	class CCanvas_SaleKinds* m_arrCanvass[SHOPMENU::MAINITEM_END] = { nullptr, nullptr, nullptr };
	SHOPMENU	m_eShopMunu = { SHOPMENU::ALL };

	_bool	m_bReset = { false };

public:
	static CCanvas_Sale* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END