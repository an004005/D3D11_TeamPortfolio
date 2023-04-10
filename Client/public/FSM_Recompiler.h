#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CPlayer;

// ��� ���� ��� ��ü�� �ش� Ŭ���� Friend���� �ʿ�
// ������ Player�� SetUp_Component�Լ� ���� �Ʒ��κ� ����
// ��� �� ��� ��ü���� Release�ʿ�

class CFSM_Recompiler : public CBase
{
public:
	CFSM_Recompiler();
	CFSM_Recompiler(const CFSM_Recompiler& rhs) = default;
	virtual ~CFSM_Recompiler() = default;

public:
	virtual HRESULT Initialize(CPlayer* pPlayer);
	void			Tick();	// ��� ��ü�� Imgui_RenderProperty �κп��� ����

private:
	void			FSM_Recompile();


// �ش� Ŭ������ ����� ��ü�� ����� ��� ����
private:
	CPlayer* m_pPlayer = nullptr;

public:
	static CFSM_Recompiler* Create(CPlayer* pPlayer);
	virtual void Free();
};

END