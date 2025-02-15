#pragma once

#include "Canvas.h"
#include "Client_Defines.h"
#include "Item_Manager.h"

BEGIN(Client)

class CCanvas_PurchaseKinds : public CCanvas
{
protected:
	CCanvas_PurchaseKinds(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_PurchaseKinds(const CCanvas_PurchaseKinds& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void   Tick(_double TimeDelta) override;
	virtual void   Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void   Imgui_RenderProperty() override;
	virtual void   LoadFromJson(const Json& json) override;

private:
	void		Add_ShopListCanvas();
	void		Clone_ShopListCanvas(const size_t iIndex);

	void		CurrentList();
	void		Key_Input();

private:
	vector<pair<wstring, class CCanvas_ShopListBar*>>	m_vecShopCanvass;

	vector<pair<wstring, CItem_Manager::ITEMINFO>> m_vecItemInfo;	// 아이템 매니저의 데이터를 멤버 변수로 보관
	CItem_Manager::MAINITEM m_eMainItem = { CItem_Manager::MAINITEM::MAINITEM_END };

	class		CCanvas_ListCheck* m_pCanvas_ListCheck = { nullptr };
	class    CCanvas_Shortage* m_pCanvas_Shortage = { nullptr };

	size_t	m_iCanvasIndex = { 0 };
	size_t	m_iItemIndex = { 0 };	
	_uint	m_iItemCount = { 0 };

	_bool	m_bLastCheck = { false };
	_bool	m_bShortage = { false };

public:
	static CCanvas_PurchaseKinds* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END