#pragma once
#include "Client_Defines.h"
#include "AnimationInstance.h"

BEGIN(Client)

class CBaseAnimInstance : public CAnimationInstance
{
public:
	virtual HRESULT Initialize(CModel* pModel, CGameObject* pGameObject) override;
	virtual void Tick(_double TimeDelta) override;

	virtual void UpdateTargetState(_double TimeDelta) override;

	virtual void Imgui_RenderState() override;

protected:	// 대상의 상태
	_bool	m_bAir = false;
	_bool	m_bPreAir = false;

	_bool	m_bMove = false;
	_bool	m_bWalk = false;

	CAnimationStateMachine* m_pASM_Base = nullptr;

public:
	static CBaseAnimInstance* Create(CModel* pModel, CGameObject* pGameObject);
	virtual void Free() override;
};

END