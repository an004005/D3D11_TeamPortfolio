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
END

BEGIN(Client);
class CBaseAnimInstance;
class CController;

class CPlayer : public CScarletCharacter
{
private:
	// ���� Ƚ�� ������ �ִ� State�鿡 ���� ����, Reset�Լ� ���� �� �ش� ����ü ���� ��� ���� �ʱ�ȭ�ȴ�.
	typedef struct tagUseLimit
	{
		// ���� ��
		_uint m_iNonChargeAttack_Floor = 2;
		_uint m_iNonChargeAttack_Air = 2;
		_uint m_iAttack_Air01 = 1;
		_uint m_iAttack_Air02 = 1;
		_uint m_iDoubleJump = 1;

		// ����ġ
		_uint MAX_iNonChargeAttack_Floor = 2;
		_uint MAX_iNonChargeAttack_Air = 2;
		_uint MAX_iAttack_Air01 = 1;
		_uint MAX_iAttack_Air02 = 1;
		_uint MAX_iDoubleJump = 1;

	}USELIMIT;

public:
	enum STACK { STACK_NONCHARGE_FLOOR, STACK_NONCHARGE_AIR, STACK_ATTACK_AIR01, STACK_ATTACK_AIR02, STACK_DOUBLEJUMP, STACK_END, };
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
	_float  m_fBefCharge = 0.f;			// ���� ���� ��
	_float	m_fCharge[3] = { 0.f, };	// ���� ����

	_float	m_fJumpPower = 8.f;

	_uint	m_iSkillUsableCnt = 2;

	_float	m_fPlayRatio = 0.f;

	_float	m_fTurnSpeed = 0.f;

	EMoveDir	m_eMoveDir = DIR_END;

public:
	CPlayer&	SetCanTurn(_bool is) { m_bCanTurn = is; return *this; }
	CPlayer&	SetCanMove(_bool is) { m_bCanMove = is; return *this; }
	CPlayer&	SetCanRun (_bool is) { m_bCanRun = is; return *this; }
	CPlayer&	SetOnAir  (_bool is) { m_bAir = is; return *this; }

	CPlayer&	SetWeightless(_bool is) { m_bActiveGravity = is; return *this; }	// true�����̸� ���߷� ���·� ��ȯ�ȴ�.
	CPlayer&	SetCanTurn_Attack(_bool is) { m_bCanTurn_Attack = is; return *this; }

public:
	CPlayer&	Reset_Charge();
	CPlayer&	Reset_SkillCnt();
	CPlayer&	Reset_Gravity();

protected:	// ���� ���¿� ���� ����, ȸ���� ��������, �������� ��������?
	_bool		m_bCanTurn = false;
	_bool		m_bCanMove = false;
	_bool		m_bCanRun = false;
	_bool		m_bCanTurn_Attack = false;

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
	_bool		UseSkillCnt(_uint eType);	// ��� Ƚ�� �Ǵ�
	CPlayer&	UseLimitReset(void);		// �ѵ� �ع�

protected:		// Ƚ�� ���� �� Ư�� �������� ���� �ʱ�ȭ �Ǵ� �͵��� ���� ���� ����ü
	USELIMIT	m_UseLimit;

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

public:
	static CPlayer*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END