#pragma once
#include "AIController.h"

BEGIN(Engine)
class CFSMComponent;
END

BEGIN(Client)

class CBrJ_Controller : public CAIController
{
public:
	CBrJ_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBrJ_Controller(const CBrJ_Controller& rhs);
	virtual ~CBrJ_Controller() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void AI_Tick(_double TimeDelta) override;

	void Tick_Near(_double TimeDelta);
	void Tick_Mid(_double TimeDelta);
	void Tick_Far(_double TimeDelta);

private:
	CFSMComponent* m_pFSM = nullptr;
	_float m_fToTargetDistance;
	class CBronJon* m_pCastedOwner = nullptr;

	_uint m_iNearOrder = 0;
	_uint m_iMidOrder = 0;
	_uint m_iFarOrder = 0;
	_uint m_iOutOrder = 0;

	_float m_fTurnSlowTime;
	_float m_fTurnSlowRatio;

public:
	virtual void Free() override;
	virtual CComponent* Clone(void* pArg) override;
	static CBrJ_Controller* Create();
};

END