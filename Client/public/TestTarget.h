#pragma once
#include "ScarletCharacter.h"
#include "Client_Defines.h"


BEGIN(Client)

class CTestTarget : public CScarletCharacter
{
protected:
	CTestTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestTarget(const CScarletCharacter& rhs);
	virtual ~CTestTarget() = default;

public:
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void AfterPhysX() override;

public:
	static CTestTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END