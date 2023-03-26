#pragma once
#include "Enemy_AnimInstance.h"

BEGIN(Client)

class CEM320_AnimInstance : public CEnemy_AnimInstance
{
public:
	virtual HRESULT Initialize(CModel* pModel, CGameObject* pGameObject) override;
	virtual void UpdateTargetState(_double TimeDelta) override;

private:
	// �� �Ʒ� �� ��ߵǴ°� �ƴ϶� �ʿ��Ѱ͸� �����ؼ� ����Ұ�(������ �ΰ���)
	_bool			m_bAir = false;
	_bool			m_bPreAir = false;
	_bool			m_bMove = false;

	_float3 m_vMoveAxis;
	EBaseAxis m_eMoveAxis = EBaseAxis::AXIS_END;
	EBaseAxis m_ePreMoveAxis = EBaseAxis::AXIS_END;
	EBaseTurn m_eTurn = EBaseTurn::TURN_END;
	_float m_fTurnRemain = 0.f;

public:
	static CEM320_AnimInstance* Create(CModel* pModel, CGameObject* pGameObject);
	virtual void Free() override;
};

END