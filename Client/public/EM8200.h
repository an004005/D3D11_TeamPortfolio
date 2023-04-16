#pragma once
#include <AnimCam.h>

#include "Client_Defines.h"
#include "Enemy.h"
#include "Controller.h"

BEGIN(Engine)
class CFSMComponentBuilder;
END

BEGIN(Client)
class CEM8200 :   public CEnemy
{
private:
	CEM8200(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM8200(const CEM8200& rhs);
	virtual ~CEM8200() = default;

public:
	virtual HRESULT		Initialize(void* pArg) override;

	virtual void		SetUpComponents(void* pArg) override;
	virtual void		SetUpSound() override;
	virtual void		SetUpAnimationEvent() override;
	virtual void		SetUpFSM() override;

	virtual void		BeginTick() override;
	virtual void		Tick(_double TimeDelta) override;
	virtual void		Late_Tick(_double TimeDelta) override;
	virtual void		AfterPhysX() override;

	virtual HRESULT		Render() override;
	virtual void		Imgui_RenderProperty() override;
	virtual void		SetUpUI() override;

	void				Set_StoryEnd() { m_bStoryEnd = true; }

public:
	_bool				IsMove() const { return m_vMoveAxis != _float3::Zero; }
	_float3				GetMoveAxis() const { return m_vMoveAxis; }
	_float				GetTurnRemain() const { return m_fTurnRemain; }
	_bool				IsPlayingSocket() const;
	_bool				IsRun() const { return m_bRun; }
	_bool				Check_SecondPhase() { return m_bSecondPhase; }

	void				Clear_Socket();
	void				Set_KineticObject(CGameObject* pObject);
	void				Kinetic_Combo_AttachLerpObject();	// 염력 물체를 애니메이션 포인트까지 끌고오는 함수
	void				Kinetic_Combo_KineticAnimation();	// 염력 물체를 궤도에 태우는 함수

protected:
	void				Detected_Attack();
	void				AddState_Idle(CFSMComponentBuilder& Builder);
	void				AddState_Teleport(CFSMComponentBuilder& Builder);
	void				AddState_Attack_Kick(CFSMComponentBuilder& Builder);
	void				AddState_Attack_IceNeedle(CFSMComponentBuilder& Builder);
	void				AddState_Attack_ChaseElec(CFSMComponentBuilder& Builder);
	void				AddState_Attack_AirElec(CFSMComponentBuilder& Builder);
	void				AddState_Attack_Rush(CFSMComponentBuilder& Builder);
	void				AddState_Seethrough(CFSMComponentBuilder& Builder);
	void				AddState_Counter(CFSMComponentBuilder& Builder);
	void				AddState_CaptureKinetic(CFSMComponentBuilder& Builder);
	void				AddState_BrainField(CFSMComponentBuilder& Builder);
	void				AddState_BrainCrush(CFSMComponentBuilder& Builder);

	void				AddState_Intro(CFSMComponentBuilder& Builder);



	void				AddState_Damaged(CFSMComponentBuilder& Builder);


	void				Create_Bullet();
	// void	Strew_Overlap(); // FlowerShower 공격
	// void	Spin_SweepCapsule();
	// void	Kick_SweepSphere();
	// void	Dodge_VelocityCalc();
	// void	HeavyAttackPushStart();

public:
	virtual void TakeDamage(DAMAGE_PARAM tDamageParams) override;


protected:
	virtual void CheckHP(DAMAGE_PARAM& tDamageParams) override;

private:
	void Play_MidHitAnim();
	void Play_HeavyHitAnim();
	void TP_Start();


private:
	//For Collision
	void Melee_Overlap(const string& pBornName, _uint iDamage, _float fRad, EAttackType eAtkType);
	void Range_Overlap(_float4 vPos, _uint iDamage, _float fRad, EAttackType eAtkType);

	_bool Check_PlayerDetected();
	_bool Check_PlayerDetected_Near();
	_bool Check_StoryEnd();
private:
	class CEM8200_Controller* m_pController = nullptr;
	class CEM8200_AnimInstance* m_pASM = nullptr;

	class CEffectSystem* m_pKarenMaskEf = nullptr;
	// CParticleGroup* m_pFallRoseParticle = nullptr;
	// CParticleGroup* m_pShootFlwParticle = nullptr;
	_bool			m_bStoryEnd = false;
	CDoOnce			m_bStoryModeStart;
	
private:
	CModel* m_pKineticModel = nullptr;
	CGameObject* m_pKineticObject = nullptr;

	_matrix m_KineticObjectOrigionPos = XMMatrixIdentity();

private:
	_float3 m_vMoveAxis;
	_float m_fTurnRemain = 0.f;
	_bool m_bHitAir = false;

	// Jump, dodge
	_bool m_bJumpAttack = false;
	_float3 m_vOnJumpMoveVelocity;

	_bool m_bDodge = false;
	_float4	m_vDodgeDir;
	// ~ Jump, dodge

	// spin atk
	_bool m_bSpinAtk = false;

	// Run 상태를 위한 _bool
	_bool m_bRun = false;

	CController::EHandleInput m_eInput;

	CSimpleTimeline m_HeavyAttackPushTimeline;
	_float4 m_vPushVelocity;


private:
	_float4x4 pivot1 = XMMatrixIdentity();
	_float	  m_fAnimProgress = 1.f;
	_float	  m_fLerpTime = 0.f;
	_float	  m_fDefault_LerpTime = 0.1f;

	//For Teleport
	_float4 m_vTeleportPos;
	_float	m_fTP_Range = 0.f;

	CDoOnce m_SetTPOnce;

	CDoOnce m_SecondPhase;
	CDoOnce m_DetectedOnce;

	CCoolTimeHelper m_CounterEFCoolTimeHelper;

	_bool	m_bMeleeCollStart = false; 
	_bool	m_bRangeCollStart = false;

	_float4	m_vRangeOverlapPos = { 0.f,0.f,0.f,1.f };
	_float	m_fElecRangeValue = 0.f;

	_float4x4 Pivotmat;

	// For Rush
	_bool	m_bRushStart = false;

	// For 2Phase
	_bool	m_bSecondPhase = false;

private:
	CEffectGroup* m_pDashEF = nullptr;

	vector<wstring>	m_vecRandomTeleportEffect{
		L"Sas_Teleport_Effect_A",
		L"Sas_Teleport_Effect_B",
		L"Sas_Teleport_Effect_C",
		L"Sas_Teleport_Effect_D"
	};


private:
	//For Karen BrainField
	CAnimCam* m_pKaren_AnimCam = nullptr;

private:
	CSimpleTimeline			m_TPStart;
	CSimpleTimeline			m_TPEnd;

	CSimpleTimeline			m_CounterStart;
	CSimpleTimeline			m_CounterEnd;

	CSimpleTimeline			m_CaptureStart;
	CSimpleTimeline			m_CaptureEnd;

	CSimpleTimeline			m_KarenMaskStart;

	class CEM8200_CopyRush* m_pLeftCopy = nullptr;
	class CEM8200_CopyRush* m_pRightCopy = nullptr;

	class CEM8200_BrainField* m_pBrainField = nullptr;

	CDoOnce m_BrainCrushOnce;
	class CEM8200_BrainCrushCables* m_pBrainCrushCables = nullptr;

	CCurveFloatImpl* m_pBrainCrushRimLight = nullptr;
	CCurveFloatImpl* m_pBrainCrushChromaticAberration = nullptr;
	CCurveFloatImpl* m_pBrainCrushChromaticAberrationCrash = nullptr;
	class CPostVFX_ChromaticAberration* m_pChromaticAberration = nullptr;

	class CCanvas_MainTalk* m_pCanvas_MainTalk = nullptr;

	CDoOnce m_ItemSpawn;
	CDoOnce m_BlackOut;

	class CConsumption_Item* m_pLastItem = nullptr;



public:
	void	SetBrainCrashCommand() { m_bBrainCrashCommand = true; }

private:
	_bool	m_bBrainCrashCommand = false;

public:
	static CEM8200*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg = nullptr) override;
	virtual void			Free() override;
};




END
