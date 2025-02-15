#pragma once
#include "AIController.h"

BEGIN(Client)

class CEM0210_Controller : public CAIController
{
public:
	CEM0210_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEM0210_Controller(const CEM0210_Controller& rhs);
	virtual ~CEM0210_Controller() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void AI_Tick(_double TimeDelta) override;


	void Tick_Near(_double TimeDelta);
	void Tick_Mid(_double TimeDelta);
	void Tick_Far(_double TimeDelta);
	void Tick_Outside(_double TimeDelta);

	void Run_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio = 1.f);
	_bool IsRun() const { return m_bRun; }

private:
	void DefineState(_double TimeDelta);

private:
	class CEM0210* m_pCastedOwner = nullptr;

private:
	_uint m_iNearOrder = 0;
	_uint m_iMidOrder = 0;
	_uint m_iFarOrder = 0;

	_bool m_bRun = false;
public:
	static CEM0210_Controller* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END