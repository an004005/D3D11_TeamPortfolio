#include "stdafx.h"
#include "EM0650.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "RigidBody.h"
#include "EM0650_AnimInstance.h"
#include "EM0650_Controller.h"
#include "EnemyBullet.h"
CEM0650::CEM0650(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
	m_eMonsterName = EEnemyName::EM0650;
}

CEM0650::CEM0650(const CEM0650 & rhs)
	: CEnemy(rhs)
{
}

HRESULT CEM0650::Initialize(void * pArg)
{
	//Json em0200_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/FlowerLeg/FlowerLegTrigger.json");
	//pArg = &em0200_json;

	/*m_strDeathSoundTag = "mon_5_fx_death";
	m_strImpactVoiceTag = "mon_5_impact_voice";*/

	// 배치툴에서 조절할 수 있게 하기
	{
		m_iMaxHP = 3000;
		m_iHP = 3000; // ★

		m_iAtkDamage = 50;
		iMonsterLevel = 2;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	m_eMonsterName = EEnemyName::EM0650;
	m_bHasCrushGage = true;
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
		auto pObj = CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_Bullet"), TEXT("EnemyBullet"));

		if (CEnemyBullet* pBullet = dynamic_cast<CEnemyBullet*>(pObj))
		{
			pBullet->Set_Owner(this);
	
			_matrix BoneMtx = m_pModelCom->GetBoneMatrix("Alga_F_03") * m_pTransformCom->Get_WorldMatrix();
			_vector vPrePos = BoneMtx.r[3];

			pBullet->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vPrePos);
			pBullet->GetTransform()->LookAt(m_LastSpotTargetPos); // vPrePos + vLook);
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
			.AddTransition("Idle to Death", "Death")
				.Predicator([this] { return m_bDead; })

			.AddTransition("Idle to Hit_ToAir", "Hit_ToAir")
				.Predicator([this] { return m_eCurAttackType == EAttackType::ATK_TO_AIR; })
			.AddTransition("Idle to Hit_Mid_Heavy", "Hit_Mid_Heavy")
				.Predicator([this] { return m_eCurAttackType == EAttackType::ATK_HEAVY || m_eCurAttackType == EAttackType::ATK_MIDDLE; })
			.AddTransition("Idle to Hit_Light", "Hit_Light")
				.Predicator([this] { return m_eCurAttackType == EAttackType::ATK_LIGHT; })

			.AddTransition("Idle to Attack_Shot", "Attack_Shot")
				.Predicator([this] { return m_eInput == CController::MOUSE_LB; })
	

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
	
		.AddState("Hit_ToAir")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0600_432_AL_blow_start_F");
				m_fGravity = 20.f;
				m_fYSpeed = 12.f;
			})
			.Tick([this](_double)
			{
			// 공중 추가타로 살짝 올라가는 애님
				m_bHitAir = true;
				if (m_eCurAttackType != EAttackType::ATK_END)
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
			.AddTransition("OnFloorGetup to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketEmpty("FullBody");
				})
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

void CEM0650::BeginTick()
{
	CEnemy::BeginTick();
	//m_pASM->AttachAnimSocket("Pool", { m_pModelCom->Find_Animation("AS_em0600_160_AL_threat") });
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
	Safe_Release(m_pRange);
}
