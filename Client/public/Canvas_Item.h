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
	virtual void	BeginTick() override;
	virtual void	Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;

public:
	void	Set_Input() {
		m_bInput = true;
	}

	void	Set_ItmeUse();
	void	Set_IconLight(const _bool	bVisble);
	void	Set_LightReset();

	_float4	 ColorIndex(const _bool	 bBack);

private:
	void	BattleItem_Choice();
	_float	NameIndex(const size_t iIndex);
	_float3	 LightColorIndex(const size_t iIndex);

	_int wrap(_int x, _int low, _int high);

	void	PlayerFontHp(_uint iHp);
	void	PlayerHpSet(const _uint iRandomMinHp, const _uint iRandomMaxHp);
	void	HanabiHpSet(const _uint iRandomMinHp, const _uint iRandomMaxHp);
	void	TsugumiSet(const _uint iRandomMinHp, const _uint iRandomMaxHp);

private:
	vector<CItem_Manager::BATTLEITEM> m_vecBattleItme;

	size_t	m_iCurrentIndex = { 0 };
	_bool	m_bInput = { false };
	_bool	m_bBattleItmeEmpty = { false };

public:
	static CCanvas_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END