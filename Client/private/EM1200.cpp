#include "stdafx.h"
#include "..\public\EM1200.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "RigidBody.h"
#include "EM1200_AnimInstance.h"
#include "EM1200_Controller.h"
CEM1200::CEM1200(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
	m_eEnemyName = EEnemyName::EM1200;
}

CEM1200::CEM1200(const CEM1200 & rhs)
	: CEnemy(rhs)
{
	m_bSpawnEffect = false;
}

HRESULT CEM1200::Initialize(void * pArg)
{
	Json em1200_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em1200/em1200Base.json");
	pArg = &em1200_json;

	// 배치툴에서 조절할 수 있게 하기
	{
		m_iMaxHP = 3000;
		m_iHP = 3000; // ★

		m_iAtkDamage = 50;
		iEemeyLevel = 2;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	m_eEnemyName = EEnemyName::EM1200;
	m_bHasCrushGauge = true;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(90.f));

	m_fGravity = 20.f;
	return S_OK;
}

void CEM1200::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em1200", L"Model", (CComponent**)&m_pModelCom));

	// 범위 충돌체(플레이어가 몬스터 위로 못 올라가게한다.)
	//Json RangeCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em1200/em1200Trunk.json");
	Json WeakCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em1200/em1200Weak.json");
	Json TrunkCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em1200/em1200Trunk.json");
	Json LegsCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em1200/em1200Legs.json");

	Add_RigidBody("Range");
	Add_RigidBody("Trunk", &TrunkCol);
	Add_RigidBody("Weak", &WeakCol);
	Add_RigidBody("Legs", &LegsCol);

	// 컨트롤러, prototype안 만들고 여기서 자체생성하기 위함
	m_pController = CEM1200_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	// ASM
	m_pASM = CEM1200_AnimInstance::Create(m_pModelCom, this);
}

void CEM1200::SetUpSound()
{
	CEnemy::SetUpSound();
}

void CEM1200::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();
	
	m_pModelCom->Add_EventCaller("Fall", [this]
	{
		ClearDamagedTarget();
		Fall_Overlap();
	});

	m_pModelCom->Add_EventCaller("Stamp", [this]
	{
		ClearDamagedTarget();
		Stamp_Overlap();
	});

	m_pModelCom->Add_EventCaller("Swing_R_Start", [this]
	{
		ClearDamagedTarget();
		m_bAttack = true;
	});

	m_pModelCom->Add_EventCaller("Swing_R_End", [this]
	{
		m_bAttack = false;
	});

	m_pModelCom->Add_EventCaller("Swing_L_Start", [this]
	{
		ClearDamagedTarget();
		m_bAttack = true;
	});

	m_pModelCom->Add_EventCaller("Swing_L_End", [this]
	{
		m_bAttack = false;
	});

	m_pModelCom->Add_EventCaller("Rush_Start", [this]
	{
		ClearDamagedTarget();
		m_bAttack = false;
	});

	m_pModelCom->Add_EventCaller("Rush_End", [this]
	{
		m_bAttack = false;
	});

	//Cable 공격은 loop때 처리
}

void CEM1200::SetUpFSM()
{
	CEnemy::SetUpFSM();

	/*
	F : Fall
	NUM_1 : Shout1
	NUM_2 : Shout2
	S : Stamp
	MOUSE_LB : Swing_L
	MOUSE_RB : Swing_R
	R : Rush
	C : Cable

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
	
			.AddTransition("Idle to Hit_Light", "Hit_Light")
				.Predicator([this] { return
					m_eCurAttackType == EAttackType::ATK_LIGHT
					|| m_eCurAttackType == EAttackType::ATK_SPECIAL_LOOP; })

			.AddTransition("Idle to Fall", "Fall")
				.Predicator([this] { return m_eInput == CController::EHandleInput::F; })

			.AddTransition("Idle to Shout1_Start", "Shout1_Start")
				.Predicator([this] { return m_eInput == CController::EHandleInput::NUM_1; })

			.AddTransition("Idle to Shout2_Start", "Shout2_Start")
				.Predicator([this] { return m_eInput == CController::EHandleInput::NUM_2; })

			.AddTransition("Idle to Stamp_Start", "Stamp_Start")
				.Predicator([this] { return m_eInput == CController::EHandleInput::S; })

			.AddTransition("Idle to Swing_L", "Swing_L")
				.Predicator([this] { return m_eInput == CController::EHandleInput::MOUSE_LB; })

			.AddTransition("Idle to Swing_R", "Swing_R")
				.Predicator([this] { return m_eInput == CController::EHandleInput::MOUSE_RB; })

			.AddTransition("Idle to Rush_Start", "Rush_Start")
				.Predicator([this] { return m_eInput == CController::EHandleInput::R; })

			.AddTransition("Idle to Cable_Start", "Cable_Start")
				.Predicator([this] { return m_eInput == CController::EHandleInput::C; })


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


		.AddState("Hit_Light")
			.OnStart([this]
			{
				//FSM의 상태가 변경 되면, 현 상태의 Exit과 변경될 상태의 Start를 실행하고 종료,
				// Tick은 상태 변경 후 다음 틱 부터 실행된다.
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


		.AddState("Death")
				.OnStart([this]
				{
					//이친구는 안죽음...
					//m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_474_AL_dead_down02");
				})


		.AddState("OnFloorGetup")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketMany("FullBody", { "AS_em1200_426_AL_down2", "AS_em1200_427_AL_getup2" });
				m_fGravity = 20.f;
			})
			.AddTransition("OnFloorGetup to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketEmpty("FullBody");
				})

///////////////////////////////////////////////////////////////////////////////////////////

		
		.AddState("Fall")
			.OnStart([this]
			{
					m_pASM->AttachAnimSocketMany("FullBody", { "AS_em1200_200_AL_atk_a1_fall", "AS_em1200_423_AL_getup1" });
			})
			.Tick([this](_double TimeDelta)
			{
				SocketLocalMove(m_pASM);
			})
			.AddTransition("Fall to Idle", "Idle")
				.Predicator([this]
				{
					return  m_bDead || m_pASM->isSocketEmpty("FullBody");
				})



		//Loop때만 데미지 처리
		.AddState("Shout1_Start")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_205_AL_atk_a2_shout1_start");
				})
				.Tick([this](_double)
				{
					SocketLocalMove(m_pASM);
				})
				.AddTransition("Shout1_Start to Shout1_Loop", "Shout1_Loop")
					.Predicator([this]
					{
						return  m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

		.AddState("Shout1_Loop")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_206_AL_atk_a2_shout1_loop");
					m_pModelCom->Find_Animation("AS_em1200_206_AL_atk_a2_shout1_loop")->SetLooping(true);
					ClearDamagedTarget();

					m_dLoopTime = 0.0;
					m_dLoopTick = 0.8;
				})
				.Tick([this](_double TimeDelta)
				{
					//여기서 데미지 처리
					m_dLoopTime += TimeDelta;

					if (m_dLoopTime >= m_dLoopTick)
					{
						ClearDamagedTarget();
						m_dLoopTick += 0.8;               
					}

					Shout1_Overlap();
				})
				.AddTransition("Shout1_Loop to Shout1_End", "Shout1_End")
					.Predicator([this]
					{
						return  m_bDead || m_dLoopTime >= 3.0;
					})

			.AddState("Shout1_End")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_207_AL_atk_a2_shout1_end");
				})
				.Tick([this](_double)
				{
					SocketLocalMove(m_pASM);
				})
	
				.AddTransition("Shout1_End to Idle", "Idle")
					.Predicator([this]
					{
						return  m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})


		//Loop때만 데미지 처리
			.AddState("Shout2_Start")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_224_AL_atk_a8_shout2_start");
					ClearDamagedTarget();
				})
				.Tick([this](_double)
				{
					SocketLocalMove(m_pASM);
				})
				.AddTransition("Shout2_Start to Shout2_Loop", "Shout2_Loop")
					.Predicator([this]
					{
						return  m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

			.AddState("Shout2_Loop")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_225_AL_atk_a8_shout2_loop");
					m_pModelCom->Find_Animation("AS_em1200_225_AL_atk_a8_shout2_loop")->SetLooping(true);
					ClearDamagedTarget();

					m_dLoopTime = 0.0;
					m_dLoopTick = 0.5;
				})

				.Tick([this](_double TimeDelta)
				{
					//여기서 데미지 처리
					m_dLoopTime += TimeDelta;

					if (m_dLoopTime >= m_dLoopTick)
					{
						ClearDamagedTarget();
						m_dLoopTick += 0.5;
					}

					Shout2_Overlap();
				})
				.AddTransition("Shout2_Loop to Shout2_End", "Shout2_End")
					.Predicator([this]
					{
						return  m_bDead || m_dLoopTime >= 3.0;
					})

			.AddState("Shout2_End")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_226_AL_atk_a8_shout2_end");
				})
				.Tick([this](_double)
				{
					SocketLocalMove(m_pASM);
				})
				.AddTransition("Shout2_End to Idle", "Idle")
					.Predicator([this]
					{
						return  m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})


			//Loop빼고 로컬 다 돌려주기, HIT때 이벤트로 공격처리하기
			.AddState("Stamp_Start")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_230_AL_atk9_punch_start");
				})
				.Tick([this](_double TimeDelta)
				{
					SocketLocalMove(m_pASM);
				})
				.AddTransition("Stamp_Start to Stamp_StartLoop", "Stamp_StartLoop")
					.Predicator([this]
					{
						return  m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

			.AddState("Stamp_StartLoop")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_231_AL_atk9_punch_startloop");
				})
					.Tick([this](_double TimeDelta)
				{
					SocketLocalMove(m_pASM);
				})
				.AddTransition("Stamp_StartLoop to Stamp_Hit", "Stamp_Hit")
					.Predicator([this]
					{
						return  m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

			.AddState("Stamp_Hit")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_232_AL_atk9_punch_hit");
				})
				.Tick([this](_double TimeDelta)
				{
					SocketLocalMove(m_pASM);
				})
				.AddTransition("Stamp_Hit to Stamp_Loop", "Stamp_Loop")
					.Predicator([this]
					{
						return  m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

			.AddState("Stamp_Loop")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_233_AL_atk9_punch_loop");
					m_pModelCom->Find_Animation("AS_em1200_233_AL_atk9_punch_loop")->SetLooping(true);
					m_dLoopTime = 0.0;
				})
				.Tick([this](_double TimeDelta)
				{
					m_dLoopTime += TimeDelta;
				})
				.AddTransition("Stamp_Loop to Stamp_End", "Stamp_End")
					.Predicator([this]
					{
						return  m_bDead || m_dLoopTime >= 2.0;
					})

			.AddState("Stamp_End")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_234_AL_atk9_punch_end");
				})
				.Tick([this](_double TimeDelta)
				{
					SocketLocalMove(m_pASM);
				})
				.AddTransition("Stamp_End to Idle", "Idle")
					.Predicator([this]
					{
						return  m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

			//이벤트에서 스윕 타이밍 알려줌
			.AddState("Swing_L")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_222_AL_atk_a6_Cleave_L");
				})
				.Tick([this](_double TimeDelta)
				{
					m_pTransformCom->LookAt_Smooth(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION), TimeDelta);
					SocketLocalMove(m_pASM);

					if (m_bAttack)
						Swing_SweepSphere("LeftHand");

				})
				.AddTransition("Swing_L to Idle", "Idle")
					.Predicator([this]
					{
						return  m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

			.AddState("Swing_R")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_223_AL_atk_a6_Cleave");
				})
				.Tick([this](_double TimeDelta)
				{
					m_pTransformCom->LookAt_Smooth(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION), TimeDelta);
					SocketLocalMove(m_pASM);

					if (m_bAttack)
						Swing_SweepSphere("RightHand");

				})
				.AddTransition("Swing_R to Idle", "Idle")
					.Predicator([this]
					{
						return  m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

			//Loop빼고 로컬, Start때 이벤트로 sweep 타이밍 알려줌
			.AddState("Rush_Start")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_210_AL_atk_a3_tackle2_start");
				})
				.Tick([this](_double TimeDelta)
				{
					SocketLocalMove(m_pASM);

					if (m_bAttack)
						Rush_SweepSphere();

				})
				.AddTransition("Rush_Start to Rush_Loop", "Rush_Loop")
					.Predicator([this]
					{
						return  m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

			.AddState("Rush_Loop")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_211_AL_atk_a3_tackle2_loop");
					m_pModelCom->Find_Animation("AS_em1200_211_AL_atk_a3_tackle2_loop")->SetLooping(true);
					m_dLoopTime = 0.0;
				})
				.Tick([this](_double TimeDelta)
				{
					m_dLoopTime += TimeDelta;
				})
				.AddTransition("Rush_Loop to Rush_End", "Rush_End")
					.Predicator([this]
					{
						return  m_bDead || m_dLoopTime >= 3.0;
					})

			.AddState("Rush_End")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_212_AL_atk_a3_tackle2_end");
				})
				.Tick([this](_double TimeDelta)
				{
					SocketLocalMove(m_pASM);
				})
				.AddTransition("Rush_End to Idle", "Idle")
					.Predicator([this]
					{
						return  m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

			//Start끝나는 모션이 조금 길게한데 걍 루프에서.. 4번 공격하고 끝남
			//Loop에서 공격할때마다 ClearDamage해주기
			.AddState("Cable_Start")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_217_AL_atk_a5_motif2_start");
				})
				.Tick([this](_double TimeDelta)
				{
					SocketLocalMove(m_pASM);
				})
				.AddTransition("Cable_Start to Cable_Loop", "Cable_Loop")
					.Predicator([this]
					{
						return  m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

			.AddState("Cable_Loop")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_218_AL_atk_a5_motif2_loop");
					m_pModelCom->Find_Animation("AS_em1200_218_AL_atk_a5_motif2_loop")->SetLooping(true);
					m_iAttackCount = 0;
					m_dLoopTime = 0.0;
				})
				.Tick([this](_double TimeDelta)
				{
					m_dLoopTime += TimeDelta;

					if (m_dLoopTime >= 1.0)
					{
						m_dLoopTime = 0.0;
						//케이블 공격
						++m_iAttackCount;
					}
				})
				.OnExit([this]
				{
					//m_pASM->ClearSocketAnim("FullBody", 0.f);
				})
				.AddTransition("Cable_Loop to Cable_End", "Cable_End")
					.Predicator([this]
					{
						return  m_bDead || m_iAttackCount == 4;
					})
				
			.AddState("Cable_End")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_219_AL_atk_a5_motif2_end");
				})
				.Tick([this](_double TimeDelta)
				{
					SocketLocalMove(m_pASM);
				})
				.AddTransition("Cable_End to Idle", "Idle")
					.Predicator([this]
					{
						return  m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})
///////////////////////////////////////////////////////////////////////////////////////////

		.Build();
}

void CEM1200::BeginTick()
{
	CEnemy::BeginTick();
}

void CEM1200::Tick(_double TimeDelta)
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
	m_eTurn = m_pController->GetBaseTurn();
	m_bChangePhase = m_pController->IsChangePhase();

	//ASM, FSM tick
	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);

	const _float MoveSpeed = m_bRun ? 7.f : 1.2f;
	if (m_vMoveAxis.LengthSquared() > 0.f)
	{
		_float3 vVelocity;
		const _float fYaw = m_pTransformCom->GetYaw_Radian();
		XMStoreFloat3(&vVelocity, MoveSpeed * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}

	// Tick의 제일 마지막에서 실행한다.
	ResetHitData();
}

void CEM1200::Late_Tick(_double TimeDelta)
{
	CEnemy::Late_Tick(TimeDelta);
}

void CEM1200::AfterPhysX()
{
	CEnemy::AfterPhysX();

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	GetRigidBody("Range")->Update_Tick(WorldMatrix);
	GetRigidBody("Trunk")->Update_Tick(m_pModelCom->GetBoneMatrix("Head") * WorldMatrix);
	GetRigidBody("Weak")->Update_Tick(m_pModelCom->GetBoneMatrix("Flower") * WorldMatrix);
	GetRigidBody("Legs")->Update_Tick(m_pModelCom->GetBoneMatrix("Hips") * WorldMatrix);
}

HRESULT CEM1200::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CEM1200::Imgui_RenderProperty()
{
	CEnemy::Imgui_RenderProperty();
	if (ImGui::CollapsingHeader("ASM"))
	{
		m_pASM->Imgui_RenderState();
	}
	m_pFSM->Imgui_RenderProperty();
	ImGui::Text("hp %d", m_iHP);
}

_bool CEM1200::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

void CEM1200::Play_LightHitAnim()
{
	if (m_bChangePhase == false)
	{
		if (m_eSimpleHitFrom == ESimpleAxis::NORTH)
			m_pASM->InputAnimSocketOne("FullBody", "AS_em1200_401_AL_damage_l_F1");
		else
			m_pASM->InputAnimSocketOne("FullBody", "AS_em1200_402_AL_damage_l_B1");
	}
	else
	{
		if (m_eSimpleHitFrom == ESimpleAxis::NORTH)
			m_pASM->InputAnimSocketOne("FullBody", "AS_em1200_404_AL_damage_l_F2");
		else
			m_pASM->InputAnimSocketOne("FullBody", "AS_em1200_405_AL_damage_l_B2");
	}
	
}

void CEM1200::Play_MidHitAnim()
{
	switch (m_eHitFrom)
	{
	case EBaseAxis::NORTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em1200_411_AL_damage_m_F2");
		break;
	case EBaseAxis::EAST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em1200_414_AL_damage_m_R2");
		break;
	case EBaseAxis::SOUTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em1200_412_AL_damage_m_B2");
		break;
	case EBaseAxis::WEST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em1200_413_AL_damage_m_L2");
		break;
	case EBaseAxis::AXIS_END:
		FALLTHROUGH;
	default:
		NODEFAULT;
	}
}

void CEM1200::HeavyAttackPushStart()
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

//
ESimpleAxis CEM1200::TargetSimpleAxis()
{
	_vector vTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_float fAngle = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vMyLook), XMVector3Normalize(vTargetPos - vMyPos)));

	//정면에서 45도 정도
	if (fAngle > 0.5f)
		return ESimpleAxis::NORTH;
	else
		return ESimpleAxis::SOUTH;
}

EBaseTurn CEM1200::TargetBaseTurn()
{
	_vector vTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	_float fAngle = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vMyRight), XMVector3Normalize(vTargetPos - vMyPos)));

	//오른쪽
	if (fAngle > 0)
		return EBaseTurn::TURN_RIGHT;
	else
		return EBaseTurn::TURN_LEFT;
}

void CEM1200::Fall_Overlap()
{
	_matrix BoneMatrix = m_pModelCom->GetBoneMatrix("Target") * m_pTransformCom->Get_WorldMatrix();

	_vector vBoneVector = BoneMatrix.r[3];
	_float3 fBone = vBoneVector;

	physx::PxOverlapHit hitBuffer[3];
	physx::PxOverlapBuffer overlapOut(hitBuffer, 3);

	SphereOverlapParams param;
	param.fVisibleTime = 0.1f;
	param.iTargetType = CTB_PLAYER;
	param.fRadius = 5.f;
	param.vPos = XMVectorSetW(fBone, 1.f);
	param.overlapOut = &overlapOut;

	if (CGameInstance::GetInstance()->OverlapSphere(param))
	{
		HitTargets(overlapOut, static_cast<_int>(m_iAtkDamage * 1.5f), EAttackType::ATK_DOWN);
	}
}

void CEM1200::Shout1_Overlap()
{
	_matrix BoneMatrix = m_pModelCom->GetBoneMatrix("Target") * m_pTransformCom->Get_WorldMatrix();

	_vector vBoneVector = BoneMatrix.r[3];
	_float3 fBone = vBoneVector;

	physx::PxOverlapHit hitBuffer[3];
	physx::PxOverlapBuffer overlapOut(hitBuffer, 3);

	SphereOverlapParams param;
	param.fVisibleTime = 0.1f;
	param.iTargetType = CTB_PLAYER;
	param.fRadius = 8.f;
	param.vPos = XMVectorSetW(fBone, 1.f);
	param.overlapOut = &overlapOut;

	if (CGameInstance::GetInstance()->OverlapSphere(param))
	{
		HitTargets(overlapOut, static_cast<_int>(m_iAtkDamage * 1.5f), EAttackType::ATK_HEAVY);
	}
}

void CEM1200::Shout2_Overlap()
{
	_matrix BoneMatrix = m_pModelCom->GetBoneMatrix("Target") * m_pTransformCom->Get_WorldMatrix();

	_vector vBoneVector = BoneMatrix.r[3];
	_float3 fBone = vBoneVector;

	physx::PxOverlapHit hitBuffer[3];
	physx::PxOverlapBuffer overlapOut(hitBuffer, 3);

	SphereOverlapParams param;
	param.fVisibleTime = 0.1f;
	param.iTargetType = CTB_PLAYER;
	param.fRadius = 5.f;
	param.vPos = XMVectorSetW(fBone, 1.f);
	param.overlapOut = &overlapOut;

	if (CGameInstance::GetInstance()->OverlapSphere(param))
	{
		HitTargets(overlapOut, static_cast<_int>(m_iAtkDamage * 1.5f), EAttackType::ATK_HEAVY);
	}
}

void CEM1200::Stamp_Overlap()
{
	_matrix BoneMatrix = m_pModelCom->GetBoneMatrix("RightHand") * m_pTransformCom->Get_WorldMatrix();

	_vector vBoneVector = BoneMatrix.r[3];
	_float3 fBone = vBoneVector;

	physx::PxOverlapHit hitBuffer[3];
	physx::PxOverlapBuffer overlapOut(hitBuffer, 3);

	SphereOverlapParams param;
	param.fVisibleTime = 0.1f;
	param.iTargetType = CTB_PLAYER;
	param.fRadius = 2.f;
	param.vPos = XMVectorSetW(fBone, 1.f);
	param.overlapOut = &overlapOut;

	if (CGameInstance::GetInstance()->OverlapSphere(param))
	{
		HitTargets(overlapOut, static_cast<_int>(m_iAtkDamage * 1.5f), EAttackType::ATK_HEAVY);
	}
}

void CEM1200::Swing_SweepSphere(const string & BoneName)
{
	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);

	//Tail4가 꼬리 중앙에 있음
	_float4x4 BoneMatrix = GetBoneMatrix(BoneName) * m_pTransformCom->Get_WorldMatrix();
	_float4 vBonePos = _float4{ BoneMatrix.m[3][0], BoneMatrix.m[3][1], BoneMatrix.m[3][2], BoneMatrix.m[3][3] };

	_vector	vDir = vBonePos - m_BeforePos;

	SphereSweepParams tParams;
	tParams.fVisibleTime = 0.2f;
	tParams.iTargetType = CTB_PLAYER;
	tParams.fRadius = 2.f;
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

void CEM1200::Rush_SweepSphere()
{
	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);

	//Tail4가 꼬리 중앙에 있음
	_float4x4 BoneMatrix = GetBoneMatrix("Target") * m_pTransformCom->Get_WorldMatrix();
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


CEM1200 * CEM1200::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEM1200* pInstance = new CEM1200(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM1200");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEM1200::Clone(void * pArg)
{
	CEM1200*		pInstance = new CEM1200(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM1200");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM1200::Free()
{
	CEnemy::Free();
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
}
