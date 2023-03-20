#pragma once
#include "AIController.h"

BEGIN(Engine)
class CFSMComponent;
END

BEGIN(Client)

class CEM0200_Controller : public CAIController
{
public:
	CEM0200_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEM0200_Controller(const CEM0200_Controller& rhs);
	virtual ~CEM0200_Controller() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void AI_Tick(_double TimeDelta) override;

	_bool IsRun() const { return m_bRun; }

	void Tick_Near(_double TimeDelta);
	void Tick_Mid(_double TimeDelta);
	void Tick_Far(_double TimeDelta);
	void Tick_Outside(_double TimeDelta);

	void Run(EMoveAxis eAxis);
	void Run_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio = 1.f);

private:
	CFSMComponent* m_pFSM = nullptr;
	_float m_fToTargetDistance;
	class CEM0200* m_pCastedOwner = nullptr;
	
	_uint m_iNearOrder = 0;
	_uint m_iMidOrder = 0;
	_uint m_iFarOrder = 0;
	_uint m_iOutOrder = 0;

	_float m_fTurnSlowTime;
	_float m_fTurnSlowRatio;

	_bool m_bRun = false;

public:
	virtual void Free() override;
	virtual CComponent* Clone(void* pArg) override;
	static CEM0200_Controller* Create();
};

END