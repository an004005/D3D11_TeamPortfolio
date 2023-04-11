#pragma once
#include "AIController.h"

BEGIN(Engine)
class CFSMComponent;
END

BEGIN(Client)

enum COOLTIME { CURTIME, MAXTIME, COOL_END};
class CEM1100_Controller : public CAIController
{
public:
	CEM1100_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEM1100_Controller(const CEM1100_Controller& rhs);
	virtual ~CEM1100_Controller() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void AI_Tick(_double TimeDelta) override;


	void Tick_Near(_double TimeDelta);
	void Tick_Far(_double TimeDelta);
	void Tick_Outside(_double TimeDelta);

	void Run_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio = 1.f);

public:
	_bool IsRun() { return m_bRun; }

private:
	void DefineState(_double TimeDelta);

private:
	class CEM1100* m_pCastedOwner = nullptr;

private:	
	_bool	m_bRun = false;
	_bool	m_bAttack = false;
	_uint m_iNearOrder = 0;
	_uint m_iFarOrder = 0;

public:
	static CEM1100_Controller* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END