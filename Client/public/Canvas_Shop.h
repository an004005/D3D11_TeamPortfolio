#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_Shop : public CCanvas
{
public:
	enum SHOPCANVAS { PURCHASE, SALE/*, EXCHANGE*/, SHOPCANVAS_END };

protected:
	CCanvas_Shop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_Shop(const CCanvas_Shop& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;

public:
	void	Set_ShopUIClose() { 
		m_bShopUI = false;
		m_arrCanvass[m_eMainCanvas]->SetVisible(false);
		for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
			iter->second->SetVisible(false);
	}

	void	Set_ShopUI() {
		m_bOpenShop = true;
	}

private:
	HRESULT	Add_MainCanvas();

	void	KeyInput();
	void	Menu_Tick();
	void	Canvas_Visible();
	
private:
	CCanvas*			m_arrCanvass[SHOPCANVAS::SHOPCANVAS_END] = { nullptr, nullptr };
	SHOPCANVAS	m_eMainCanvas = { SHOPCANVAS::PURCHASE };
	
	_tchar*	m_szShopText = { L"" };
	_bool		m_bShopUI = { false };
	
	class CShaderUI* m_pShaderUI = { nullptr };
	_bool		m_bOpen = { false };
	_bool		m_bAlpha = { false };
	_bool		m_bReverse = { false };
	_float		m_fAlpha = { 0.0f };

	_bool		m_bOpenShop = { false };

public:
	static CCanvas_Shop* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END