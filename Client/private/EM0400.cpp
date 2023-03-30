#include "stdafx.h"
#include "EM0400.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "RigidBody.h"
#include "EM0400_AnimInstance.h"
#include "EM0400_Controller.h"
#include "PhysX_Manager.h"
#include "CurveManager.h"
#include "CurveFloatMapImpl.h"

CEM0400::CEM0400(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
	m_eEnemyName = EEnemyName::EM0400;
}

CEM0400::CEM0400(const CEM0400 & rhs)
	: CEnemy(rhs)
{
}

HRESULT CEM0400::Initialize(void * pArg)
{
	Json em0400_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BuddyLumi/BuddyLumiTrigger.json");
	pArg = &em0400_json;

	/*m_strDeathSoundTag = "mon_5_fx_death";
	m_strImpactVoiceTag = "mon_5_impact_voice";*/

	// 배치툴에서 조절할 수 있게 하기
	{
		m_iMaxHP = 600;
		m_iHP = 600; // ★

		m_iAtkDamage = 50;
		iEemeyLevel = 2;
	
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	m_eEnemyName = EEnemyName::EM0400;
	m_bHasCrushGage = true;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(220.f));

	return S_OK;
}

void CEM0400::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em400", L"Model", (CComponent**)&m_pModelCom));

	//Create Collider
	//Json Weapon = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BuddyLumi/BuddyLumiWeapon.json");
	//Add_RigidBody("Weapon", &Weapon);

	// 컨트롤러, prototype안 만들고 여기서 자체생성하기 위함
	m_pController = CEM0400_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	// ASM
	m_pASM = CEM0400_AnimInstance::Create(m_pModelCom, this);
}

void CEM0400::SetUpSound()
{
	CEnemy::SetUpSound();
}

void CEM0400::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();

	// Event Caller
	m_pModelCom->Add_EventCaller("SwingEff_Start", [this]
	{
		if (!m_bDead)
		{
			//			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0400_Attack")->Start_Attach(this, m_strBoneName, false);

			m_pSwingEffect = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0400_Attack");
			m_pSwingEffect->Start_Attach(this, "RightShoulder", true);
			Safe_AddRef(m_pSwingEffect);
		}
	});

	m_pModelCom->Add_EventCaller("Swing_Start", [this]
	{
		m_bAtkSwitch = true;
		m_bHitMove = true;
	});
	m_pModelCom->Add_EventCaller("Swing_End", [this]
	{
		m_bHitMove = false;
		m_bAtkSwitch = false;
	});

	m_pModelCom->Add_EventCaller("SwingEff_End", [this]
	{
		if (m_bCloned == true)
		{
			if (m_pSwingEffect != nullptr)
			{
				m_pSwingEffect->SetDelete();
				Safe_Release(m_pSwingEffect);
				m_pSwingEffect = nullptr;
			}
		}
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

	//Dodge
	m_pModelCom->Add_EventCaller("Dodge_L_Start", [this]
	{
		Dodge_VelocityCalc();
	});

	m_pModelCom->Add_EventCaller("Dodge_ R_Start", [this]
	{
		Dodge_VelocityCalc();
	});

	m_pModelCom->Add_EventCaller("Dodge_B_Start", [this]
	{
		Dodge_VelocityCalc();
	});
	
}

void CEM0400::SetUpFSM()
{
	CEnemy::SetUpFSM();

	/*
	NUM_1 : Dodge_L  AS_em0400_140_AL_dodge_L
	NUM_2 : Dodge_R  AS_em0400_145_AL_dodge_R
	NUM_3 : Dodge_B  AS_em0400_135_AL_dodge_B
	MOUSE_LB : Attack_Swing  AS_em0400_200_AL_atk_a1
	MOUSE_RB : Threat  AS_em0400_160_AL_threat
	*/

	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.AddTransition("Idle to Death", "Death")
				.Predicator([this] { return m_bDead; })

			.AddTransition("Idle to Hit_ToAir", "Hit_ToAir")
				.Predicator([this] { return m_eCurAttackType == EAttackType::ATK_TO_AIR; })
			.AddTransition("Idle to Hit_Mid_Heavy", "Hit_Mid_Heavy")
				.Predicator([this] { return m_eCurAttackType == EAttackType::ATK_HEAVY || m_eCurAttackType == EAttackType::ATK_MIDDLE; })
			.AddTransition("Idle to Hit_Light", "Hit_Light")
				.Predicator([this] { return m_eCurAttackType == EAttackType::ATK_LIGHT; })

			.AddTransition("Idle to Attack_a1", "Attack_a1")
				.Predicator([this] { return m_eInput == CController::MOUSE_LB; })
			.AddTransition("Idle to Dodge", "Dodge")
				.Predicator([this] { return m_eInput == CController::NUM_1 || m_eInput == CController::NUM_2 || m_eInput == CController::NUM_3; })
			.AddTransition("Idle to Threat", "Threat")
				.Predicator([this] { return m_eInput == CController::MOUSE_RB; })

///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Hit_Light")
			.OnStart([this]
			{
				Play_LightHitAnim();
			})
			.Tick([this](_double)
			{
				if (m_eCurAttackType == EAttackType::ATK_LIGHT)
				{
					Play_LightHitAnim();
				}
			})
			.AddTransition("Hit_Light to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f)
						|| (m_eCurAttackType != EAttackType::ATK_LIGHT && m_eCurAttackType != EAttackType::ATK_END);
				})

		.AddState("Hit_Mid_Heavy")
			.OnStart([this]
			{
				Play_MidHitAnim();
			})
			.Tick([this](_double)
			{
				if (m_eCurAttackType == EAttackType::ATK_HEAVY || m_eCurAttackType == EAttackType::ATK_MIDDLE)
				{
					Play_MidHitAnim();
				}
			})
			.AddTransition("Hit_Mid_Heavy to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f)
						|| m_eCurAttackType == EAttackType::ATK_TO_AIR;
				})

		.AddState("Death")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0400_424_AL_dead_down");
			})

		.AddState("Hit_ToAir")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0400_432_AL_blow_start_F");
				m_fGravity = 20.f;
				m_fYSpeed = 12.f;
			})
			.Tick([this](_double)
			{
				// 공중 추가타로 살짝 올라가는 애님
				m_bHitAir = true;
				if (m_eCurAttackType != EAttackType::ATK_END)
				{
					m_pASM->InputAnimSocketOne("FullBody", "AS_em0400_455_AL_rise_start");
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
				m_pASM->InputAnimSocketMany("FullBody", { "AS_em0400_433_AL_blow_landing_F", "AS_em0400_427_AL_getup_01" });
				m_fGravity = 20.f;
			})
				.AddTransition("OnFloorGetup to Idle", "Idle")
				.Predicator([this]
			{
				return m_bDead || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketEmpty("FullBody");
			})
///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Dodge")
			.OnStart([this]
			{
				m_vDodgeDir = _float4::Zero;
				if (m_eInput == CController::NUM_1) // left
				{
					m_vDodgeDir.x = -1.f;
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em0400_140_AL_dodge_L");
				}
				else if (m_eInput == CController::NUM_2) // back
				{
					m_vDodgeDir.z = -1.f;
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em0400_135_AL_dodge_B");
				}
				else if (m_eInput == CController::NUM_3) //right
				{
					m_vDodgeDir.x = 1.f;
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em0400_145_AL_dodge_R");
				}
			})
			.Tick([this](_double TimeDelta)
			{
				if (m_bDodge)
				{
					m_pTransformCom->MoveVelocity(TimeDelta, m_vDodgeVelocity);
				}
			})
			.OnExit([this]
			{
				m_bDodge = false;
			})
			.AddTransition("Dodge to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Attack_a1")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0400_200_AL_atk_a1");
			})
			.Tick([this](_double) 
			{
				if (m_bAtkSwitch)
					Swing_SweepSphere();
			})
			.AddTransition("Attack_a1 to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_eCurAttackType == EAttackType::ATK_TO_AIR || m_pASM->isSocketPassby("FullBody", 0.95f);
				})
///////////////////////////////////////////////////////////////////////////////////////////
	
		.AddState("Threat")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0400_160_AL_threat");
			})
			.AddTransition("Threat to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

///////////////////////////////////////////////////////////////////////////////////////////

		.Build();
}

void CEM0400::SetUpUI()
{
	//HP UI
	_float4x4 UI_PivotMatrix = Matrix(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.41f, 0.0f, 1.0f
	);

	m_UI_PivotMatrixes[ENEMY_INFOBAR] = UI_PivotMatrix;

	//FindEye
	UI_PivotMatrix = Matrix(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-0.324f, 1.014f, 0.0f, 1.0f
	);

	m_UI_PivotMatrixes[ENEMY_FINDEYES] = UI_PivotMatrix;
}

void CEM0400::BeginTick()
{
	CEnemy::BeginTick();
}

void CEM0400::Tick(_double TimeDelta)
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

	if (m_vMoveAxis.LengthSquared() > 0.f)
	{
		const _float fMoveSpeed = CCurveManager::GetInstance()->GetCurve("em0400_Walk")
			->GetValue(m_pModelCom->GetPlayAnimation()->GetPlayRatio());

		const _float fYaw = m_pTransformCom->GetYaw_Radian();
		_float3 vVelocity;
		XMStoreFloat3(&vVelocity, fMoveSpeed * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}

	ResetHitData();
}

void CEM0400::Late_Tick(_double TimeDelta)
{
	CEnemy::Late_Tick(TimeDelta);
}

void CEM0400::AfterPhysX()
{
	CEnemy::AfterPhysX();
	//GetRigidBody("Weapon")->Update_Tick(m_pModelCom->GetBoneMatrix("RightWeapon") * m_pTransformCom->Get_WorldMatrix());
}

HRESULT CEM0400::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CEM0400::Imgui_RenderProperty()
{
	CEnemy::Imgui_RenderProperty();
	if (ImGui::CollapsingHeader("ASM"))
	{
		m_pASM->Imgui_RenderState();
	}

	m_pFSM->Imgui_RenderProperty();
}

_bool CEM0400::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

void CEM0400::Dodge_VelocityCalc()
{
	m_bDodge = true;

	const _float fDistance = 3.f;
	const _vector vDirection = m_vDodgeDir;

	const _float fYaw = m_pTransformCom->GetYaw_Radian();
	m_vDodgeVelocity = XMVector3TransformNormal(vDirection * fDistance, XMMatrixRotationY(fYaw));
}

void CEM0400::Play_LightHitAnim()
{
	if (m_eSimpleHitFrom == ESimpleAxis::NORTH)
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0400_401_AL_damage_l_F");
	else
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0400_402_AL_damage_l_B");
}

void CEM0400::Play_MidHitAnim()
{
	switch (m_eHitFrom)
	{
	case EBaseAxis::NORTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0400_411_AL_damage_m_F");
		break;
	case EBaseAxis::SOUTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0400_412_AL_damage_m_B");
		break;
	case EBaseAxis::WEST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0400_413_AL_damage_m_L");
		break;
	case EBaseAxis::EAST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0400_414_AL_damage_m_R");
		break;
	case EBaseAxis::AXIS_END:
		FALLTHROUGH;
	default:
		NODEFAULT;
	}
}

void CEM0400::Swing_SweepSphere()
{
	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);

	//Tail4가 꼬리 중앙에 있음
	_float4x4 BoneMatrix = GetBoneMatrix("RightWeapon") * m_pTransformCom->Get_WorldMatrix();
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
		HitTargets(sweepOut, static_cast<_int>(m_iAtkDamage * 1.2f), EAttackType::ATK_LIGHT);
	}

	m_BeforePos = vBonePos;
}


CEM0400 * CEM0400::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEM0400* pInstance = new CEM0400(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0400");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEM0400::Clone(void * pArg)
{
	CEM0400*		pInstance = new CEM0400(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0400");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0400::Free()
{
	CEnemy::Free();
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
	Safe_Release(m_pRange);

	if (m_bCloned)
	{
		if (m_pSwingEffect != nullptr)
		{
			m_pSwingEffect->SetDelete();
			Safe_Release(m_pSwingEffect);
			m_pSwingEffect = nullptr;
		}
	}
}
