#include "stdafx.h"
#include "EM0210.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "RigidBody.h"
#include "EM0210_AnimInstance.h"
#include "EM0210_Controller.h"
#include "PhysX_Manager.h"
#include "GameInstance.h"
#include "ImguiUtils.h"
#include "Material.h"
#include "EMBrain.h"

#include "ControlledRigidBody.h"
#include "RigidBody.h"

CEM0210::CEM0210(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
	m_eEnemyName = EEnemyName::EM0210;
}

CEM0210::CEM0210(const CEM0210 & rhs)
	: CEnemy(rhs)
{
	m_SpawnEffectPivot = CImguiUtils::CreateMatrixFromImGuizmoData({0.f, 1.143f, 1.3f}, {0.f, 0.f, 0.f}, {0.7f,0.8f,0.7f});
	m_fSpawnDistortionDistancePivot = 0.5f;
}

HRESULT CEM0210::Initialize(void * pArg)
{
	Json em0210_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em0210/em0210Base.json");
	pArg = &em0210_json;

	m_strDeathSoundTag = "mon_5_fx_death";
	m_strImpactVoiceTag = "mon_5_impact_voice";

	// 초기값 지정. LEVEL_NOW 에 따라
	{
		_uint iBaseLevel = max(0, _int(LEVEL_NOW - 20));
		
		m_iMaxHP = LEVEL_NOW* (50 + (CMathUtils::RandomUInt(10)));
		m_iHP = m_iMaxHP;

		m_iMaxCrushGauge = m_iMaxHP * 1.2f;
		m_iCrushGauge = m_iMaxCrushGauge;

		iEemeyLevel = (iBaseLevel * 2.5) + CMathUtils::RandomUInt(3) + 1;
		m_iAtkDamage = iEemeyLevel * (CMathUtils::RandomUInt(4) + 8);

		m_eEnemyName = EEnemyName::EM0210;
		m_bHasCrushGauge = true;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	m_pTransformCom->SetRotPerSec(XMConvertToRadians(220.f));

	//시작부터 투명상태 적용
	m_IsInvisible = true;

	//브레인 생성
	m_pBrain = dynamic_cast<CEMBrain*>(CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_Brain"), TEXT("Prototype_EMBrain")));
	assert(m_pBrain != nullptr);

	return S_OK;
}

void CEM0210::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW,
		L"Prototype_Model_em210", L"Model",
		(CComponent**)&m_pModelCom));

	Json RangeCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em0210/em0210Range.json");

	//Create Collider
	Add_RigidBody("Range", &RangeCol);

	// 컨트롤러, prototype안 만들고 여기서 자체생성하기 위함
	m_pController = CEM0210_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	// ASM
	m_pASM = CEM0210_AnimInstance::Create(m_pModelCom, this);
}

void CEM0210::SetUpSound()
{
	CEnemy::SetUpSound();

	m_SoundStore.CloneSound("voidflower_attack_spin");
	m_SoundStore.CloneSound("voidflower_attack_upper");
	m_SoundStore.CloneSound("mon_5_backdodge");
	m_SoundStore.CloneSound("mon_5_sidedodge");
	m_SoundStore.CloneSound("mon_5_step");
	m_SoundStore.CloneSound("mon_5_rush");
	m_SoundStore.CloneSound("mon_5_voice_laugh");
	m_SoundStore.CloneSound("Break_Brain");

	m_pModelCom->Add_EventCaller("voidflower_attack_spin", [this] {m_SoundStore.PlaySound("voidflower_attack_spin", m_pTransformCom); });
	m_pModelCom->Add_EventCaller("voidflower_attack_upper", [this] {m_SoundStore.PlaySound("voidflower_attack_upper", m_pTransformCom); });
	m_pModelCom->Add_EventCaller("mon_5_backdodge", [this] {m_SoundStore.PlaySound("mon_5_backdodge", m_pTransformCom); });
	m_pModelCom->Add_EventCaller("mon_5_sidedodge", [this] {m_SoundStore.PlaySound("mon_5_sidedodge", m_pTransformCom); });
	m_pModelCom->Add_EventCaller("mon_5_step", [this] {m_SoundStore.PlaySound("mon_5_step", m_pTransformCom); });
	m_pModelCom->Add_EventCaller("mon_5_rush", [this] {m_SoundStore.PlaySound("mon_5_rush", m_pTransformCom); });
}

void CEM0210::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();

	// Event Caller

	m_pModelCom->Add_EventCaller("DodgeL_Start", [this]
	{
			Dodge_VelocityCalc();
	});
	m_pModelCom->Add_EventCaller("DodgeR_Start", [this]
	{
			Dodge_VelocityCalc();
	});
	m_pModelCom->Add_EventCaller("DodgeB_Start", [this]
	{
			Dodge_VelocityCalc();
	});

	//Spin start
	m_pModelCom->Add_EventCaller("Spin_Start", [this]
	{
		ClearDamagedTarget();
		m_bAttack = true;

		_matrix SpinEffectPivotMatirx = CImguiUtils::CreateMatrixFromImGuizmoData(
			{ 1.65f, -0.55f, 1.121f },
			{ 180.f, 0.f, 0.f, },
			{ 1.f, 1.f, 1.f });

		 CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0210_Spin_Attack")
		->Start_AttachPivot(this, SpinEffectPivotMatirx, "Target", true, true);

	});

	m_pModelCom->Add_EventCaller("Spin_Effect1", [this]
		{
			_matrix SpinEffectPivotMatirx = CImguiUtils::CreateMatrixFromImGuizmoData(
				{ 1.65f, -0.55f, 1.121f },
				{ 180.f, 0.f, 0.f, },
				{ 1.f, 1.f, 1.f });

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0210_Spin_Attack")
				->Start_AttachPivot(this, SpinEffectPivotMatirx, "Target", true, true);
		});

	//Spin end
	m_pModelCom->Add_EventCaller("Spin_End", [this]
	{
		m_bAttack = false;
	});


	//Somersault start
	m_pModelCom->Add_EventCaller("Somersault_Start", [this]
	{
		ClearDamagedTarget();
		m_bAttack = true;

		_matrix SpinEffectPivotMatirx = CImguiUtils::CreateMatrixFromImGuizmoData(
			{ 0.f, 0.f, 0.f },
			{ 180.f, 0.f, 0.f, },
			{ 1.f, 1.f, 1.f });

		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0210_Spin_Attack_A")
			->Start_AttachPivot(this, SpinEffectPivotMatirx, "RightToeBase", true, true);
		
	});

	//Somersault end
	m_pModelCom->Add_EventCaller("Somersault_End", [this]
	{
		m_bAttack = false;
	});

	// 공중에서 추가타 맞을 때
	m_pModelCom->Add_EventCaller("Successive", [this]
		{
			m_fGravity = 3.f;
			m_fYSpeed = 1.5f;
		});
	// 공중에서 추가타 맞고 다시 떨어지는 순간
	m_pModelCom->Add_EventCaller("AirDamageReset", [this]
		{
			m_fGravity = 20.f;
			m_fYSpeed = 0.f;
		});

	m_pModelCom->Add_EventCaller("DeadFlower", [this]
		{
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0220DeadFlower")
				->Start_NoAttach(this, false);
		});
}

void CEM0210::SetUpFSM()
{
	CEnemy::SetUpFSM();

	/*
	R : Attack_Spin 
	G : Attack_Somersault
	C : Threat

	NUM1 : Dodge_L
	NUM2 : Dodge_B
	NUM3 : Dodge_R
	*/

	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.OnStart([this]
			{
				m_fGravity = 20.f;
			})
			.AddTransition("Idle to BrainCrushStart", "BrainCrushStart")
				.Predicator([this] { return m_bCrushStart; })
			.AddTransition("Idle to Death", "Death")
				.Predicator([this] { return m_bDead; })
			.AddTransition("Idle to DownFirstHit", "DownFirstHit")
				.Predicator([this]	{ return m_bDownFirstHit; })
			.AddTransition("Idle to Down", "Down")
				.Predicator([this] { return !m_bDown && m_eDeBuff == EDeBuffType::DEBUFF_THUNDER 
					&& m_eCurAttackType != EAttackType::ATK_TO_AIR; })

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


			.AddTransition("Idle to Attack_Spin", "Attack_Spin")
				.Predicator([this] { return m_eInput == CController::R; })
			.AddTransition("Idle to Attack_Somersault", "Attack_Somersault")
				.Predicator([this] { return m_eInput == CController::G; })
			.AddTransition("Idle to Threat", "Threat")
				.Predicator([this] { return m_eInput == CController::C; })
			.AddTransition("Idle to Dodge", "Dodge")
				.Predicator([this] { return m_eInput == CController::NUM_1 || m_eInput == CController::NUM_2 || m_eInput == CController::NUM_3; })


///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Hit_Light")
			.OnStart([this]
			{
				Play_LightHitAnim();
			})
			.Tick([this](_double)
			{
				if (m_eCurAttackType == EAttackType::ATK_LIGHT
					|| m_eCurAttackType == EAttackType::ATK_SPECIAL_LOOP)
				{
					Play_LightHitAnim();
				}
			})
			.AddTransition("Hit_Light to Idle", "Idle")
				.Predicator([this]
				{
						return PriorityCondition()
							|| m_pASM->isSocketPassby("FullBody", 0.95f)
							|| (m_eCurAttackType != EAttackType::ATK_LIGHT
								&& m_eCurAttackType != EAttackType::ATK_SPECIAL_LOOP
								&& m_eCurAttackType != EAttackType::ATK_END);
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
						return PriorityCondition()
							|| m_pASM->isSocketPassby("FullBody", 0.95f)
							|| m_eCurAttackType == EAttackType::ATK_TO_AIR
							|| m_eCurAttackType == EAttackType::ATK_SPECIAL_LOOP;
				})

		.AddState("Death")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_424_AL_dead_down");
			})

		.AddState("Hit_ToAir")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_432_AL_blow_start_F");
				m_fGravity = 20.f;
				m_fYSpeed = 12.f;
				m_bDown = true;
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
						m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_455_AL_rise_start");
					}
			})
			.OnExit([this]
			{
					m_bHitAir = false;
			})
			.AddTransition("Hit_ToAir to Landing", "Landing")
				.Predicator([this]
			{
				return !m_bPreOnFloor && m_bOnFloor && m_bHitAir;
			})
		.AddState("Landing")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_433_AL_blow_landing_F");
				m_fGravity = 20.f;
			})
			.AddTransition("Landing to Getup", "Getup")
				.Predicator([this]
				{
					return m_bCrushStart || m_pASM->isSocketPassby("FullBody", 0.98f);
				})
			.AddTransition("Landing to Hit_ToAir", "Hit_ToAir")
				.Predicator([this]
				{
					return m_eCurAttackType == EAttackType::ATK_TO_AIR;
				})
			.AddTransition("Landing to Shock", "Shock")
				.Predicator([this]
				{
					return (m_pASM->isSocketPassby("FullBody", 0.95f) && m_eDeBuff == EDeBuffType::DEBUFF_THUNDER);
				})

		.AddState("DownFirstHit")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_425_AL_down_start");
				m_bDownFirstHit = false;
				m_bDown = true;
			})
			.AddTransition("DownFirstHit to Getup", "Getup")
				.Predicator([this]
				{
						return PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Down")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_425_AL_down_start");
				m_bDown = true;
			})
			.AddTransition("Down to Shock", "Shock")
				.Predicator([this]
				{
					return m_bCrushStart || m_eCurAttackType == EAttackType::ATK_TO_AIR || m_pASM->isSocketPassby("FullBody", 0.99f);
				})

		.AddState("Shock")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_480_AL_down_shock");
				m_pModelCom->Find_Animation("AS_em0200_480_AL_down_shock")->SetLooping(true);
			})
			.AddTransition("Shock to Getup", "Getup")
				.Predicator([this]
				{
					return m_bCrushStart || m_eDeBuff != EDeBuffType::DEBUFF_THUNDER;
				})
			.AddTransition("Shock to Hit_ToAir", "Hit_ToAir")
				.Predicator([this]
				{
					return m_eCurAttackType == EAttackType::ATK_TO_AIR;
				})

		.AddState("Getup")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_427_AL_getup");
			})
			.OnExit([this]
			{
				m_bDown = false;
			})
			.AddTransition("Getup to Idle", "Idle")
				.Predicator([this]
				{
					return PriorityCondition() || m_eCurAttackType == EAttackType::ATK_TO_AIR || m_pASM->isSocketEmpty("FullBody");
				})

///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("BrainCrushStart")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody",  "AS_em0200_485_AL_BCchance_start");
			})
			.AddTransition("BrainCrushStart to BrainCrushLoop", "BrainCrushLoop")
				.Predicator([this]
				{
					return m_bBrainCrush || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("BrainCrushLoop")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_486_AL_BCchance_loop");
				m_pModelCom->Find_Animation("AS_em0200_486_AL_BCchance_loop")->SetLooping(true);

				m_pBrain->InitBC();
				m_BCLoopTime = 0.0;
			})
			.Tick([this](_double TimeDelta)
			{
					_matrix WeakBoneMatrix = GetBoneMatrix("Weak01") * m_pTransformCom->Get_WorldMatrix();

					if(m_pBrain == nullptr)
						m_pBrain = dynamic_cast<CEMBrain*>(CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_EnemyBrain"), TEXT("Prototype_EMBrain")));
					else
						m_pBrain->GetTransform()->Set_WorldMatrix(WeakBoneMatrix);
					m_BCLoopTime += TimeDelta;
			})

			.AddTransition("BrainCrushLoop to BrainCrushLoopDead", "BrainCrushLoopDead")
				.Predicator([this]
				{
					//브레인 크러쉬를 하지 않았을때는 부모쪽에서 3초 안에 죽임
					return m_BCLoopTime >= 3.0;
				})
			.AddTransition("BrainCrushLoop to BrainCrushEnd", "BrainCrushEnd")
				.Predicator([this]
					{
						return	m_bBrainCrush;
					})
		.AddState("BrainCrushLoopDead")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_487_AL_BCchance_end");
				m_pBrain->SetDelete();
				SetDead();
			})
		
		.AddState("BrainCrushEnd")
			.OnStart([this]
			{
				_float4 vTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				_float4 vDistance = XMLoadFloat4(&vTargetPos) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float fDistance = vDistance.Length();

				if (5.f >= fDistance)
				{
					m_pASM->InputAnimSocketOne("FullBody", "AS_BC_em0200m_em0200");
				}
				else
				{
					m_pASM->InputAnimSocketOne("FullBody", "AS_BC_em_common_em0200");
				}

				m_pBrain->BeginBC();
			})
			.Tick([this](_double)
			{
					SocketLocalMove(m_pASM);

					if (m_pBrain != nullptr)
					{
						_matrix WeakBoneMatrix = GetBoneMatrix("Weak01") * m_pTransformCom->Get_WorldMatrix();
						m_pBrain->GetTransform()->Set_WorldMatrix(WeakBoneMatrix);

						//const _float fPlayRatio = m_CanFullBC ? 0.8f : 0.2f;
						const _float fPlayRatio = m_CanFullBC ? 0.7f : 0.5f;

						if (m_pASM->isSocketPassby("FullBody", fPlayRatio))
						{
							m_SoundStore.PlaySound("Break_Brain", m_pTransformCom);
							m_pBrain->EndBC();
							m_pBrain->SetDelete();
							m_pBrain = nullptr;
							SetDead();
						}
					}
				
			})
///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Dodge")
			.OnStart([this]
			{
				// 바닥 도착보다 dodge start 모션이 더 일찍 끝나서 controller에서 새로운 명령이 온다.
				// 이를 방지하기 위해서 임시적으로 컨트롤러 비활성화(안하면 dodge stop 하면서 움직임)
				m_pController->SetActive(false);
				m_vDodgeDir = _float4::Zero;
				if (m_eInput == CController::NUM_1) // left
				{
					m_vDodgeDir.x = -1.f;
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_140_AL_dodge_L_start");
				}
				else if (m_eInput == CController::NUM_2) // back
				{
					m_vDodgeDir.z = -1.f;
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_135_AL_dodge_B_start");
				}
				else if (m_eInput == CController::NUM_3) //right
				{
					m_vDodgeDir.x = 1.f;
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_145_AL_dodge_R_start");
				}
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
			.AddTransition("Dodge to DodgeStop", "DodgeStop")
				.Predicator([this]
				{
					return PriorityCondition() || (m_bDodge && m_bOnFloor);
				})
			.AddTransition("Dodge to Idle", "Idle")
				.Predicator([this]
				{
					return PriorityCondition() || m_eCurAttackType != EAttackType::ATK_END;
				})
			.AddState("DodgeStop")
				.OnStart([this]
				{
					if (CMathUtils::FloatCmp(m_vDodgeDir.x, -1.f))
						m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_141AL_dodge_L_stop");
					else if (CMathUtils::FloatCmp(m_vDodgeDir.z, -1.f))
						m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_136_AL_dodge_B_stop");
					else if (CMathUtils::FloatCmp(m_vDodgeDir.x, 1.f))
						m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_146_AL_dodge_R_stop");
				})
				.AddTransition("DodgeStop to Idle", "Idle")
					.Predicator([this]
					{
						return PriorityCondition() || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketPassby("FullBody", 0.99f);
					})


///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Attack_Spin")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0210_212_AL_spin");
			})
			.Tick([this](_double) 
			{
				SocketLocalMove(m_pASM);
				if (m_bAttack)
					Swing_SweepSphere();
			})
			.AddTransition("Attack_Spin to Idle", "Idle")
				.Predicator([this]
				{
					return PriorityCondition() 
						|| HitHeavyCondition()
						|| m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Attack_Somersault")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0210_211_AL_somersault");
			})
			.Tick([this](_double) 
			{
				SocketLocalMove(m_pASM);
				if (m_bAttack)
					Somersault_SweepSphere();
			})
			.AddTransition("Attack_Somersault to Idle", "Idle")
				.Predicator([this]
			{
				return  PriorityCondition()
					|| HitHeavyCondition()
					|| m_pASM->isSocketPassby("FullBody", 0.95f);
			})

///////////////////////////////////////////////////////////////////////////////////////////
	
		.AddState("Threat")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_160_AL_threat");
			})
			.AddTransition("Threat to Idle", "Idle")
				.Predicator([this]
				{
					return PriorityCondition() || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

///////////////////////////////////////////////////////////////////////////////////////////

		.Build();
}

void CEM0210::BeginTick()
{
	CEnemy::BeginTick();
	m_SoundStore.PlaySound("mon_5_voice_laugh");
}

void CEM0210::Tick(_double TimeDelta)
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
	m_eSASType = CheckSASType(ESASType::SAS_PENETRATE);

	if (m_eSASType == true)
		m_dRenderChangeDelay += TimeDelta;
	else
		m_dRenderChangeDelay = 0.0;

	if (m_IsFirstHit == false 
		&& m_eSASType
		&& m_eCurAttackType != EAttackType::ATK_END)
	{
		m_IsInvisible = false;
		m_bDownFirstHit = true;
		m_IsFirstHit = true;
	}

	// 투명 풀리기 디솔브
	if (m_IsInvisible == false)
	{
		for (auto pMtrl : m_pModelCom->GetMaterials())
		{
			if (pMtrl->GetParam().Floats[1] >= 1.f)
				break;
			pMtrl->GetParam().Floats[1] += (_float)TimeDelta * 0.3f;
		}
	}

	//ASM, FSM tick
	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);

	const _float fMoveSpeed = m_bRun ? 4.f : 2.f;


	if (m_vMoveAxis.LengthSquared() > 0.f)
	{
		const _float fYaw = m_pTransformCom->GetYaw_Radian();
		_float3 vVelocity;
		XMStoreFloat3(&vVelocity, fMoveSpeed * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}

	ResetHitData();
	
}

void CEM0210::Late_Tick(_double TimeDelta)
{
	CEnemy::Late_Tick(TimeDelta);

	if (m_IsInvisible)
	{
		if (m_dRenderChangeDelay >=0.5)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		else
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MESH_ALPHABLEND, this);
	}
	else
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}


void CEM0210::AfterPhysX()
{
	CEnemy::AfterPhysX();

	GetRigidBody("Range")->Update_Tick(m_pTransformCom->Get_WorldMatrix());
}

HRESULT CEM0210::Render()
{
	if (m_IsInvisible)
	{
		if (m_dRenderChangeDelay >= 0.7)
			m_pModelCom->Render(m_pTransformCom);
		else
			m_pModelCom->Render_Pass(m_pTransformCom, 5);
	}
	else
		m_pModelCom->Render(m_pTransformCom);

	return S_OK;
}

void CEM0210::Imgui_RenderProperty()
{
	CEnemy::Imgui_RenderProperty();
	if (ImGui::CollapsingHeader("ASM"))
	{
		m_pASM->Imgui_RenderState();
	}

	m_pFSM->Imgui_RenderProperty();

	if (ImGui::Button("Fire"))
		m_eDeBuff = EDeBuffType::DEBUFF_FIRE;

	if (ImGui::Button("Water"))
		m_eDeBuff = EDeBuffType::DEBUFF_WATER;

	if (ImGui::Button("Oil"))
		m_eDeBuff = EDeBuffType::DEBUFF_OIL;

	if (ImGui::Button("Thunder"))
		m_eDeBuff = EDeBuffType::DEBUFF_THUNDER;

}

_bool CEM0210::Exclude()
{
	return m_IsInvisible && CheckSASType(ESASType::SAS_PENETRATE) == false;
}

void CEM0210::PlayBC()
{
	__super::PlayBC();

	// 브레인 크러시 동작 중에는 모든 콜라이더를 사용하지 않도록 함
	m_pCollider->SetActive(false);
	for (auto& iter : m_pRigidBodies)
	{
		iter.second->Activate(false);
	}

	const _float fMinDist = 5.f; //플레이어에서 벽까지의 거리, 
	const _float fMaxDist = 7.f; //플레이어에서 몬스터까지의 거리

	m_CanFullBC = CanMove4BC(fMinDist);

	//가까우면 들어서 올림
	if (m_CanFullBC == false) return; 

	_vector vTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vTargetLook = m_pTarget->GetTransform()->Get_State(CTransform::STATE_LOOK);

	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float fDist = XMVectorGetX(XMVector4Length(vMyPos - vTargetPos));
	
	//너무 멀면 이동 안시킴
	if (fDist > fMaxDist)
	{
		m_CanFullBC = false;
		return;
	}

	//몬스터가 이동할 위치
//	vMyPos = vTargetPos + XMVector3Normalize(vTargetLook) * 4.f;
//	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vMyPos);
}

_bool CEM0210::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

void CEM0210::Dodge_VelocityCalc()
{
	m_bDodge = true;
	m_fGravity = 80.f;
	m_fYSpeed = 10.f;

	const _float fJumpMoveTime = (2 * m_fYSpeed) / m_fGravity;

	const _float fDistance = 5.f;
	const _vector vDirection = m_vDodgeDir;

	const _float fYaw = m_pTransformCom->GetYaw_Radian();
	m_vOnJumpMoveVelocity = XMVector3TransformNormal(vDirection * (fDistance / fJumpMoveTime), XMMatrixRotationY(fYaw));
}

void CEM0210::Play_LightHitAnim()
{
	if (m_eSimpleHitFrom == ESimpleAxis::NORTH)
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_401_AL_damage_l_F");
	else
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_402_AL_damage_l_B");
}

void CEM0210::Play_MidHitAnim()
{
	switch (m_eHitFrom)
	{
	case EBaseAxis::NORTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_411_AL_damage_m_F");
		break;
	case EBaseAxis::SOUTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_412_AL_damage_m_B");
		break;
	case EBaseAxis::WEST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_413_AL_damage_m_L");
		break;
	case EBaseAxis::EAST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_414_AL_damage_m_R");
		break;
	case EBaseAxis::AXIS_END:
		FALLTHROUGH;
	default:
		NODEFAULT;
	}
}

_bool CEM0210::CanMove4BC(_float fMinDist)
{
	_vector vTargetPos = m_pTarget->GetColliderPosition();
	physx::PxRaycastHit hitBuffer[1];
	physx::PxRaycastBuffer t(hitBuffer, 1);

	RayCastParams params;
	params.rayOut = &t;
	params.vOrigin = vTargetPos;
	params.vDir = m_pTarget->GetTransform()->Get_State(CTransform::STATE_LOOK);
	params.fDistance = 10.f;
	params.iTargetType = CTB_STATIC;
	params.bSingle = true;

	if (CGameInstance::GetInstance()->RayCast(params))
	{
		for (int i = 0; i < t.getNbAnyHits(); ++i)
		{
			auto p = t.getAnyHit(i);

			_float4 vPos{ p.position.x, p.position.y , p.position.z, 1.f };

			_float fDist = XMVectorGetX(XMVector4Length(vTargetPos - XMLoadFloat4(&vPos)));

			//최소거리보다 멀면 이동시켜서 브레인 크러쉬 연출
			return fDist > fMinDist ? true : false;
		}
	}
	
	//장애물이 없으니 무조건 가능
	return true;

}

//void CEM0210::Play_HitAdditiveAnim(_double TimeDelta)
//{
//	//AS_em0200_429_AL_damage_down_add
//	m_pModelCom->Set_AdditiveAnim("AS_em0200_429_AL_damage_down_add");
//	m_pModelCom->Play_Animation_Additive(TimeDelta);
//}

void CEM0210::Swing_SweepSphere()
{
	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);

	//Tail4가 꼬리 중앙에 있음
	_float4x4 BoneMatrix = GetBoneMatrix("RightToeBase") * m_pTransformCom->Get_WorldMatrix();
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

void CEM0210::Somersault_SweepSphere()
{
	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);

	//Tail4가 꼬리 중앙에 있음
	_float4x4 BoneMatrix = GetBoneMatrix("RightToeBase") * m_pTransformCom->Get_WorldMatrix();
	_float4 vBonePos = _float4{ BoneMatrix.m[3][0], BoneMatrix.m[3][1], BoneMatrix.m[3][2], BoneMatrix.m[3][3] };

	_vector	vDir = vBonePos - m_BeforePos;

	SphereSweepParams tParams;
	tParams.fVisibleTime = 0.2f;
	tParams.iTargetType = CTB_PLAYER;
	tParams.fRadius = 1.5f;
	tParams.fDistance = 1.f;
	tParams.vPos = vBonePos;
	tParams.sweepOut = &sweepOut;
	tParams.vUnitDir = vDir;

	if (CGameInstance::GetInstance()->SweepSphere(tParams))
	{
		HitTargets(sweepOut, static_cast<_int>(m_iAtkDamage * 1.f), EAttackType::ATK_TO_AIR);
	}

	m_BeforePos = vBonePos;
}

void CEM0210::HeavyAttackPushStart()
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

_bool CEM0210::PriorityCondition()
{
	return m_bCrushStart || m_bDead || m_bDownFirstHit || m_eDeBuff == EDeBuffType::DEBUFF_THUNDER;
}

_bool CEM0210::HitHeavyCondition()
{
	return m_eCurAttackType == EAttackType::ATK_HEAVY || m_eCurAttackType == EAttackType::ATK_SPECIAL_LOOP || m_eCurAttackType == EAttackType::ATK_SPECIAL_END || m_eCurAttackType == EAttackType::ATK_TO_AIR;
}


CEM0210 * CEM0210::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEM0210* pInstance = new CEM0210(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0210");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEM0210::Clone(void* pArg)
{
	CEM0210* pInstance = new CEM0210(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0210");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0210::Free()
{
	CEnemy::Free();
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
}
