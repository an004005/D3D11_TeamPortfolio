#pragma once
#include "Client_Defines.h"
#include "ImguiObject.h"
#include "ScarletCharacter.h"
#include "HelperClasses.h"

BEGIN(Client)
class CAI_CH0300_AnimInstance;

class CAI_CH0300 : public CScarletCharacter
{
public:
	typedef struct tagDamageDesc
	{
		_int		m_iDamage;
		_vector		m_vHitDir;
		EAttackType	m_iDamageType;
		EBaseAxis	m_eHitDir;
	}DAMAGE_DESC;

private:
	CAI_CH0300(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAI_CH0300(const CAI_CH0300& rhs);
	virtual ~CAI_CH0300() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void BeginTick();
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual void AfterPhysX() override;
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth() override;

public:
	virtual void TakeDamage(DAMAGE_PARAM tDamageParams);
	virtual void Imgui_RenderProperty() override;

public:
	virtual _float4x4 GetBoneMatrix(const string& strBoneName, _bool bPivotapply = true) override;
	virtual _float4x4 GetPivotMatrix() override;

private:
	HRESULT SetUp_Components(void* pArg);
	HRESULT SetUp_Event();
	HRESULT SetUp_EffectEvent();
	HRESULT SetUp_Sound();
	HRESULT SetUp_AttackDesc();

	unordered_map<string, std::function<void()>>	m_mapCollisionEvent;

private:
	_double	m_fTimeDelta = 0.f;

private:
	CRenderer*	m_pRenderer = nullptr;
	CModel*		m_pModel = nullptr;
	CAI_CH0300_AnimInstance* m_pASM = nullptr;

private:
	CGameObject* m_pPlayer = nullptr;
	_float4		m_vPlayerPos = { 0.f, 0.f, 0.f, 1.f };

private:
	wstring		m_ModelName;

private:
	void		BehaviorCheck();
	void		DistanceCheck();	// 플레이어와의 거리 탐지
	void		MovePerSecondCheck();
	void		TeleportCheck();

private:
	void		TeleportToPlayerBack();

private:
	void		Collision_Start();
	void		Collision_End();
	void		Collision_Twist();
	void		Twist_Particle();

public:
	void		WeaponDissolve(_bool bDissolve) { m_bWeaponOn = bDissolve; }
	
private:
	HRESULT		Setup_Parts();
	vector<CGameObject*>	m_vecWeapon;
	_bool		m_bWeaponOn = false;
	_float		m_fWeaponDissolve = 1.f;

// 구조체 상태 반환
private:
	DAMAGE_DESC		m_DamageDesc;
	DAMAGE_PARAM	m_AttackDesc;

public:
	DAMAGE_DESC Get_DamageDesc() const { return m_DamageDesc; }
	void		Reset_DamageDesc() { ZeroMemory(&m_DamageDesc, sizeof(DAMAGE_DESC)); }

// 실수값 상태 반환
private:
	_float		m_fDistance_toPlayer = 0.f;
	_float		m_fDistance_toEnemy = -1.f;
	_float		m_fMovePerSecond = 1.f;

public:
	_float		Get_Distance_Player() const { return m_fDistance_toPlayer; }
	_float		Get_Distance_Enemy() const { return m_fDistance_toEnemy; }
	void		Set_Jump(_float fSpeed) { m_fYSpeed = fSpeed; }

// bool값 상태 반환
private:
	_bool		m_bHit = false;
	_bool		m_bJump = true;

public:
	_bool		Get_Hit() const { return m_bHit; }
	void		Reset_Hit() { m_bHit = false; }
	_bool		Get_OnFloor() const { return m_bOnFloor; }
	_bool		Get_Jump() const { return m_bJump; }
	void		Reset_Jump() { m_bJump = false; }

// 장애물 탐지
private:
	_float		m_fObstacleDetectTimer = 0.f;
	_float4		m_vBefPos = { 0.f, 0.f, 0.f, 1.f };

// 피타격 관련
private:
	_bool		m_bAttackEnable = false;

public:
	static CAI_CH0300* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END