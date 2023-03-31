#include "stdafx.h"
#include "..\public\EM1100.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "RigidBody.h"
#include "EM1100_AnimInstance.h"
#include "EM1100_Controller.h"

CEM1100::CEM1100(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
	m_eEnemyName = EEnemyName::EM1100;
}

CEM1100::CEM1100(const CEM1100 & rhs)
	: CEnemy(rhs)
{
}

HRESULT CEM1100::Initialize(void * pArg)
{
	Json em1100_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em1100/em1100Base.json");
	pArg = &em1100_json;

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

	m_eEnemyName = EEnemyName::EM1100;
	m_bHasCrushGauge = true;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(120.f));

	m_fGravity = 20.f;
	return S_OK;
}

void CEM1100::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em1100", L"Model", (CComponent**)&m_pModelCom));

	// 범위 충돌체(플레이어가 몬스터 위로 못 올라가게한다.)
	/*Json FlowerLegRangeCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/FlowerLeg/FlowerLegRange.json");*/

	FAILED_CHECK(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("RangeColl"),
		(CComponent**)&m_pRange, pArg));

	FAILED_CHECK(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("HeadColl"),
		(CComponent**)&m_pHead))
		

	// 컨트롤러, prototype안 만들고 여기서 자체생성하기 위함
	m_pController = CEM1100_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	// ASM
	m_pASM = CEM1100_AnimInstance::Create(m_pModelCom, this);
}

void CEM1100::SetUpSound()
{
	CEnemy::SetUpSound();
}

void CEM1100::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();
	
	//진짜 Run해야하는 타이밍
	m_pModelCom->Add_EventCaller("Run_Start", [this]
	{
		m_bRun_Start = true;
	});


	m_pModelCom->Add_EventCaller("Dodge_Start", [this]
	{
		Dodge_VelocityCalc();
	});


	m_pModelCom->Add_EventCaller("Rush_Start", [this]
	{
		m_bRush = true;
		ClearDamagedTarget();
	});

	m_pModelCom->Add_EventCaller("Shot", [this]
	{
		//전기볼 생성
	});

	//주먹으로 땅 찍을때 한번만 실행
	m_pModelCom->Add_EventCaller("Stamp", [this]
	{
		ClearDamagedTarget();
		Stamp_Overlap();
	});

	m_pModelCom->Add_EventCaller("TailSwing_Start", [this]
	{
		ClearDamagedTarget();
		m_bTailSwing = true;
	});

	m_pModelCom->Add_EventCaller("TailSwing_End", [this]
	{
		m_bTailSwing = false;;
	});

	m_pModelCom->Add_EventCaller("DeadFlower", [this]
		{
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em1100DeadFlower")
				->Start_NoAttach(this, false);
		});
}

void CEM1100::SetUpFSM()
{
	CEnemy::SetUpFSM();

	/*
	W : waterattack
	E : Electricball
	S : Stamp
	R : Rush
	T : TailSwing

	SHIFT : Dodge
	SPACE : DodgeTurn
	*/

	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.OnStart([this]
			{
				m_fGravity = 20.f;
			})
			.AddTransition("Idle to Death" , "Death")
				.Predicator([this] { return m_bDead; })

			.AddTransition("Idle to Hit_Mid_Heavy", "Hit_Mid_Heavy")
				.Predicator([this] { return
					m_eCurAttackType == EAttackType::ATK_HEAVY
					|| m_eCurAttackType == EAttackType::ATK_MIDDLE
					|| m_eCurAttackType == EAttackType::ATK_SPECIAL_END; })
	
			.AddTransition("Idle to Dodge_Start", "Dodge_Start")
				.Predicator([this] { return m_eInput == CController::SHIFT; })

			.AddTransition("Idle to WaterAttack_Omen", "WaterAttack_Omen")
				.Predicator([this] { return m_eInput == CController::W; })

			.AddTransition("Idle to ElectricBall_Omen", "ElectricBall_Omen")
				.Predicator([this] { return m_eInput == CController::E; })

			.AddTransition("Idle to Stamp_Omen", "Stamp_Omen")
				.Predicator([this] { return m_eInput == CController::S; })

			.AddTransition("Idle to Rush_Omen", "Rush_Omen")
				.Predicator([this] { return m_eInput == CController::R; })

			.AddTransition("Idle to TailSwing", "TailSwing")
				.Predicator([this] { return m_eInput == CController::T; })
			
///////////////////////////////////////////////////////////////////////////////////////////
	
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

		//로컬없음, start에서 이벤트로 점프시작 알려줌
		.AddState("Dodge_Start")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_135_AL_dodge_B_start");
				m_pController->SetActive(false);
			})
			.Tick([this](_double TimeDelta)
			{
				if (m_bDodge)
				{
					m_pTransformCom->MoveVelocity(TimeDelta, m_vOnJumpMoveVelocity);
				}
			})
			.OnExit([this] 
			{
				m_pController->SetActive(true);
				m_bDodge = false;
				m_fGravity = 20.f;
			})
			.AddTransition("Dodge_Start to Dodge_Stop", "Dodge_Stop")
				.Predicator([this]
				{
					return  m_bDead || (m_bDodge && m_bOnFloor);
				})

				//착지. 움직임x
		.AddState("Dodge_Stop")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_136_AL_dodge_B_stop");
				})
	
				.AddTransition("Dodge_Stop to Idle", "Idle")
					.Predicator([this]
					{
						return  m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

///////////////////////////////////////////////////////////////////////////////////////////

		//셋다 로컬, 이벤트 없이 start에서 데미지 처리 해주면됨
		.AddState("WaterAttack_Omen")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_215_AL_atk_a5_drainage_omen");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("WaterAttack_Omen to WaterAttack_Start", "WaterAttack_Start")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("WaterAttack_Start")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_215_AL_atk_a5_drainage_start");
				})
				.Tick([this](_double)
				{
					SocketLocalMove(m_pASM);
				})
				.OnExit([this]
				{
					m_pASM->ClearSocketAnim("FullBody", 0.f);
				})
				.AddTransition("WaterAttack_Start to WaterAttack_End", "WaterAttack_End")
					.Predicator([this]
					{
						return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

			.AddState("WaterAttack_End")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_215_AL_atk_a5_drainage_end");
				})
				.Tick([this](_double)
				{
					SocketLocalMove(m_pASM);
				})
				.OnExit([this]
				{
					m_pASM->ClearSocketAnim("FullBody", 0.f);
				})
				.AddTransition("WaterAttack_End to Idle", "Idle")
					.Predicator([this]
					{
						return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

		//셋다 로컬있음 , 전기볼이 플레이어를 쫓아가야함, 이벤트로 전기볼 생성
		.AddState("ElectricBall_Omen")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_208_AL_atk_a3_guidanceshot_omen");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("ElectricBall_Omen to ElectricBall_Start", "ElectricBall_Start")
				.Predicator([this]
			{
				return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
			})

		.AddState("ElectricBall_Start")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_208_AL_atk_a3_guidanceshot_start");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("ElectricBall_Start to ElectricBall_End", "ElectricBall_End")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("ElectricBall_End")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_208_AL_atk_a3_guidanceshot_end");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("ElectricBall_End to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})


		//셋다 로컬이 조금씩 있음, Start에 이벤트로 데미지 한번만 주게 설정
		.AddState("Stamp_Omen")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_202_AL_atk_a1_bringdown_omen");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Stamp_Omen to Stamp_Start", "Stamp_Start")
				.Predicator([this]
			{
				return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
			})

		.AddState("Stamp_Start")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_202_AL_atk_a1_bringdown_start");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Stamp_Start to Stamp_End", "Stamp_End")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Stamp_End")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_202_AL_atk_a1_bringdown_end");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Stamp_End to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})


		//start와 end에 로컬있음, start에서 돌진할때 이벤트 알려줌
		.AddState("Rush_Omen")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1120_211_AL_atk_a4_sliding_omen");
			})
			.Tick([this](_double TimeDelta)
			{
				m_pTransformCom->LookAt_Smooth(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION), TimeDelta);
			})

			.AddTransition("Rush_Omen to Rush_Start", "Rush_Start")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Rush_Start")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1120_211_AL_atk_a4_sliding_start");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);

				if (m_bRush)
				{
					Rush_SweepCapsule();
				}
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
				m_bRush = false;
			})
			.AddTransition("Rush_Start to Rush_End", "Rush_End")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Rush_End")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1120_211_AL_atk_a4_sliding_end");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Rush_End to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		//꼬리치기 후 도는모션까지 로컬 있음 ㅎㅎ, 시작과 끝 이벤트로 데미지 처리
		.AddState("TailSwing")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_204_AL_atk_a2_tailall");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);

				if (m_bTailSwing)
				{
					TailSwing_SweepSphere();
				}
			})
			.OnExit([this]
			{
				AfterLocal180Turn();
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("TailSwing to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Threat")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_160_AL_threat");
			})
			.AddTransition("Threat to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
			})


				///////////////////////////////////////////////////////////////////////////////////////////

		.Build();
}

void CEM1100::BeginTick()
{
	CEnemy::BeginTick();
}

void CEM1100::Tick(_double TimeDelta)
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
	m_bRun = m_pController->IsRun();

	//Run이 끝날때  RunStart를 초기화 해줄 방법이 없어서 Run이 아니면 false로 만듦
	if (m_bRun == false)
		m_bRun_Start = false;

	//ASM, FSM tick
	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);


	if (m_vMoveAxis.LengthSquared() > 0.f)
	{
		_float3 vVelocity;

		//뛸때는 방향을 보면서 뛰어야하고 걸을때는 안보고 걸어야 함
		if (m_bRun)
		{
			const _float fYaw = m_pTransformCom->GetYaw_Radian();
			XMStoreFloat3(&vVelocity, 8.f /*MoveSpeed*/ * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
		}
		else
			XMStoreFloat3(&vVelocity, 2.f /*MoveSpeed*/ * XMVector3Normalize(m_vMoveAxis));

		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}

	// Tick의 제일 마지막에서 실행한다.
	ResetHitData();
}

void CEM1100::Late_Tick(_double TimeDelta)
{
	CEnemy::Late_Tick(TimeDelta);
}

void CEM1100::AfterPhysX()
{
	CEnemy::AfterPhysX();
}

HRESULT CEM1100::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CEM1100::Imgui_RenderProperty()
{
	CEnemy::Imgui_RenderProperty();
	if (ImGui::CollapsingHeader("ASM"))
	{
		m_pASM->Imgui_RenderState();
	}
	m_pFSM->Imgui_RenderProperty();
}

_bool CEM1100::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

//void CEM1100::Play_LightHitAnim()
//{
//	if (m_eSimpleHitFrom == ESimpleAxis::NORTH)
//		m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_401_AL_damage_l_F");
//	else
//		m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_402_AL_damage_l_B");
//}

void CEM1100::Play_MidHitAnim()
{
	switch (m_eHitFrom)
	{
	case EBaseAxis::NORTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em1100_411_AL_damage_m_F");
		break;
	case EBaseAxis::EAST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em1100_414_AL_damage_m_R");
		break;
	case EBaseAxis::SOUTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em1100_412_AL_damage_m_B");
		break;
	case EBaseAxis::WEST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em1100_413_AL_damage_m_L");
		break;
	case EBaseAxis::AXIS_END:
		FALLTHROUGH;
	default:
		NODEFAULT;
	}
}

void CEM1100::HeavyAttackPushStart()
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

_bool CEM1100::IsTargetFront()
{
	_vector vTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_float fAngle = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vMyLook), XMVector3Normalize(vTargetPos - vMyPos)));

	//0~ 90도 사이(정면)
	if (fAngle > 0)
		return true;
	else
		return false;

}

void CEM1100::Rush_SweepCapsule()
{
	_float4x4 HeadMatrix = m_pHead->GetPxWorldMatrix();
	_float4 vHeadPos = _float4{ HeadMatrix.m[3][0], HeadMatrix.m[3][1], HeadMatrix.m[3][2], HeadMatrix.m[3][3] };

	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);

	PxCapsuleSweepParams tParams;
	tParams.sweepOut = &sweepOut;
	tParams.CapsuleGeo = m_pHead->Get_CapsuleGeometry();
	tParams.pxTransform = m_pHead->Get_PxTransform();

	_float4	vDir = vHeadPos - m_BeforePos;
	tParams.vUnitDir = _float3(vDir.x, vDir.y, vDir.z);
	tParams.fDistance = tParams.vUnitDir.Length();
	tParams.iTargetType = CTB_PLAYER;
	tParams.fVisibleTime = 0.2f;

	if (CGameInstance::GetInstance()->PxSweepCapsule(tParams))
	{
		HitTargets(sweepOut, static_cast<_int>(m_iAtkDamage * 1.5f), EAttackType::ATK_TO_AIR);
	}

	m_BeforePos = vHeadPos;
}

void CEM1100::TailSwing_SweepSphere()
{
	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);

	//Tail4가 꼬리 중앙에 있음
	_float4x4 TailMatrix = GetBoneMatrix("Tail4") * m_pTransformCom->Get_WorldMatrix();
	_float4 vTailPos = _float4{ TailMatrix.m[3][0], TailMatrix.m[3][1], TailMatrix.m[3][2], TailMatrix.m[3][3]};

	_vector	vTailDir = vTailPos - m_BeforePos;

	SphereSweepParams tParams;
	tParams.fVisibleTime = 0.2f;
	tParams.iTargetType = CTB_PLAYER;
	tParams.fRadius = 2.f;
	tParams.fDistance = 1.f;
	tParams.vPos = vTailPos;
	tParams.sweepOut = &sweepOut;
	tParams.vUnitDir = vTailDir;

	if (CGameInstance::GetInstance()->SweepSphere(tParams))
	{
		HitTargets(sweepOut, static_cast<_int>(m_iAtkDamage * 1.2f), EAttackType::ATK_LIGHT);
	}

	m_BeforePos = vTailPos;
}

void CEM1100::Stamp_Overlap()
{	
	_matrix BoneMatrix = m_pModelCom->GetBoneMatrix("Target") * m_pTransformCom->Get_WorldMatrix();

	_vector vBoneVector = BoneMatrix.r[3];
	_float3 fBone = vBoneVector;

	physx::PxOverlapHit hitBuffer[3];
	physx::PxOverlapBuffer overlapOut(hitBuffer, 3);

	SphereOverlapParams param;
	param.fVisibleTime = 0.1f;
	param.iTargetType = CTB_PLAYER;
	param.fRadius = 3.f;
	param.vPos = XMVectorSetW(fBone, 1.f);
	param.overlapOut = &overlapOut;

	if (CGameInstance::GetInstance()->OverlapSphere(param))
	{
		HitTargets(overlapOut, static_cast<_int>(m_iAtkDamage * 1.5f), EAttackType::ATK_DOWN);
	}
}

void CEM1100::Dodge_VelocityCalc()
{
	m_bDodge = true;
	m_fGravity = 100.f;
	m_fYSpeed = 10.f;

	const _float fJumpMoveTime = (2 * m_fYSpeed) / m_fGravity;

	const _float fDistance = 8.f;
	const _vector vDirection = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) - m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);

	const _float fYaw = m_pTransformCom->GetYaw_Radian();
	m_vOnJumpMoveVelocity = XMVector3TransformNormal(XMVector3Normalize(vDirection) * (fDistance / fJumpMoveTime), XMMatrixRotationY(fYaw));
}

void CEM1100::AfterLocal180Turn()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vPos -= XMVector3Normalize(vLook) * 20.f;
	m_pTransformCom->LookAt(vPos);
}


CEM1100 * CEM1100::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEM1100* pInstance = new CEM1100(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM1100");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEM1100::Clone(void * pArg)
{
	CEM1100*		pInstance = new CEM1100(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM1100");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM1100::Free()
{
	CEnemy::Free();
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
	Safe_Release(m_pRange);
	Safe_Release(m_pHead);
}
