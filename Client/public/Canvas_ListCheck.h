#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_ListCheck : public CCanvas
{
protected:
	CCanvas_ListCheck(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_ListCheck(const CCanvas_ListCheck& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void   Tick(_double TimeDelta) override;
	virtual void   Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool	Get_Visible() {
		return m_bVisible;
	}

	void	Set_ItemInfo(const size_t iItemIndex, const _uint iItemCount);

private:
	void	CheckIcon_Tick();
	void	KeyInput_Yes();
	void	KeyInput_No();
	void	Purchase();
	void	TransferringBar_Tick(const _double & TimeDelta);

private:
	size_t	m_iItemIndex = { 0 };
	_uint	m_iItemCount = { 0 };

	_int		m_iYesCount = { 0 };
	_int		m_iNoCount = { 0 };

	_bool	m_bTransferring = { false };
	_float	m_fTransferringBar = { 0.0f };

	_bool	m_bSound = { false };

public:
	static CCanvas_ListCheck* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END