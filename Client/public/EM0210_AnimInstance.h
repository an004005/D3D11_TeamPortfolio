#pragma once
#include "Client_Defines.h"
#include "Enemy_AnimInstance.h"

BEGIN(Client)

class CEM0210_AnimInstance : public CEnemy_AnimInstance
{
public:
	virtual HRESULT Initialize(CModel* pModel, CGameObject* pGameObject) override;
	virtual void UpdateTargetState(_double TimeDelta) override;

private:
	// 이 아래 다 써야되는게 아니라 필요한것만 선언해서 사용할것(예제로 두겠음)
	_bool			m_bMove = false;
	_bool			m_bRun = false;
	_bool			m_bAir = false;

	_float3 m_vMoveAxis;
	EBaseAxis m_eMoveAxis = EBaseAxis::AXIS_END;
	EBaseAxis m_ePreMoveAxis = EBaseAxis::AXIS_END;
	EBaseTurn m_eTurn = EBaseTurn::TURN_END;
	_float m_fTurnRemain = 0.f;

private:
	_float	m_fLerpTime = 0.f;
	_float	m_fLerpDuration = 0.2f;
	_bool	m_bLerp = false;
	_bool	m_bAttach = false;

public:
	static CEM0210_AnimInstance* Create(CModel* pModel, CGameObject* pGameObject);
	virtual void Free() override;
};

END