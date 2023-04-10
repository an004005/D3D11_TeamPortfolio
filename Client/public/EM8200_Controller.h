#pragma once
#include "Client_Defines.h"
#include "AIController.h"
#include "TimerHelper.h"

BEGIN(Engine)
class CFSMComponent;
END

BEGIN(Client)
class CEM8200_Controller : public CAIController
{
public:
	CEM8200_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEM8200_Controller(const CEM8200_Controller& rhs);
	virtual ~CEM8200_Controller() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void AI_Tick(_double TimeDelta) override;

	_bool IsRun() const { return m_bRun; }

	void Tick_Near(_double TimeDelta);
	void Tick_Mid(_double TimeDelta);
	void Tick_Far(_double TimeDelta);
	void Tick_Outside(_double TimeDelta);

	void Run_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio = 1.f);
	void Teleport_TurnToTarget(EMoveAxis eAxis, _float fSpeedRatio = 1.f);

	void LookAtNow();

private:
	void Detected_Attack();
	void Tick_TP_Cooltime(_double TimeDelta);
	void Tick_CoolTimeHelper(_double TimeDelta);

private:
	void Initialize_CoolTimeHelper();

private:
	CFSMComponent* m_pFSM = nullptr;
	class CEM8200* m_pCastedOwner = nullptr;

	_uint m_iNearOrder = 0;
	_uint m_iMidOrder = 0;
	_uint m_iFarOrder = 0;
	_uint m_iOutOrder = 0;

	_float m_fTurnSlowTime;
	_float m_fTurnSlowRatio;

	_bool m_bRun = false;


private:
	// CoolTime Manage

	// Ice Needle Atk
	CCoolTimeHelper m_IceNeedle_CoolTimeHelper;

	CCoolTimeHelper m_KickAtk_CoolTimeHelper;
	
	CCoolTimeHelper m_RushAtk_CoolTimeHelper;
	
	CCoolTimeHelper m_ChaseElec_CoolTimeHelper;

	CCoolTimeHelper m_AirElec_CoolTimeHelper;

	CCoolTimeHelper m_Seethrough_CoolTimeHelper;

	CCoolTimeHelper m_Counter_CoolTimeHelper;

	CCoolTimeHelper m_CaptureKinetic_CoolTimeHelper;

private:
	_float			m_fTP_CurCoolTime = 0.f;
	_float			m_fTP_CoolTime = 0.f;
	_float2			m_fTP_RandCoolTime = { 0.f ,0.5f };
	_bool			m_bCanUseTeleport = false;
public:
	virtual void Free() override;
	virtual CComponent* Clone(void* pArg) override;
	static CEM8200_Controller* Create();
};




END
