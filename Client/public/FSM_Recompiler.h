#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CPlayer;

// 사용 전에 대상 객체에 해당 클래스 Friend선언 필요
// 생성은 Player의 SetUp_Component함수 가장 아래부분 참조
// 사용 후 대상 객체에서 Release필요

class CFSM_Recompiler : public CBase
{
public:
	CFSM_Recompiler();
	CFSM_Recompiler(const CFSM_Recompiler& rhs) = default;
	virtual ~CFSM_Recompiler() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer);
	void			Tick();	// 대상 객체의 Imgui_RenderProperty 부분에서 동작

private:
	void			FSM_Recompile();


// 해당 클래스를 사용할 객체를 멤버로 들고 생성
private:
	CPlayer* m_pPlayer = nullptr;

public:
	static CFSM_Recompiler* Create(CPlayer* pPlayer);
	virtual void Free();
};

END