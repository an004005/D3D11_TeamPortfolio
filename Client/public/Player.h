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
class CCurveFloatImpl;
class CGameObject;
END

BEGIN(Client);
class CBaseAnimInstance;
class CController;
class CEffectGroup;
class CCamSpot;
class CSAS_Portrait;

typedef struct tagRemote
{
	// 행동 관련
	_bool bCanTurn = false;	// 카메라 보는 방향으로 회전
	_bool bCanMove = false;	// 월드 무브 가능 여부
	_bool bCanRun = false;	// 달리기 여부
	_bool bAttackTurn = false;	// 공격 중 회전(카메라 방향) 여부

	// 상태 관련
	_bool bOnAir = false;	// 공중 상태 여부
	_bool bGravity = false;	// 중력 적용 여부

							// 제한 관련
	_bool bMoveLimitReset = false;	// 횟수 제한 이동 초기화
	_bool bAttackLimitReset = false;	// 횟수 제한 공격 초기화
	_bool bChargeReset = false;	// 차지 상태 초기화

	_bool bLocalRevise = false;

	tagRemote(	_bool bCanTurn, _bool bCanMove, _bool bCanRun, _bool bAttackTurn,
				_bool bOnAir, _bool bGravity, 
				_bool bMoveLimitReset, _bool bAttackLimitReset, _bool bChargeReset,
				_bool bLocalRevise)
	:bCanTurn(bCanTurn), bCanMove(bCanMove), bCanRun(bCanRun), bAttackTurn(bAttackTurn), 
		bOnAir(bOnAir), bGravity(bGravity), 
		bMoveLimitReset(bMoveLimitReset), bAttackLimitReset(bAttackLimitReset), bChargeReset(bChargeReset),
		bLocalRevise(bLocalRevise){}

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

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	//For Effect
	virtual _float4x4 GetBoneMatrix(const string& strBoneName, _bool vPivotapply = true) override;
	virtual _float4x4 GetPivotMatrix() override;
	//~For Effect

	//~For UI 
	_float		Get_KineticCharge() { return m_fKineticCharge; }

	_bool		Get_Hit() { return m_bHit; }
	_bool		Get_Dash() { return m_bDash; }

	_bool		Get_SASSkillInput(const _uint iInputNumber) { return m_bSASSkillInput[iInputNumber]; }
	void		Set_SASSkillInput(const _uint iInputNumber, const _bool bInput) { m_bSASSkillInput[iInputNumber] = bInput; }

//	PLAYER_STAT	Get_PlayerStat() { return m_PlayerStat; }
//	void		Set_Usable(const ESASType & eESASType, const _bool & bUsable) { m_PlayerStat.Sasese[_int(eESASType)].bUsable = bUsable; }

//	ESASType	Get_PlayerSasType() { return m_PlayerSasType; }
	//~For UI

	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void BeginTick();
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual void AfterPhysX() override;
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth() override;

	virtual void TakeDamage(DAMAGE_PARAM tDamageParams);

	virtual void Imgui_RenderProperty() override;

public:	// 현재 타게팅 된 몬스터 포인터 가져오는 함수
//	CGameObject*	Get_TargetedEnemy();

private:
	void CamBoneTest();	// 액션캠 예시

private:
	void			SasMgr();
//	PLAYER_STAT		m_PlayerStat;
	DAMAGE_DESC		m_DamageDesc;
	DAMAGE_PARAM	m_AttackDesc;

private:
	_bool			m_bAttackEnable = false;

private:
	HRESULT SetUp_Components(void* pArg);
	HRESULT SetUp_RigidBody();
	HRESULT SetUp_Event();
	HRESULT SetUp_EffectEvent();
	HRESULT Setup_KineticStateMachine();
	HRESULT	SetUp_HitStateMachine();
	HRESULT SetUp_KineticComboStateMachine();
	HRESULT SetUp_JustDodgeStateMachine();
	HRESULT SetUp_Sound();

	HRESULT SetUp_AttackDesc();

	unordered_map<string, std::function<void()>>	m_mapCollisionEvent;

	CFSMComponent*		m_pKineticStataMachine = nullptr;
	CFSMComponent*		m_pHitStateMachine = nullptr;
	CFSMComponent*		m_pKineticComboStateMachine = nullptr;
	CFSMComponent*		m_pJustDodgeStateMachine = nullptr;

	CBaseAnimInstance*	m_pASM = nullptr;

	CRenderer*			m_pRenderer = nullptr;
	CModel*				m_pModel = nullptr;
	CController*		m_pController = nullptr;

	CModel*				m_pKineticAnimModel = nullptr;

	//UI
	class CNoticeNeonUI* m_pNeonUI = { nullptr };
	class CMonsterLockonUI*	m_pUI_LockOn = nullptr;
	CGameObject*		m_pSettedTarget = nullptr;

private:	// 특수연출용 FSM
	HRESULT				SetUp_TrainStateMachine();
	CFSMComponent*		m_pTrainStateMachine_Left = nullptr;

	HRESULT				SetUp_TelephonePoleStateMachine();
	CFSMComponent*		m_pTelephonePoleStateMachine_Left = nullptr;

	HRESULT				SetUp_BrainCrashStateMachine();
	CFSMComponent*		m_pBrainCrashStateMachine = nullptr;

private:	// 특수연출용 소켓 애니메이션
	list<CAnimation*>	m_Train_Charge_L;	// 좌측 기차 차지
	list<CAnimation*>	m_Train_Cancel_L;	// 좌측 기차 취소
	list<CAnimation*>	m_Train_Throw_L;	// 좌측 기차 던짐

	list<CAnimation*>	m_TelephonePole_Charge_L;	// 좌측 전봇대 차지
	list<CAnimation*>	m_TelephonePole_Cancel_L;	// 좌측 전봇대 취소
	list<CAnimation*>	m_TelephonePole_Start_L;	// 좌측 전봇대 뽑음
	list<CAnimation*>	m_TelephonePole_Throw_L;	// 좌측 전봇대 휘두름
	list<CAnimation*>	m_TelephonePole_Wait_L;		// 추가타 대기
	list<CAnimation*>	m_TelephonePole_End_L;		// 추가타 발생하지 않고 종료됨
	list<CAnimation*>	m_TelephonePole_Swing_L;	// 좌측 전봇대 추가 휘두름

	list<CAnimation*>	m_BrainCrash_CutScene;
	list<CAnimation*>	m_BrainCrash_Activate;

private:
	HRESULT				Setup_AnimSocket();
	list<CAnimation*>	m_TestAnimSocket;
	list<CAnimation*>	m_TransNeutralSocket;

private:	// 염력 소켓 애니메이션
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

private:	// 키네틱 연계기 소켓 애니메이션
	list<CAnimation*>	m_KineticCombo_Slash01;	// 키네틱 돌진베기
	list<CAnimation*>	m_KineticCombo_Slash02;	// 키네틱 X자 베기
	list<CAnimation*>	m_KineticCombo_Slash03;	// 키네틱 돌려베기(3단)
	list<CAnimation*>	m_KineticCombo_Slash04;	// 키네틱 돌려베기(4단)
	
	list<CAnimation*>	m_KineticCombo_sLcLeR_Start;
	list<CAnimation*>	m_KineticCombo_sLcLeR_End;
	list<CAnimation*>	m_KineticCombo_sLcLeL_Start;
	list<CAnimation*>	m_KineticCombo_sLcLeL_End;
	list<CAnimation*>	m_KineticCombo_sLcReR_Start;
	list<CAnimation*>	m_KineticCombo_sLcReR_End;
	list<CAnimation*>	m_KineticCombo_sLcReL_Start;
	list<CAnimation*>	m_KineticCombo_sLcReL_End;

	list<CAnimation*>	m_KineticCombo_sRcLeL_Start;
	list<CAnimation*>	m_KineticCombo_sRcLeL_End;
	list<CAnimation*>	m_KineticCombo_sRcLeR_Start;
	list<CAnimation*>	m_KineticCombo_sRcLeR_End;
	list<CAnimation*>	m_KineticCombo_sRcReL_Start;
	list<CAnimation*>	m_KineticCombo_sRcReL_End;
	list<CAnimation*>	m_KineticCombo_sRcReR_Start;
	list<CAnimation*>	m_KineticCombo_sRcReR_End;

	list<CAnimation*>	m_KineticCombo_Pcon_cLeR_Lv1;
	list<CAnimation*>	m_KineticCombo_Pcon_cLeL_Lv1;
	list<CAnimation*>	m_KineticCombo_Pcon_cReR_Lv1;
	list<CAnimation*>	m_KineticCombo_Pcon_cReL_Lv1;
										
	list<CAnimation*>	m_KineticCombo_Pcon_cLeR_Lv2;
	list<CAnimation*>	m_KineticCombo_Pcon_cLeL_Lv2;
	list<CAnimation*>	m_KineticCombo_Pcon_cReR_Lv2;
	list<CAnimation*>	m_KineticCombo_Pcon_cReL_Lv2;

	list<CAnimation*>	m_KineticCombo_Pcon_cLeR_Lv3;
	list<CAnimation*>	m_KineticCombo_Pcon_cLeL_Lv3;
	list<CAnimation*>	m_KineticCombo_Pcon_cReR_Lv3;
	list<CAnimation*>	m_KineticCombo_Pcon_cReL_Lv3;

	list<CAnimation*>	m_KineticCombo_Pcon_cLeR_Lv4;
	list<CAnimation*>	m_KineticCombo_Pcon_cLeL_Lv4;
	list<CAnimation*>	m_KineticCombo_Pcon_cReR_Lv4;
	list<CAnimation*>	m_KineticCombo_Pcon_cReL_Lv4;

private:	// 피격 소켓 애니메이션
	list<CAnimation*>	m_Hit_FL_Level01;
	list<CAnimation*>	m_Hit_F_Level01;
	list<CAnimation*>	m_Hit_FR_Level01;
	list<CAnimation*>	m_Hit_B_Level01;

	list<CAnimation*>	m_Hit_F_Level02;
	list<CAnimation*>	m_Hit_B_Level02;
	list<CAnimation*>	m_Hit_L_Level02;
	list<CAnimation*>	m_Hit_R_Level02;

	list<CAnimation*>	m_Knuckback;

	list<CAnimation*>	m_Airborne;

	list<CAnimation*>	m_Fall;
	list<CAnimation*>	m_FallDown_Back;

	list<CAnimation*>	m_BreakFall_Front;
	list<CAnimation*>	m_BreakFall_Back;

private:	// 피격 관련 변수
	_vector	m_vHitDir;

private:
	list<CAnimation*>	m_JustDodgeSlash;

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
	_bool	isUpper() { return m_bUpper; }
	_bool	isBattle() { return m_bOnBattle; }
	_bool	isOptionalMove() { return m_bOptionalMove; }

	_float	GetPlayRatio() { return m_fPlayRatio; }
	_float	GetfYSpeed() { return m_fYSpeed; }

private:
	_bool	m_bSASSkillInput[4] = { false, false, false, false };

	_bool	m_bHit = false;
	_bool	m_bBreakFall = false;

	_bool	m_bOptionalMove = false;

	_bool	m_bAir = false;		// 의도한 공중 상태인지 파악을 위함
	_bool	m_PreAir = false;
	_bool	m_bMove = false;
	_bool	m_bWalk = false;
	_bool	m_bLeftClick = false;
	_bool	m_bDash = false;
	_bool	m_bJump = false;
	_bool	m_bSeperateAnim = false;
	_bool	m_bUpper = false;

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
	_bool	m_bKineticSpecial = false;	// 염력 특수 연출중인지?
	_bool	m_bBrainCrash = false;		// 브레인 크러시 사용중인지?
	
	_float	m_fJustDodgeAble = 0.f;

	_float	m_fJumpPower = 10.f;

	_uint	m_iSkillUsableCnt = 2;

	_float	m_fPlayRatio = 0.f;

	_float	m_fTurnSpeed = 0.f;

	EMoveDir	m_eMoveDir = DIR_END;

	_vector		m_vCamLook;

public:	// ASM용, 상태마다 리모컨 값을 싹 다 지정하지 않으면 점프했는데 공중부양하고 그럼
	CPlayer&	SetAbleState(REMOTE	tagRemote);

public:	//EventCaller용
	void		Event_SetCanTurn(_bool is) { m_bCanTurn = is; }
	void		Event_SetCanMove(_bool is) { m_bCanMove = is; }
	void		Event_SetCanRun(_bool is) { m_bCanRun = is; }
	void		Event_SetCanTurn_Attack(_bool is) { m_bCanTurn_Attack = is; }
	void		Event_SetOnAir(_bool is) { m_bAir = is; }
	void		Event_SetGravity(_bool is) { m_bActiveGravity = is; }
	void		Event_MoveLimitReset() { MoveLimitReset(); }
	void		Event_AttackLimitReset() { AttackLimitReset(); }
	void		Event_ChargeReset() { Reset_Charge(); }
	void		Event_LookAtMove() { LookAtDir(m_vMoveDir); }

	void		Event_Effect(string szEffectName, _float fSize = 1.f, string szBoneName = "Eff01");
	void		Event_EffectSound(const string& strSoundName);

	void		Event_CollisionStart();
	void		Event_collisionEnd();

	void		Event_CamMod();
	void		Event_FinishFovActionCam();

	void		Event_Kinetic_Throw();
	void		Event_KineticSlowAction();

	void		Event_Trail(_bool bTrail);
	void		Event_Dust();

	void		Event_KineticCircleEffect();
	void		Event_KineticCircleEffect_Attach();

private:
	_bool		m_bCollisionAble = false;
	
public:
	void		Set_KineticCombo_Kinetic() { m_fKineticCombo_Kinetic = 10.f; }	// 키네틱 오브젝트에서 지정, 충돌 발생시 콤보 가능하도록 해준다.

private:
	void		Reset_Charge();

private:
	void		Load_Effect(const char* pEffectDir);

	unordered_map<string, string>	m_mapDefaultEffect;
	unordered_map<string, string>	m_mapFireEffect;

private:	// 현재 상태에 따라 제어, 회전이 가능한지, 움직임이 가능한지?
	_bool		m_bCanTurn = false;
	_bool		m_bCanMove = false;
	_bool		m_bCanRun = false;
	_bool		m_bCanTurn_Attack = false;
	_bool		m_bOnBattle = false;	// 전투상태인지?

public:
	_bool		isPlayerAttack(void);	// 공격 중인 애니메이션일 때 true 반환
	
public:
	_bool		BeforeCharge(_float fBeforeCharge);
	_bool		Charge(_uint iNum, _float fCharge);
	_bool		m_bChargeEffect = false;

public:
	void		Jump();
	void		AirBorne() { m_fYSpeed = 10.f; }
	void		SetGravity_Optional(_float fGravity) { m_fYSpeed = fGravity; }
	void		SmoothTurn_Attack(_double TimeDelta);

public:
	void		Update_TargetUI();
	void		Create_TargetInfoBar(CGameObject* pTarget);

private:
	void		BehaviorCheck(_double TimeDelta);
	void		MoveStateCheck(_double TimeDelta);
	void		SeperateCheck();
	void		HitCheck();
	void		SocketLocalMoveCheck();

private:
	void		Update_NotiveNeon();

private:
	_float		m_fNetualTimer = 0.f;
	_float		m_fBattleParticleTime = 0.f; // 글자 뿅뿅
	void		NetualChecker(_double TimeDelta);

public:
	EMoveDir	GetMoveDir() const { return m_eMoveDir; }
	Vector3		GetMoveVector() { return m_vMoveDir; }
	void		LookAtDir(Vector3 Vector);
	void		LookTarget();	// 락온

public:
	_bool		UseAttackCnt(_uint eType);
	_bool		UseMoveCnt(_uint eType);

	void		AttackLimitReset(void);
	void		MoveLimitReset(void);

private:		// 횟수 제한 및 특정 조건으로 스택 초기화 되는 것들의 스택 모임 구조체
	ATTACKLIMIT	m_AttackLimit;		// 공격 관련
	MOVELIMIT	m_MoveLimit;		// 이동 관련

private:
	HRESULT					Setup_Parts();
	vector<CGameObject*>	m_vecWeapon;

private:
	Vector3		m_vMoveDir		= Vector3();
	Vector4		m_vLastDir		= Vector4();
	Vector4		m_vBefPos		= Vector4();
	Matrix		m_vMatCamRot	= Matrix();

private:
	_bool		m_bTestKey = false;

private:
	wstring		m_ModelName;

	CCamera*	m_pPlayerCam = nullptr;
	CCamSpot*	m_pCamSpot = nullptr;
	CRigidBody* m_pRange = nullptr;

//private:
//	void			Attack_Effect(const string& szBoneName, _float fSize);
//	CEffectGroup*	m_pEffect = nullptr;

private:
	void			Search_Usable_KineticObject();
	void			Enemy_Targeting(_bool bNear);
	void			KineticObject_Targeting();
	void			KineticObject_OutLineCheck();

	void			SpecialObject_Targeting();
	void			SpecialObject_OutLineCheck();

private:
	void			Spline_Kinetic(_double TimeDelta);
	void			Kinetic_Test(_float fRatio);
	void			Kinetic_ByTurn();

private:
	void			Kinetic_Combo_KineticAnimation();	// 염력 물체를 궤도에 태우는 함수
	void			Kinetic_Combo_MoveToKineticPoint();	// 염력 물체 애니메이션을 돌리기 전에 애니메이션의 위치를 잡아주는 함수
	void			Kinetic_Combo_AttachLerpObject();	// 염력 물체를 애니메이션 포인트까지 끌고오는 함수

private:
	_vector			m_vKineticComboRefPoint; // 키네틱 콤보를 할 때 이동해야 하는 포인트
	_matrix			m_KineticObjectOrigionPos = XMMatrixIdentity();
	
	_matrix			m_SpecialObjectOriginPos =  XMMatrixIdentity();

private:
	_float4 m_vSplinePoint_01;
	_float4 m_vSplinePoint_02;
	_float4 m_vSplinePoint_03;
	_float4 m_vSplinePoint_04;

private:
	_vector	m_vToKineticObj;	// 뒤로 붕 돌릴 때의 보간 시작 지점
	_vector m_vKineticPoint;	// 뒤로 붕 돌릴 때의 보간 완료 지점
	_vector m_vKineticOrbit;
	_bool	m_bSwingKineticThrow = false;
	_vector m_vThrow;
	_float	m_fSwingLerpTimer = 0.f;
	_bool	m_bRight = false;
	_vector m_vKineticInitLook;

private:	// 플레이어 림라이트, 외곽선 관련
	CCurveFloatImpl*	m_pCurve = nullptr;
	void	Start_RimLight(const string& strCurveName);
	void	Tick_RimLight(_float fRatio);
	void	End_RimLight();

private:
	_float	m_fThrowPower = 100000.f;
	_float	m_fChargePower = 3000.f;

	_float	m_fRotX = 0.f;
	_float  m_fRotY = 0.f;
	_float	m_fRotZ = 0.f;
	_float	m_fTransX = 0.f;
	_float	m_fTransY = 0.f;
	_float	m_fTransZ = 0.f;

	_float4x4 pivot1;
	_float4x4 pivot2;

private:
	CSAS_Portrait* m_pSasPortrait = nullptr;

private:
	CParticleGroup*	m_pSwordParticle = nullptr;

private:
	vector<wstring>	m_vecRandomLandingDustName;

public:
	static CPlayer*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END