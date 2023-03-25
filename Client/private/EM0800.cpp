#include "stdafx.h"
#include "..\public\EM0800.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "RigidBody.h"
#include "EM0800_AnimInstance.h"
#include "EM0800_Controller.h"
#include "EnemyBullet.h"
CEM0800::CEM0800(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
	m_eMonsterName = EEnemyName::EM0700;
}

CEM0800::CEM0800(const CEM0800 & rhs)
	: CEnemy(rhs)
{
}

HRESULT CEM0800::Initialize(void * pArg)
{
	//Json em0200_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/FlowerLeg/FlowerLegTrigger.json");
	//pArg = &em0200_json;

	/*m_strDeathSoundTag = "mon_5_fx_death";
	m_strImpactVoiceTag = "mon_5_impact_voice";*/

	// 배치툴에서 조절할 수 있게 하기
	{
		m_iMaxHP = 5000;
		m_iHP = m_iMaxHP;

		m_iAtkDamage = 50;
		iMonsterLevel = 2;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	m_eMonsterName = EEnemyName::EM0800;
	m_bHasCrushGage = true;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(70.f));
	m_fGravity = 20.f;

	return S_OK;
}

void CEM0800::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em800", L"Model", (CComponent**)&m_pModelCom));

	// 범위 충돌체(플레이어가 몬스터 위로 못 올라가게한다.)
	/*Json FlowerLegRangeCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/FlowerLeg/FlowerLegRange.json");
*/

	FAILED_CHECK(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("RangeColl"),
		(CComponent**)&m_pRange, pArg));

	FAILED_CHECK(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("BodyColl"),
		(CComponent**)&m_pBody))
		

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
}

void CEM0800::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();

	// Event Caller

	//m_pModelCom->Add_EventCaller("Bite_Start", [this]
	//{
	//	//		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0800_Bite_OnHit")->Start_Attach(this, "Jaw", true);
	//	m_SoundStore.PlaySound("mon_4_attack_bite", m_pTransformCom);
	//	m_bAtkBite = true;
	//});
	//m_pModelCom->Add_EventCaller("Bite_End", [this]
	//{
	//	m_bAtkBite = false;
	//	ClearDamagedTarget();
	//});

	/*m_pModelCom->Add_EventCaller("Laser_Create", [this] { m_bAtkLaser = true; });
	m_pModelCom->Add_EventCaller("LaserSound_Start", [this] { m_SoundStore.PlaySound("mon_4_attack_laser", m_pTransformCom); });
	m_pModelCom->Add_EventCaller("Laser_Finish", [this]
	{
		m_bAtkLaser = false;
		ClearDamagedTarget();
	});
*/

	//m_pModelCom->Add_EventCaller("LaserEff_Start", [this]
	//{
	//	_matrix			PivotMatrix = XMMatrixIdentity();
	//	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0800_Laser")->Start_AttachPivot(this, PivotMatrix, "Eff02", true, true, true);

	//	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0800_Laser_Mouth")->Start_Attach(this, "Eff02", true);
	//});
	//m_pModelCom->Add_EventCaller("Damage_End", [this] { m_bHitMove = false; });
	//m_pModelCom->Add_EventCaller("Untouch", [this] { m_bUntouchable = true; });
	//m_pModelCom->Add_EventCaller("Untouch_End", [this] { m_bUntouchable = false; });

	//m_pModelCom->Add_EventCaller("Groggy_End", [this] { m_bDown = false; });

}

void CEM0800::SetUpFSM()
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
			.AddTransition("Idle to Death" , "Death")
				.Predicator([this] { return m_bDead; })

			.AddTransition("Idle to Hit_Mid_Heavy", "Hit_Mid_Heavy")
				.Predicator([this] { return
					m_eCurAttackType == EAttackType::ATK_HEAVY
					|| m_eCurAttackType == EAttackType::ATK_MIDDLE
					|| m_eCurAttackType == EAttackType::ATK_SPECIAL_END; })
		
			.AddTransition("Idle to Bite", "Bite")
				.Predicator([this] { return m_eInput == CController::MOUSE_RB; })

			.AddTransition("Idle to Laser_Start", "Laser_Start")
				.Predicator([this] { return m_eInput == CController::MOUSE_LB; })

///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Hit_Mid_Heavy")
				.OnStart([this]
			{
				Play_MidHitAnim();
				HeavyAttackPushStart();
			})
			.Tick([this](_double TimeDelta)
			{
				SocketLocalMove(m_pASM);
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
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
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

		.AddState("Bite")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0810_201_AL_atk_a1_bite");
				ClearDamagedTarget();
			})
			.Tick([this](_double TimeDelta)
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

///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Laser_Start")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0800_214_AL_atk_a7_laser2_start");
			})
			.AddTransition("Laser_Start to Laser_Loop", "Laser_Loop")
				.Predicator([this]
			{
				return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
			})
		.AddState("Laser_Loop")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0800_215_AL_atk_a7_laser2_loop");
				m_pModelCom->GetPlayAnimation()->SetLooping(true);
				m_fLaserTime = 0.f;
			})
			.Tick([this](_double TimeDetla)
			{
				m_fLaserTime += TimeDetla;
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Laser_Loop to Laser_End", "Laser_End")
				.Predicator([this]
			{
				return m_bDead || m_fLaserTime >= 3.f;
			})
		.AddState("Laser_End")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0800_216_AL_atk_a7_laser2_end");
			})
			.AddTransition("Laser_End to Idle", "Idle")
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

void CEM0800::BeginTick()
{
	CEnemy::BeginTick();
}

void CEM0800::Tick(_double TimeDelta)
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

	const _float fMoveSpeed = 2.f;

	if (m_vMoveAxis.LengthSquared() > 0.f)
	{
		_float3 vVelocity;
		XMStoreFloat3(&vVelocity, fMoveSpeed * XMVector3Normalize(m_vMoveAxis));
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}

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
	Safe_Release(m_pRange);
	Safe_Release(m_pBody);
}
