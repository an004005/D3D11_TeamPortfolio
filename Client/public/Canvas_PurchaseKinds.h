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

private:
	vector<pair<wstring, class CCanvas_ShopListBar*>>	m_vecShopCanvass;

	vector<pair<wstring, CItem_Manager::ITEMINFO>> m_vecItemInfo;
	CItem_Manager::MAINITEM m_eMainItem = { CItem_Manager::MAINITEM::MAINITEM_END };

public:
	static CCanvas_PurchaseKinds* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END