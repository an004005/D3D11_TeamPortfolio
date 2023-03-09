#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CGameInstance;
class CShader;
class CAnimation;
class CFSMComponent;
class CModel;
class CRenderer;
class CRigidBody;
END

BEGIN(Client)

class CSkummyPool : public CMonster
{
//public:
//	enum HITAXISTYPE { HAS_FL, HAS_BL, HAS_LL, HAS_RL, HAS_FM, HAS_BM, HAS_LM, HAS_RM, HAS_END };

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

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CModel*						m_pModelCom = nullptr;

	class CSkmP_Controller*		m_pController = nullptr;
	class CSkmP_AnimInstance*	m_pASM = nullptr;
	
	CRigidBody*					m_pTrigger = nullptr;

private:
	CAnimation*			m_pAtk_Shoot = nullptr; // 
	CAnimation*			m_pThreat = nullptr; // µµ¹ß

	CAnimation*			m_pDamage_L_F = nullptr;
	CAnimation*			m_pDamage_L_B = nullptr;

	CAnimation*			m_pDamage_M_F = nullptr;
	CAnimation*			m_pDamage_M_B = nullptr;
	CAnimation*			m_pDamage_M_L = nullptr;
	CAnimation*			m_pDamage_M_R = nullptr;

	CAnimation*			m_pDeadAnim = nullptr;

	// Damage
	_bool		m_bStruck = false;

	EBaseAxis	m_eHitDir = EBaseAxis::AXIS_END;
	EAttackType	m_eAtkType = EAttackType::ATK_END;
	
//	HITAXISTYPE	m_Haxistype = HAS_END;

private:
	_bool			m_bCreateBullet = false;

private:
	CScarletCharacter* m_pTarget = nullptr;

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