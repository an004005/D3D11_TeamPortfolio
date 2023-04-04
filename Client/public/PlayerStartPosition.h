#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CPlayerStartPosition : public CGameObject
{
private:
	CPlayerStartPosition(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerStartPosition(const CPlayerStartPosition& rhs);
	virtual ~CPlayerStartPosition() = default;

public:
	static CPlayerStartPosition* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};


class CHanabiStartPosition : public CGameObject
{
private:
	CHanabiStartPosition(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHanabiStartPosition(const CHanabiStartPosition& rhs);
	virtual ~CHanabiStartPosition() = default;

public:
	static CHanabiStartPosition* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

class CTsugumiStartPosition : public CGameObject
{
private:
	CTsugumiStartPosition(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTsugumiStartPosition(const CTsugumiStartPosition& rhs);
	virtual ~CTsugumiStartPosition() = default;

public:
	static CTsugumiStartPosition* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END