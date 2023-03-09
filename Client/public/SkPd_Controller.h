#pragma once
#include "AIController.h"

BEGIN(Engine)
class CFSMComponent;
END

BEGIN(Client)

class CSkPd_Controller : public CAIController
{
public:
	CSkPd_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSkPd_Controller(const CSkPd_Controller& rhs);
	virtual ~CSkPd_Controller() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void AI_Tick(_double TimeDelta) override;
	
	void Tick_Near(_double TimeDelta);
	void Tick_Mid(_double TimeDelta);
	void Tick_Far(_double TimeDelta);
	void Tick_Outside(_double TimeDelta);

	void Rush(EMoveAxis eAxis);

	_bool IsRush() const { return m_bRush; }

private:
	CFSMComponent* m_pFSM = nullptr;
	_float m_fToTargetDistance;
	class CSkummyPandou* m_pCastedOwner = nullptr;
	
	_uint m_iNearOrder = 0;
	_uint m_iMidOrder = 0;
	_uint m_iFarOrder = 0;
	_uint m_iOutOrder = 0;

	_float m_fTurnSlowTime;
	_float m_fTurnSlowRatio;

	_bool m_bRush = false;

public:
	virtual void Free() override;
	virtual CComponent* Clone(void* pArg) override;
	static CSkPd_Controller* Create();
};

END