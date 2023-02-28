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

	tagRemote(	_bool CanTurn, _bool CanMove, _bool CanRun, _bool AttackTurn,
				_bool OnAir, _bool Gravity, 
				_bool MoveLimitReset, _bool AttackLimitReset, _bool ChargeReset)
	:CanTurn(CanTurn), CanMove(CanMove), CanRun(CanRun), AttackTurn(AttackTurn), 
		OnAir(OnAir), Gravity(Gravity), 
		MoveLimitReset(MoveLimitReset), AttackLimitReset(AttackLimitReset), ChargeReset(ChargeReset){}

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
	virtual void BeginTick() {}
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual void AfterPhysX() override;
	virtual HRESULT Render();

	virtual void Imgui_RenderProperty() override;

protected:
	HRESULT SetUp_Components(void* pArg);
	HRESULT	SetUp_AttackFSM();
	HRESULT SetUp_Event();

	CFSMComponent*		m_pFSM = nullptr;
	CBaseAnimInstance*	m_pASM = nullptr;

	CRenderer*			m_pRenderer = nullptr;
	CModel*				m_pModel = nullptr;
	CController*		m_pController = nullptr;

protected:
	HRESULT				Setup_AnimSocket();
	list<CAnimation*>	m_TestAnimSocket;

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

	_float	GetPlayRatio() { return m_fPlayRatio; }

protected:
	_bool	m_bAir = false;
	_bool	m_PreAir = false;
	_bool	m_bMove = false;
	_bool	m_bWalk = false;
	_bool	m_bLeftClick = false;
	_bool	m_bDash = false;
	_bool	m_bJump = false;

	_bool	m_bNonCharge = false;
	_bool	m_bCharge = false;
	_float  m_fBefCharge = 0.f;			// 실제 차지 전
	_float	m_fCharge[3] = { 0.f, };	// 실제 차지

	_float	m_fJumpPower = 8.f;

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
	void		Event_SetSyncAxis(_uint eType, _bool is) { m_bCanSync_Axis[eType] = is; }
	void		Event_MoveLimitReset() { MoveLimitReset(); }
	void		Event_AttackLimitReset() { AttackLimitReset(); }
	void		Event_ChargeReset() { Reset_Charge(); }
	/*
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
	*/


protected:
	void		Reset_Charge();

protected:	// 현재 상태에 따라 제어, 회전이 가능한지, 움직임이 가능한지?
	_bool		m_bCanTurn = false;
	_bool		m_bCanMove = false;
	_bool		m_bCanRun = false;
	_bool		m_bCanTurn_Attack = false;
	_bool		m_bCanSync_Axis[4] = { false, false, false, false };	// eMoveDir 사용하기

public:
	_bool		isPlayerAttack(void);	// 공격 중인 애니메이션일 때 true 반환
	
public:
	_bool		BeforeCharge(_float fBeforeCharge);
	_bool		Charge(_uint iNum, _float fCharge);

public:
	void		Jump();
	void		SetGravity_Optional(_float fGravity) { m_fYSpeed = fGravity; }
	void		SmoothTurn_Attack(_double TimeDelta);

protected:
	void		BehaviorCheck(_double TimeDelta);
	void		MoveStateCheck(_double TimeDelta);

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
	wstring		m_ModelName;

	CCamera*	m_pPlayerCam = nullptr;

public:
	static CPlayer*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END