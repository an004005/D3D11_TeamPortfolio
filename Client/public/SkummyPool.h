#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CFSMComponent;
class CGameInstance;
class CAnimation;
class CRigidBody;
END

BEGIN(Client)

class CSkummyPool : public CMonster
{
private:
	CSkummyPool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkummyPool(const CSkummyPool& rhs);
	virtual ~CSkummyPool() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;
	virtual void TakeDamage(DAMAGE_PARAM tDamageParams) override;
	virtual void AfterPhysX();

	void HitDir(_double TimeDelta);
//	virtual void SetActive() override;

private:
	class CSkmP_Controller*		m_pController = nullptr;
	class CSkmP_AnimInstance*	m_pASM = nullptr;
	
	CRigidBody*					m_pTrigger = nullptr;

private:
	CAnimation*			m_pAtk_Shoot = nullptr; // 
//	CAnimation*			m_pThreat = nullptr; // µµ¹ß

// Damage
	CAnimation*			m_pDamage_L_F = nullptr;
	CAnimation*			m_pDamage_L_B = nullptr;

	CAnimation*			m_pDamage_M_F = nullptr;
	CAnimation*			m_pDamage_M_B = nullptr;
	CAnimation*			m_pDamage_M_L = nullptr;
	CAnimation*			m_pDamage_M_R = nullptr;

// Air Damage
	CAnimation*			m_pBlowStart = nullptr;
	CAnimation*			m_pBlowLand = nullptr;
	CAnimation*			m_pGetUp = nullptr;
	CAnimation*			m_pRiseStart = nullptr;

	CAnimation*			m_pDeadAnim = nullptr;

	// Damage
	_bool		m_bStruck = false;
	_bool		m_bAirStruck = false;

	_bool		m_bAirMaintain = false;

	// HitDir
	_bool		m_bHitMove = false;

	_vector		m_vPreDir;
	_vector		m_vCurDir;
	_vector		m_vFinDir;
	_bool		m_bOneTick = false;

	// TakeDamage
	EBaseAxis	m_eHitDir = EBaseAxis::AXIS_END;
	EAttackType	m_eAtkType = EAttackType::ATK_END;

private:
	CScarletCharacter* m_pTarget = nullptr;

class	CSkMpBullet*		m_pBullet = nullptr;

	_float3 m_vMoveAxis;
	_float m_fTurnRemain = 0.f;

public:
	_bool IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3 GetMoveAxis() const { return m_vMoveAxis; }
	_float GetTurnRemain() const { return m_fTurnRemain; }
	_bool IsPlayingSocket() const;

public:
	static CSkummyPool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END