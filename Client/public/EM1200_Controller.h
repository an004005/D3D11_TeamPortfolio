#pragma once
#include "AIController.h"

BEGIN(Engine)
class CFSMComponent;
END

BEGIN(Client)

enum COOLTIME { CURTIME, MAXTIME, COOL_END};
class CEM1200_Controller : public CAIController
{
public:
	CEM1200_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEM1200_Controller(const CEM1200_Controller& rhs);
	virtual ~CEM1200_Controller() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void AI_Tick(_double TimeDelta) override;


	void Tick_Near(_double TimeDelta);
	void Tick_Mid(_double TimeDelta);
	void Tick_Far(_double TimeDelta);
	void Tick_Outside(_double TimeDelta);

	void Run_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio = 1.f);

public:
	_bool IsRun() { return m_bRun; }

private:
	void DefineState(_double TimeDelta);

private:
	class CEM1200* m_pCastedOwner = nullptr;

private:
	_float m_fToTargetDistance;
	
	_bool	m_bRun = false;

	_uint m_iMidOrder = 0;
	_uint m_iFarOrder = 0;

	_double m_dRushCoolTime[COOL_END] = { 0.0, 40.0 };


public:
	static CEM1200_Controller* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END