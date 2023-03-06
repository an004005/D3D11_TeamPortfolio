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
	// 행동 관련
	_bool CanTurn = false;	// 카메라 보는 방향으로 회전
	_bool CanMove = false;	// 월드 무브 가능 여부
	_bool CanRun = false;	// 달리기 여부
	_bool AttackTurn = false;	// 공격 중 회전(카메라 방향) 여부

								// 상태 관련
	_bool OnAir = false;	// 공중 상태 여부
	_bool Gravity = false;	// 중력 적용 여부

							// 제한 관련
	_bool MoveLimitReset = false;	// 횟수 제한 이동 초기화
	_bool AttackLimitReset = false;	// 횟수 제한 공격 초기화
	_bool ChargeReset = false;	// 차지 상태 초기화

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
	// 공격 횟수 제한이 있는 State들에 대해 관리, Reset함수 동작 시 해당 구조체 내의 모든 값이 초기화된다.
	typedef struct tagAttackLimit
	{
		// 현재 값
		_uint m_iNonChargeAttack_Floor = 2;
		_uint m_iNonChargeAttack_Air = 2;
		_uint m_iAttack_Air01 = 1;
		_uint m_iAttack_Air02 = 1;
		_uint m_iAttack_AirDodge = 1;

		// 상한치
		_uint MAX_iNonChargeAttack_Floor = 2;
		_uint MAX_iNonChargeAttack_Air = 2;
		_uint MAX_iAttack_Air01 = 1;
		_uint MAX_iAttack_Air02 = 1;
		_uint MAX_iAttack_AirDodge = 1;

	}ATTACKLIMIT;

	typedef struct tagMoveLimit
	{	
		// 현재 값
		_uint m_iDoubleJump = 1;
		_uint m_iAirDodge = 1;

		// 상한치
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

protected:	// 염력 소켓 애니메이션
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

protected:	// 키네틱 연계기 소켓 애니메이션
	list<CAnimation*>	m_KineticCombo_Slash01;	// 키네틱 돌진베기
	list<CAnimation*>	m_KineticCombo_Slash02;	// 키네틱 X자 베기
	list<CAnimation*>	m_KineticCombo_Slash03;	// 키네틱 돌려베기(3단)
	list<CAnimation*>	m_KineticCombo_Slash04;	// 키네틱 돌려베기(4단)
	
	list<CAnimation*>	m_KineticCombo_Kinetic01_Start;		// 연계 키네틱 차지
	list<CAnimation*>	m_KineticCombo_Kinetic01_Throw;		// 연계 키네틱 던지기
	list<CAnimation*>	m_KineticCombo_Kinetic01_Cancel;	// 연계 키네틱 취소, 일반 상태로 돌아감
	list<CAnimation*>	m_KineticCombo_Kinetic01_ThrowEnd;	// 이하 동일
										
	list<CAnimation*>	m_KineticCombo_Kinetic02_Start;
	list<CAnimation*>	m_KineticCombo_Kinetic02_Throw;
	list<CAnimation*>	m_KineticCombo_Kinetic02_Cancel;

	list<CAnimation*>	m_KineticCombo_Kinetic03_Start;
	list<CAnimation*>	m_KineticCombo_Kinetic03_Throw;
	list<CAnimation*>	m_KineticCombo_Kinetic03_Cancel;

	list<CAnimation*>	m_KineticCombo_KineticFinish_Start;
	list<CAnimation*>	m_KineticCombo_KineticFinish_Throw;
	list<CAnimation*>	m_KineticCombo_KineticFinish_Cancel;

protected:	// 피격 소켓 애니메이션
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

protected:	// 피격 관련 변수
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

	_bool	m_bAir = false;		// 의도한 공중 상태인지 파악을 위함
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
	_float  m_fBefCharge = 0.f;			// 실제 차지 전
	_float	m_fCharge[3] = { 0.f, };	// 실제 차지

	_float	m_fKineticCombo_Slash = 0.f;	// 평타 타격 시 갱신
	_float	m_fKineticCombo_Kinetic = 0.f;	// 키네틱 타격 시 갱신
	_float	m_fKineticCharge = 0.f;			// 염력 차지 시간, 기본적으로 1초

	_bool	m_bKineticCombo = false;	// 현재 공격 진행중인지?

	_float	m_fJumpPower = 10.f;

	_uint	m_iSkillUsableCnt = 2;

	_float	m_fPlayRatio = 0.f;

	_float	m_fTurnSpeed = 0.f;

	EMoveDir	m_eMoveDir = DIR_END;

public:	// ASM용, 상태마다 리모컨 값을 싹 다 지정하지 않으면 점프했는데 공중부양하고 그럼
	CPlayer&	SetAbleState(REMOTE	tagRemote);

public:	//EventCaller용
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
	void		Set_KineticCombo_Kinetic() { m_fKineticCombo_Kinetic = 1.f; }	// 키네틱 오브젝트에서 지정, 충돌 발생시 콤보 가능하도록 해준다.

protected:
	void		Reset_Charge();

protected:
	unordered_map<string, string>	m_mapEffectGroup;
	void		Load_DefaultEffects(const char* pEffectDir);
	void		EffectMaker();
	unordered_map<string, CGameObject*>	m_mapPlayerEffect;

protected:	// 현재 상태에 따라 제어, 회전이 가능한지, 움직임이 가능한지?
	_bool		m_bCanTurn = false;
	_bool		m_bCanMove = false;
	_bool		m_bCanRun = false;
	_bool		m_bCanTurn_Attack = false;
	_bool		m_bLocalRevise = false;		//	좌우 한 걸음 고정

public:
	_bool		isPlayerAttack(void);	// 공격 중인 애니메이션일 때 true 반환
	
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

protected:		// 횟수 제한 및 특정 조건으로 스택 초기화 되는 것들의 스택 모임 구조체
	ATTACKLIMIT	m_AttackLimit;		// 공격 관련
	MOVELIMIT	m_MoveLimit;		// 이동 관련

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
	_vector	m_vToKineticObj;	// 뒤로 붕 돌릴 때의 보간 시작 지점
	_vector m_vKineticPoint;	// 뒤로 붕 돌릴 때의 보간 완료 지점
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