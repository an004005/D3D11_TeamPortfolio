#include "stdafx.h"
#include "..\public\EM0700.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "RigidBody.h"
#include "EM0700_AnimInstance.h"
#include "EM0700_Controller.h"
#include "BulletBuilder.h"
#include "SuperSpeedTrail.h"
#include "ImguiUtils.h"

CEM0700::CEM0700(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
	m_eEnemyName = EEnemyName::EM0700;
}

CEM0700::CEM0700(const CEM0700 & rhs)
	: CEnemy(rhs)
{
}

HRESULT CEM0700::Initialize(void * pArg)
{
	Json em0700_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em0700/em0700Base.json");
	pArg = &em0700_json;

	/*m_strDeathSoundTag = "mon_5_fx_death";
	m_strImpactVoiceTag = "mon_5_impact_voice";*/

	// 배치툴에서 조절할 수 있게 하기
	{
		m_iMaxHP = 3000;
		m_iHP = 3000; // ★

		m_iAtkDamage = 50;
		iEemeyLevel = 2;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	m_eEnemyName = EEnemyName::EM0700;
	m_bHasCrushGage = true;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(180.f));
	m_pTransformCom->SetSpeed(7.f);

	m_fGravity = 0.f;
	return S_OK;
}

void CEM0700::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em700", L"Model", (CComponent**)&m_pModelCom));

	FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_SuperSpeedTrail", L"SuperSpeedTrail", (CComponent**)&m_pTrail));
	m_pTrail->SetOwnerModel(m_pModelCom);

	//Trail 초기설정
	m_pTrail->SetTrailCoolTime(0.05f);
	m_pTrail->SetTrailLife(0.3f);

	// 컨트롤러, prototype안 만들고 여기서 자체생성하기 위함
	m_pController = CEM0700_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	// ASM
	m_pASM = CEM0700_AnimInstance::Create(m_pModelCom, this);
}

void CEM0700::SetUpSound()
{
	CEnemy::SetUpSound();
}

void CEM0700::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();

	m_pModelCom->Add_EventCaller("DeadFlower", [this]
		{
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0700DeadFlower")
				->Start_NoAttach(this, false);
		});
}

void CEM0700::SetUpFSM()
{
	CEnemy::SetUpFSM();

	/*
	SPACE : Escape
	NUM_1 : Attack_a1 (돌진 공격)
	NUM_2 : Attack_a3 (원거리 공격)
	MOUSE_LB : RandomMove_L
	MOUSE_RB : RandomMove_R
	Threat : C
	*/

	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
		.OnStart([this]
			{
				m_fGravity = 0.f;
			})
		.Tick([this](_double TimeDelta)
			{
				Check_Height();
				Move_YAxis(TimeDelta);
			})
			.AddTransition("Idle to Death" , "Death")
				.Predicator([this] { return m_bDead; })

			.AddTransition("Idle to Hit_ToAir", "Hit_ToAir")
				.Predicator([this] { return m_eCurAttackType == EAttackType::ATK_TO_AIR; })
			.AddTransition("Idle to Hit_Mid_Heavy", "Hit_Mid_Heavy")
				.Predicator([this] { return
					m_eCurAttackType == EAttackType::ATK_HEAVY
					|| m_eCurAttackType == EAttackType::ATK_MIDDLE
					|| m_eCurAttackType == EAttackType::ATK_SPECIAL_END; })
			.AddTransition("Idle to Hit_Light", "Hit_Light")
				.Predicator([this] { return
					m_eCurAttackType == EAttackType::ATK_LIGHT
					|| m_eCurAttackType == EAttackType::ATK_SPECIAL_LOOP; })

			.AddTransition("Idle to Escape", "Escape")
				.Predicator([this] { return m_eInput == CController::SPACE; })

			.AddTransition("Idle to Threat", "Threat")
				.Predicator([this] { return m_eInput == CController::SHIFT; })

			.AddTransition("Idle to Rush_Start", "Rush_Start")
				.Predicator([this] { return m_eInput == CController::C; })

			.AddTransition("Idle to Shot", "Shot")
				.Predicator([this] { return m_eInput == CController::R; })
			

///////////////////////////////////////////////////////////////////////////////////////////
		.AddState("Hit_Light")
			.OnStart([this]
			{
				Play_LightHitAnim();
			})
			.Tick([this](_double)
			{
				if (m_eCurAttackType == EAttackType::ATK_LIGHT || m_eCurAttackType == EAttackType::ATK_SPECIAL_LOOP)
				{
					Play_LightHitAnim();
				}
			})
			.AddTransition("Hit_Light to Idle", "Idle")
				.Predicator([this]
			{
				return m_bDead
					|| m_pASM->isSocketPassby("FullBody", 0.95f)
					|| (m_eCurAttackType != EAttackType::ATK_LIGHT && m_eCurAttackType != EAttackType::ATK_END);
			})


			.AddState("Hit_Mid_Heavy")
				.OnStart([this]
			{
				Play_MidHitAnim();
				HeavyAttackPushStart();
			})
			.Tick([this](_double TimeDelta)
			{
				if (m_eCurAttackType == EAttackType::ATK_HEAVY
					|| m_eCurAttackType == EAttackType::ATK_MIDDLE
					|| m_eCurAttackType == EAttackType::ATK_SPECIAL_END)
				{
					HeavyAttackPushStart();
					Play_MidHitAnim();
				}

				_float fPower;
				if (m_HeavyAttackPushTimeline.Tick(TimeDelta, fPower))
				{
					_float3 vVelocity = { m_vPushVelocity.x, m_vPushVelocity.y, m_vPushVelocity.z };
					m_pTransformCom->MoveVelocity(TimeDelta, vVelocity * fPower);
					//m_pTransformCom->MoveVelocity(TimeDelta, m_vPushVelocity * fPower);
				}
			})
			.AddTransition("Hit_Mid_Heavy to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead
						|| m_pASM->isSocketPassby("FullBody", 0.95f)
						|| m_eCurAttackType == EAttackType::ATK_TO_AIR
						|| m_eCurAttackType == EAttackType::ATK_SPECIAL_LOOP;
				})
		.AddState("Death")
				.OnStart([this]
				{
					m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_474_AL_dead_down02");
				})

		.AddState("Hit_ToAir")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_432_AL_blow_start_F");
				Check_Height();
				m_fGravity = 20.f;
				m_fYSpeed = sqrtf((2.5f - m_fHeight) * m_fGravity * 2.f);
			})
			.Tick([this](_double)
			{
				// 공중 추가타로 살짝 올라가는 애님
				m_bHitAir = true;
				if (m_bAirToDown)
				{
					m_fYSpeed = -20.f;
				}
				else if (m_eCurAttackType != EAttackType::ATK_END)
				{
					m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_455_AL_rise_start");
				}
			})
			.OnExit([this]
			{
				m_bHitAir = false;
			})
			.AddTransition("Hit_ToAir to OnFloorGetup", "OnFloorGetup")
				.Predicator([this]
				{
					return !m_bPreOnFloor && m_bOnFloor && m_bHitAir;
				})
		.AddState("OnFloorGetup")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketMany("FullBody", { "AS_em0700_433_AL_blow_landing_F", "AS_em0700_427_AL_getup" });
				m_fGravity = 20.f;
			})
			.AddTransition("OnFloorGetup to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketEmpty("FullBody");
				})

///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Escape")
			.OnStart([this]
			{
				SelectEscapeAnim_Overlap();
			})
			.Tick([this](_double)
			{	
				SocketLocalMove(m_pASM);
				m_pTrail->SetActive(true);
			})
			.OnExit([this]
			{
					m_pTrail->SetActive(false);
			})
			.AddTransition("Escape to Idle", "Idle")
				.Predicator([this]
			{
				return m_bDead || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketPassby("FullBody", 0.95f);
			})

///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Threat")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_160_AL_threat");
			})
			.AddTransition("Threat to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

///////////////////////////////////////////////////////////////////////////////////////////

		//시작과 끝은 로컬이 있음
		.AddState("Rush_Start")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_201_AL_atk_a1_start");
				ClearDamagedTarget();
			})
			.Tick([this](_double TimeDelta)
			{
				SocketLocalMove(m_pASM);
			})
			.AddTransition("Rush_Start to Rush_Loop", "Rush_Loop")
				.Predicator([this]
			{
				return  m_pASM->isSocketPassby("FullBody", 0.95f);
			})
			.AddTransition("Rush_Start to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || (m_eCurAttackType != EAttackType::ATK_LIGHT && m_eCurAttackType != EAttackType::ATK_END);
				})

		.AddState("Rush_Loop")
			.OnStart([this]
			{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_202_AL_atk_a1_loop");
					m_pModelCom->Find_Animation("AS_em0700_202_AL_atk_a1_loop")->SetLooping(true);
					m_BeforePos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					m_fRushTime = 1.5f;
					m_pTransformCom->SetSpeed(20.f);

					_matrix RushEffectPivotMatrix = CImguiUtils::CreateMatrixFromImGuizmoData(
						{ 0.08f, -0.112f, -1.617f },
						{ 180.f, 0.f, 180.f, },
						{ 15.f, 15.f, 15.f });

					m_pRushEffect = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0750_Dash_Attack");
					m_pRushEffect->Start_AttachPivot(this, RushEffectPivotMatrix, "Target", true, true);
					Safe_AddRef(m_pRushEffect);

			})
			.Tick([this](_double TimeDelta)
			{
				
					m_pTransformCom->Go_Straight(TimeDelta);
					m_fRushTime -= (_float)TimeDelta;

					Rush_StaticCheckSweep();
					Rush_SweepSphere();
			
			})

			.AddTransition("Rush_Loop to Rush_End", "Rush_End")
				.Predicator([this]
				{
						return m_bDead || m_fRushTime <= 0.f;
				})

		.AddState("Rush_End")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_203_AL_atk_a1_end");

				//이펙트 삭제
				if (m_pRushEffect != nullptr)
				{
					m_pRushEffect->SetDelete();
					Safe_Release(m_pRushEffect);
					m_pRushEffect = nullptr;
				}
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
			.AddTransition("Rush_End to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Shot")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0710_207_AL_atk_a3");
			})
			.OnExit([this] 
			{
					Create_Bullet();
			})
				.AddTransition("AS_em0710_207_AL_atk_a3 to Idle", "Idle")
				.Predicator([this]
			{
				return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
			})


				///////////////////////////////////////////////////////////////////////////////////////////

		.Build();
}

void CEM0700::SetUpUI()
{
	//HP UI
	_float4x4 UI_PivotMatrix = Matrix(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.413f, 0.0f, 1.0f
	);

	m_UI_PivotMatrixes[ENEMY_INFOBAR] = UI_PivotMatrix;

	//FindEye
	UI_PivotMatrix = Matrix(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.413f, 0.0f, 1.0f
	);

	m_UI_PivotMatrixes[ENEMY_FINDEYES] = UI_PivotMatrix;
}

void CEM0700::BeginTick()
{
	CEnemy::BeginTick();
}

void CEM0700::Tick(_double TimeDelta)
{
	CEnemy::Tick(TimeDelta);

	//AIInstance tick
	m_pController->SetTarget(m_pTarget);

	if (m_bDead == false)
		m_pController->Tick(TimeDelta);
	else
		m_pController->Invalidate();

	//변수 업데이트
	m_vMoveAxis = m_pController->GetMoveAxis();
	m_vMoveAxis.Normalize();
	m_eInput = m_pController->GetAIInput();

	//ASM, FSM tick
	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);

	const _float fMoveSpeed = 2.f;

	if (m_vMoveAxis.LengthSquared() > 0.f)
	{
		const _float fYaw = m_pTransformCom->GetYaw_Radian();
		_float3 vVelocity;
		XMStoreFloat3(&vVelocity, fMoveSpeed * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}

	if (m_pTrail != nullptr)
		m_pTrail->Tick(TimeDelta);

	// Tick의 제일 마지막에서 실행한다.
	ResetHitData();
}

void CEM0700::Late_Tick(_double TimeDelta)
{
	CEnemy::Late_Tick(TimeDelta);
}

void CEM0700::AfterPhysX()
{
	CEnemy::AfterPhysX();
}

HRESULT CEM0700::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CEM0700::Imgui_RenderProperty()
{
	CEnemy::Imgui_RenderProperty();
	if (ImGui::CollapsingHeader("ASM"))
	{
		m_pASM->Imgui_RenderState();
	}
	m_pFSM->Imgui_RenderProperty();

	//ImGui::DragFloat("TrailCool", &trailcoll);
	//ImGui::DragFloat("TrailLife", &traillife);

	//m_pTrail->SetTrailCoolTime(trailcoll);
	//m_pTrail->SetTrailLife(traillife);

	//	static _bool tt = false;
	//ImGui::Checkbox("Modify Pivot", &tt);

	//if (tt)
	//{
	//	static GUIZMO_INFO tInfo;
	//	CImguiUtils::Render_Guizmo(&pivot, tInfo, true, true);

	//	if (ImGui::Button("Create_Effect"))
	//	{
	//		 CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0750_Dash_Attack")
	//		->Start_AttachPivot(this, pivot, "Target", true, true);
	//
	//	}
	//}
}

_bool CEM0700::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

void CEM0700::Play_LightHitAnim()
{
	if (m_eSimpleHitFrom == ESimpleAxis::NORTH)
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_401_AL_damage_l_F");
	else
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_402_AL_damage_l_B");
}

void CEM0700::Play_MidHitAnim()
{
	switch (m_eHitFrom)
	{
	case EBaseAxis::NORTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_411_AL_damage_m_F");
		break;
	case EBaseAxis::EAST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_414_AL_damage_m_R");
		break;
	case EBaseAxis::SOUTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_412_AL_damage_m_B");
		break;
	case EBaseAxis::WEST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_413_AL_damage_m_L");
		break;
	case EBaseAxis::AXIS_END:
		FALLTHROUGH;
	default:
		NODEFAULT;
	}
}

void CEM0700::HeavyAttackPushStart()
{
	if (m_eCurAttackType == EAttackType::ATK_MIDDLE || m_eCurAttackType == EAttackType::ATK_HEAVY || m_eCurAttackType == EAttackType::ATK_SPECIAL_END)
	{
		m_HeavyAttackPushTimeline.PlayFromStart();
		m_vPushVelocity = CClientUtils::GetDirFromAxis(m_eHitFrom);
		m_vPushVelocity *= -4.f; // 공격 온 방향의 반대로 이동

		const _float fYaw = m_pTransformCom->GetYaw_Radian();
		m_vPushVelocity = XMVector3TransformNormal(m_vPushVelocity, XMMatrixRotationY(fYaw));
	}
}

void CEM0700::SelectRandomMoveAnim()
{
	_vector vTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	_float fAngle = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vMyRight), XMVector3Normalize(vTargetPos - vMyPos)));

	//오른쪽
	if (fAngle > 0)
		m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_122_AL_randommove_R");
	else
		m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_121_AL_randommove_L");
}

void CEM0700::Rush_StaticCheckSweep()
{
	physx::PxSweepHit hitBuffer[1];
	physx::PxSweepBuffer sweepOut(hitBuffer, 1);
	SphereSweepParams tParam;

	tParam.sweepOut = &sweepOut;
	tParam.fRadius = 1.f;
	tParam.fVisibleTime = .1f;
	tParam.iTargetType = CTB_STATIC;
	tParam.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION); // 공중에 떠있으니 transform써도 될 듯
	tParam.vUnitDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	tParam.fDistance = 4.f;

	if (CGameInstance::GetInstance()->SweepSphere(tParam))
	{
		m_fRushTime = -1.f;
	}
}

void CEM0700::Rush_SweepSphere()
{
	_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	SphereSweepParams tParams;

	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);
	tParams.sweepOut = &sweepOut;
	tParams.fRadius = 1.5;
	tParams.fVisibleTime = .1f;
	tParams.iTargetType = CTB_PLAYER;
	tParams.vPos = m_BeforePos;
	tParams.vUnitDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	tParams.fDistance = XMVectorGetX(XMVector3LengthEst(vPos - m_BeforePos));

	if (CGameInstance::GetInstance()->SweepSphere(tParams))
	{
		HitTargets(sweepOut, m_iAtkDamage * 2.f, EAttackType::ATK_HEAVY);
	}

	m_BeforePos = vPos;
}

void CEM0700::Create_Bullet()
{
	DAMAGE_PARAM eDamageParam;
	eDamageParam.eAttackType = EAttackType::ATK_LIGHT;
	eDamageParam.eDeBuff = EDeBuffType::DEBUFF_END;
	eDamageParam.iDamage = 20;

	_matrix BoneMtx = m_pModelCom->GetBoneMatrix("Tail7") * m_pTransformCom->Get_WorldMatrix();
	_vector BonePos = BoneMtx.r[3];

	CBulletBuilder()
		.CreateBullet()
			.Set_Owner(this)
			.Set_InitBulletEffect({ L"em0650_Bullet_Birth" , L"Em0650_Bullet_Loop" })
			.Set_InitBulletParticle(L"em0650_Bullet_Loop")
			.Set_ShootSpped(8.f)
			.Set_Life(4.f)
			.Set_DamageParam(eDamageParam)
			.Set_DeadBulletEffect({ L"em0650_Bullet_Dead" })
			.Set_DeadBulletParticle(L"em0650_Bullet_Dead_Particle")
			.Set_Position(BonePos)
			.Set_LookAt(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION))
		.Build();
}

void CEM0700::Check_Height()
{
	_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	physx::PxRaycastHit hitBuffer[1];
	physx::PxRaycastBuffer t(hitBuffer, 1);

	RayCastParams params;
	params.rayOut = &t;
	params.vOrigin = vPos;
	params.vOrigin.y += 2.f;// 발 보다 살짝 위에서 레이 케스트(바닥보다 무조건 높은 위치에서 쏘기 위해서 더해준다.
	params.vDir = -_float4::UnitY;
	params.fDistance = 1000.f;
	params.iTargetType = CTB_STATIC;
	params.bSingle = true;

	if (CGameInstance::GetInstance()->RayCast(params))
	{
		for (int i = 0; i < t.getNbAnyHits(); ++i)
		{
			auto p = t.getAnyHit(i);
			const _float4 vFloorPos{ p.position.x, p.position.y, p.position.z, 1.f };
			m_fHeight = XMVectorGetX(XMVector3LengthEst(vPos - vFloorPos));
		}
	}
}

void CEM0700::Move_YAxis(_double TimeDelta)
{
	if (m_fHeight < m_fMaxHeight)
	{
		_float3 vVelocity;
		vVelocity.y = 4.f;
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}
	else if (m_fHeight > m_fMaxHeight + 0.5f)
	{
		_float3 vVelocity;
		vVelocity.y = -4.f;
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}
}

void CEM0700::AfterLocal180Turn()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vPos -= XMVector3Normalize(vLook) * 10.f;
	m_pTransformCom->LookAt(vPos);
}



void CEM0700::SelectEscapeAnim_Overlap()
{
	//뒤에 벽이 있는지 체크
	_float fLength = -5.f;

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
		m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_120_AL_escape");
	else
		SelectRandomMoveAnim();
}



CEM0700 * CEM0700::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEM0700* pInstance = new CEM0700(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0700");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEM0700::Clone(void * pArg)
{
	CEM0700*		pInstance = new CEM0700(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0700");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0700::Free()
{
	CEnemy::Free();
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
	Safe_Release(m_pTrail);

	if (m_bCloned)
	{
		if (m_pRushEffect != nullptr)
		{
			m_pRushEffect->SetDelete();
			Safe_Release(m_pRushEffect);
			m_pRushEffect = nullptr;
		}
	}
}
