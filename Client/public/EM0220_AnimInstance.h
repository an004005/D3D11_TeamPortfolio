#pragma once
#include "Client_Defines.h"
#include "Enemy_AnimInstance.h"

BEGIN(Client)

class CEM0220_AnimInstance : public CEnemy_AnimInstance
{
public:
	virtual HRESULT Initialize(CModel* pModel, CGameObject* pGameObject) override;
	virtual void UpdateTargetState(_double TimeDelta) override;


public:
	static CEM0220_AnimInstance* Create(CModel* pModel, CGameObject* pGameObject);
	virtual void Free() override;
};

END