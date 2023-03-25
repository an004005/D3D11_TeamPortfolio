#pragma once
#include "AIController.h"

BEGIN(Engine)
class CFSMComponent;
END

BEGIN(Client)

class CEM0220_Controller : public CAIController
{
public:
	CEM0220_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEM0220_Controller(const CEM0220_Controller& rhs);
	virtual ~CEM0220_Controller() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void AI_Tick(_double TimeDelta) override;


	void Tick_Near(_double TimeDelta);
	void Tick_Far(_double TimeDelta);
	void Tick_Outside(_double TimeDelta);

public:
	//두번째 인자에 true를 넣으면 더 큰값일때 true 반환
	_bool	Check_TargetDist(_float fDist, _bool IsGreater = false);

private:
	void DefineState(_double TimeDelta);

private:
	class CEM0220* m_pCastedOwner = nullptr;

private:
	_float m_fToTargetDistance;
	
public:
	static CEM0220_Controller* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END