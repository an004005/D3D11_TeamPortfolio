#pragma once
#include "Client_Defines.h"
#include "ImguiObject.h"
#include "ScarletCharacter.h"

BEGIN(Engine)
class CGameInstance;
class CShader;
class CAnimationInstance;
class CAnimation;
class CFSMComponent;
class CModel;
class CRenderer;
class CCamera;
END

BEGIN(Client);
class CBaseAnimInstance;
class CController;
class CEffectGroup;

typedef struct tagRemote
{
	// �ൿ ����
	_bool CanTurn = false;	// ī�޶� ���� �������� ȸ��
	_bool CanMove = false;	// ���� ���� ���� ����
	_bool CanRun = false;	// �޸��� ����
	_bool AttackTurn = false;	// ���� �� ȸ��(ī�޶� ����) ����

								// ���� ����
	_bool OnAir = false;	// ���� ���� ����
	_bool Gravity = false;	// �߷� ���� ����

							// ���� ����
	_bool MoveLimitReset = false;	// Ƚ�� ���� �̵� �ʱ�ȭ
	_bool AttackLimitReset = false;	// Ƚ�� ���� ���� �ʱ�ȭ
	_bool ChargeReset = false;	// ���� ���� �ʱ�ȭ

	_bool LocalRevise = false;

	tagRemote(	_bool CanTurn, _bool CanMove, _bool CanRun, _bool AttackTurn,
				_bool OnAir, _bool Gravity, 
				_bool MoveLimitReset, _bool AttackLimitReset, _bool ChargeReset,
				_bool LocalRevise)
	:CanTurn(CanTurn), CanMove(CanMove), CanRun(CanRun), AttackTurn(AttackTurn), 
		OnAir(OnAir), Gravity(Gravity), 
		MoveLimitReset(MoveLimitReset), AttackLimitReset(AttackLimitReset), ChargeReset(ChargeReset),
		LocalRevise(LocalRevise){}

}REMOTE;

class CPlayer : public CScarletCharacter
{
private:
	// ���� Ƚ�� ������ �ִ� State�鿡 ���� ����, Reset�Լ� ���� �� �ش� ����ü ���� ��� ���� �ʱ�ȭ�ȴ�.
	typedef struct tagAttackLimit
	{
		// ���� ��
		_uint m_iNonChargeAttack_Floor = 2;
		_uint m_iNonChargeAttack_Air = 2;
		_uint m_iAttack_Air01 = 1;
		_uint m_iAttack_Air02 = 1;
		_uint m_iAttack_AirDodge = 1;

		// ����ġ
		_uint MAX_iNonChargeAttack_Floor = 2;
		_uint MAX_iNonChargeAttack_Air = 2;
		_uint MAX_iAttack_Air01 = 1;
		_uint MAX_iAttack_Air02 = 1;
		_uint MAX_iAttack_AirDodge = 1;

	}ATTACKLIMIT;

	typedef struct tagMoveLimit
	{	
		// ���� ��
		_uint m_iDoubleJump = 1;
		_uint m_iAirDodge = 1;

		// ����ġ
		_uint MAX_iDoubleJump = 1;
		_uint MAX_iAirDodge = 1;

	}MOVELIMIT;

	typedef struct tagPlayerStatus
	{
		_uint m_iHP = 100;
		_uint m_iKineticEnergy = 100;
	}PLAYER_STAT;

	typedef struct tagDamageDesc
	{
		_int		m_iDamage;
		_vector		m_vHitDir;
		EAttackType	m_iDamageType;
		EBaseAxis	m_eHitDir;
	}DAMAGE_DESC;

public:
	enum EATTACK_LIMIT { LIMIT_NONCHARGE_FLOOR, LIMIT_NONCHARGE_AIR, LIMIT_AIRATK01, LIMIT_AIRATK02, LIMIT_AIRDODGEATK, };
	enum EMOVE_LIMIT { LIMIT_DOUBLEJUMP, LIMIT_AIRDODGE, };
	enum EMoveDir { DIR_F, DIR_B, DIR_L, DIR_R, DIR_FL, DIR_FR, DIR_BL, DIR_BR, DIR_END, };

protected:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void BeginTick();
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual void AfterPhysX() override;
	virtual HRESULT Render();

	virtual void TakeDamage(DAMAGE_PARAM tDamageParams);

	virtual void Imgui_RenderProperty() override;

protected:
	PLAYER_STAT m_PlayerStat;
	DAMAGE_DESC m_DamageDesc;

protected:
	HRESULT SetUp_Components(void* pArg);
	HRESULT SetUp_Event();
	HRESULT Setup_KineticStateMachine();
	HRESULT	SetUp_HitStateMachine();
	HRESULT SetUp_KineticComboStateMachine();

	CFSMComponent*		m_pKineticStataMachine = nullptr;
	CFSMComponent*		m_pHitStateMachine = nullptr;
	CFSMComponent*		m_pKineticComboStateMachine = nullptr;
	CBaseAnimInstance*	m_pASM = nullptr;

	CRenderer*			m_pRenderer = nullptr;
	CModel*				m_pModel = nullptr;
	CController*		m_pController = nullptr;

protected:
	HRESULT				Setup_AnimSocket();
	list<CAnimation*>	m_TestAnimSocket;
	list<CAnimation*>	m_TransNeutralSocket;

protected:	// ���� ���� �ִϸ��̼�
	list<CAnimation*>	m_Kinetic_RB_Start;
	list<CAnimation*>	m_Kinetic_RB_Loop;
	list<CAnimation*>	m_Kinetic_RB_Cancel;

	list<CAnimation*>	m_Kinetic_RB_Throw01_Start;
	list<CAnimation*>	m_Kinetic_RB_Throw01_Loop;
	list<CAnimation*>	m_Kinetic_RB_Throw01_Cancel;

	list<CAnimation*>	m_Kinetic_RB_Throw02_Start;
	list<CAnimation*>	m_Kinetic_RB_Throw02_Loop;
	list<CAnimation*>	m_Kinetic_RB_Throw02_Cancel;

	list<CAnimation*>	m_Kinetic_RB_Air_Start;
	list<CAnimation*>	m_Kinetic_RB_Air_Loop;
	list<CAnimation*>	m_Kinetic_RB_Air_Cancel;

	list<CAnimation*>	m_Kinetic_RB_Air_Throw01_Start;
	list<CAnimation*>	m_Kinetic_RB_Air_Throw01_Loop;
	list<CAnimation*>	m_Kinetic_RB_Air_Throw01_Cancel;

	list<CAnimation*>	m_Kinetic_RB_Air_Throw02_Start;
	list<CAnimation*>	m_Kinetic_RB_Air_Throw02_Loop;
	list<CAnimation*>	m_Kinetic_RB_Air_Throw02_Cancel;

protected:	// Ű��ƽ ����� ���� �ִϸ��̼�
	list<CAnimation*>	m_KineticCombo_Slash01;	// Ű��ƽ ��������
	list<CAnimation*>	m_KineticCombo_Slash02;	// Ű��ƽ X�� ����
	list<CAnimation*>	m_KineticCombo_Slash03;	// Ű��ƽ ��������(3��)
	list<CAnimation*>	m_KineticCombo_Slash04;	// Ű��ƽ ��������(4��)
	
	list<CAnimation*>	m_KineticCombo_Kinetic01_Start;		// ���� Ű��ƽ ����
	list<CAnimation*>	m_KineticCombo_Kinetic01_Throw;		// ���� Ű��ƽ ������
	list<CAnimation*>	m_KineticCombo_Kinetic01_Cancel;	// ���� Ű��ƽ ���, �Ϲ� ���·� ���ư�
	list<CAnimation*>	m_KineticCombo_Kinetic01_ThrowEnd;	// ���� ����
										
	list<CAnimation*>	m_KineticCombo_Kinetic02_Start;
	list<CAnimation*>	m_KineticCombo_Kinetic02_Throw;
	list<CAnimation*>	m_KineticCombo_Kinetic02_Cancel;

	list<CAnimation*>	m_KineticCombo_Kinetic03_Start;
	list<CAnimation*>	m_KineticCombo_Kinetic03_Throw;
	list<CAnimation*>	m_KineticCombo_Kinetic03_Cancel;

	list<CAnimation*>	m_KineticCombo_KineticFinish_Start;
	list<CAnimation*>	m_KineticCombo_KineticFinish_Throw;
	list<CAnimation*>	m_KineticCombo_KineticFinish_Cancel;

protected:	// �ǰ� ���� �ִϸ��̼�
	list<CAnimation*>	m_Hit_FL_Level01;
	list<CAnimation*>	m_Hit_F_Level01;
	list<CAnimation*>	m_Hit_FR_Level01;
	list<CAnimation*>	m_Hit_B_Level01;

	list<CAnimation*>	m_Hit_F_Level02;
	list<CAnimation*>	m_Hit_B_Level02;
	list<CAnimation*>	m_Hit_L_Level02;
	list<CAnimation*>	m_Hit_R_Level02;

	list<CAnimation*>	m_Knuckback;

	list<CAnimation*>	m_Fall;
	list<CAnimation*>	m_FallDown_Back;

	list<CAnimation*>	m_BreakFall_Front;
	list<CAnimation*>	m_BreakFall_Back;

protected:	// �ǰ� ���� ����
	_vector	m_vHitDir;

public:
	_bool	isAir() { return m_bAir; }
	_bool	isMove() { return m_bMove; }
	_bool	isWalk() { return m_bWalk; }
	_bool	isLeftClick() { return m_bLeftClick; }
	_bool	isDash() { return m_bDash; }
	_bool	isNonCharge() { return m_bNonCharge; }
	_bool	isCharge() { return m_bCharge; }
	_bool	isJump() { return m_bJump; }
	_bool	isOnFloor() { return m_bOnFloor; }
	_bool	isSeperateAnim() { return m_bSeperateAnim; }

	_float	GetPlayRatio() { return m_fPlayRatio; }
	_float	GetfYSpeed() { return m_fYSpeed; }

protected:
	_bool	m_bHit = false;
	_bool	m_bBreakFall = false;

	_bool	m_bAir = false;		// �ǵ��� ���� �������� �ľ��� ����
	_bool	m_PreAir = false;
	_bool	m_bMove = false;
	_bool	m_bWalk = false;
	_bool	m_bLeftClick = false;
	_bool	m_bDash = false;
	_bool	m_bJump = false;
	_bool	m_bSeperateAnim = false;

	_bool	m_bKineticRB = false;
	_bool	m_bKineticG = false;
	_bool	m_bKineticMove = false;

	_bool	m_bNonCharge = false;
	_bool	m_bCharge = false;
	_float  m_fBefCharge = 0.f;			// ���� ���� ��
	_float	m_fCharge[3] = { 0.f, };	// ���� ����

	_float	m_fKineticCombo_Slash = 0.f;	// ��Ÿ Ÿ�� �� ����
	_float	m_fKineticCombo_Kinetic = 0.f;	// Ű��ƽ Ÿ�� �� ����
	_float	m_fKineticCharge = 0.f;			// ���� ���� �ð�, �⺻������ 1��

	_bool	m_bKineticCombo = false;	// ���� ���� ����������?

	_float	m_fJumpPower = 10.f;

	_uint	m_iSkillUsableCnt = 2;

	_float	m_fPlayRatio = 0.f;

	_float	m_fTurnSpeed = 0.f;

	EMoveDir	m_eMoveDir = DIR_END;

public:	// ASM��, ���¸��� ������ ���� �� �� �������� ������ �����ߴµ� ���ߺξ��ϰ� �׷�
	CPlayer&	SetAbleState(REMOTE	tagRemote);

public:	//EventCaller��
	void		Event_SetCanTurn(_bool is) { m_bCanTurn = is; }
	void		Event_SetCanMove(_bool is) { m_bCanMove = is; }
	void		Event_SetCanRun(_bool is) { m_bCanRun = is; }
	void		Event_SetCanTurn_Attack(_bool is) { m_bCanTurn_Attack = is; }
	void		Event_SetOnAir(_bool is) { m_bAir = is; }
	void		Event_SetGravity(_bool is) { m_bActiveGravity = is; }
	void		Event_SetLocalRevise(_bool is) { m_bLocalRevise = is; }
	void		Event_MoveLimitReset() { MoveLimitReset(); }
	void		Event_AttackLimitReset() { AttackLimitReset(); }
	void		Event_ChargeReset() { Reset_Charge(); }

	void		Event_Effect(string szEffectName);

public:
	void		Set_KineticCombo_Kinetic() { m_fKineticCombo_Kinetic = 1.f; }	// Ű��ƽ ������Ʈ���� ����, �浹 �߻��� �޺� �����ϵ��� ���ش�.

protected:
	void		Reset_Charge();

protected:
	unordered_map<string, string>	m_mapEffectGroup;
	void		Load_DefaultEffects(const char* pEffectDir);
	void		EffectMaker();
	unordered_map<string, CGameObject*>	m_mapPlayerEffect;

protected:	// ���� ���¿� ���� ����, ȸ���� ��������, �������� ��������?
	_bool		m_bCanTurn = false;
	_bool		m_bCanMove = false;
	_bool		m_bCanRun = false;
	_bool		m_bCanTurn_Attack = false;
	_bool		m_bLocalRevise = false;		//	�¿� �� ���� ����

public:
	_bool		isPlayerAttack(void);	// ���� ���� �ִϸ��̼��� �� true ��ȯ
	
public:
	_bool		BeforeCharge(_float fBeforeCharge);
	_bool		Charge(_uint iNum, _float fCharge);

public:
	void		Jump();
	void		AirBorne() { m_fYSpeed = 10.f; }
	void		SetGravity_Optional(_float fGravity) { m_fYSpeed = fGravity; }
	void		SmoothTurn_Attack(_double TimeDelta);

protected:
	void		BehaviorCheck(_double TimeDelta);
	void		MoveStateCheck(_double TimeDelta);
	void		SeperateCheck();
	void		HitCheck();

public:
	EMoveDir	GetMoveDir() const { return m_eMoveDir; }
	Vector3		GetMoveVector() { return m_vMoveDir; }
	void		LookAtDir(Vector3 Vector);

public:
	_bool		UseAttackCnt(_uint eType);
	_bool		UseMoveCnt(_uint eType);

	void		AttackLimitReset(void);
	void		MoveLimitReset(void);

protected:		// Ƚ�� ���� �� Ư�� �������� ���� �ʱ�ȭ �Ǵ� �͵��� ���� ���� ����ü
	ATTACKLIMIT	m_AttackLimit;		// ���� ����
	MOVELIMIT	m_MoveLimit;		// �̵� ����

protected:
	HRESULT					Setup_Parts();
	vector<CGameObject*>	m_vecWeapon;

protected:
	Vector3		m_vMoveDir		= Vector3();
	Vector4		m_vLastDir		= Vector4();
	Vector4		m_vBefPos		= Vector4();
	Matrix		m_vMatCamRot	= Matrix();

protected:
	_bool		m_bTestKey = false;

protected:
	wstring		m_ModelName;

	CCamera*	m_pPlayerCam = nullptr;

protected:
	void			Attack_Effect(const string& szBoneName, _float fSize);
	CEffectGroup*	m_pEffect = nullptr;

protected:
	void			Search_Usable_KineticObject();
	void			Spline_Kinetic(_double TimeDelta);
	void			Kinetic_Test(_float fRatio);
	void			Kinetic_ByTurn();
	CGameObject*	m_pKineticObject = nullptr;
	CGameObject*	m_pTargetedEnemy = nullptr;
	_vector			m_vCamLook;

protected:
	_float4 m_vSplinePoint_01;
	_float4 m_vSplinePoint_02;
	_float4 m_vSplinePoint_03;
	_float4 m_vSplinePoint_04;

protected:
	_vector	m_vToKineticObj;	// �ڷ� �� ���� ���� ���� ���� ����
	_vector m_vKineticPoint;	// �ڷ� �� ���� ���� ���� �Ϸ� ����
	_vector m_vKineticOrbit;
	_bool	m_bSwingKineticThrow = false;
	_vector m_vThrow;
	_float	m_fSwingLerpTimer = 0.f;
	_bool	m_bRight = false;
	_vector m_vKineticInitLook;

public:
	static CPlayer*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END