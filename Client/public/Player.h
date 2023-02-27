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
public:
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
	_bool	isShiftClick() { return m_bShiftClick; }
	_bool	isShiftPress() { return m_bShiftPress; }

	_float	GetPlayRatio() { return m_fPlayRatio; }

protected:
	_bool	m_bAir = false;
	_bool	m_PreAir = false;
	_bool	m_bMove = false;
	_bool	m_bWalk = false;
	_bool	m_bLeftClick = false;
	_bool	m_bShiftClick = false;
	_bool	m_bShiftPress = false;

	_float	m_fPlayRatio = 0.f;

	_float	m_fTurnSpeed = 0.f;

	EMoveDir	m_eMoveDir = DIR_END;

public:
	CPlayer&	SetCanTurn(_bool is) { m_bCanTurn = is; return *this; }
	CPlayer&	SetCanMove(_bool is) { m_bCanMove = is; return *this; }

protected:	// 현재 상태에 따라 제어, 회전이 가능한지, 움직임이 가능한지?
	_bool	m_bCanTurn = false;
	_bool	m_bCanMove = false;

protected:
	void	BehaviorCheck(_double TimeDelta);
	void	MoveStateCheck(_double TimeDelta);

public:
	EMoveDir	GetMoveDir() const { return m_eMoveDir; }
	Vector3		GetMoveVector() { return m_vMoveDir; }
	void		LookAtDir(Vector3 Vector);
	Matrix		GetMatCamRot() const { return m_vMatCamRot; }
	void		Turn(_fvector vAxis, _float fRange) { m_pTransformCom->Turn(vAxis, fRange); }

protected:
	Vector3		m_vMoveDir = Vector3();
	Vector4		m_vLastDir = Vector4();
	Vector4		m_vBefPos = Vector4();
	Matrix		m_vMatCamRot = Matrix();

protected:
	wstring		m_ModelName;

public:
	static CPlayer*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END