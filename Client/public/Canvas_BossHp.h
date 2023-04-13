#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_BossHp : public CCanvas
{
protected:
	CCanvas_BossHp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_BossHp(const CCanvas_BossHp& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;

public:
	void	Set_BossHp();
	void	Set_Name(const _float fName);

public:
	static CCanvas_BossHp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END