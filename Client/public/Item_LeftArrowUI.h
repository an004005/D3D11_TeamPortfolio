#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CItem_LeftArrowUI final : public CUI
{
private:
	CItem_LeftArrowUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem_LeftArrowUI(const CItem_LeftArrowUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

private:
	_bool	m_bInput = { false };
	_float	m_fStartX = { 0.0f };
	_double m_fMoveTimeAcc = { 0.0f };

public:
	static CItem_LeftArrowUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CUI* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END