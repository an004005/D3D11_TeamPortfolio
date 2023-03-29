#pragma once

#include "Canvas.h"
#include "Client_Defines.h"
#include "Item_Manager.h"

BEGIN(Client)

class CCanvas_Item : public CCanvas
{
protected:
	CCanvas_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_Item(const CCanvas_Item& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

public:
	void	Set_ItmeUse();

private:
	void	BattleItem_Tick();
	void	CurrentItem();

private:
	vector<CItem_Manager::BATTLEITEM>	m_vecBattleItme;
	size_t	m_iCurrentIndex = { 0 };

public:
	static CCanvas_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END