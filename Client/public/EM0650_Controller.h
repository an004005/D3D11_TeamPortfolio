#pragma once
#include "AIController.h"

BEGIN(Engine)
class CFSMComponent;
END

BEGIN(Client)

class CEM0650_Controller : public CAIController
{
public:
	CEM0650_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEM0650_Controller(const CEM0650_Controller& rhs);
	virtual ~CEM0650_Controller() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void AI_Tick(_double TimeDelta) override;


	void Tick_Near(_double TimeDelta);
	void Tick_Far(_double TimeDelta);
	void Tick_Outside(_double TimeDelta);

private:
	void DefineState(_double TimeDelta);

private:
	class CEM0650* m_pCastedOwner = nullptr;

private:
	_float m_fToTargetDistance;
	
	_uint m_iNearOrder = 0;
	_uint m_iFarOrder = 0;

	_float m_fTurnSlowTime;
	_float m_fTurnSlowRatio;

public:
	static CEM0650_Controller* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END