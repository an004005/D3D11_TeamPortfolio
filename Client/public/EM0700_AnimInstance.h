#pragma once
#include "Client_Defines.h"
#include "Enemy_AnimInstance.h"

BEGIN(Client)

class CEM0700_AnimInstance : public CEnemy_AnimInstance
{
public:
	virtual HRESULT Initialize(CModel* pModel, CGameObject* pGameObject) override;
	virtual void UpdateTargetState(_double TimeDelta) override;

private:
	_bool			m_bMove = false;

	_float3 m_vMoveAxis;
	EBaseAxis m_eMoveAxis = EBaseAxis::AXIS_END;

public:
	static CEM0700_AnimInstance* Create(CModel* pModel, CGameObject* pGameObject);
	virtual void Free() override;
};

END