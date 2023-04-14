#include "stdafx.h"
#include "..\public\EM1200.h"
#include <FSMComponent.h>
#include <AnimCam.h>
#include "RigidBody.h"
#include "JsonStorage.h"
#include "EM1200_AnimInstance.h"
#include "EM1200_Controller.h"
#include "ImguiUtils.h"
#include "MathUtils.h"
#include "Material.h"
#include "EMCable.h"
#include "PlayerInfoManager.h"
#include "HelperClasses.h"
#include "ShaderUI.h"
#include "UI_Manager.h"
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

	m_strDeathSoundTag = "crawl_fx_death";

	// 초기값 지정. LEVEL_NOW 에 따라
	{
		m_iMaxHP = 25000;
		m_iHP = m_iMaxHP;

		m_iMaxCrushGauge = m_iMaxHP * 10.f;
		m_iCrushGauge = m_iMaxCrushGauge;

		iEemeyLevel = 39;
		m_iAtkDamage = 200;

		m_eEnemyName = EEnemyName::EM1200;
		m_bHasCrushGauge = true;
		m_bBoss = true;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));


	m_pTransformCom->SetRotPerSec(XMConvertToRadians(180.f));
	m_fGravity = 20.f;

	//fog
	m_pRendererCom->GetFogDesc().vFogColor = _float3(0.2667f, 0.247f, 0.1804f);
	m_pRendererCom->GetFogDesc().fStartDepth = 5.f;

	//shaderUI
	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/ShaderUI.json");
	m_pShaderUI = dynamic_cast<CShaderUI*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"Shader_UI", &json));
	assert(m_pShaderUI != nullptr && "Failed to Clone : CFullUI");
	return S_OK;
}

void CEM1200::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em1200", L"Model", (CComponent**)&m_pModelCom));

	m_pAnimCam = dynamic_cast<CAnimCam*>(m_pGameInstance->FindCamera("EnemyAnimCamera"));
	Safe_AddRef(m_pAnimCam);

	if (m_pAnimCam == nullptr)
	{
		m_pAnimCam = dynamic_cast<CAnimCam*>(m_pGameInstance->Add_Camera("EnemyAnimCamera", LEVEL_NOW, L"Layer_Camera", L"Prototype_AnimCam"));
		Safe_AddRef(m_pAnimCam);
	}

	m_pWeak = m_pModelCom->FindMaterial(L"MI_em1200_WEAK_0");
	assert(m_pWeak != nullptr);

	// 범위 충돌체(플레이어가 몬스터 위로 못 올라가게한다.)
	Json WeakCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em1200/em1200Weak.json");
	Json TrunkCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em1200/em1200Trunk.json");
	Json LegsCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em1200/em1200Legs.json");

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

	m_SoundStore.CloneSound("crawl_attack_scream");
	m_SoundStore.CloneSound("crawl_attack_shout");
	m_SoundStore.CloneSound("crawl_attack_smsh");
	m_SoundStore.CloneSound("crawl_attack_swing");
	m_SoundStore.CloneSound("crawl_attack_tenta");
	m_SoundStore.CloneSound("crawl_attack_tenta_charging_loop");
	m_SoundStore.CloneSound("crawl_attack_tenta_up");
	m_SoundStore.CloneSound("crawl_move_groggy");

	m_pModelCom->Add_EventCaller("crawl_move_groggy", [this] {m_SoundStore.PlaySound("crawl_move_groggy"); });
}

void CEM1200::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();
	
	m_pModelCom->Add_EventCaller("Fall", [this]
	{
		ClearDamagedTarget();
		Fall_Overlap();

		_int iNum = 5;

		while (iNum--)
		{
			_float RandomX = CMathUtils::RandomFloat(-4.f, 4.f);
			_float RandomZ = CMathUtils::RandomFloat(-4.f, 4.f);

			_float4x4 DustEffectPivot = CImguiUtils::CreateMatrixFromImGuizmoData(
				{ RandomX, 0.f, RandomZ },
				{ 0.f, 0.f, 0.f },
				{ 3.f, 2.f, 3.f }
			);

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"Smoke_Ivory_0" + to_wstring(CMathUtils::RandomUInt(4)))
				->Start_NoAttachPivot(this, DustEffectPivot);
		}
	
	});

	m_pModelCom->Add_EventCaller("Stamp", [this]
	{
		ClearDamagedTarget();
		Stamp_Overlap();

		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em1200_Stamp_Impact")
			->Start_Attach(this,  "RightHand", false, true);

		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em1200_Stamp_Particles")
			->Start_Attach(this,  "RightHand",  false, true);
	});

	m_pModelCom->Add_EventCaller("Swing_R_Start", [this]
	{
		ClearDamagedTarget();
		m_bAttack = true;

		_float4x4 SwingRPivot = CImguiUtils::CreateMatrixFromImGuizmoData(
			{ -1.485f, -2.6f, 1.168f },
			{ -180.f, -30.121f, 180.f },
			{ 1.f, 1.f, 1.f }
		);

		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em1200_Slash")
			->Start_AttachPivot(this, SwingRPivot, "Target", true, false);

		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"Smoke_Ivory_0" + to_wstring(CMathUtils::RandomUInt(4)))
			->Start_AttachPivot(this, SwingRPivot, "Target", true, false);
	});

	m_pModelCom->Add_EventCaller("Swing_R_End", [this]
	{
		m_bAttack = false;
	});

	m_pModelCom->Add_EventCaller("Swing_L_Start", [this]
	{
		ClearDamagedTarget();
		m_bAttack = true;

		_float4x4 SwingRPivot = CImguiUtils::CreateMatrixFromImGuizmoData(
			{ 1.227f, -3.635f, 0.f },
			{ -180.f, 75.183f, -180.f },
			{ 1.f, 1.f, 1.f }
		);

		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em1200_Slash")
			->Start_AttachPivot(this, SwingRPivot, "Target", true, false);

		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"Smoke_Ivory_0" + to_wstring(CMathUtils::RandomUInt(4)))
			->Start_AttachPivot(this, SwingRPivot, "Target", true, false);
	});

	m_pModelCom->Add_EventCaller("Swing_L_End", [this]
	{
		m_bAttack = false;
	});

	m_pModelCom->Add_EventCaller("Rush_Start", [this]
	{
		ClearDamagedTarget();
		m_bAttack = true;

		_int iNum = 5;
		while (iNum--)
		{
			_float RandomX = CMathUtils::RandomFloat(-4.f, 4.f);
			_float RandomZ = CMathUtils::RandomFloat(-4.f, 4.f);

			_float4x4 DustEffectPivot = CImguiUtils::CreateMatrixFromImGuizmoData(
				{ RandomX, 0.f, RandomZ },
				{ 0.f, 0.f, 0.f },
				{ 3.f, 2.f, 3.f }
			);

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"Smoke_Ivory_0" + to_wstring(CMathUtils::RandomUInt(4)))
				->Start_AttachPivot(this, DustEffectPivot, "Eff03", true);
		}
	});

	m_pModelCom->Add_EventCaller("Rush_End", [this]
	{
			m_bAttack = false;

			_int iNum = 4;
			while (iNum--)
			{
				_float RandomX = CMathUtils::RandomFloat(-4.f, 4.f);
				_float RandomZ = CMathUtils::RandomFloat(-4.f, 4.f);

				_float4x4 DustEffectPivot = CImguiUtils::CreateMatrixFromImGuizmoData(
					{ RandomX, 0.f, RandomZ },
					{ 0.f, 0.f, 0.f },
					{ 5.f, 2.f, 5.f }
				);

				CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"Smoke_Ivory_0" + to_wstring(CMathUtils::RandomUInt(4)))
					->Start_AttachPivot(this, DustEffectPivot, "Eff03", true);
			}

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em1200_Stamp_Distortion")
				->Start_Attach(this, "Target", false, true);
	});

	m_pModelCom->Add_EventCaller("Shout2_Start", [this]
	{
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em1200_Fear")
				->Start_Attach(this, "Head", false, true);

			_float4x4 FearPivot = CImguiUtils::CreateMatrixFromImGuizmoData(
				{ 0.f, 0.5f, 0.f },
				{ 0.f, 0.f, 0.f },
				{ 10.f, 1.f, 10.f }
			);

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em1200_Fear_Particle")
				->Start_NoAttachPivot(this, FearPivot);
	});

	//////////////////////////////////////CamEvent//////////////////////////////////////


	m_pAnimCam->AddEvent("em1200IntroEnd", [this]
	{
		m_pController->SetActive(true);
		m_pASM->ClearSocketAnim("FullBody");
		SetUpMainFSM();
		m_pEMUI->Create_BossUI();
		m_OnAnimCam = false;

		CUI_Manager::GetInstance()->Set_TempOff(false);
	});


	m_pAnimCam->AddEvent("em1200Change2End", [this]
	{
			m_pController->SetActive(true);
			SetUpMainFSM();
			m_pEMUI->Create_UIInfo();
			m_OnAnimCam = false;
			CUI_Manager::GetInstance()->Set_TempOff(false);
	});
}

void CEM1200::SetUpMainFSM()
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
			.AddTransition("Idle to Death", "Death")
				.Predicator([this] { return m_bDead; })
			.AddTransition("Idle to Down", "Down")
				.Predicator([this] { return /*m_eCurAttackType == EAttackType::ATK_SPECIAL_END*/
					m_eInput == CController::EHandleInput::CTRL; })

			.AddTransition("Idle to Hit_Heavy", "Hit_Heavy")
				.Predicator([this] { return
					(m_eCurAttackType == EAttackType::ATK_SPECIAL_END
					&& m_bChangePhase); })

			.AddTransition("Idle to Hit_Light", "Hit_Light")
				.Predicator([this] { return
					m_eCurAttackType == EAttackType::ATK_SPECIAL_LOOP
					|| m_eCurAttackType == EAttackType::ATK_HEAVY; })


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
			.AddState("Down")
				.OnStart([this]
				{
						if(m_bChangePhase == false)
							m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_421_AL_down_start1");
						else
							m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_425_AL_down_start2");
				})
				.AddTransition("Down to DownLoop", "DownLoop")
					.Predicator([this]
					{
						return m_bDead
							|| m_pASM->isSocketPassby("FullBody", 0.95f);
					})

			.AddState("DownLoop")
				.OnStart([this]
				{
						if (m_bChangePhase == false)
						{
							m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_422_AL_down1");
							m_pModelCom->Find_Animation("AS_em1200_422_AL_down1")->SetLooping(true);
						}
						else
						{
							m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_426_AL_down2");
							m_pModelCom->Find_Animation("AS_em1200_426_AL_down2")->SetLooping(true);
						}

					m_dLoopTime = 0.0;
				})
				.Tick([this](_double TimeDelta)
				{
						m_dLoopTime += TimeDelta;
				})
				.AddTransition("DownLoop to Getup", "Getup")
					.Predicator([this]
					{
						return m_bDead
							|| m_dLoopTime >= 5.f;
					})

			.AddState("Getup")
				.OnStart([this]
				{
					if (m_bChangePhase == false)
					{
						m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_423_AL_getup1");
					}
					else
					{
						m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_427_AL_getup2");
					}

				})
				.AddTransition("Getup to Idle", "Idle")
					.Predicator([this]
					{
						return PriorityCondition()
							|| m_pASM->isSocketPassby("FullBody", 0.95f);
					})


///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Hit_Light")
			.OnStart([this]
			{
				Play_LightHitAnim();
			})
			.Tick([this](_double)
			{
				if (m_eCurAttackType == EAttackType::ATK_SPECIAL_LOOP
					|| m_eCurAttackType == EAttackType::ATK_HEAVY)
				{
					Play_LightHitAnim();
				}
			})
			.AddTransition("Hit_Light to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead
						|| m_pASM->isSocketPassby("FullBody", 0.95f)
						|| (m_eCurAttackType != EAttackType::ATK_SPECIAL_LOOP
							&& m_eCurAttackType != EAttackType::ATK_HEAVY
							&& m_eCurAttackType != EAttackType::ATK_END);
				})

		.AddState("Hit_Heavy")
			.OnStart([this]
			{
				Play_MidHitAnim();
				HeavyAttackPushStart();
			})
			.Tick([this](_double TimeDelta)
			{
				if(m_eCurAttackType == EAttackType::ATK_SPECIAL_END
					&& m_bChangePhase)
				{
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
			.AddTransition("Hit_Heavy to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead
						|| m_pASM->isSocketPassby("FullBody", 0.95f)
						|| m_eCurAttackType == EAttackType::ATK_SPECIAL_LOOP
						|| m_eCurAttackType == EAttackType::ATK_HEAVY;
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
					return  PriorityCondition() || m_pASM->isSocketEmpty("FullBody");
				})

		//Loop때만 데미지 처리
		.AddState("Shout1_Start")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_205_AL_atk_a2_shout1_start");
					m_SoundStore.PlaySound("crawl_attack_scream", m_pTransformCom);

				})
				.Tick([this](_double)
				{
					SocketLocalMove(m_pASM);
				})
				.AddTransition("Shout1_Start to Shout1_Loop", "Shout1_Loop")
					.Predicator([this]
					{
						return  PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

		.AddState("Shout1_Loop")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_206_AL_atk_a2_shout1_loop");
					m_pModelCom->Find_Animation("AS_em1200_206_AL_atk_a2_shout1_loop")->SetLooping(true);
					ClearDamagedTarget();

					m_dLoopTime = 0.0;
					m_dLoopTick = 0.8;

					CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em1200_Fear")
						->Start_Attach(this, "Target", false, true);

					_float4x4 FearPivot = CImguiUtils::CreateMatrixFromImGuizmoData(
						{ 0.f, 0.5f, 0.f },
						{ 0.f, 0.f, 0.f },
						{ 7.f, 1.f, 7.f }
					);

					CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em1200_Fear_Particle")
						->Start_NoAttachPivot(this, FearPivot);

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

					Shout_Overlap();
				})
				.AddTransition("Shout1_Loop to Shout1_End", "Shout1_End")
					.Predicator([this]
					{
						return  PriorityCondition() || m_dLoopTime >= 3.0;
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
						return  PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
					})


		//Loop때만 데미지 처리
			.AddState("Shout2_Start")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_224_AL_atk_a8_shout2_start");
					m_SoundStore.PlaySound("crawl_attack_shout", m_pTransformCom);

					ClearDamagedTarget();
				})
				.Tick([this](_double)
				{
						SocketLocalMove(m_pASM);
				})

				.AddTransition("Shout2_Start to Shout2_Loop", "Shout2_Loop")
					.Predicator([this]
					{
						return  PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

			.AddState("Shout2_Loop")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_225_AL_atk_a8_shout2_loop");
					m_pModelCom->Find_Animation("AS_em1200_225_AL_atk_a8_shout2_loop")->SetLooping(true);
					ClearDamagedTarget();

					//데미지
					m_dLoopTime = 0.0;
					m_dLoopTick = 0.6;
					
					//안개
					m_dFogTime = 0.0;
					m_pRendererCom->SetFog(true);
					m_pRendererCom->GetFogDesc().fGlobalDensity = 0.f;

					////UI
					//m_pEMUI->Delete_UIInfo();
					//CPlayerInfoManager::GetInstance()->Set_TargetedMonster(nullptr);
				})

				.Tick([this](_double TimeDelta)
				{
					m_dLoopTime += TimeDelta;

					if (m_dLoopTime >= m_dLoopTick)
					{
						ClearDamagedTarget();
						m_dLoopTick += 0.6;
					}

					Shout_Overlap();
				/*	_float fMaxFogDensity = 0.8f;
					_float FogGlobalDensity = m_pRendererCom->GetFogDesc().fGlobalDensity += TimeDelta * 0.5;
					if (FogGlobalDensity >= fMaxFogDensity)
						m_pRendererCom->GetFogDesc().fGlobalDensity = fMaxFogDensity;*/

				})
				.OnExit([this]
				{
						m_pASM->ClearSocketAnim("FullBody", 0.f);
				})
				.AddTransition("Shout2_Loop to Shout2_End", "Shout2_End")
					.Predicator([this]
					{
						return  PriorityCondition() || m_dLoopTime >= 3.0;
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
						return  PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
					})


			//Loop빼고 로컬 다 돌려주기, HIT때 이벤트로 공격처리하기
			.AddState("Stamp_Start")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_230_AL_atk9_punch_start");
					m_SoundStore.PlaySound("crawl_attack_smsh", m_pTransformCom);
				})
				.Tick([this](_double TimeDelta)
				{
					SocketLocalMove(m_pASM);
				})
				.AddTransition("Stamp_Start to Stamp_StartLoop", "Stamp_StartLoop")
					.Predicator([this]
					{
						return  PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
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
				.OnExit([this] 
				{
						m_pASM->ClearSocketAnim("FullBody", 0.f);
				})
				.AddTransition("Stamp_StartLoop to Stamp_Hit", "Stamp_Hit")
					.Predicator([this]
					{
						return  PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
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
						return  PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
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
						return  PriorityCondition() || m_dLoopTime >= 2.0;
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
						return  PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

			//얜 절대 로컬 움직이지않기
			.AddState("Swing_L")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_222_AL_atk_a6_Cleave_L");
					m_SoundStore.PlaySound("crawl_attack_swing", m_pTransformCom);

				})
				.Tick([this](_double TimeDelta)
				{
					m_pTransformCom->LookAt_Smooth(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION), TimeDelta);

					if (m_bAttack)
						Swing_SweepSphere("LeftHand");

				})
				.AddTransition("Swing_L to Idle", "Idle")
					.Predicator([this]
					{
						return  PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

			.AddState("Swing_R")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_223_AL_atk_a6_Cleave");
					m_SoundStore.PlaySound("crawl_attack_swing", m_pTransformCom);

				})
				.Tick([this](_double TimeDelta)
				{
					m_pTransformCom->LookAt_Smooth(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION), TimeDelta);

					if (m_bAttack)
						Swing_SweepSphere("RightHand");

				})
				.AddTransition("Swing_R to Idle", "Idle")
					.Predicator([this]
					{
						return  PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
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
						return  PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
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
				.OnExit([this]
					{
						m_pASM->ClearSocketAnim("FullBody", 0.f);
				})
				.AddTransition("Rush_Loop to Rush_End", "Rush_End")
					.Predicator([this]
					{
						return  PriorityCondition() || m_dLoopTime >= 3.0;
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
						return  PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

			//Start끝나는 모션이 조금 길게한데 걍 루프에서.. 4번 공격하고 끝남
			//Loop에서 공격할때마다 ClearDamage해주기
			.AddState("Cable_Start")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_217_AL_atk_a5_motif2_start");
					m_SoundStore.PlaySound("crawl_attack_tenta", m_pTransformCom);

				})
				.Tick([this](_double TimeDelta)
				{
					SocketLocalMove(m_pASM);
				})
				.AddTransition("Cable_Start to Cable_Loop", "Cable_Loop")
					.Predicator([this]
					{
						return  PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

			.AddState("Cable_Loop")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_218_AL_atk_a5_motif2_loop");
					m_pModelCom->Find_Animation("AS_em1200_218_AL_atk_a5_motif2_loop")->SetLooping(true);
					m_SoundStore.PlaySound("crawl_attack_tenta_charging_loop", m_pTransformCom);

					m_iAttackCount = 0;
					m_iPreAttackCount = -1;
					m_dLoopTime = 0.0;
				})
				.Tick([this](_double TimeDelta)
				{
					m_dLoopTime += TimeDelta;

					//공격할 위치 미리 저장
					if (m_dLoopTime >= 0.5 && m_iPreAttackCount != m_iAttackCount)
					{
						m_SaveTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
						m_SaveTargetPos.y = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + XMVectorSet(0.f, -1.f, 0.f, 0.f) * 2.f);
						m_iPreAttackCount = m_iAttackCount;
					}

					if (m_dLoopTime >= 1.0)
					{
						ClearDamagedTarget();
						CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em1200_Ribbon_Particle")
							->Start_AttachPosition(this, m_SaveTargetPos, _float4(0.f, 1.f, 0.f, 0.f), false);

						CGameObject* pCable = CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_Cable"), TEXT("Prototype_EMCable"));
						dynamic_cast<CEMCable*>(pCable)->Set_Dest(m_SaveTargetPos);
						pCable->Set_Owner(this);

						m_SoundStore.PlaySound("crawl_attack_tenta_up", m_pTransformCom);

						m_dLoopTime = 0.0;
						++m_iAttackCount;
					}
				})
				.OnExit([this]
				{
					m_pASM->ClearSocketAnim("FullBody", 0.f);
				})
				.AddTransition("Cable_Loop to Cable_End", "Cable_End")
					.Predicator([this]
					{
						return  PriorityCondition() || m_iAttackCount == 4;
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
						return  PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
					})
///////////////////////////////////////////////////////////////////////////////////////////

		.Build();
}

void CEM1200::SetUpIntro()
{
	m_pController->SetActive(false);
	m_OnAnimCam = true;

	m_pASM->AttachAnimSocketOne("FullBody", "AS_em1200_165_AL_waver_motif1");
	m_pModelCom->Find_Animation("AS_em1200_165_AL_waver_motif1")->SetLooping(true);

	auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("em1200Intro");
	m_pAnimCam->StartCamAnim_Return_Update(pCamAnim, CPlayerInfoManager::GetInstance()->Get_PlayerCam(), m_pTransformCom, 0.f, 0.f);

	CUI_Manager::GetInstance()->Set_TempOff(true);
}

void CEM1200::SetUpChangeFSM()
{
	CEnemy::SetUpFSM();

	m_OnAnimCam = true;
	m_pController->SetActive(false);
	m_pEMUI->Delete_UIInfo();
	CUI_Manager::GetInstance()->Set_TempOff(true);
	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.OnStart([this]
			{
				m_fGravity = 20.f;
			})
			.AddTransition("Idle to Dark", "Dark")
				.Predicator([this] { return m_OnAnimCam && m_bAlpha == false; })
			
		.AddState("Dark")
			.OnStart([this]
			{
				m_bAlpha = true;
				m_pShaderUI->SetVisible(true);
			})
			.Tick([this](_double TimeDelta)
			{
					GetDark(TimeDelta);
			})
			.AddTransition("Dark to Change1", "Change1")
				.Predicator([this] { return m_bAlpha == false; })

		.AddState("Change1")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody",  "AS_m02c00920c01_em1200");

				auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("em1200Change");
				m_pAnimCam->StartCamAnim_Return_Update(pCamAnim, CPlayerInfoManager::GetInstance()->Get_PlayerCam(), m_pTransformCom, 0.f, 0.f);

			})
			.AddTransition("Change1 to Change2", "Change2")
				.Predicator([this] { return m_pASM->isSocketPassby("FullBody", 0.99f); })

		.AddState("Change2")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_m02c00920c02_em1200");

				auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("em1200Change2");
				m_pAnimCam->StartCamAnim_Return_Update(pCamAnim, CPlayerInfoManager::GetInstance()->Get_PlayerCam(), m_pTransformCom, 0.f, 0.f);

			})
			.OnExit([this]
			{
					m_bAlpha = true;
					m_pShaderUI->SetVisible(true);
			})
			.AddTransition("Change2 to Idle", "Idle")
				.Predicator([this] { return m_pASM->isSocketPassby("FullBody", 0.99f); })

		.Build();

}

void CEM1200::BeginTick()
{
	CEnemy::BeginTick();
	SetUpIntro();
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

	PlayAnimCam_PhaseChange();

	//ASM, FSM tick
	if(m_pFSM != nullptr)
		m_pFSM->Tick(TimeDelta);

	m_pASM->Tick(TimeDelta);

	const _float MoveSpeed = m_bRun ? 2.f : 0.7f;
	if (m_vMoveAxis.LengthSquared() > 0.f)
	{
		_float3 vVelocity;
		const _float fYaw = m_pTransformCom->GetYaw_Radian();
		XMStoreFloat3(&vVelocity, MoveSpeed * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}


	FogControl(TimeDelta);
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

	static _bool tt = false;
	ImGui::Checkbox("Modify Pivot", &tt);

	if (tt)
	{
		static GUIZMO_INFO tInfo;
		CImguiUtils::Render_Guizmo(&pivot, tInfo, true, true);

		if (ImGui::Button("RushDustTest"))
		{

			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"Smoke_Ivory_0" + to_wstring(CMathUtils::RandomUInt(4)))
				->Start_NoAttachPivot(this, pivot);
		}
	}
	//	if (ImGui::Button("Rush"))
	//	{

	//		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em1200_Stamp_Distortion")
	//			->Start_AttachPivot(this, pivot, "Target", true, false);

	//		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"Smoke_Ivory_0" + to_wstring(CMathUtils::RandomUInt(4)))
	//			->Start_AttachPivot(this, pivot, "Target", true, false);
	//	}

	//}

	static _bool ff = false;
	if (ImGui::Button("Fog"))
	{
		ff = true;
		m_pRendererCom->SetFog(true);
		m_pRendererCom->GetFogDesc().fGlobalDensity = 2.f;
		m_pRendererCom->GetFogDesc().fStartDepth = 13.f;
	}

	if (ff)
	{
		FOG_DESC& tFogDesc = m_pRendererCom->GetFogDesc();
		ImGui::InputFloat("StartDepth", &tFogDesc.fStartDepth);
		ImGui::InputFloat("Density", &tFogDesc.fGlobalDensity);
		ImGui::InputFloat("HeightFalloff", &tFogDesc.fHeightFalloff);
	}



}

_bool CEM1200::IsWeak(CRigidBody* pHitPart)
{
	return 	pHitPart == GetRigidBody("Weak");
}

_float4 CEM1200::GetKineticTargetPos()
{
	return XMLoadFloat4x4(&GetRigidBody("Weak")->GetPxWorldMatrix()).r[3];
}

_bool CEM1200::Exclude()
{
	if (m_OnAnimCam == true)
		return true;

	if (CheckSASType(ESASType::SAS_PENETRATE) || m_pRendererCom->GetFog() == false)
		return false;
	else
		return true;
}

_bool CEM1200::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

_bool CEM1200::PriorityCondition()
{
	return m_bDead || m_eCurAttackType == EAttackType::ATK_SPECIAL_END;
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


void CEM1200::HitWeakProcess(_double TimeDelta)
{
	if (m_bHitWeak)
	{
		m_bWeakProcess = true;
		m_pWeak->GetParam().Floats[1] = 1.f;
	}

	if (m_bWeakProcess)
	{
		m_pWeak->GetParam().Floats[1] -= static_cast<_float>(TimeDelta);

		if (m_pWeak->GetParam().Floats[1] <= 0.f)
		{
			m_pWeak->GetParam().Floats[1] = 0.f;
			m_bWeakProcess = false;
		}
	}
}

void CEM1200::FogControl(_double TimeDelta)
{
	if (m_pRendererCom->GetFog() == false) return;
	
	m_dFogTime += TimeDelta;

	//fog 켜지고 12초후 density를 천천히 줄이는데 0이 되면 안개 끔.
	//12초 전에 플레이어가 투시를 키면 density를 0.4로 낮춤
	if (m_dFogTime > 12.0)
	{
		m_pRendererCom->GetFogDesc().fGlobalDensity -= TimeDelta * 0.7f;

		if (m_pRendererCom->GetFogDesc().fGlobalDensity <= 0.f)
		{
			m_pRendererCom->GetFogDesc().fGlobalDensity = 0.f;
			m_dFogTime = 0.0;
			m_pRendererCom->SetFog(false);
		}
	}
	else
	{
		if (CheckSASType(ESASType::SAS_PENETRATE))
			m_pRendererCom->GetFogDesc().fGlobalDensity = 0.1f;
	}
	
	if (CheckSASType(ESASType::SAS_PENETRATE) == false)
	{
		m_pEMUI->Delete_UIInfo();
		CPlayerInfoManager::GetInstance()->Set_TargetedMonster(nullptr);

		_float fMaxFogDensity = 0.8f;
		_float FogGlobalDensity = m_pRendererCom->GetFogDesc().fGlobalDensity += TimeDelta * 0.5;
		if (FogGlobalDensity >= fMaxFogDensity)
			m_pRendererCom->GetFogDesc().fGlobalDensity = fMaxFogDensity;
	}
}

void CEM1200::PlayAnimCam_PhaseChange()
{
	//AnimCam
	if (m_bChangePhase == true)
	{
		m_bSetUpChange.IsNotDo([this]
		{
			SetUpChangeFSM();
		});
	}
}

void CEM1200::GetDark(_double TimeDelta)
{
	if (m_bAlpha == false) return; 

	m_pShaderUI->Set_Size({ _float(g_iWinSizeX), _float(g_iWinSizeY) });
	_float fAlpha = m_pShaderUI->Get_Float4s_W();
	if (m_bReverse == false && fAlpha >= 0.5f)
	{
		m_bReverse = true;
	}
	else if (m_bReverse == true && fAlpha <= 0.0f)
	{
		m_bReverse = false;
		m_bAlpha = false;
		m_pShaderUI->SetVisible(false);
	}

	_float fSpeed = 0.3f;
	if (m_bReverse == false)
		fAlpha += _float(TimeDelta) * fSpeed;
	else
		fAlpha -= _float(TimeDelta) * fSpeed;
	m_pShaderUI->Set_Float4s_W(fAlpha);
}

void CEM1200::Fall_Overlap()
{
	_matrix BoneMatrix = m_pModelCom->GetBoneMatrix("Target") * m_pTransformCom->Get_WorldMatrix();

	_vector vBoneVector = BoneMatrix.r[3];
	_float3 fBone = vBoneVector;

	physx::PxOverlapHit hitBuffer[3];
	physx::PxOverlapBuffer overlapOut(hitBuffer, 3);

	SphereOverlapParams param;
	param.fVisibleTime = 1.f;
	param.iTargetType = CTB_PLAYER;
	param.fRadius = 6.f;
	param.vPos = XMVectorSetW(fBone, 1.f);
	param.overlapOut = &overlapOut;

	if (CGameInstance::GetInstance()->OverlapSphere(param))
	{
		HitTargets(overlapOut, static_cast<_int>(m_iAtkDamage * 1.5f), EAttackType::ATK_HEAVY);
	}
}

void CEM1200::Shout_Overlap()
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

void CEM1200::Stamp_Overlap()
{
	_matrix BoneMatrix = m_pModelCom->GetBoneMatrix("RightHand") * m_pTransformCom->Get_WorldMatrix();

	_vector vBoneVector = BoneMatrix.r[3];
	_float3 fBone = vBoneVector;

	physx::PxOverlapHit hitBuffer[3];
	physx::PxOverlapBuffer overlapOut(hitBuffer, 3);

	SphereOverlapParams param;
	param.fVisibleTime = 1.f;
	param.iTargetType = CTB_PLAYER;
	param.fRadius = 4.f;
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
	tParams.fVisibleTime = 1.f;
	tParams.iTargetType = CTB_PLAYER;
	tParams.fRadius = 3.f;
	tParams.fDistance = XMVectorGetX(XMVector4Length(vDir));
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

	_float4x4 BoneMatrix = GetBoneMatrix("Head") * m_pTransformCom->Get_WorldMatrix();
	_float4 vBonePos = _float4{ BoneMatrix.m[3][0], BoneMatrix.m[3][1], BoneMatrix.m[3][2], BoneMatrix.m[3][3] };

	_vector	vDir = vBonePos - m_BeforePos;

	SphereSweepParams tParams;
	tParams.fVisibleTime = 1.f;
	tParams.iTargetType = CTB_PLAYER;
	tParams.fRadius = 5.f;
	tParams.fDistance = XMVectorGetX(XMVector4Length(vDir));
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
	Safe_Release(m_pAnimCam);
}
