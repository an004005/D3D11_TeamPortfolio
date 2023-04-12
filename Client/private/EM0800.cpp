#include "stdafx.h"
#include "..\public\EM0800.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "RigidBody.h"
#include "EM0800_AnimInstance.h"
#include "EM0800_Controller.h"
#include "ImguiUtils.h"
#include "Material.h"

#include "RigidBody.h"
#include "ControlledRigidBody.h"
#include "EMBrain.h"

CEM0800::CEM0800(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
	m_eEnemyName = EEnemyName::EM0800;
}

CEM0800::CEM0800(const CEM0800 & rhs)
	: CEnemy(rhs)
{
	m_SpawnEffectPivot = CImguiUtils::CreateMatrixFromImGuizmoData({0.f, 1.5f, 1.5f}, {0.f, 0.f, 0.f}, {1.3f,1.3f,1.3f});
	m_fSpawnDistortionDistancePivot = 3.f;
}

HRESULT CEM0800::Initialize(void * pArg)
{
	Json em0800_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BronJon/BronJonTrigger.json");
	pArg = &em0800_json;

	/*m_strDeathSoundTag = "mon_5_fx_death";
	m_strImpactVoiceTag = "mon_5_impact_voice";*/

	// 배치툴에서 조절할 수 있게 하기
	{
		m_iMaxHP = 55000;
		m_iHP = m_iMaxHP;

		m_iCrushGauge = m_iMaxHP;
		m_iMaxCrushGauge = m_iMaxHP * 0.7f;

		m_iAtkDamage = 50;
		iEemeyLevel = 2;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	m_eEnemyName = EEnemyName::EM0800;
	m_bHasCrushGauge = true;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(120.f));
	m_fGravity = 20.f;

	return S_OK;
}

void CEM0800::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em800", L"Model", (CComponent**)&m_pModelCom));

	m_pWeak = m_pModelCom->FindMaterial(L"MI_em0800_weak_0");
	assert(m_pWeak != nullptr);
	// 범위 충돌체(플레이어가 몬스터 위로 못 올라가게한다.)
	/*Json FlowerLegRangeCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/FlowerLeg/FlowerLegRange.json");
*/
	Json BronJonJaw = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BronJon/BronJonJaw.json");
	Json BronJonArm_L = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BronJon/BronJonLeftArm.json");
	Json BronJonArm_R = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BronJon/BronJonRightArm.json");
	Json BronJonRange = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BronJon/BronJonRange.json");

	Add_RigidBody("Range", &BronJonRange);
	Add_RigidBody("Trunk", &BronJonJaw);
	Add_RigidBody("Weak_LeftArm", &BronJonArm_L);
	Add_RigidBody("Weak_RightArm", &BronJonArm_R);

	// 컨트롤러, prototype안 만들고 여기서 자체생성하기 위함
	m_pController = CEM0800_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	// ASM
	m_pASM = CEM0800_AnimInstance::Create(m_pModelCom, this);
}

void CEM0800::SetUpSound()
{
	CEnemy::SetUpSound();

	m_SoundStore.CloneSound("croco_attack_splash");
	m_SoundStore.CloneSound("mon_4_attack_bite");
	m_SoundStore.CloneSound("mon_4_attack_laser");
	m_SoundStore.CloneSound("mon_4_groggy");
	m_SoundStore.CloneSound("mon_4_impact_fly");
	m_SoundStore.CloneSound("mon_4_move");

	m_pModelCom->Add_EventCaller("mon_4_move", [this] {m_SoundStore.PlaySound("mon_4_move", m_pTransformCom); });
}

void CEM0800::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();

	m_pModelCom->Add_EventCaller("Laser_Charge", [this]
	{
		_matrix			PivotMatrix = XMMatrixIdentity();
		PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0800_Laser")->Start_AttachPivot(this, PivotMatrix, "Eff02", true, true, true);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0800_Laser_Mouth")->Start_Attach(this, "Eff02", true);

	});

	m_pModelCom->Add_EventCaller("Laser_Launch", [this]
	{
		ClearDamagedTarget();
		m_bLaser = true;
	});

	//Bite 중 깨물었을때
	m_pModelCom->Add_EventCaller("Bite", [this]
	{
		ClearDamagedTarget();
		Bite_Overlap();

		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0800_Bite_OnHit")->Start_Attach(this, "Jaw", true);
		//m_SoundStore.PlaySound("mon_4_attack_bite", m_pTransformCom);
	});

	m_pModelCom->Add_EventCaller("Rolling1", [this]
		{
			_float4x4 WaterEffPivot = CImguiUtils::CreateMatrixFromImGuizmoData(
				{ 0.f, -0.8f, 0.f },
				{ 0.f, 0.f, 0.f },
				{ 0.5f, 0.2f, 0.5f });

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0800_Water_Dive_In")
				->Start_AttachPivot(this, WaterEffPivot, "Eff03", true, false, false);
		});

	m_pModelCom->Add_EventCaller("Rolling2", [this]
		{
			_float4x4 WaterEffPivot = CImguiUtils::CreateMatrixFromImGuizmoData(
				{ 0.f, -0.8f, 0.f },
				{ 0.f, 0.f, 0.f },
				{ 0.5f, 0.2f, 0.5f });

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0800_Water_Dive_In")
				->Start_AttachPivot(this, WaterEffPivot, "Eff03", true, false, false);
		});

	m_pModelCom->Add_EventCaller("Landing", [this]
		{
			_float4x4 WaterEffPivot = CImguiUtils::CreateMatrixFromImGuizmoData(
				{ 0.f, -0.5f, 0.f },
				{ 0.f, 0.f, 0.f },
				{ 1.f, 0.2f, 1.f });

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0800_Water_Dive_In")
				->Start_AttachPivot(this, WaterEffPivot, "Eff03", true, false, false);
		});

	//Submergence에서 물위로 뛰어오르는 타이밍
	m_pModelCom->Add_EventCaller("ComeUp", [this]
	{
		m_bComeUp = true;

		_float4x4 WaterEffPivot = CImguiUtils::CreateMatrixFromImGuizmoData(
			{ 0.f, 0.f, 0.f },
			{ 0.f, 0.f, 0.f },
			{ 0.5f, 0.5f, 0.5f });

		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0800_Water_Dive_In")
			->Start_AttachPivot(this, WaterEffPivot, "Eff03", true, false, true);
	});


	m_pModelCom->Add_EventCaller("DeadFlower", [this]
		{
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0800DeadFlower")
				->Start_NoAttach(this, false);
		});
}

void CEM0800::SetUpFSM()
{
	CEnemy::SetUpFSM();

	/*

	L : Laser
	S : Submergence
	B : Bite

	*/
	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")

			.AddTransition("Idle to BrainCrushStart", "BrainCrushStart")
				.Predicator([this] { return m_bCrushStart; })

			.AddTransition("Idle to Death" , "Death")
				.Predicator([this] { return m_bDead; })

			.AddTransition("Idle to Hit_Heavy", "Hit_Heavy")
				.Predicator([this] { return
					m_eCurAttackType == EAttackType::ATK_HEAVY
					|| m_eCurAttackType == EAttackType::ATK_SPECIAL_END; })
		
			.AddTransition("Idle to Bite", "Bite")
				.Predicator([this] { return m_eInput == CController::B; })

			.AddTransition("Idle to Laser_Start", "Laser_Start")
				.Predicator([this] { return m_eInput == CController::L; })

			.AddTransition("Idle to Submergence", "Submergence")
				.Predicator([this] { return m_eInput == CController::S; })

///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Hit_Heavy")
				.OnStart([this]
			{
				Play_MidHitAnim();
				HeavyAttackPushStart();
				m_SoundStore.PlaySound("mon_4_groggy", m_pTransformCom);

			})
			.Tick([this](_double TimeDelta)
			{
				SocketLocalMove(m_pASM);
				if (m_eCurAttackType == EAttackType::ATK_HEAVY
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
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Hit_Heavy to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead
						|| m_pASM->isSocketPassby("FullBody", 0.95f)
						|| m_eCurAttackType == EAttackType::ATK_SPECIAL_LOOP;
				})
		.AddState("Death")
				.OnStart([this]
				{
					m_pASM->InputAnimSocketOne("FullBody", "AS_em0800_424_AL_dead_down");
				})

		.AddState("OnFloorGetup")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketMany("FullBody", { "AS_em0800_427_AL_getup" });
			})
			.AddTransition("OnFloorGetup to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

///////////////////////////////////////////////////////////////////////////////////////////
	
///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("BrainCrushStart")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody",  "AS_em0800_485_AL_BCchance_start");
			})
			.AddTransition("BrainCrushStart to BrainCrushLoop", "BrainCrushLoop")
				.Predicator([this]
				{
					return m_bBrainCrush || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("BrainCrushLoop")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0800_486_AL_BCchance_loop");
				m_pModelCom->Find_Animation("AS_em0800_486_AL_BCchance_loop")->SetLooping(true);

				//브레인 생성
				m_pBrain = dynamic_cast<CEMBrain*>(CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_EnemyBrain"), TEXT("Prototype_EMBrain")));

				m_BCLoopTime = 0.0;
			})
			.Tick([this](_double TimeDelta)
			{
					_matrix WeakBoneMatrix = GetBoneMatrix("Weak01") * m_pTransformCom->Get_WorldMatrix();
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
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0800_487_AL_BCchance_end");
				m_pBrain->SetDelete();
				SetDead();
			})
		
		.AddState("BrainCrushEnd")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_BC_em0800m_em0800");
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
						const _float fPlayRatio = m_CanFullBC ? 0.8f : 0.55f;

						if (m_pASM->isSocketPassby("FullBody", fPlayRatio))
						{
							m_pBrain->EndBC();
							m_pBrain->SetDelete();
							m_pBrain = nullptr;
							SetDead();
						}
					}
				
			})

///////////////////////////////////////////////////////////////////////////////////////////

		//로컬 o, event에서 깨물때 데미지 처리
		.AddState("Bite")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0810_201_AL_atk_a1_bite");
				m_SoundStore.PlaySound("mon_4_attack_bite", m_pTransformCom);

			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Rush_Start to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})


		//로컬 x, event에서 레이서 충전, 시작 알림
		.AddState("Laser_Start")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0800_208_AL_atk_a4_laser_start");
				m_SoundStore.PlaySound("mon_4_attack_laser", m_pTransformCom);
			})
			.Tick([this](_double)
			{
				if (m_bLaser)
				{
					Laser_SweepSphere();
				}
			})
			.AddTransition("Laser_Start to Laser_Loop", "Laser_Loop")
				.Predicator([this]
			{
				return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
			})

		.AddState("Laser_Loop")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0800_209_AL_atk_a4_laser_loop");
				m_pModelCom->Find_Animation("AS_em0800_209_AL_atk_a4_laser_loop")->SetLooping(true);
				m_fLaserTime = 0.f;
			})
			.Tick([this](_double TimeDetla)
			{
				m_fLaserTime += static_cast<_float>(TimeDetla);
				Laser_SweepSphere();
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
				m_bLaser = false;
			})
			.AddTransition("Laser_Loop to Laser_End", "Laser_End")
				.Predicator([this]
			{
				return m_bDead || m_fLaserTime >= 3.f;
			})

		.AddState("Laser_End")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0800_210_AL_atk_a4_laser_end");
			})
			.AddTransition("Laser_End to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		
		//event에서 물위로 올라올때 알려줌(TICK에서 플레이어 바라보게)
		.AddState("Submergence")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em0800_212_AL_atk_a6_circumference2");
					m_SoundStore.PlaySound("croco_attack_splash", m_pTransformCom);

					ClearDamagedTarget();
				})
				.Tick([this](_double TimeDetla)
				{
					Submergence_Overlap();

					if (m_bComeUp)
						m_pTransformCom->LookAt_Smooth(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION), TimeDetla);
				})
				.OnExit([this]
				{
					m_bComeUp = false;
				})
				.AddTransition("Submergence to Idle", "Idle")
					.Predicator([this]
					{
						return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

///////////////////////////////////////////////////////////////////////////////////////////

		.Build();
}

void CEM0800::BeginTick()
{
	CEnemy::BeginTick();

	m_pLeftArmBugParticle = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0800_Bug_Particle");
	m_pLeftArmBugParticle->Start_Attach(this, "LeftForeArm", true, true);
	Safe_AddRef(m_pLeftArmBugParticle);

	m_pRightArmBugParticle = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0800_Bug_Particle");
	m_pRightArmBugParticle->Start_Attach(this, "RightForeArm", true, true);
	Safe_AddRef(m_pRightArmBugParticle);
}

void CEM0800::Tick(_double TimeDelta)
{
	CEnemy::Tick(TimeDelta);

	if (m_pWaterPool == nullptr && GetHpRatio() <= 0.5f)
	{
		_matrix			PivotMatrix = XMMatrixIdentity();
		PivotMatrix = XMMatrixTranslation(0.f, 0.2f, 0.f) * XMMatrixScaling(10.f, 1.f, 10.f);
		m_pWaterPool = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0800_Water");
		m_pWaterPool->Start_AttachPivot(this, PivotMatrix, "Eff01", true, true, false);
		Safe_AddRef(m_pWaterPool);
	}
	//AIInstance tick
	m_pController->SetTarget(m_pTarget);

	if (m_bDead == false)
		m_pController->Tick(TimeDelta);
	else
		m_pController->Invalidate();

	//변수 업데이트
	m_vMoveAxis = m_pController->GetMoveAxis();
	m_vMoveAxis.Normalize();
	m_eTurn = m_pController->GetBaseTurn();
	m_bTurn = m_pController->IsTurn();
	m_eInput = m_pController->GetAIInput();

	//ASM, FSM tick
	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);

	const _float fMoveSpeed = 1.2f;

	if (m_vMoveAxis.LengthSquared() > 0.f)
	{
		_float3 vVelocity;
		XMStoreFloat3(&vVelocity, fMoveSpeed * XMVector3Normalize(m_vMoveAxis));
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}

	m_Sound_Fly.Tick(TimeDelta);
	if(m_Sound_Fly.Use())
		m_SoundStore.PlaySound("mon_4_impact_fly", m_pTransformCom);

	// Tick의 제일 마지막에서 실행한다.
	ResetHitData();
}

void CEM0800::Late_Tick(_double TimeDelta)
{
	CEnemy::Late_Tick(TimeDelta);
}

void CEM0800::AfterPhysX()
{
	CEnemy::AfterPhysX();

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	GetRigidBody("Range")->Update_Tick(WorldMatrix);
	GetRigidBody("Trunk")->Update_Tick(m_pModelCom->GetBoneMatrix("Eff02") * WorldMatrix);
	GetRigidBody("Weak_LeftArm")->Update_Tick(m_pModelCom->GetBoneMatrix("LeftForeArm") * WorldMatrix);
	GetRigidBody("Weak_RightArm")->Update_Tick(m_pModelCom->GetBoneMatrix("RightForeArm") * WorldMatrix);
}

HRESULT CEM0800::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CEM0800::Imgui_RenderProperty()
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

		if (ImGui::Button("Test0800Effect"))
		{
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0800_Water_Dive_In")
				->Start_AttachPivot(this, pivot, "Eff03", true, false, false);
		}

		if (ImGui::Button("Test0800Effect2"))
		{
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0800_Water_Attack_Particle")
				->Start_AttachPivot(this, pivot, "Eff03", true, false, false);
		}
	}
}

_bool CEM0800::IsWeak(CRigidBody* pHitPart)
{
	return 	pHitPart == GetRigidBody("Weak_LeftArm") || pHitPart == GetRigidBody("Weak_RightArm");
}

void CEM0800::PlayBC()
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
}

_float4 CEM0800::GetKineticTargetPos()
{
	_uint random = CMathUtils::RandomUInt(1);

	_float4	LeftWeak = XMLoadFloat4x4(&GetRigidBody("Weak_LeftArm")->GetPxWorldMatrix()).r[3];
	_float4 RightWeak = XMLoadFloat4x4(&GetRigidBody("Weak_RightArm")->GetPxWorldMatrix()).r[3];
	
	return random == 0 ? LeftWeak : RightWeak;
}

_bool CEM0800::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

void CEM0800::Play_LightHitAnim()
{
	if (m_eSimpleHitFrom == ESimpleAxis::NORTH)
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0800_401_AL_damage_l_F");
	else
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0800_402_AL_damage_l_B");
}

void CEM0800::Play_MidHitAnim()
{
	switch (m_eHitFrom)
	{
	case EBaseAxis::NORTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0800_411_AL_damage_m_F");
		break;
	case EBaseAxis::EAST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0800_414_AL_damage_m_R");
		break;
	case EBaseAxis::SOUTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0800_412_AL_damage_m_B");
		break;
	case EBaseAxis::WEST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0800_413_AL_damage_m_L");
		break;
	case EBaseAxis::AXIS_END:
		FALLTHROUGH;
	default:
		NODEFAULT;
	}
}

void CEM0800::HeavyAttackPushStart()
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

_bool CEM0800::CanMove4BC(_float fMinDist)
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

EBaseTurn CEM0800::FindTargetDirection()
{
	_vector vTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	
	_float fAngle = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vMyLook), XMVector3Normalize(vTargetPos - vMyPos)));

	//뒤에 있음
	if (fAngle < 0)	
	{
		_vector vMyRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		fAngle = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vMyRight), XMVector3Normalize(vTargetPos - vMyPos)));

		if (fAngle > 0) //오른쪽에 있음
			return EBaseTurn::TURN_RIGHT;
		else
			return EBaseTurn::TURN_LEFT;
	}

	return EBaseTurn::TURN_END;
}

void CEM0800::HitWeakProcess(_double TimeDelta)
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

void CEM0800::Bite_Overlap()
{
	_float fLength = 2.f;

	_matrix BoneMatrix = m_pModelCom->GetBoneMatrix("Jaw") * m_pTransformCom->Get_WorldMatrix();

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
	param.iTargetType = CTB_PLAYER;
	param.fRadius = 1.3f;
	param.vPos = XMVectorSetW(fFinish, 1.f);
	param.overlapOut = &overlapOut;

	if (CGameInstance::GetInstance()->OverlapSphere(param))
	{
		HitTargets(overlapOut, m_iAtkDamage * 2.f, EAttackType::ATK_HEAVY);
	}
}

void CEM0800::Submergence_Overlap()
{
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	physx::PxOverlapHit hitBuffer[3];
	physx::PxOverlapBuffer overlapOut(hitBuffer, 3);

	SphereOverlapParams param;
	param.fVisibleTime = 0.1f;
	param.iTargetType = CTB_PLAYER;
	param.fRadius = 3.f;
	param.vPos = vMyPos;
	param.overlapOut = &overlapOut;

	if (CGameInstance::GetInstance()->OverlapSphere(param))
	{
		HitTargets(overlapOut, m_iAtkDamage * 2.f, EAttackType::ATK_TO_AIR);
	}
}

void CEM0800::Laser_SweepSphere()
{
	Matrix mLaserMatrix = GetBoneMatrix("Eff02") * m_pTransformCom->Get_WorldMatrix();
	_float4 vLaserPos = _float4(mLaserMatrix.Translation().x, mLaserMatrix.Translation().y, mLaserMatrix.Translation().z, 1.f);

	physx::PxSweepHit hitBuffer[5];
	physx::PxSweepBuffer sweepOut(hitBuffer, 5);

	SphereSweepParams Sparam;   
	Sparam.fVisibleTime = 0.f;
	Sparam.iTargetType = CTB_PLAYER;
	Sparam.fRadius = 1.4f;
	Sparam.fDistance = 20.f;
	Sparam.vPos = vLaserPos;
	Sparam.sweepOut = &sweepOut;
	Sparam.vUnitDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	if (CGameInstance::GetInstance()->SweepSphere(Sparam))
		HitTargets(sweepOut, (rand() % 150) + 25, EAttackType::ATK_HEAVY);
}


CEM0800 * CEM0800::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEM0800* pInstance = new CEM0800(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0800");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEM0800::Clone(void * pArg)
{
	CEM0800*		pInstance = new CEM0800(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0800");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0800::Free()
{
	CEnemy::Free();
	Safe_Release(m_pASM);
	Safe_Release(m_pController);

	if (m_bCloned)
	{
		if (m_pWaterPool != nullptr)
		{
			m_pWaterPool->SetDelete();
			Safe_Release(m_pWaterPool);
			m_pWaterPool = nullptr;
		}

		if (m_pLeftArmBugParticle != nullptr)
		{
			m_pLeftArmBugParticle->SetDelete();
			Safe_Release(m_pLeftArmBugParticle);
			m_pLeftArmBugParticle = nullptr;
		}

		if (m_pRightArmBugParticle != nullptr)
		{
			m_pRightArmBugParticle->SetDelete();
			Safe_Release(m_pRightArmBugParticle);
			m_pRightArmBugParticle = nullptr;
		}
	}
}
