#pragma once
#include "AIController.h"

BEGIN(Client)

class CBoss1_AIController : public CAIController
{
protected:
	CBoss1_AIController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBoss1_AIController(const CBoss1_AIController& rhs);
	virtual ~CBoss1_AIController() override = default;

public:
	virtual void Tick(_double TimeDelta) override;


public:
	virtual CComponent* Clone(void* pArg) override;
	static CBoss1_AIController* Create();
};

END