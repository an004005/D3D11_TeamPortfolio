#pragma once

#include "Canvas.h"
#include "Client_Defines.h"
#include "Item_Manager.h"

BEGIN(Client)

class CCanvas_MainItemBattle : public CCanvas
{
public:
	typedef struct tagBattleInfo
	{
		array<_bool, 17>	arrIcon;
		CCanvas*			pItemCanvas = { nullptr };

	}BATTLEINFO;

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
	void	Add_ItemCanvas(const size_t & iIndex);
	void	Item_Tick();

private:
	vector<pair<wstring, CCanvas*>>	m_vecItemCanvass;

	_int				m_iItemIndex = { 0 };
	vector<pair<wstring, CItem_Manager::ITEMINFO>> m_vecItemInfo;

public:
	static CCanvas_MainItemBattle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END