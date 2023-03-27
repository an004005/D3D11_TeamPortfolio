#include "stdafx.h"
#include "EM0210.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "RigidBody.h"
#include "EM0210_AnimInstance.h"
#include "EM0210_Controller.h"
#include "PhysX_Manager.h"

CEM0210::CEM0210(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
	m_eEnemyName = EEnemyName::EM0210;
}

CEM0210::CEM0210(const CEM0210 & rhs)
	: CEnemy(rhs)
{
}

HRESULT CEM0210::Initialize(void * pArg)
{
	//Json em0200_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/FlowerLeg/FlowerLegTrigger.json");
	//pArg = &em0200_json;

	/*m_strDeathSoundTag = "mon_5_fx_death";
	m_strImpactVoiceTag = "mon_5_impact_voice";*/

	// 배치툴에서 조절할 수 있게 하기
	{
		m_iMaxHP = 1100;
		m_iHP = 1100; // ★

		m_iAtkDamage = 50;
		iEemeyLevel = 2;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	m_eEnemyName = EEnemyName::EM0210;
	m_bHasCrushGage = true;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(220.f));

	return S_OK;
}

void CEM0210::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW,
		L"Prototype_Model_em210", L"Model",
		(CComponent**)&m_pModelCom));

	//Create Collider
	FAILED_CHECK(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("TailCol"),
		(CComponent**)&m_pTailCol, pArg));

	FAILED_CHECK(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("WeaponCol"),
		(CComponent**)&m_pWeaponCol, pArg));

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
	});

	//Somersault end
	m_pModelCom->Add_EventCaller("Somersault_End", [this]
	{
		m_bAttack = false;
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
			.AddTransition("Idle to Death", "Death")
				.Predicator([this] { return m_bDead; })

			.AddTransition("Idle to Hit_ToAir", "Hit_ToAir")
				.Predicator([this] { return m_eCurAttackType == EAttackType::ATK_TO_AIR; })
			.AddTransition("Idle to Hit_Mid_Heavy", "Hit_Mid_Heavy")
				.Predicator([this] { return m_eCurAttackType == EAttackType::ATK_HEAVY || m_eCurAttackType == EAttackType::ATK_MIDDLE; })
			.AddTransition("Idle to Hit_Light", "Hit_Light")
				.Predicator([this] { return m_eCurAttackType == EAttackType::ATK_LIGHT; })

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
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_424_AL_dead_down");
			})

		.AddState("Hit_ToAir")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_432_AL_blow_start_F");
				m_fGravity = 20.f;
				m_fYSpeed = 12.f;
			})
			.Tick([this](_double)
			{
				// 공중 추가타로 살짝 올라가는 애님
				m_bHitAir = true;
				if (m_eCurAttackType != EAttackType::ATK_END)
				{
					m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_455_AL_rise_start");
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
				m_pASM->InputAnimSocketMany("FullBody", { "AS_em0200_433_AL_blow_landing_F", "AS_em0200_427_AL_getup" });
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
					return m_bDead || (m_bDodge && m_bOnFloor);
				})
			.AddTransition("Dodge to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_eCurAttackType != EAttackType::ATK_END;
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
						return m_bDead || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketPassby("FullBody", 0.99f);
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
					SweepCapsule(m_pTailCol);
			})
			.AddTransition("Attack_Spin to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_eCurAttackType == EAttackType::ATK_TO_AIR || m_pASM->isSocketPassby("FullBody", 0.95f);
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
					SweepCapsule(m_pTailCol);
			})
			.AddTransition("Attack_Somersault to Idle", "Idle")
				.Predicator([this]
			{
				return m_bDead || m_eCurAttackType == EAttackType::ATK_TO_AIR || m_pASM->isSocketPassby("FullBody", 0.95f);
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
					return m_bDead || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

///////////////////////////////////////////////////////////////////////////////////////////

		.Build();
}

void CEM0210::BeginTick()
{
	CEnemy::BeginTick();
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
}

void CEM0210::AfterPhysX()
{
	CEnemy::AfterPhysX();
}

HRESULT CEM0210::Render()
{
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

void CEM0210::SweepCapsule(CRigidBody * pCol)
{
	Matrix ColMatrix = pCol->GetPxWorldMatrix();
	_float4 vTailPos = _float4(ColMatrix.Translation().x, ColMatrix.Translation().y, ColMatrix.Translation().z, 1.f);

	physx::PxSweepHit hitBuffer[5];
	physx::PxSweepBuffer SweepOut(hitBuffer, 5);

	PxCapsuleSweepParams param;
	param.sweepOut = &SweepOut;
	param.CapsuleGeo = pCol->Get_CapsuleGeometry();
	param.pxTransform = pCol->Get_PxTransform();

	_float4	vDir = vTailPos - m_BeforePos;

	param.vUnitDir = _float3(vDir.x, vDir.y, vDir.z);
	param.fDistance = param.vUnitDir.Length();
	param.iTargetType = CTB_PLAYER;
	param.fVisibleTime = 0.f;

	if (CGameInstance::GetInstance()->PxSweepCapsule(param))
	{
		HitTargets(SweepOut, m_iAtkDamage * 1.5f, EAttackType::ATK_HEAVY);
	}

	m_BeforePos = vTailPos;
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

CGameObject * CEM0210::Clone(void * pArg)
{
	CEM0210*		pInstance = new CEM0210(*this);

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
	Safe_Release(m_pRange);
	Safe_Release(m_pTailCol);
	Safe_Release(m_pWeaponCol);

}
