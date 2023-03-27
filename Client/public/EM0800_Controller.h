#pragma once
#include "AIController.h"

BEGIN(Engine)
class CFSMComponent;
END

BEGIN(Client)

class CEM0800_Controller : public CAIController
{
public:
	CEM0800_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEM0800_Controller(const CEM0800_Controller& rhs);
	virtual ~CEM0800_Controller() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void AI_Tick(_double TimeDelta) override;


	void Tick_Near(_double TimeDelta);
	void Tick_Mid(_double TimeDelta);
	void Tick_Far(_double TimeDelta);

public:
	_bool	IsTurn() { return m_bTurn; }
	EBaseTurn GetBaseTurn() { return m_eTurn; }

private:
	void DefineState(_double TimeDelta);
	void DefineTurnCommand();

private:
	class CEM0800* m_pCastedOwner = nullptr;

private:
	EBaseTurn m_eTurn = EBaseTurn::TURN_END;
	_bool	m_bTurn = false;

	_uint m_iNearOrder = 0;
	_uint m_iMidOrder = 0;

public:
	static CEM0800_Controller* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END