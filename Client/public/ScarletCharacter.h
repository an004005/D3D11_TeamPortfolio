#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "GameInstance.h"

BEGIN(Engine)
class CControlledRigidBody;
END

BEGIN(Client)

class CScarletCharacter abstract : public CGameObject
{
protected:
	CScarletCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CScarletCharacter(const CScarletCharacter& rhs);
	virtual ~CScarletCharacter() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual void AfterPhysX() override;

public:
	virtual void TakeDamage(DAMAGE_PARAM tDamageParams){}

public:
	EDeBuffType GetDeBuffType() const { return m_eDeBuff; }

protected:
	EDeBuffType m_eDeBuff = EDeBuffType::DEBUFF_END;
	CControlledRigidBody* m_pCollider = nullptr;
	CGameInstance* m_pGameInstance = nullptr;
	_float4 m_vPrePos;
	_bool m_bOnFloor = false;
	_bool m_bOnSide = false;

	_bool m_bActiveGravity = false;
	_float m_fYSpeed = 0.f;
	_float m_fGravity = 20.f;


public:
	virtual void Free() override;
};

END