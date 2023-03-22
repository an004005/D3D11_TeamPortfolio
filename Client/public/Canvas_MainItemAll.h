#pragma once

#include "Canvas.h"
#include "Client_Defines.h"
#include "Item_Manager.h"

BEGIN(Client)

class CCanvas_MainItemAll : public CCanvas
{
protected:
	CCanvas_MainItemAll(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_MainItemAll(const CCanvas_MainItemAll& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	LoadFromJson(const Json& json) override;

private:
	HRESULT	Initialize_ItemCanvas();
	void	Add_ItemCanvas(const size_t & iIndex);
	void	Item_Tick();
	void	ChildPickUI_Tick();

private:
	vector<pair<wstring, class CCanvas_ItemWindow*>>	m_vecItemCanvass;

	size_t				m_iPickIndex = { 0 };
	size_t				m_iPrePickIndex = { 0 };
	vector<pair<wstring, CItem_Manager::ITEMINFO>> m_vecItemInfo;
	CItem_Manager::MAINITEM m_eMainItem = { CItem_Manager::MAINITEM_END };

	_bool				m_bBeSeen = { false };
	_bool				m_bPickInitialize = { false };
		
public:
	static CCanvas_MainItemAll* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END