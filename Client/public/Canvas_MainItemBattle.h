#pragma once

#include "Canvas.h"
#include "Client_Defines.h"
#include "Item_Manager.h"

BEGIN(Client)

class CCanvas_MainItemBattle : public CCanvas
{
protected:
	CCanvas_MainItemBattle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_MainItemBattle(const CCanvas_MainItemBattle& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;

private:
	HRESULT	Initialize_ItemCanvas();
	void	Add_ItemCanvas(const size_t & iIndex);
	void	Item_Tick();
	void	ChildPickUI_Tick();

private:
	vector<pair<wstring, class CCanvas_ItemWindow*>>	m_vecItemCanvass;

	size_t				m_iItemIndex = { 0 };
	size_t				m_iPickIndex = { 0 };
	size_t				m_iPrePickIndex = { 0 };
	vector<pair<wstring, CItem_Manager::ITEMINFO>> m_vecItemInfo;

	_bool				m_bBeSeen = { false };

public:
	static CCanvas_MainItemBattle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END