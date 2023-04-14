#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_MapBar : public CCanvas
{
protected:
	CCanvas_MapBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_MapBar(const CCanvas_MapBar& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Set_Name(const wstring wsName) {
		m_wsName = wsName;
	}

	void	Set_PosY(const _float& fPosY);

	_bool	Get_OnButton();
	void		Set_OnButton();
	void		Set_OnAlpha();

private:
	wstring	m_wsName = { L"" };

public:
	static CCanvas_MapBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END