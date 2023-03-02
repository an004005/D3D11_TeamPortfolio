#pragma once
#include "AnimationInstance.h"
#include "Client_Defines.h"

BEGIN(Client)

class CBoss1_AnimationInstance : public CAnimationInstance
{
public:
	virtual HRESULT Initialize(CModel* pModel, CGameObject* pGameObject) override;
	virtual void Tick(_double TimeDelta) override;

	virtual void UpdateTargetState(_double TimeDelta) override;

	virtual void Imgui_RenderState() override;

private:
	CAnimationStateMachine* m_pASM = nullptr;

	_bool			m_bAir = false;
	_bool			m_bPreAir = false;
	_bool			m_bMove = false;

	_float3 m_vMoveAxis;
	EBaseAxis m_eMoveAxis = EBaseAxis::AXIS_END;
	EBaseAxis m_ePreMoveAxis = EBaseAxis::AXIS_END;
	EBaseTurn m_eTurn = EBaseTurn::TURN_END;
	_float m_fTurnRemain = 0.f;



public:
	static CBoss1_AnimationInstance* Create(CModel* pModel, CGameObject* pGameObject);
	virtual void Free() override;
};

END