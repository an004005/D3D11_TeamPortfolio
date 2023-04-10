#include "stdafx.h"
#include "EM0650.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "EM0650_AnimInstance.h"
#include "EM0650_Controller.h"
#include "BulletBuilder.h"
#include "ImguiUtils.h"

CEM0650::CEM0650(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
	m_eEnemyName = EEnemyName::EM0650;
}

CEM0650::CEM0650(const CEM0650 & rhs)
	: CEnemy(rhs)
{
	m_SpawnEffectPivot = CImguiUtils::CreateMatrixFromImGuizmoData({0.f, 1.f, 0.f}, {0.f, 0.f, 0.f}, {0.6f,0.6f,0.6f});
	m_fSpawnDistortionDistancePivot = 0.5f;
}

HRESULT CEM0650::Initialize(void * pArg)
{
	Json em0650_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/SkummyPool/Test.json");
	pArg = &em0650_json;

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

	m_eEnemyName = EEnemyName::EM0650;
	m_bHasCrushGauge = false;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(180.f));
	m_pTransformCom->SetSpeed(2.f);

	return S_OK;
}

void CEM0650::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em650", L"Model", (CComponent**)&m_pModelCom));

	// 컨트롤러, prototype안 만들고 여기서 자체생성하기 위함
	m_pController = CEM0650_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	// ASM
	m_pASM = CEM0650_AnimInstance::Create(m_pModelCom, this);
}

void CEM0650::SetUpSound()
{
	CEnemy::SetUpSound();
}

void CEM0650::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();

	// Event Caller

	m_pModelCom->Add_EventCaller("Muzzle", [this]
	{
		if (!m_bDead)
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0650_Bullet_Birth")->Start_Attach(this, "Eff02");
	});
	m_pModelCom->Add_EventCaller("LastSpot", [this]
	{
		_vector vTargetColPos = dynamic_cast<CScarletCharacter*>(m_pTarget)->GetColliderPosition();
		m_LastSpotTargetPos = vTargetColPos;
	});
	m_pModelCom->Add_EventCaller("Shoot", [this]
	{
			Create_Bullet();

		//auto pObj = CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_Bullet"), TEXT("Prototype_RedBullet"));

		//if (CRedBullet* pBullet = dynamic_cast<CRedBullet*>(pObj))
		//{
		//	pBullet->Set_Owner(this);
	
		//	_matrix BoneMtx = m_pModelCom->GetBoneMatrix("Alga_F_03") * m_pTransformCom->Get_WorldMatrix();
		//	_vector vPrePos = BoneMtx.r[3];

		//	pBullet->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vPrePos);
		//	pBullet->GetTransform()->LookAt(m_LastSpotTargetPos); // vPrePos + vLook);
		//}
	});
	m_pModelCom->Add_EventCaller("Upper", [this]
	{
		m_fGravity = 20.f;
		m_fYSpeed = 10.f;
	});
	m_pModelCom->Add_EventCaller("Successive", [this]
	{
		m_fGravity = 3.f;
		m_fYSpeed = 1.5f;
	});
	m_pModelCom->Add_EventCaller("AirDamageReset", [this]
	{
		m_fGravity = 20.f;
		m_fYSpeed = 0.f;
	});
	m_pModelCom->Add_EventCaller("Damage_End", [this] { m_bHitMove = false; });


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
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0700DeadFlower")
				->Start_NoOwnerOnlyPos(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	});

	//m_pModelCom->Add_EventCaller("mon_2_attack_ready", [this] {m_SoundStore.PlaySound("mon_2_attack_ready", m_pTransformCom); });
	//m_pModelCom->Add_EventCaller("mon_2_attack_shoot", [this] {m_SoundStore.PlaySound("mon_2_attack_shoot", m_pTransformCom); });
	//m_pModelCom->Add_EventCaller("mon_2_fx_death", [this] {m_SoundStore.PlaySound("mon_2_fx_death", m_pTransformCom); });
	//m_pModelCom->Add_EventCaller("mon_2_move", [this] {m_SoundStore.PlaySound("mon_2_move", m_pTransformCom); });

	// ~Event Caller
}

void CEM0650::SetUpFSM()
{
	CEnemy::SetUpFSM();

	/*
	MOUSE_LB : Attack
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

			.AddTransition("Idle to Attack_Shot", "Attack_Shot")
				.Predicator([this] { return m_eInput == CController::MOUSE_LB; })
	

///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Hit_Light")
			.OnStart([this]
			{
				Play_LightHitAnim();
				m_vMoveAxis = _float3::Zero;
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

				m_vMoveAxis = _float3::Zero;
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
	
		.AddState("Hit_ToAir")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0600_432_AL_blow_start_F");
				m_fGravity = 20.f;
				m_fYSpeed = 10.f;
				m_bDown = true;

				m_vMoveAxis = _float3::Zero;
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
					m_pASM->InputAnimSocketOne("FullBody", "AS_em0600_455_AL_rise_start");
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
				m_pASM->InputAnimSocketMany("FullBody", { "AS_em0600_433_AL_blow_landing_F", "AS_em0600_427_AL_getup" });
				m_fGravity = 20.f;
			})
			.OnExit([this]
			{
				m_bDown = true;
			})
			.AddTransition("OnFloorGetup to Idle", "Idle")
				.Predicator([this]
				{
					return PriorityCondition() || m_pASM->isSocketEmpty("FullBody");
				})

		.AddState("Down")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0600_425_AL_down_start");
				m_bDown = true;
			})
			.AddTransition("Down to Shock", "Shock")
				.Predicator([this]
				{
					return m_eCurAttackType == EAttackType::ATK_TO_AIR || m_pASM->isSocketPassby("FullBody", 0.99f);
				})

		.AddState("Shock")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0600_480_AL_down_shock");
				m_pModelCom->Find_Animation("AS_em0600_480_AL_down_shock")->SetLooping(true);
			})
			.OnExit([this]
			{
				m_bDown = false;
			})
			.AddTransition("Shock to Getup", "Getup")
				.Predicator([this]
				{
					return m_eCurAttackType == EAttackType::ATK_TO_AIR
						|| m_eDeBuff != EDeBuffType::DEBUFF_THUNDER;
				})

		.AddState("Getup")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0600_427_AL_getup");
			})
			.OnExit([this]
			{
				m_bDown = false;
			})
			.AddTransition("Getup to Idle", "Idle")
				.Predicator([this]
				{
					return PriorityCondition() || m_eCurAttackType == EAttackType::ATK_TO_AIR || m_pASM->isSocketEmpty("FullBody");					})

		.AddState("Death")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0600_424_AL_dead_down");
			})

///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Attack_Shot")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0600_204_AL_atk_a3_shot");
			})
			.AddTransition("Attack to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})
///////////////////////////////////////////////////////////////////////////////////////////
	
		.Build();
}

void CEM0650::SetUpUI()
{
	__super::SetUpUI();

	//HP UI
	_float4x4 UI_InfoPivotMatrix = Matrix(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.241f, 0.0f, 1.0f
	);

	//FindEye
	_float4x4 UI_EyesPivotMatrix = Matrix(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.724f, 0.0f, 1.0f
	);

	m_pEMUI->SetUpPivots(UI_InfoPivotMatrix, UI_EyesPivotMatrix);
}

void CEM0650::BeginTick()
{
	CEnemy::BeginTick();
}

void CEM0650::Tick(_double TimeDelta)
{
	CEnemy::Tick(TimeDelta);

	//AIInstance tick
	m_pController->SetTarget(m_pTarget);

	if (m_bDead == false)
		m_pController->Tick(TimeDelta);
	else
		m_pController->Invalidate();

	//변수 업데이트
	m_fTurnRemain = m_pController->GetTurnRemain();
	m_vMoveAxis = m_pController->GetMoveAxis();
	m_vMoveAxis.Normalize();
	m_eInput = m_pController->GetAIInput();

	//ASM, FSM tick
	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);

	const _float fMoveSpeed = 1.2f;

	if (m_vMoveAxis.LengthSquared() > 0.f)
	{
		const _float fYaw = m_pTransformCom->GetYaw_Radian();
		_float3 vVelocity;
		XMStoreFloat3(&vVelocity, fMoveSpeed * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}

	Make_Decal(TimeDelta);
	ResetHitData();
}

void CEM0650::Late_Tick(_double TimeDelta)
{
	CEnemy::Late_Tick(TimeDelta);
}

void CEM0650::AfterPhysX()
{
	CEnemy::AfterPhysX();
}

HRESULT CEM0650::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CEM0650::Imgui_RenderProperty()
{
	CEnemy::Imgui_RenderProperty();
	if (ImGui::CollapsingHeader("ASM"))
	{
		m_pASM->Imgui_RenderState();
	}

	m_pFSM->Imgui_RenderProperty();

}

_bool CEM0650::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

void CEM0650::Play_LightHitAnim()
{
	if (m_eSimpleHitFrom == ESimpleAxis::NORTH)
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0600_401_AL_damage_l_F");
	else
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0600_402_AL_damage_l_B");
}

void CEM0650::Play_MidHitAnim()
{
	switch (m_eHitFrom)
	{
	case EBaseAxis::NORTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0600_411_AL_damage_m_F");
		break;
	case EBaseAxis::SOUTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0600_412_AL_damage_m_B");
		break;
	case EBaseAxis::WEST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0600_413_AL_damage_m_L");
		break;
	case EBaseAxis::EAST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0600_414_AL_damage_m_R");
		break;
	case EBaseAxis::AXIS_END:
		FALLTHROUGH;
	default:
		NODEFAULT;
	}
}

void CEM0650::Create_Bullet()
{
	DAMAGE_PARAM eDamageParam;
	eDamageParam.eAttackType = EAttackType::ATK_END;
	eDamageParam.eDeBuff = EDeBuffType::DEBUFF_END;
	eDamageParam.iDamage = 20;

	_matrix BoneMtx = m_pModelCom->GetBoneMatrix("Alga_F_03") * m_pTransformCom->Get_WorldMatrix();
	_vector vPrePos = BoneMtx.r[3];

	CBulletBuilder()
		.CreateBullet()
			.Set_Owner(this)
			.Set_InitBulletEffect({ L"em0650_Bullet_Birth", L"Em0650_Bullet_Loop" })
			.Set_InitBulletParticle(L"em0650_Bullet_Loop")
			.Set_ShootSpeed(18.f)
			.Set_Life(2.f)
			.Set_DamageParam(eDamageParam)
			.Set_DeadBulletEffect({ L"em0650_Bullet_Dead" })
			.Set_DeadBulletParticle(L"em0650_Bullet_Dead_Particle")
			.Set_Position(vPrePos)
			.Set_LookAt(m_LastSpotTargetPos)
		.Build();
}

void CEM0650::Make_Decal(_double TimeDelta)
{
	m_dDecalTime += TimeDelta;

	if (m_dDecalTime >= 1.0)
	{
		_float4x4 DecalEffectPivot = CImguiUtils::CreateMatrixFromImGuizmoData(
			{ 0.f, 0.f, 0.f },
			{ 0.f,  0.f, 0.f },
			{ 1.3f, 1.f, 1.3f }
		);

		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0650_Move_Decal")
			->Start_AttachPivot(this, DecalEffectPivot, "Reference", false, false);

		m_dDecalTime = 0.0;
	}

}

_bool CEM0650::PriorityCondition()
{
	return  m_bDead || m_eDeBuff == EDeBuffType::DEBUFF_THUNDER;
}

void CEM0650::HeavyAttackPushStart()
{
	if (m_eCurAttackType == EAttackType::ATK_MIDDLE || m_eCurAttackType == EAttackType::ATK_HEAVY || m_eCurAttackType == EAttackType::ATK_SPECIAL_END)
	{
		m_HeavyAttackPushTimeline.PlayFromStart();
		m_vPushVelocity = CClientUtils::GetDirFromAxis(m_eHitFrom);
		m_vPushVelocity *= -2.5f; // 공격 온 방향의 반대로 이동

		const _float fYaw = m_pTransformCom->GetYaw_Radian();
		m_vPushVelocity = XMVector3TransformNormal(m_vPushVelocity, XMMatrixRotationY(fYaw));
	}
}


CEM0650 * CEM0650::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEM0650* pInstance = new CEM0650(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0650");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEM0650::Clone(void * pArg)
{
	CEM0650*		pInstance = new CEM0650(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0650");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0650::Free()
{
	CEnemy::Free();
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
}
