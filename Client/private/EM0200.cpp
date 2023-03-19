#include "stdafx.h"
#include "..\public\EM0200.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "RigidBody.h"
#include "EM0200_AnimInstance.h"
#include "EM0200_Controller.h"

CEM0200::CEM0200(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonsterEx(pDevice, pContext)
{
}

CEM0200::CEM0200(const CEM0200& rhs)
	: CMonsterEx(rhs)
{
}

HRESULT CEM0200::Initialize(void* pArg)
{
	Json em0200_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/FlowerLeg/FlowerLegTrigger.json");
	MoveJsonData(em0200_json, pArg);
	pArg = &em0200_json;

	m_strDeathSoundTag = "mon_5_fx_death";
	m_strImpactVoiceTag = "mon_5_impact_voice";

	// 배치툴에서 조절할 수 있게 하기
	{
		m_iMaxHP = 3000;
		m_iHP = 3000; // ★

		m_iAtkDamage = 50;
		iMonsterLevel = 2;
	}

	FAILED_CHECK(CMonsterEx::Initialize(pArg));

	m_eMonsterName = FLOWERLEG;
	m_bHasCrushGage = false;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(180.f));

	return S_OK;
}

void CEM0200::SetUpComponents(void* pArg)
{
	CMonsterEx::SetUpComponents(pArg);
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em200", L"Model", (CComponent**)&m_pModelCom));

	// 꼬리 충돌체
	Json FlowerLegTailCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/FlowerLeg/FlowerLegTailCol.json");
	FAILED_CHECK(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("TailCol"),
		(CComponent**)&m_pTailCol, &FlowerLegTailCol));

	// 범위 충돌체(플레이어가 몬스터 위로 못 올라가게한다.)
	Json FlowerLegRangeCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/FlowerLeg/FlowerLegRange.json");
	FAILED_CHECK(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("RangeCollider"),
		(CComponent**)&m_pRange, &FlowerLegRangeCol))

	// 컨트롤러, prototype안 만들고 여기서 자체생성하기 위함
	m_pController = CEM0200_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	// ASM
	m_pASM = CEM0200_AnimInstance::Create(m_pModelCom, this);
}

void CEM0200::SetUpSound()
{
	CMonsterEx::SetUpSound();

	m_SoundStore.CloneSound("mon_5_backdodge");
	m_SoundStore.CloneSound("mon_5_sidedodge");
	m_SoundStore.CloneSound("mon_5_bellattack_ready");
	m_SoundStore.CloneSound("mon_5_bellattack");
	m_SoundStore.CloneSound("mon_5_jumpattack_ready");
	m_SoundStore.CloneSound("mon_5_jumpattack_jump");
	m_SoundStore.CloneSound("mon_5_jumpattack_kick");
	m_SoundStore.CloneSound("mon_5_sprayattack_ready");
	m_SoundStore.CloneSound("mon_5_sprayattack");
	m_SoundStore.CloneSound("mon_5_step");
	m_SoundStore.CloneSound("mon_5_voice_laugh");
	m_SoundStore.CloneSound("mon_5_rush");

	m_pModelCom->Add_EventCaller("mon_5_backdodge", [this] {m_SoundStore.PlaySound("mon_5_backdodge", m_pTransformCom); });
	m_pModelCom->Add_EventCaller("mon_5_sidedodge", [this] {m_SoundStore.PlaySound("mon_5_sidedodge", m_pTransformCom); });
	m_pModelCom->Add_EventCaller("mon_5_bellattack_ready", [this] {m_SoundStore.PlaySound("mon_5_bellattack_ready", m_pTransformCom); });
	m_pModelCom->Add_EventCaller("mon_5_bellattack", [this] {m_SoundStore.PlaySound("mon_5_bellattack", m_pTransformCom); });
	m_pModelCom->Add_EventCaller("mon_5_jumpattack_ready", [this] {m_SoundStore.PlaySound("mon_5_jumpattack_ready", m_pTransformCom); });
	m_pModelCom->Add_EventCaller("mon_5_jumpattack_jump", [this] {m_SoundStore.PlaySound("mon_5_jumpattack_jump", m_pTransformCom); });
	m_pModelCom->Add_EventCaller("mon_5_jumpattack_kick", [this] {m_SoundStore.PlaySound("mon_5_jumpattack_kick", m_pTransformCom); });
	m_pModelCom->Add_EventCaller("mon_5_sprayattack_ready", [this] {m_SoundStore.PlaySound("mon_5_sprayattack_ready", m_pTransformCom); });
	m_pModelCom->Add_EventCaller("mon_5_sprayattack", [this] {m_SoundStore.PlaySound("mon_5_sprayattack", m_pTransformCom); });
	m_pModelCom->Add_EventCaller("mon_5_step", [this] {m_SoundStore.PlaySound("mon_5_step", m_pTransformCom); });
	m_pModelCom->Add_EventCaller("mon_5_rush", [this] {m_SoundStore.PlaySound("mon_5_rush", m_pTransformCom); });
}

void CEM0200::SetUpAnimationEvent()
{
	CMonsterEx::SetUpAnimationEvent();

	m_pModelCom->Add_EventCaller("JumpAttackStart", [this]
	{
		if (m_pTarget)
		{
			m_bJumpAttack = true;
			m_fGravity = 80.f;
			m_fYSpeed = 30.f;

			_vector vOrigin = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			vOrigin = XMVectorSetY(vOrigin, 0.f);
			_vector vDest = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
			vDest = XMVectorSetY(vDest, 0.f);
			const _float fJumpMoveTime = (2 * m_fYSpeed) / m_fGravity;

			const _vector vDiff = vDest - vOrigin;
			const _float fDistance = XMVectorGetX(XMVector3LengthEst(vDiff));
			const _vector vDirection = XMVector3NormalizeEst(vDiff);

			m_vOnJumpMoveVelocity = vDirection * (fDistance / fJumpMoveTime);
		}
		else	// 예외 처리
		{
			m_vOnJumpMoveVelocity = _float3::Zero;
		}
	});

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

	m_pModelCom->Add_EventCaller("Spin_Atk", [this] 
	{
		ClearDamagedTarget();
		if (!m_bDead)
		{
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0200_Spin_Attack")
					->Start_Attach(this, "Target", false);
		}
		m_bSpinAtk = true;
	});
	m_pModelCom->Add_EventCaller("LightOn", [this]
	{
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0200_Tail_Bell")
			->Start_Attach(this, "Tail6", false);
	});

	m_pModelCom->Add_EventCaller("Spin_AtkEnd", [this] 
	{
		m_bSpinAtk = false;
	});

	m_pModelCom->Add_EventCaller("FallRose_Start", [this]
	{
		if (!m_bDead)
		{
//			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0200_Fall_Rose")->Start_Attach(this, "Eff02", true);
			m_pFallRoseParticle = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0200_Fall_Rose");
			m_pFallRoseParticle->Start_Attach(this, "Eff02", true);
			Safe_AddRef(m_pFallRoseParticle);
		}
	});
	
	m_pModelCom->Add_EventCaller("Invincible_Start", [this] 
	{
		// Flower 흩뿌리는 Effect
		if (!m_bDead)
		{
//			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0200_Shoot_Flower")->Start_Attach(this, "RightFlower5", true);
			m_pShootFlwParticle = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0200_Shoot_Flower");
			m_pShootFlwParticle->Start_Attach(this, "RightFlower5", true);
			Safe_AddRef(m_pShootFlwParticle);
		}
	

//		m_bInvisible = true; 
	});

	// 머리흔들어서 꽃뿌리기 공격
	m_pModelCom->Add_EventCaller("OverLap", [this] { Strew_Overlap(); });
	m_pModelCom->Add_EventCaller("Invincible_End", [this] 
	{ 
//		m_bInvisible = false; 
	});

	m_pModelCom->Add_EventCaller("FallRose_End", [this]
	{
		if (m_bCloned == true)
		{
			if (m_pShootFlwParticle != nullptr)
			{
				m_pShootFlwParticle->SetDelete();
				Safe_Release(m_pShootFlwParticle);
				m_pShootFlwParticle = nullptr;
			}
			if (m_pFallRoseParticle != nullptr)
			{
				m_pFallRoseParticle->SetDelete();
				Safe_Release(m_pFallRoseParticle);
				m_pFallRoseParticle = nullptr;
			}
		}
	});

	m_pModelCom->Add_EventCaller("Kick_Event", [this] { Kick_SweepSphere(); });
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
	m_pModelCom->Add_EventCaller("Damage_End", [this]
	{
		// m_bHitMove = false;
	});
}

void CEM0200::SetUpFSM()
{
	CMonsterEx::SetUpFSM();

	/*
	 *input R : spin 공격
	 *input G : 꽃뿌리기
	 *input C : 위협
	 *input mouse RB : 점프 공격
	 *
	 *num 1 : 왼쪽 회피
	 *num 2 : 뒤로 회피
	 *num3 : 오른쪽 회피
	 */
	/*
	 * 피격, 소, 중, 공중
	 * 공중일 때 피격
	 * 죽는 모션
	 */

	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.OnStart([this]
			{
				m_fGravity = 20.f;
			})
			.AddTransition("Idle to Death", "Death")
				.Predicator([this]{ return m_bDead; })
			.AddTransition("Idle to Hit_Light", "Hit_Light")
				.Predicator([this] { return m_eLastAttackType == EAttackType::ATK_LIGHT; })
			.AddTransition("Idle to SpinAtk", "SpinAtk")
				.Predicator([this]{ return m_eInput == CController::R; })
			.AddTransition("Idle to FlowerShower", "FlowerShower")
				.Predicator([this]{ return m_eInput == CController::G; })
			.AddTransition("Idle to Threat", "Threat")
				.Predicator([this]{ return m_eInput == CController::C; })
			.AddTransition("Idle to JumpAtk", "JumpAtk")
				.Predicator([this]{ return m_eInput == CController::MOUSE_RB; })
			.AddTransition("Idle to Dodge", "Dodge")
				.Predicator([this]{ return m_eInput == CController::NUM_1 || m_eInput == CController::NUM_2 || m_eInput == CController::NUM_3; })



		.AddState("SpinAtk")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_202_AL_atk_a2");
			})
			.Tick([this](_double TimeDelta)
			{
				if (m_bSpinAtk)
				{
					Spin_SweepCapsule();
				}
			})
			.OnExit([this]
			{
				m_bSpinAtk = false;
			})
			.AddTransition("SpinAtk to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_eLastAttackType == EAttackType::ATK_TO_AIR || m_pASM->isSocketPassby("FullBody", 0.95f);
				})



		.AddState("FlowerShower")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_203_AL_atk_a3");
			})
			.AddTransition("FlowerShower to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_eLastAttackType == EAttackType::ATK_TO_AIR || m_pASM->isSocketPassby("FullBody", 0.95f);
				})



		.AddState("JumpAtk")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_204_AL_atk_a4_start");
				ClearDamagedTarget();
			})
			.Tick([this](_double TimeDelta)
			{
				if (m_bJumpAttack)
				{
					m_pTransformCom->MoveVelocity(TimeDelta, m_vOnJumpMoveVelocity);
					Kick_SweepSphere();
				}
			})
			.OnExit([this]
			{
				m_fGravity = 20.f;
				m_bJumpAttack = false;
			})
			.AddTransition("JumpAtk to JumpAtkLand", "JumpAtkLand")
				.Predicator([this]
				{
					return m_bDead || (m_bJumpAttack && m_bOnFloor);
				})
		.AddState("JumpAtkLand")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_205_AL_atk_a4_landing");
			})
			.AddTransition("SpinAtk to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_eLastAttackType == EAttackType::ATK_TO_AIR || m_pASM->isSocketPassby("FullBody", 0.95f);
				})



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
			.AddTransition("Dodge to DodgeStop", "DodgeStop")
				.Predicator([this]
				{
					return m_bDead || (m_bDodge && m_bOnFloor);
				})
			.AddTransition("Dodge to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_eLastAttackType != EAttackType::ATK_END;
				})
		.AddState("DodgeStop")
			.OnStart([this]
			{
				m_pController->SetActive(true);
				m_bDodge = false;
				m_fGravity = 20.f;
				if (CMathUtils::FloatCmp(m_vDodgeDir.x, -1.f))
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_141AL_dodge_L_stop");
				else if (CMathUtils::FloatCmp(m_vDodgeDir.z, -1.f))
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_136_AL_dodge_B_stop");
				else if (CMathUtils::FloatCmp(m_vDodgeDir.x,  1.f))
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_146_AL_dodge_R_stop");
			})
			.AddTransition("DodgeStop to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_eLastAttackType != EAttackType::ATK_END || m_pASM->isSocketPassby("FullBody", 0.99f);
				})



		.AddState("Threat")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0200_160_AL_threat");
			})
			.AddTransition("Threat to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_eLastAttackType != EAttackType::ATK_END || m_pASM->isSocketPassby("FullBody", 0.95f);
				})


		.AddState("Hit_Light")
		.AddState("Hit_Mid_Heavy")
		.AddState("Death")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_424_AL_dead_down");
			})
		.AddState("Hit_ToAir")
		.AddState("OnAir")
		.AddState("Hit_OnAir")
		

		.Build();
}

void CEM0200::BeginTick()
{
	CMonsterEx::BeginTick();
}

void CEM0200::Tick(_double TimeDelta)
{
	CMonsterEx::Tick(TimeDelta);

	m_pController->SetTarget(m_pTarget);
	if (m_bDead == false)
		m_pController->Tick(TimeDelta);
	else
		m_pController->Invalidate();

	m_bRun = m_pController->IsRun();	
	m_fTurnRemain = m_pController->GetTurnRemain();
	m_vMoveAxis = m_pController->GetMoveAxis();
	m_vMoveAxis.Normalize();
	m_eInput = m_pController->GetAIInput();

	m_pFSM->Tick(TimeDelta);

	_float fMoveSpeed = 0.f;
	if (m_bRun)
		fMoveSpeed = 4.f;
	else
		fMoveSpeed = 2.f;

	m_pASM->Tick(TimeDelta);

	if (m_vMoveAxis.LengthSquared() > 0.f)
	{
		const _float fYaw = m_pTransformCom->GetYaw_Radian();
		_float3 vVelocity;
		XMStoreFloat3(&vVelocity, fMoveSpeed * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}
}

void CEM0200::Late_Tick(_double TimeDelta)
{
	CMonsterEx::Late_Tick(TimeDelta);
}

HRESULT CEM0200::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CEM0200::Imgui_RenderProperty()
{
	CMonsterEx::Imgui_RenderProperty();
	if (ImGui::CollapsingHeader("ASM"))
	{
		m_pASM->Imgui_RenderState();
	}
}

void CEM0200::AfterPhysX()
{
	CMonsterEx::AfterPhysX();

	m_pRange->Update_Tick(m_pTransformCom->Get_WorldMatrix());
	m_pTailCol->Update_Tick(m_pModelCom->GetBoneMatrix("Tail4") * m_pTransformCom->Get_WorldMatrix());
}

void CEM0200::Strew_Overlap()
{
	ClearDamagedTarget();

	_float fLength = 1.f;

	_matrix BoneMatrix = m_pModelCom->GetBoneMatrix("TopFlower4") * m_pTransformCom->Get_WorldMatrix();
	
	_vector vBoneVector = BoneMatrix.r[3];
	_float3 fBone = vBoneVector;

	_vector vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vMyLook = XMVectorSetY(vMyLook, 0.f);
	_float3 fDest = vMyLook;
	
	_float3 fFinish = { (fBone.x + fLength * fDest.x), fBone.y, (fBone.z + fLength * fDest.z) };
	
	physx::PxOverlapHit hitBuffer[3];
	physx::PxOverlapBuffer overlapOut(hitBuffer, 3);

	SphereOverlapParams param;
	param.fVisibleTime = 0.4f;
	param.iTargetType = CTB_PLAYER;
	param.fRadius = 1.5f;
	param.vPos = XMVectorSetW(fFinish, 1.f);
	param.overlapOut = &overlapOut;

	if (CGameInstance::GetInstance()->OverlapSphere(param))
	{
		for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
		{
			HitTargets(overlapOut, m_iAtkDamage * 0.6f, EAttackType::ATK_LIGHT);
		}
	}
}

void CEM0200::Spin_SweepCapsule()
{
	_float4x4 TailMatrix = m_pTailCol->GetPxWorldMatrix();
	_float4 vTailPos = _float4{ TailMatrix.m[3][0], TailMatrix.m[3][1], TailMatrix.m[3][2], TailMatrix.m[3][3] };

	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);

	PxCapsuleSweepParams tParams;
	tParams.sweepOut = &sweepOut;
	tParams.CapsuleGeo = m_pTailCol->Get_CapsuleGeometry();
	tParams.pxTransform = m_pTailCol->Get_PxTransform();

	_float4	vWeaponDir = vTailPos - m_BeforePos;
	tParams.vUnitDir = _float3(vWeaponDir.x, vWeaponDir.y, vWeaponDir.z);
	tParams.fDistance = tParams.vUnitDir.Length();
	tParams.iTargetType = CTB_PLAYER;
	tParams.fVisibleTime = 0.f;

	if (CGameInstance::GetInstance()->PxSweepCapsule(tParams))
	{
		HitTargets(sweepOut, m_iAtkDamage * 1.5f, EAttackType::ATK_HEAVY);
	}

	m_BeforePos = vTailPos;
}

void CEM0200::Kick_SweepSphere()
{
	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);

	SphereSweepParams tParams;
	tParams.fVisibleTime = 0.2f;
	tParams.iTargetType = CTB_PLAYER;
	tParams.fRadius = 1.f;
	tParams.fDistance = 1.f;
	tParams.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	tParams.sweepOut = &sweepOut;
	tParams.vUnitDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	if (CGameInstance::GetInstance()->SweepSphere(tParams))
	{
		for (int i = 0; i < sweepOut.getNbAnyHits(); ++i)
		{
			HitTargets(sweepOut, m_iAtkDamage * 2, EAttackType::ATK_HEAVY);
		}
	}
}

void CEM0200::Dodge_VelocityCalc()
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

_bool CEM0200::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

CEM0200* CEM0200::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEM0200* pInstance = new CEM0200(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0200");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEM0200::Clone(void* pArg)
{
	CEM0200*		pInstance = new CEM0200(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0200");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0200::Free()
{
	CMonsterEx::Free();
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
	Safe_Release(m_pTailCol);
	Safe_Release(m_pRange);
}
