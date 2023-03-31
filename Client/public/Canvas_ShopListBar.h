#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_ShopListBar : public CCanvas
{
protected:
	CCanvas_ShopListBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_ShopListBar(const CCanvas_ShopListBar& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

public:
	_bool	Get_OnButton();
	void	Set_OnButton();
	void	Set_OnAlpha();

	void	Set_ShopListBar(const _float2 & fItemPos, const _float2 & fIconPos, const size_t iIndex);
	void	Set_NmberPurchases(const _uint iNmberPurchases) {
		m_iNmberPurchases = iNmberPurchases;
	}

private:
	size_t		m_iItemInfoIndex = { 0 };
	_uint		m_iNmberPurchases = { 0 };
	_tchar*	m_szAmount = { L"" };


public:
	static CCanvas_ShopListBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END