#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_MainItem : public CCanvas
{
public:
	enum MAINITEM { ALL, BATTLE, WEAPON, ETC, MAINITEM_END };

protected:
	CCanvas_MainItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_MainItem(const CCanvas_MainItem& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

private:
	HRESULT	Add_MainCanvas();

	void	Menu_Tick();
	void	Canvas_Visible();

private:
	CCanvas*		m_arrCanvass[MAINITEM_END];
	MAINITEM	m_eMainItem = { ALL };

public:
	static CCanvas_MainItem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END