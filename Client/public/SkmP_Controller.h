#pragma once
#include "AIController.h"

BEGIN(Engine)
class CFSMComponent;
END

BEGIN(Client)

class CSkmP_Controller : public CAIController
{
public:
	CSkmP_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit  CSkmP_Controller(const CSkmP_Controller& rhs);
	virtual ~CSkmP_Controller() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void AI_Tick(_double TimeDelta) override;

	void Tick_Near(_double TimeDelta);
	void Tick_Far(_double TimeDelta);

private:
	CFSMComponent* m_pFSM = nullptr;
	_float m_fToTargetDistance;
	class CSkummyPool* m_pCastedOwner = nullptr;


	_uint m_iNearOrder = 0;
	_uint m_iFarOrder = 0;

	_float m_fTurnSlowTime;
	_float m_fTurnSlowRatio;

public:
	virtual void Free() override;
	virtual CComponent* Clone(void* pArg) override;
	static CSkmP_Controller* Create();
};

END