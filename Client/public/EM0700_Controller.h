#pragma once
#include "AIController.h"

BEGIN(Engine)
class CFSMComponent;
END

BEGIN(Client)

class CEM0700_Controller : public CAIController
{
public:
	CEM0700_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEM0700_Controller(const CEM0700_Controller& rhs);
	virtual ~CEM0700_Controller() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void AI_Tick(_double TimeDelta) override;


	void Tick_Near(_double TimeDelta);
	void Tick_Mid(_double TimeDelta);
	void Tick_Far(_double TimeDelta);
	void Tick_Outside(_double TimeDelta);

private:
	void DefineState(_double TimeDelta);

private:
	class CEM0700* m_pCastedOwner = nullptr;

private:
	
	_uint m_iNearOrder = 0;
	_uint m_iMidOrder = 0;
	_uint m_iOutOrder = 0;

public:
	static CEM0700_Controller* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END