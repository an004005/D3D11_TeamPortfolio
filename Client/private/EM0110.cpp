#include "stdafx.h"
#include "EM0110.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "RigidBody.h"
#include "EM0110_AnimInstance.h"
#include "EM0110_Controller.h"
#include "PhysX_Manager.h"
#include "CurveManager.h"
#include "CurveFloatMapImpl.h"
#include "ImguiUtils.h"

CEM0110::CEM0110(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
	m_eEnemyName = EEnemyName::EM0110;
}

CEM0110::CEM0110(const CEM0110 & rhs)
	: CEnemy(rhs)
{
}

HRESULT CEM0110::Initialize(void * pArg)
{
	Json em0110_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em0110/em0110Base.json");
	pArg = &em0110_json;

	

	// 배치툴에서 조절할 수 있게 하기
	{
		m_iMaxHP = 1100;
		m_iHP = 1100; // ★

		m_iAtkDamage = 50;
		iEemeyLevel = 2;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	m_eEnemyName = EEnemyName::EM0110;
	m_bHasCrushGage = true;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(120.f));

	//Create BugParticle
	CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0110_Bug_Particle")->Start_Attach(this, "Jaw", true);
	
	return S_OK;
}

void CEM0110::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em110", L"Model", (CComponent**)&m_pModelCom));

	Json RangeCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em0110/em0110Range.json");
	Json WeakCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em0110/em0110Weak.json");

	Add_RigidBody("Range", &RangeCol);
	Add_RigidBody("Weak", &WeakCol);

	// 컨트롤러, prototype안 만들고 여기서 자체생성하기 위함
	m_pController = CEM0110_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	// ASM
	m_pASM = CEM0110_AnimInstance::Create(m_pModelCom, this);
}

void CEM0110::SetUpSound()
{
	CEnemy::SetUpSound();
}

void CEM0110::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();

	// Event Caller
	
	m_pModelCom->Add_EventCaller("Turn_R_Start", [this]
	{ 
		ClearDamagedTarget();
		m_bAttack = true;
	});

	m_pModelCom->Add_EventCaller("Turn_R_End", [this] 
	{ 
		m_bAttack = false;	
	});

	m_pModelCom->Add_EventCaller("Turn_L_Start", [this]
	{ 
		ClearDamagedTarget();
		m_bAttack = true;
	});

	m_pModelCom->Add_EventCaller("Turn_L_End", [this] 
	{ 
		m_bAttack = false;	
	});

	//장판공격 시작(Area of Effect)
	m_pModelCom->Add_EventCaller("AOE_Start", [this] 
	{ 
		m_bAttack = true;

		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0110_Large_Bubbles")->Start_NoAttach(this);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0110_Little_Bubbles_A")->Start_NoAttach(this);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0110_Pop_Bubbles")->Start_NoAttach(this);

	});

	m_pModelCom->Add_EventCaller("AOE_End", [this] 
	{ 
		m_bAttack = false;
	});

	m_pModelCom->Add_EventCaller("Rush_Start", [this] 
	{
		ClearDamagedTarget();
		m_bAttack = true;

		//위치 앞으로 옮기려면 x랑 y에 똑같은값을 더해주면 됨
		_matrix RushEfeectPivotMatirx = CImguiUtils::CreateMatrixFromImGuizmoData(
		{ 1.5f, 1.5f, 0.f },
		{ -90.f, 0.f, -40.f, }, 
		{ 1.f, 1.f, 1.f });

		m_pRushEffect = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0110_Dash_Attack");
		m_pRushEffect->Start_AttachPivot(this, RushEfeectPivotMatirx, "Jaw", true, true);
		Safe_AddRef(m_pRushEffect);
	});

	m_pModelCom->Add_EventCaller("Rush_End", [this] 
	{ 
		m_bAttack = false;

		m_pRushEffect->SetDelete();
		Safe_Release(m_pRushEffect);
		m_pRushEffect = nullptr;
	});

}

void CEM0110::SetUpFSM()
{
	CEnemy::SetUpFSM();

	/*
	R : Attack_turn (발차기)
	G : Attack_c1 (장판)
	C : Attack_b2 (돌진)
	*/

	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.OnStart([this]
			{
				m_fGravity = 20.f;	
			})
			.AddTransition("Idle to Death", "Death")
				.Predicator([this] { return m_bDead; })

			.AddTransition("Idle to Hit_Heavy", "Hit_Heavy")
				.Predicator([this] { return m_eCurAttackType == EAttackType::ATK_HEAVY; })
			.AddTransition("Idle to Down", "Down")
				.Predicator([this] { return  m_iCrushGage <= 0; })

			.AddTransition("Idle to Attack_turn", "Attack_turn")
				.Predicator([this] { return m_eInput == CController::R; })
			.AddTransition("Idle to Attack_c1", "Attack_c1")
				.Predicator([this] { return m_eInput == CController::G; })
			.AddTransition("Idle to Attack_b2_Start", "Attack_b2_Start")
				.Predicator([this] { return m_eInput == CController::C; })
		
///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Hit_Heavy")
			.OnStart([this]
			{
				Play_HeavbyHitAnim();
			})
			.Tick([this](_double)
			{
				if (m_eCurAttackType == EAttackType::ATK_HEAVY)
				{
					Play_HeavbyHitAnim();
				}
			})
			.AddTransition("Hit_Heavy to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Death")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0100_474_AL_dead_down02");
			})

		.AddState("Down")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0100_475_AL_down_start02");
			})
			.AddTransition("Down to OnFloorGetup", "OnFloorGetup")
				.Predicator([this]
			{
				return m_pASM->isSocketPassby("FullBody", 0.95f);
			})

		.AddState("OnFloorGetup")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketMany("FullBody", { "AS_em0100_480_AL_down_shock" "AS_em0100_477_AL_getup02" });
				m_fGravity = 20.f;
			})
				.AddTransition("OnFloorGetup to Idle", "Idle")
				.Predicator([this]
			{
				return  m_bDead || m_pASM->isSocketEmpty("FullBody");
			})
///////////////////////////////////////////////////////////////////////////////////////////

		//발차기
		.AddState("Attack_turn")
			.OnStart([this]
			{
				if (IsTargetFront())
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em0100_230_AL_atk_g1_turn_L");
				else
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em0100_241_AL_atk_h1_turn_R");
		
			})
			.Tick([this](_double) 
			{
				SocketLocalMove(m_pASM);

				if (m_bAttack)
					Kick_SweepSphere();
			})
			.OnExit([this]
			{
				//돌려차기를 했으면 회전한 방향으로 쳐다보게 함
				if (m_pASM->GetCurSocketAnimName() == "AS_em0100_230_AL_atk_g1_turn_L")
					AfterLocal180Turn();		
			
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Attack_turn to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})
		//장판
		.AddState("Attack_c1")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0110_209_AL_atk_c1");
			})
			.Tick([this](_double) 
			{
					//이땐 데미지 어떻게?
			})
			.AddTransition("Attack_c1 to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})
		//돌진
		.AddState("Attack_b2_Start")
			.OnStart([this]
			{
				//m_vRushDirection = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				m_bRush = true;
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0110_206_AL_atk_b2_start");
			})
			.Tick([this](_double) 
			{
				SocketLocalMove(m_pASM);

				if (m_bAttack)
				{
					Rush_Overlap();
					Rush_SweepSphere();
				}
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Attack_b2_Start to Attack_b2_Loop", "Attack_b2_Loop")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Attack_b2_Loop")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0110_207_AL_atk_b2_loop");
				m_pModelCom->Find_Animation("AS_em0110_207_AL_atk_b2_loop")->SetLooping(true);
			})
			.Tick([this](_double TimeDelta)
			{
				SocketLocalMove(m_pASM);
				Rush_Overlap();
				Rush_SweepSphere();
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Attack_b2_Loop to Attack_b2_Stop", "Attack_b2_Stop")
				.Predicator([this]
				{
					return m_bDead || !m_bRush;
				})

		.AddState("Attack_b2_Stop")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0110_208_AL_atk_b2_stop");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				AfterLocal180Turn();
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Attack_b2_Stop to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})
///////////////////////////////////////////////////////////////////////////////////////////
	
		.Build();
}

void CEM0110::BeginTick()
{
	CEnemy::BeginTick();
}

void CEM0110::Tick(_double TimeDelta)
{
	CEnemy::Tick(TimeDelta);

	//AIInstance tick
	m_pController->SetTarget(m_pTarget);

	if (m_bDead == false)
		m_pController->Tick(TimeDelta);
	else
		m_pController->Invalidate();

	//변수 업데이트
	m_bRun = m_pController->IsRun();
	m_fTurnRemain = m_pController->GetTurnRemain();
	m_vMoveAxis = m_pController->GetMoveAxis();
	m_vMoveAxis.Normalize();
	m_eInput = m_pController->GetAIInput();

	//ASM, FSM tick
	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);

	Adjust_MoveAxis(TimeDelta);
	ResetHitData();
}

void CEM0110::Late_Tick(_double TimeDelta)
{
	CEnemy::Late_Tick(TimeDelta);
}

void CEM0110::AfterPhysX()
{
	CEnemy::AfterPhysX();
	
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	GetRigidBody("Range")->Update_Tick(WorldMatrix);
	GetRigidBody("Weak")->Update_Tick(m_pModelCom->GetBoneMatrix("LeftFlower1") * WorldMatrix);
}

HRESULT CEM0110::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CEM0110::Imgui_RenderProperty()
{
	CEnemy::Imgui_RenderProperty();
	if (ImGui::CollapsingHeader("ASM"))
	{
		m_pASM->Imgui_RenderState();
	}
	
	m_pFSM->Imgui_RenderProperty();

	/*static GUIZMO_INFO tInfo;
	CImguiUtils::Render_Guizmo(&pivot, tInfo, true, true);

	if (ImGui::Button("Create_RushEffect"))
	{
		m_pRushEffect = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0110_Dash_Attack");
		m_pRushEffect->Start_AttachPivot(this, pivot, "Jaw", true, true);
	}

	if (ImGui::Button("Delete_RushEffect"))
	{
		if (m_pRushEffect != nullptr)
		{
			m_pRushEffect->SetDelete();
			m_pRushEffect = nullptr;
		}

	}*/
}

_bool CEM0110::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

void CEM0110::AfterLocal180Turn()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vPos -= XMVector3Normalize(vLook) * 10.f;
	m_pTransformCom->LookAt(vPos);
}


void CEM0110::Adjust_MoveAxis(_double TimeDelta)
{
	_float fMoveSpeed = 0.f;

	if (m_bRun)
		fMoveSpeed = 3.f;
	else
	{
		if (m_pModelCom->GetPlayAnimation() != nullptr)
		{
			fMoveSpeed = CCurveManager::GetInstance()->GetCurve("em0110_Walk")
				->GetValue(m_pModelCom->GetPlayAnimation()->GetPlayRatio());
		}
	}

	if (m_vMoveAxis.LengthSquared() > 0.f)
	{
		const _float fYaw = m_pTransformCom->GetYaw_Radian();
		_float3 vVelocity;
		XMStoreFloat3(&vVelocity, fMoveSpeed * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}

}


void CEM0110::Play_HeavbyHitAnim()
{
	if (m_eSimpleHitFrom == ESimpleAxis::NORTH)
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0100_451_AL_damage_l_F02");
	else
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0100_452_AL_damage_l_B02");
}


void CEM0110::Rush_Overlap()
{

	//Length로 Bone의 위치에서부터 바라보는 방향으로 원하는 지점까지의 거리를 지정
	_float fLength = 7.f;

	_matrix BoneMatrix = m_pModelCom->GetBoneMatrix("Target") * m_pTransformCom->Get_WorldMatrix();

	_vector vBoneVector = BoneMatrix.r[3];
	_float3 fBone = vBoneVector;

	_vector vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vMyLook = XMVectorSetY(vMyLook, 0.f);
	_float3 fDest = vMyLook;

	_float3 fFinish = { (fBone.x + fLength * fDest.x), fBone.y, (fBone.z + fLength * fDest.z) };

	physx::PxOverlapHit hitBuffer[3];
	physx::PxOverlapBuffer overlapOut(hitBuffer, 3);

	SphereOverlapParams param;
	param.fVisibleTime = 0.1f;
	param.iTargetType = CTB_STATIC;
	param.fRadius = 3.f;
	param.vPos = XMVectorSetW(fFinish, 1.f);
	param.overlapOut = &overlapOut;

	if (CGameInstance::GetInstance()->OverlapSphere(param))
	{
		m_bRush = false;
	}

}

void CEM0110::Rush_SweepSphere()
{
	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);

	//Tail4가 꼬리 중앙에 있음
	_float4x4 BoneMatrix = GetBoneMatrix("LeftFlower1") * m_pTransformCom->Get_WorldMatrix();
	_float4 vBonePos = _float4{ BoneMatrix.m[3][0], BoneMatrix.m[3][1], BoneMatrix.m[3][2], BoneMatrix.m[3][3] };

	_vector	vDir = vBonePos - m_BeforePos;

	SphereSweepParams tParams;
	tParams.fVisibleTime = 0.2f;
	tParams.iTargetType = CTB_PLAYER;
	tParams.fRadius = 3.f;
	tParams.fDistance = 1.f;
	tParams.vPos = vBonePos;
	tParams.sweepOut = &sweepOut;
	tParams.vUnitDir = vDir;

	if (CGameInstance::GetInstance()->SweepSphere(tParams))
	{
		HitTargets(sweepOut, static_cast<_int>(m_iAtkDamage * 1.2f), EAttackType::ATK_HEAVY);
	}

	m_BeforePos = vBonePos;
}

void CEM0110::Kick_SweepSphere()
{
	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);

	SphereSweepParams tParams;
	tParams.fVisibleTime = 0.2f;
	tParams.iTargetType = CTB_PLAYER;
	tParams.fRadius = 1.f;
	tParams.fDistance = 1.f;
	tParams.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	tParams.sweepOut = &sweepOut;
	tParams.vUnitDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	if (CGameInstance::GetInstance()->SweepSphere(tParams))
	{
		HitTargets(sweepOut, m_iAtkDamage * 1.3f, EAttackType::ATK_TO_AIR);
	}
}


CEM0110 * CEM0110::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEM0110* pInstance = new CEM0110(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0110");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEM0110::Clone(void * pArg)
{
	CEM0110*		pInstance = new CEM0110(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0110");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0110::Free()
{
	CEnemy::Free();
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
	Safe_Release(m_pRushEffect);
}
