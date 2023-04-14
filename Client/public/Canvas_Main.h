#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_Main : public CCanvas
{
public:
	enum MAINCANVAS { PARTY, ITEM, EQUIPMENT, BRAINMAP, MAINCANVAS_END };

protected:
	CCanvas_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_Main(const CCanvas_Main& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;

public:
	void	Set_MainUIClose() { 
		m_bMainUI = false;
		m_arrCanvass[m_eMainCanvas]->SetVisible(false);
		for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
			iter->second->SetVisible(false);
	}

private:
	HRESULT	Add_MainCanvas();

	void	KeyInput();
	void	Menu_Tick();
	void	Canvas_Visible();
	
private:
	CCanvas* m_arrCanvass[MAINCANVAS_END] = { nullptr, nullptr, nullptr, nullptr };
	MAINCANVAS	m_eMainCanvas = { MAINCANVAS:: PARTY };
	
	_tchar*	m_szManuText = { L"" };
	_bool		m_bMainUI = { false };

	class CShaderUI* m_pShaderUI = { nullptr };
	_bool		m_bOpen = { false };
	_bool		m_bAlpha = { false };
	_bool		m_bReverse = { false };

public:
	static CCanvas_Main* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END