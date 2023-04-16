#pragma once

#include "Canvas.h"
#include "Client_Defines.h"
#include "Item_Manager.h"

BEGIN(Client)

class CCanvas_ItemMove : public CCanvas
{
protected:
	CCanvas_ItemMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_ItemMove(const CCanvas_ItemMove& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	BeginTick() override;
	virtual void	Tick(_double TimeDelta) override;

public:
	void	Set_Input() {
		m_bInput = true;
	}

private:
	void	Key_Input();

private:
	_bool	m_bInput = { false };
	_bool	m_bInputSound = { false };

public:
	static CCanvas_ItemMove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END