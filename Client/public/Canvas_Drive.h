#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_Drive : public CCanvas
{
protected:
	CCanvas_Drive(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_Drive(const CCanvas_Drive& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Set_DriveB(const _bool DriveB);

public:
	static CCanvas_Drive* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END