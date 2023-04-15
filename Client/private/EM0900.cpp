#include "stdafx.h"
#include "..\public\EM0900.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "EM0900_AnimInstance.h"
#include "EM0900_Controller.h"
#include "ImguiUtils.h"
#include "BulletBuilder.h"
#include "Material.h"
#include "RigidBody.h"
#include "ControlledRigidBody.h"
#include "EMBrain.h"
#include "EMGift.h"

CEM0900::CEM0900(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
	m_eEnemyName = EEnemyName::EM0900;
}

CEM0900::CEM0900(const CEM0900 & rhs)
	: CEnemy(rhs)
{
	m_bSpawnEffect = false;
}

HRESULT CEM0900::Initialize(void * pArg)
{
	Json em900_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em0900/em0900Base.json");
	pArg = &em900_json;


	// 초기값 지정. LEVEL_NOW 에 따라
	{
		m_iMaxHP = LEVEL_NOW * (250 + (CMathUtils::RandomUInt(10)));
		m_iHP = m_iMaxHP;

		m_iMaxCrushGauge = m_iMaxHP * 0.7f;
		m_iCrushGauge = m_iMaxCrushGauge;

		iEemeyLevel = ((LEVEL_NOW - 20) * 4) + (CMathUtils::RandomUInt(3) + 1);
		m_iAtkDamage = iEemeyLevel * (CMathUtils::RandomUInt(4) + 8);

		m_eEnemyName = EEnemyName::EM0900;
		m_bHasCrushGauge = true;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));


	m_pTransformCom->SetRotPerSec(XMConvertToRadians(90.f));

	m_fGravity = 20.f;

	//브레인 생성
	m_pBrain = dynamic_cast<CEMBrain*>(CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_Brain"), TEXT("Prototype_EMBrain")));
	assert(m_pBrain != nullptr);

	return S_OK;
}

void CEM0900::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em900", L"Model", (CComponent**)&m_pModelCom));

	m_pWeak = m_pModelCom->FindMaterial(L"MI_em0900_weak_0");
	m_pArmor = m_pModelCom->FindMaterial(L"MI_em0900_armor_0");

	assert(m_pWeak != nullptr);
	assert(m_pArmor != nullptr);

	m_pWeak->SetActive(false); //약점이 갑옷밖으로 삐져나와서 꺼둠

	Json WeakCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em0900/em0900Weak.json");

	Add_RigidBody("Weak", &WeakCol);
	
	// 컨트롤러, prototype안 만들고 여기서 자체생성하기 위함
	m_pController = CEM0900_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	// ASM
	m_pASM = CEM0900_AnimInstance::Create(m_pModelCom, this);
}

void CEM0900::SetUpSound()
{
	CEnemy::SetUpSound();
}

void CEM0900::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();
	
	//Feint
	m_pModelCom->Add_EventCaller("Feint_1Atk_Start", [this]
	{
		ClearDamagedTarget();
		m_bAttack = true;
	});
	m_pModelCom->Add_EventCaller("Feint_1Atk_End", [this]
	{
		m_bAttack = false;
	});
	m_pModelCom->Add_EventCaller("Feint_2Atk_Start", [this]
	{
		ClearDamagedTarget();
		m_bAttack = true;
	});
	m_pModelCom->Add_EventCaller("Feint_2Atk_End", [this]
	{
		m_bAttack = false;
	});

	//4Attack 땅에 내려치는 타이밍
	m_pModelCom->Add_EventCaller("1Attack", [this]
	{
		ClearDamagedTarget();
		FourAttack_Overlap();
	});
	m_pModelCom->Add_EventCaller("2Attack", [this]
	{
		ClearDamagedTarget();
		FourAttack_Overlap();
	});
	m_pModelCom->Add_EventCaller("3Attack", [this]
	{
		ClearDamagedTarget();
		FourAttack_Overlap();
	});
	m_pModelCom->Add_EventCaller("4Attack", [this]
	{
		ClearDamagedTarget();
		FourAttack_Overlap(true);
	});

	//Swing
	m_pModelCom->Add_EventCaller("Swing_Start", [this]
	{
		ClearDamagedTarget();
		m_bAttack = true;
	});
	m_pModelCom->Add_EventCaller("Swing_End", [this]
	{
		m_bAttack = false;
	});

	//Gift 생성 타이밍
	m_pModelCom->Add_EventCaller("Take_gift_out", [this]
	{
			m_pGift = dynamic_cast<CEMGift*>(CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_MonsterUse"), TEXT("Prototype_EMGift")));
			m_pGift->Set_Owner(this);
			Select_Gift();
	});

	//Gift target에 던질 타이밍
	m_pModelCom->Add_EventCaller("Throw_gift", [this]
	{
		m_pGift->DoThrow(m_pTarget->GetColliderPosition());

		//던지고 버림
		m_pGift = nullptr;
	});



	m_pModelCom->Add_EventCaller("Dead_Flower", [this]
	{
		/*	CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em1100DeadFlower")
				->Start_NoAttach(this, false);*/
	});
}

void CEM0900::SetUpFSM()
{
	CEnemy::SetUpFSM();

	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.OnStart([this]
			{
				m_fGravity = 20.f;
			})
			.AddTransition("Idle to BrainCrushStart", "BrainCrushStart")
				.Predicator([this] { return m_bCrushStart; })

			.AddTransition("Idle to Death" , "Death")
				.Predicator([this] { return m_bDead; })

			.AddTransition("Idle to Hit_Heavy", "Hit_Heavy")
				.Predicator([this] { return
					m_bDestroyArmor
					||m_eCurAttackType == EAttackType::ATK_SPECIAL_END; })

			.AddTransition("Idle to Hit_Light", "Hit_Light")
				.Predicator([this] { return
					m_eCurAttackType == EAttackType::ATK_SPECIAL_LOOP
					|| m_eCurAttackType == EAttackType::ATK_HEAVY; })


			.AddTransition("Idle to 4Attack", "4Attack")
				.Predicator([this] { return m_eInput == CController::F; })

			.AddTransition("Idle to Spin", "Spin")
				.Predicator([this] { return m_eInput == CController::S; })

			.AddTransition("Idle to Throw_OmenStart", "Throw_OmenStart")
				.Predicator([this] { return m_eInput == CController::T; })
			
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
					return PriorityCondition()
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
				if (m_eCurAttackType == EAttackType::ATK_SPECIAL_END)
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
					return m_bDead || m_bCrushStart
						|| m_pASM->isSocketPassby("FullBody", 0.95f)
						|| (m_eCurAttackType == EAttackType::ATK_SPECIAL_LOOP && !m_bDestroyArmor)
						|| (m_eCurAttackType == EAttackType::ATK_HEAVY && !m_bDestroyArmor);
				})

		.AddState("Death")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0900_424_AL_dead_down");
			})

///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("BrainCrushStart")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody",  "AS_em0900_485_AL_BCchance_start");
			})
			.AddTransition("BrainCrushStart to BrainCrushLoop", "BrainCrushLoop")
				.Predicator([this]
				{
					return m_bBrainCrush || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("BrainCrushLoop")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0900_486_AL_BCchance_loop");
				m_pModelCom->Find_Animation("AS_em0900_486_AL_BCchance_loop")->SetLooping(true);

				//브레인 생성
				m_pBrain->InitBC();
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
					return m_BCLoopTime >= 4.0;
				})
			.AddTransition("BrainCrushLoop to BrainCrushEnd", "BrainCrushEnd")
				.Predicator([this]
					{
						return	m_bBrainCrush;
					})
		.AddState("BrainCrushLoopDead")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0900_487_AL_BCchance_end");
				m_pBrain->SetDelete();
				SetDead();
			})
		
		.AddState("BrainCrushEnd")
			.OnStart([this]
			{ 
				m_pASM->InputAnimSocketOne("FullBody", "AS_BC_em_common_em0900");
				m_pBrain->BeginBC();
			})
			.Tick([this](_double)
			{
					SocketLocalMove(m_pASM);

					if (m_pBrain != nullptr)
					{
						_matrix WeakBoneMatrix = GetBoneMatrix("Weak01") * m_pTransformCom->Get_WorldMatrix();
						m_pBrain->GetTransform()->Set_WorldMatrix(WeakBoneMatrix);

						if (m_pASM->isSocketPassby("FullBody", 0.5f))
						{
							m_pBrain->EndBC();
							m_pBrain->SetDelete();
							m_pBrain = nullptr;
							SetDead();
						}
					}
				
			})

///////////////////////////////////////////////////////////////////////////////////////////
		
		//때릴때마다 이벤트로 공격
		.AddState("4Attack")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0900_205_AL_atk_a5_4attack");
			})
			.Tick([this](_double TimeDelta)
			{
				m_pTransformCom->LookAt_Smooth(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION), TimeDelta);
			})
			.AddTransition("4Attack to Idle", "Idle")
				.Predicator([this]
				{
					return PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Spin")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0900_207_AL_atk_a7");
				m_bAttack = false;
				m_dLoopTick = 0.5;

			})
			.Tick([this](_double TimeDelta)
			{
				SocketLocalMove(m_pASM);
				if (m_bAttack)
				{
					Spin_SweepSphere();

					m_dLoopTick += TimeDelta;
					if (m_dLoopTick > 0.3)
					{
						m_dLoopTick = 0.0;
						_matrix PivotMatrix = XMMatrixIdentity();
						PivotMatrix = XMMatrixScaling(2.6f,2.f, 2.6f);
						CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0320_Spin_Attack")->Start_AttachPivot(this, PivotMatrix, "Target", true, false, false);
					}
				}
								
			})
			.AddTransition("Spin to Idle", "Idle")
				.Predicator([this]
				{
					return PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Throw_OmenStart")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0900_218_AL_atk_a1_omenstart");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.AddTransition("Throw_OmenStart to Throw_OmenLoop", "Throw_OmenLoop")
				.Predicator([this]
				{
					return PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Throw_OmenLoop")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0900_219_AL_atk_a1_omensloop");
				m_dLoopTick = 0.0;
			})
			.Tick([this](_double TimeDelta)
			{
				SocketLocalMove(m_pASM);
				m_dLoopTick += TimeDelta;
			})
			.AddTransition("Throw_OmenLoop to Feint", "Feint")
				.Predicator([this]
				{
					return  PriorityCondition() || (m_dLoopTick>= 0.5 && m_pController->GetGapDistance() < 7.f);
				})
			.AddTransition("Throw_OmenLoop to Throw_Start", "Throw_Start")
				.Predicator([this]
				{
					return PriorityCondition() || (m_dLoopTick >= 0.5 && m_pController->GetGapDistance() >= 7.f);
				})

		.AddState("Feint")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0900_202_AL_atk_a2_feint");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);

				if (m_bAttack)
					Feint_SweepSphere();
			})
			.AddTransition("Feint to Idle", "Idle")
				.Predicator([this]
				{
					return PriorityCondition() || m_bDestroyArmor || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Throw_Start")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0900_220_AL_atk_a1_start");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.AddTransition("Throw_Start to Throw_Loop", "Throw_Loop")
				.Predicator([this]
				{
					return PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Throw_Loop")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0900_221_AL_atk_a1_loop");
			})
			.Tick([this](_double TimeDelta)
			{
					m_pTransformCom->LookAt_Smooth(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION), TimeDelta);
			})
			.AddTransition("Throw_Loop to Throw_End", "Throw_End")
				.Predicator([this]
				{
					return PriorityCondition() || IsTargetFront(15.f);
				})

		//이벤트에서 플레이어쪽으로 던짐
		.AddState("Throw_End")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0900_222_AL_atk_a1_end");
			})
			.AddTransition("Throw_End to Idle", "Idle")
				.Predicator([this]
				{
					return PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
				})
	
///////////////////////////////////////////////////////////////////////////////////////////

		.Build();
}

void CEM0900::BeginTick()
{
	CEnemy::BeginTick();
	m_iArmorHp = m_iMaxHP * 0.3f;
}

void CEM0900::Tick(_double TimeDelta)
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


	if (m_vMoveAxis.LengthSquared() > 0.f)
	{
		_float3 vVelocity;
		XMStoreFloat3(&vVelocity, 2.f /*MoveSpeed*/ * XMVector3Normalize(m_vMoveAxis));
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}

	HitWeakProcess(TimeDelta);

	// Tick의 제일 마지막에서 실행한다.
	ResetHitData();
}

void CEM0900::Late_Tick(_double TimeDelta)
{
	CEnemy::Late_Tick(TimeDelta);
}

void CEM0900::AfterPhysX()
{
	CEnemy::AfterPhysX();

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	GetRigidBody("Weak")->Update_Tick(m_pModelCom->GetBoneMatrix("Target") * WorldMatrix);
}

HRESULT CEM0900::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CEM0900::Imgui_RenderProperty()
{
	CEnemy::Imgui_RenderProperty();
	if (ImGui::CollapsingHeader("ASM"))
	{
		m_pASM->Imgui_RenderState();
	}
	m_pFSM->Imgui_RenderProperty();

	if (ImGui::Button("BrainCrashOn"))
	{
		m_bCrushStart = true;
	}

	//static _bool tt = false;
	//ImGui::Checkbox("Modify Pivot", &tt);

	//if (tt)
	//{
	//	static GUIZMO_INFO tInfo;
	//	CImguiUtils::Render_Guizmo(&pivot, tInfo, true, true);

	//	if (ImGui::Button("TetsEffect"))
	//	{

	//		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em1100_Elec_Bullet_Loop")
	//		->Start_NoAttachPivot(this, pivot, true, true);
	//	}
	//}
}

_bool CEM0900::IsWeak(CRigidBody* pHitPart)
{
	return true;
}

void CEM0900::CheckHP(DAMAGE_PARAM& tDamageParams)
{
	if (m_bHitWeak && m_iArmorHp > 0)
	{
		m_iArmorHp -= tDamageParams.iDamage;

		//데미지 폰트 안띄우려고 
		tDamageParams.iDamage = 1;

		//hp가 0보다 작아지면 아머 삭제. 
		if (m_iArmorHp <= 0)
			m_bDestroyArmor = true;
	}
	else
		__super::CheckHP(tDamageParams);
}

_bool CEM0900::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

void CEM0900::Play_LightHitAnim()
{
	if (m_eSimpleHitFrom == ESimpleAxis::NORTH)
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0900_401_AL_damage_l_F");
	else
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0900_402_AL_damage_l_B");
}

void CEM0900::Play_MidHitAnim()
{
	switch (m_eHitFrom)
	{
	case EBaseAxis::NORTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0900_411_AL_damage_m_F");
		break;
	case EBaseAxis::SOUTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0900_412_AL_damage_m_B");
		break;
	case EBaseAxis::WEST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0900_413_AL_damage_m_L");
		break;
	case EBaseAxis::EAST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0900_414_AL_damage_m_R");
		break;
	case EBaseAxis::AXIS_END:
		FALLTHROUGH;
	default:
		NODEFAULT;
	}
}

void CEM0900::HeavyAttackPushStart()
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

void CEM0900::HitWeakProcess(_double TimeDelta)
{
	if (m_bHitWeak)
	{
		if (m_iArmorHp > 0)
		{
			m_bArmorProcess = true;
			m_pArmor->GetParam().Floats[1] = 1.f;
		}
		else
		{
			m_bWeakProcess = true;
			m_pWeak->GetParam().Floats[1] = 1.f;
		}
	}

	//1번째가 약점 맞았을때 반짝거리는거
	//2번째가 아머 삭제될때 디졸브

	if (m_bArmorProcess)
	{
		m_pArmor->GetParam().Floats[1] -= static_cast<_float>(TimeDelta);

		if (m_pArmor->GetParam().Floats[1] <= 0.f)
		{
			m_pArmor->GetParam().Floats[1] = 0.f;
			m_bArmorProcess = false;
		}

	}

	else if (m_bWeakProcess)
	{
		m_pWeak->GetParam().Floats[1] -= static_cast<_float>(TimeDelta);

		if (m_pWeak->GetParam().Floats[1] <= 0.f)
		{
			m_pWeak->GetParam().Floats[1] = 0.f;
			m_bWeakProcess = false;
		}
	}

	//아머가 파괴될때 한번 실행
	if (m_bDestroyArmor == true)
	{
		m_pArmor->GetParam().Floats[2] += static_cast<_float>(TimeDelta);

		if (m_pArmor->GetParam().Floats[2] >= 1.f)
		{
			m_pArmor->GetParam().Floats[2] = 1.f;
			m_pWeak->SetActive(true);
			m_bDestroyArmor = false;
		}
	}
}


_bool CEM0900::PriorityCondition()
{
	return m_bCrushStart || m_bDead || m_bDestroyArmor || m_eCurAttackType == EAttackType::ATK_SPECIAL_END;
}
 

void CEM0900::FourAttack_Overlap(_bool Final)
{
	_matrix BoneMatrix = m_pModelCom->GetBoneMatrix("HeadTenctacleB9") * m_pTransformCom->Get_WorldMatrix();

	_vector vBoneVector = BoneMatrix.r[3];
	_float3 fBone = vBoneVector;

	physx::PxOverlapHit hitBuffer[3];
	physx::PxOverlapBuffer overlapOut(hitBuffer, 3);

	SphereOverlapParams param;
	param.fVisibleTime = 0.1f;
	param.iTargetType = CTB_PLAYER;
	param.fRadius = Final ? 3.f : 2.f;
	param.vPos = XMVectorSetW(fBone, 1.f);
	param.overlapOut = &overlapOut;

	if (CGameInstance::GetInstance()->OverlapSphere(param))
	{
		if(Final == true)
			HitTargets(overlapOut, static_cast<_int>(m_iAtkDamage * 2.f), EAttackType::ATK_HEAVY);
		else
			HitTargets(overlapOut, static_cast<_int>(m_iAtkDamage * 1.5f), EAttackType::ATK_LIGHT);
	}
}

void CEM0900::Feint_SweepSphere()
{
	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);

	_float4x4 BoneMatrix = GetBoneMatrix("HeadTenctacleB9") * m_pTransformCom->Get_WorldMatrix();
	_float4 vBonePos = _float4{ BoneMatrix.m[3][0], BoneMatrix.m[3][1], BoneMatrix.m[3][2], BoneMatrix.m[3][3] };

	_vector	vDir = vBonePos - m_BeforePos;

	SphereSweepParams tParams;
	tParams.fVisibleTime = 0.2f;
	tParams.iTargetType = CTB_PLAYER;
	tParams.fRadius = 2.f;
	tParams.fDistance = XMVectorGetX(XMVector4Length(vDir));
	tParams.vPos = vBonePos;
	tParams.sweepOut = &sweepOut;
	tParams.vUnitDir = XMVector3Normalize(vDir);

	if (CGameInstance::GetInstance()->SweepSphere(tParams))
	{
		HitTargets(sweepOut, static_cast<_int>(m_iAtkDamage * 1.2f), EAttackType::ATK_HEAVY);
	}

	m_BeforePos = vBonePos;
}

void CEM0900::Spin_SweepSphere()
{
	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);

	_float4x4 BoneMatrix = GetBoneMatrix("HeadTenctacleB4") * m_pTransformCom->Get_WorldMatrix();
	_float4 vBonePos = _float4{ BoneMatrix.m[3][0], BoneMatrix.m[3][1], BoneMatrix.m[3][2], BoneMatrix.m[3][3] };

	_vector	vDir = vBonePos - m_BeforePos;

	SphereSweepParams tParams;
	tParams.fVisibleTime = 0.2f;
	tParams.iTargetType = CTB_PLAYER;
	tParams.fRadius = 5.f;
	tParams.fDistance = XMVectorGetX(XMVector4Length(vDir));
	tParams.vPos = vBonePos;
	tParams.sweepOut = &sweepOut;
	tParams.vUnitDir = XMVector3Normalize(vDir);

	if (CGameInstance::GetInstance()->SweepSphere(tParams))
	{
		HitTargets(sweepOut, static_cast<_int>(m_iAtkDamage * 1.2f), EAttackType::ATK_HEAVY);
	}

	m_BeforePos = vBonePos;
}

void CEM0900::Select_Gift()
{
	switch (m_pTarget->GetDeBuffType())
	{
	case EDeBuffType::DEBUFF_FIRE:
		m_pGift->Select_Gift(EDeBuffType::DEBUFF_FIRE);
		break;
	case EDeBuffType::DEBUFF_WATER:
		m_pGift->Select_Gift(EDeBuffType::DEBUFF_THUNDER);
		break;
	case EDeBuffType::DEBUFF_OIL:
		m_pGift->Select_Gift(EDeBuffType::DEBUFF_FIRE);
		break;
	case EDeBuffType::DEBUFF_THUNDER:
		m_pGift->Select_Gift(EDeBuffType::DEBUFF_THUNDER);
		break;
	default:
		if (m_ePreGiftType == EDeBuffType::DEBUFF_OIL)
		{
			m_pGift->Select_Gift(EDeBuffType::DEBUFF_WATER);
			m_ePreGiftType = EDeBuffType::DEBUFF_WATER;
		}
		else
		{
			m_pGift->Select_Gift(EDeBuffType::DEBUFF_OIL);
			m_ePreGiftType = EDeBuffType::DEBUFF_OIL;
		}
		break;
	}
}



CEM0900 * CEM0900::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEM0900* pInstance = new CEM0900(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0900");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEM0900::Clone(void * pArg)
{
	CEM0900*		pInstance = new CEM0900(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0900");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0900::Free()
{
	CEnemy::Free();
	Safe_Release(m_pASM);
	Safe_Release(m_pController);

	if (m_pGift != nullptr)
	{
		m_pGift->SetDelete();
		m_pGift = nullptr;
	}
}
