#include "stdafx.h"
#include "..\public\EM0750.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "RigidBody.h"
#include "EM0750_AnimInstance.h"
#include "EM0750_Controller.h"
#include "ImguiUtils.h"

CEM0750::CEM0750(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
	m_eEnemyName = EEnemyName::EM0750;
}

CEM0750::CEM0750(const CEM0750 & rhs)
	: CEnemy(rhs)
{
	m_SpawnEffectPivot = CImguiUtils::CreateMatrixFromImGuizmoData({0.f, 2.3f, 0.f}, {0.f, 0.f, 0.f}, {0.65f,0.65f,0.65f});
	m_fSpawnDistortionDistancePivot = 0.5f;
}

HRESULT CEM0750::Initialize(void * pArg)
{
	Json em0750_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/SkummyPandou/SkummyPandouTrigger.json");
	pArg = &em0750_json;

	m_strDeathSoundTag = "voidfly_fx_death";

	// 초기값 지정. LEVEL_NOW 에 따라
	{
		_uint iBaseLevel = max(0, _int(LEVEL_NOW - 20));

		m_iMaxHP = LEVEL_NOW * (50 + (CMathUtils::RandomUInt(10)));
		m_iHP = m_iMaxHP;

		m_iMaxCrushGauge = m_iMaxHP * 0.7f;
		m_iCrushGauge = m_iMaxCrushGauge;

		iEemeyLevel = (iBaseLevel * 2.5) + CMathUtils::RandomUInt(3) + 1;
		m_iAtkDamage = iEemeyLevel * (CMathUtils::RandomUInt(4) + 8);

		m_eEnemyName = EEnemyName::EM0750;
		m_bHasCrushGauge = false;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(180.f));
	m_pTransformCom->SetSpeed(15.f);

	m_fGravity = 0.f;
	return S_OK;
}

void CEM0750::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em750", L"Model", (CComponent**)&m_pModelCom));


	// 컨트롤러, prototype안 만들고 여기서 자체생성하기 위함
	m_pController = CEM0750_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	// ASM
	m_pASM = CEM0750_AnimInstance::Create(m_pModelCom, this);
}

void CEM0750::SetUpSound()
{
	CEnemy::SetUpSound();

	m_SoundStore.CloneSound("mon_3_attack_airdash");
	m_SoundStore.CloneSound("mon_3_attack_airdash_up");
	m_SoundStore.CloneSound("mon_3_move");
	m_SoundStore.CloneSound("mon_3_provoke");

	m_pModelCom->Add_EventCaller("mon_3_move", [this] {m_SoundStore.PlaySound("mon_3_move", m_pTransformCom); });
}

void CEM0750::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();

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
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0750DeadFlower")
				->Start_NoAttach(this, false);
		});
}

void CEM0750::SetUpFSM()
{
	CEnemy::SetUpFSM();

	/*
	NUM_1 : Attack_a1 (돌진 공격)
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

			.AddTransition("Idle to Threat", "Threat")
				.Predicator([this] { return m_eInput == CController::SHIFT; })

			.AddTransition("Idle to Rush_Start", "Rush_Start")
				.Predicator([this] { return m_eInput == CController::C; })


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
					m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_474_AL_dead_down02");
				})

		.AddState("Hit_ToAir")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_432_AL_blow_start_F");
				Check_Height();
				m_fGravity = 20.f;
				m_fYSpeed = sqrtf((2.5f - m_fHeight) * m_fGravity * 2.f);

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
					m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_455_AL_rise_start");
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
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_433_AL_blow_landing_F");
				m_fGravity = 20.f;
			})
			.AddTransition("Landing to Getup", "Getup")
				.Predicator([this]
				{
					return  m_pASM->isSocketPassby("FullBody", 0.98f);
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

		.AddState("Down")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_475_AL_down_start02");
				Check_Height();
				m_fGravity = 20.f;
				m_fYSpeed = sqrtf((2.5f - m_fHeight) * m_fGravity * 2.f);

				m_bDown = true;
			})
			.Tick([this](_double)
			{
				m_bHitAir = true;
			})
			.OnExit([this]
			{
				m_bHitAir = false;
			})
			.AddTransition("Down to Shock", "Shock")
				.Predicator([this]
				{
						return !m_bPreOnFloor && m_bOnFloor && m_bHitAir;
				})

		.AddState("Shock")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_480_AL_down_shock");
				m_pModelCom->Find_Animation("AS_em0700_480_AL_down_shock")->SetLooping(true);
				m_fGravity = 20.f;
			})
			.AddTransition("Shock to Getup", "Getup")
				.Predicator([this]
				{
					return  m_eDeBuff != EDeBuffType::DEBUFF_THUNDER;
				})
			.AddTransition("Shock to Hit_ToAir", "Hit_ToAir")
				.Predicator([this]
				{
					return m_eCurAttackType == EAttackType::ATK_TO_AIR;
				})

		.AddState("Getup")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_427_AL_getup");
				m_fGravity = 20.f;
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

		.AddState("Threat")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_160_AL_threat");
				m_SoundStore.PlaySound("mon_3_provoke", m_pTransformCom);
			})
			.AddTransition("Threat to Idle", "Idle")
				.Predicator([this]
				{
					return PriorityCondition() || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

///////////////////////////////////////////////////////////////////////////////////////////

		//시작과 끝은 로컬이 있음
		.AddState("Rush_Start")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_201_AL_atk_a1_start");
				m_SoundStore.PlaySound("mon_3_attack_airdash", m_pTransformCom);
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
						return PriorityCondition()
							|| HitHeavyCondition();
				})

		.AddState("Rush_Loop")
			.OnStart([this]
			{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_202_AL_atk_a1_loop");
					m_pModelCom->Find_Animation("AS_em0700_202_AL_atk_a1_loop")->SetLooping(true);
					m_BeforePos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					m_fRushTime = 1.f;

					_matrix RushEffectPivotMatrix = CImguiUtils::CreateMatrixFromImGuizmoData(
						{ 0.08f, -0.112f, -1.617f },
						{ 180.f, 0.f, 180.f, },
						{ 15.f, 15.f, 15.f });

					CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0750_Dash_Attack")
						->Start_AttachPivot(this, RushEffectPivotMatrix, "Target", true, true);

					m_SoundStore.PlaySound("mon_3_attack_airdash_up", m_pTransformCom);

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
					return PriorityCondition() 
						|| HitHeavyCondition()
						|| m_fRushTime <= 0.f;
				})

		.AddState("Rush_End")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_203_AL_atk_a1_end");

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
					return PriorityCondition() || m_pASM->isSocketPassby("FullBody", 0.95f);
				})


				///////////////////////////////////////////////////////////////////////////////////////////

		.Build();
}

void CEM0750::BeginTick()
{
	CEnemy::BeginTick();
}

void CEM0750::Tick(_double TimeDelta)
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

	if (m_bDown == false && m_vMoveAxis.LengthSquared() > 0.f)
	{
		const _float fYaw = m_pTransformCom->GetYaw_Radian();
		_float3 vVelocity;
		XMStoreFloat3(&vVelocity, fMoveSpeed * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}

	// Tick의 제일 마지막에서 실행한다.
	ResetHitData();
}

void CEM0750::Late_Tick(_double TimeDelta)
{
	CEnemy::Late_Tick(TimeDelta);
}

void CEM0750::AfterPhysX()
{
	CEnemy::AfterPhysX();
}

HRESULT CEM0750::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CEM0750::Imgui_RenderProperty()
{
	CEnemy::Imgui_RenderProperty();
	if (ImGui::CollapsingHeader("ASM"))
	{
		m_pASM->Imgui_RenderState();
	}
	m_pFSM->Imgui_RenderProperty();
}

_bool CEM0750::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

void CEM0750::Play_LightHitAnim()
{
	if (m_eSimpleHitFrom == ESimpleAxis::NORTH)
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_401_AL_damage_l_F");
	else
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_402_AL_damage_l_B");
}

void CEM0750::Play_MidHitAnim()
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

void CEM0750::HeavyAttackPushStart()
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

_bool CEM0750::PriorityCondition()
{
	return  m_bDead || m_eDeBuff == EDeBuffType::DEBUFF_THUNDER;

}

_bool CEM0750::HitHeavyCondition()
{
	return m_eCurAttackType == EAttackType::ATK_HEAVY || m_eCurAttackType == EAttackType::ATK_SPECIAL_LOOP || m_eCurAttackType == EAttackType::ATK_SPECIAL_END || m_eCurAttackType == EAttackType::ATK_TO_AIR;
}

void CEM0750::Rush_StaticCheckSweep()
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

void CEM0750::Rush_SweepSphere()
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

void CEM0750::Check_Height()
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

void CEM0750::Move_YAxis(_double TimeDelta)
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

void CEM0750::AfterLocal180Turn()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vPos -= XMVector3Normalize(vLook) * 10.f;
	m_pTransformCom->LookAt(vPos);
}


CEM0750 * CEM0750::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEM0750* pInstance = new CEM0750(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0750");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEM0750::Clone(void * pArg)
{
	CEM0750*		pInstance = new CEM0750(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0750");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0750::Free()
{
	CEnemy::Free();
	Safe_Release(m_pASM);
	Safe_Release(m_pController);

}
