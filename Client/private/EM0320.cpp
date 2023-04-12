#include "stdafx.h"
#include "..\public\EM0320.h"

#include <GameUtils.h>

#include "Model.h"
#include "JsonStorage.h"
#include "GameInstance.h"
#include "Player.h"
#include "JsonStorage.h"
#include "RigidBody.h"
#include "Material.h"
#include "EM0320_AnimInstance.h"
#include "EM0320_Controller.h"
#include "FSMComponent.h"

#include "BulletBuilder.h"
#include "VFX_Manager.h"

#include "Canvas_BossHpMove.h"
#include "ImguiUtils.h"

CEM0320::CEM0320(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
}

CEM0320::CEM0320(const CEM0320& rhs)
	: CEnemy(rhs)
{
	m_bSpawnEffect = false;
}

HRESULT CEM0320::Initialize(void* pArg)
{
	Json em0320_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/Boss1_en320/Boss1_en320.json");
	pArg = &em0320_json;

	// 배치에서 지정하지 않을 때의 기본 스텟
	{
		m_iMaxHP = 5000;
		m_iHP = m_iMaxHP; // ★
		m_iCrushGauge = 400;
		m_iMaxCrushGauge = 400;
		m_bHasCrushGauge = false;

		m_iAtkDamage = 200;
		iEemeyLevel = 7;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	SetUpIntroFSM();

	m_eEnemyName = EEnemyName::EM0320;
	m_bHasCrushGauge = false;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(100.f));

	return S_OK;
}

void CEM0320::SetUpComponents(void* pArg)
{
	CEnemy::SetUpComponents(pArg);
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em320", L"Model", (CComponent**)&m_pModelCom));
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Com_Weak"), (CComponent**)&m_pWeak, pArg));

	Json BossRnage = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/Boss1_en320/Boss1_Rnage.json");
	FAILED_CHECK(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("RangeCollider"),
		(CComponent**)&m_pRange, &BossRnage));


	Json BossHeadJson = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/Boss1_en320/Boss1_Head.json");
	// Json BossLeftArmJson = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/Boss1_en320/Boss1_LeftArm.json");
	// Json BossRightArmJson = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/Boss1_en320/Boss1_RightArm.json");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Com_Head"), (CComponent**)&m_pHead, &BossHeadJson));
	// FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Com_LeftArm"), (CComponent**)&m_pLeftArm, &BossLeftArmJson));
	// FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Com_RightArm"), (CComponent**)&m_pRightArm, &BossRightArmJson));

	m_pASM = CEM320_AnimInstance::Create(m_pModelCom, this);

	m_pController = CEM0320_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

}

void CEM0320::SetUpSound()
{
	CEnemy::SetUpSound();
}

void CEM0320::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();
	m_pModelCom->Add_EventCaller("JumpAttackStart", [this]
	{
		//start damage
		m_fYSpeed = 22.f;
		m_fGravity = 37.f;
		m_bJumpAttack = true;
		Start_AttackState(EAttackStateType::JUMP);

		if (m_pTarget)
		{
			_vector vOrigin = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			vOrigin = XMVectorSetY(vOrigin, 0.f);
			_vector vDest = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
			vDest = XMVectorSetY(vDest, 0.f);
			m_fJumpMoveTime = (2 * m_fYSpeed) / m_fGravity;

			const _vector vDiff = vDest - vOrigin;
			const _float fDistance = XMVectorGetX(XMVector3LengthEst(vDiff));
			const _vector vDirection = XMVector3NormalizeEst(vDiff);

			m_vOnJumpMoveVelocity = vDirection * (fDistance / m_fJumpMoveTime);
		}
		else
		{
			m_fJumpMoveTime = 0.f;
			m_vOnJumpMoveVelocity = _float3::Zero;
		}
	});
	m_pModelCom->Add_EventCaller("Jitabata", [this]
	{
		++m_iJitabaCnt;
		ClearDamagedTarget(); // 매 jitabata 마다 초기화해서 다시 공격 가능
		if (m_iJitabaCnt == 5)
		{
			m_bJumpAttack = false;
			End_AttackState();
		}
	});
	m_pModelCom->Add_EventCaller("Jitabata_Smoke", [this] 
	{
		JitabataSmokeEffect();
	});

	m_pModelCom->Add_EventCaller("Landing", [this]
		{
			_float4x4 Pivot = XMMatrixTranslation(3.f, 0.f, 2.f);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0320_Smoke_Particle")
				->Start_NoAttachPivot(this, Pivot, false, true);

			Pivot = XMMatrixTranslation(-2.f, 0.f, 3.f);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0320_Smoke_Particle")
				->Start_NoAttachPivot(this, Pivot, false, true);

			Pivot = XMMatrixTranslation(2.f, 0.f, -3.f);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0320_Smoke_Particle")
				->Start_NoAttachPivot(this, Pivot, false, true);

			Pivot = XMMatrixTranslation(-3.f, 0.f, -2.f);
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0320_Smoke_Particle")
				->Start_NoAttachPivot(this, Pivot, false, true);
	});

	m_pModelCom->Add_EventCaller("Smash", [this] // TODO : 확인해볼것
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0320_Rock_Particle")->Start_Attach(this, "Reference", false);
	});
	m_pModelCom->Add_EventCaller("Start_SweepRight", [this]
	{		
		Start_AttackState(RIGHT_SWEEP);
	});
	m_pModelCom->Add_EventCaller("Start_SweepLeft", [this]
	{		
		Start_AttackState(LEFT_SWEEP);
	});
	m_pModelCom->Add_EventCaller("SwingL_Eff", [this]
	{
			_float4x4 SwingL_EffPivot = CImguiUtils::CreateMatrixFromImGuizmoData(
				{ 0.f, -1.265f, 2.344f },
				{ -180.f, 0.f, -180.f, },
				{ 1.f, 1.f, 1.f });

		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0320_LeftHand_Slash")
			->Start_AttachPivot(this, SwingL_EffPivot, "Target", true, true, true);

	});
	m_pModelCom->Add_EventCaller("SwingR_Eff", [this]
	{
			_float4x4 SwingR_EffPivot = CImguiUtils::CreateMatrixFromImGuizmoData(
				{ 0.f, -1.265f, 2.344f },
				{ -180.f, 0.f, -180.f, },
				{ 1.f, 1.f, 1.f });

		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0320_RightHand_Slash")
			->Start_AttachPivot(this, SwingR_EffPivot, "Target", true, true, true);
	});
	m_pModelCom->Add_EventCaller("Start_Spin", [this]
	{
		Start_AttackState(SPIN);
	});
	m_pModelCom->Add_EventCaller("SpinEff", [this]
	{
		_matrix PivotMatrix = XMMatrixIdentity();
		PivotMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0320_Spin_Attack")->Start_AttachPivot(this, PivotMatrix, "Target", true, false, false);
	});
	m_pModelCom->Add_EventCaller("AttackEnd", [this]
	{
		End_AttackState();
	});
	m_pModelCom->Add_EventCaller("SmokeGrey", [this]
	{
		CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0320_Smoke_Particle")->Start_Attach(this, "Reference", false);
	});

	// Water ball Create + De_buff : Oil

	m_pModelCom->Add_EventCaller("LastSpot", [this] 
	{ 
		m_LastSpotTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		m_LastSpotTargetPos = XMVectorSetY(m_LastSpotTargetPos, XMVectorGetY(m_LastSpotTargetPos) + 1.f);
	});
	m_pModelCom->Add_EventCaller("WaterBall", [this] 
	{
		FireWaterBall();
	});

	m_pModelCom->Add_EventCaller("DeadFlower", [this]
		{
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0320DeadFlower")
				->Start_NoAttach(this, false);
		});
}

void CEM0320::SetUpMainFSM()
{
	CEnemy::SetUpFSM();
	/*
	 * mouse lb : 싸대기(왼쪽, 오른쪽은 타겟의 위치에 따라서)
	 * mouse rb : 물대포
	 * e : 점프공격
	 * g : 스핀공격
	 */

	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.OnStart([this]
			{
				End_AttackState();
				m_fGravity = 25.f;
			})
			.AddTransition("Idle to Death", "Death")
				.Predicator([this]{ return m_bDead; })
			.AddTransition("Idle to Slap", "Slap")
				.Predicator([this]{ return m_eInput == CController::MOUSE_LB; })
			.AddTransition("Idle to WaterBall", "WaterBall")
				.Predicator([this]{ return m_eInput == CController::MOUSE_RB; })
			.AddTransition("Idle to JumpAtk", "JumpAtk")
				.Predicator([this]{ return m_eInput == CController::E; })
			.AddTransition("Idle to SpinAtk", "SpinAtk")
				.Predicator([this]{ return m_eInput == CController::G; })
			.AddTransition("Idle to Down", "Down")
				.Predicator([this]{ return m_eCurAttackType == EAttackType::ATK_SPECIAL_END; })

		.AddState("Slap")
			.OnStart([this]
			{
				End_AttackState();
				EBaseTurn eTurn = CClientUtils::TurnDeltaToEnum(m_fTurnRemain);
				// Start_SweepLeft 및 Start_SweepRight 이벤트로 공격 시작
				if (eTurn == EBaseTurn::TURN_LEFT)
					m_pASM->AttachAnimSocketOne("FullBody",  "AS_em0300_201_AL_atk_a1_L");
				else
					m_pASM->AttachAnimSocketOne("FullBody",  "AS_em0300_202_AL_atk_a2_R");
			})
			.Tick([this](_double)
			{
				Tick_AttackState();
			})
			.AddTransition("Slap to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})


		.AddState("WaterBall")
			.OnStart([this]
			{
				End_AttackState();
				m_pASM->AttachAnimSocketOne("FullBody",  "AS_em0320_228_AL_atk8_waterball_F");
				// 공격 시작은 Start_Spin 이벤트로 시작
			})
			.AddTransition("WaterBall to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})


		.AddState("SpinAtk")
			.OnStart([this]
			{
				End_AttackState();
				m_pASM->AttachAnimSocketOne("FullBody",  "AS_em0320_225_AL_atk7_spin");
				// 공격 시작은 Start_Spin 이벤트로 시작
			})
			.Tick([this](_double)
			{
				Tick_AttackState();
			})
			.AddTransition("SpinAtk to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})


		.AddState("JumpAtk")
			.OnStart([this]
			{
				m_pController->SetActive(false);
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0300_204_AL_atk_a3_start");
			})
			.Tick([this](_double TimeDelta)
			{
				Tick_AttackState();
				if (m_fJumpMoveTime > 0.f && m_bJumpAttack)
				{
					m_pTransformCom->MoveVelocity(TimeDelta, m_vOnJumpMoveVelocity);
					m_fJumpMoveTime -= (_float)TimeDelta;
				}
			})
			.OnExit([this]
			{
				m_fJumpMoveTime = 0.f;
				m_fGravity = 25.f;
				m_pController->SetActive(true);
			})
			.AddTransition("JumpAtk to Jitabata", "Jitabata")
				.Predicator([this]
				{
					return  m_bJumpAttack && m_bOnFloor;
				})
			.AddTransition("JumpAtk to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead
					|| m_eCurAttackType == EAttackType::ATK_SPECIAL_END;
				})
		.AddState("Jitabata")
			.OnStart([this]
			{
				m_bCanSpecial = true;
				m_pASM->InputAnimSocketMany("FullBody", 
				{  "AS_em0300_205_AL_atk_a3_landing",
					"AS_em0300_233_AL_atk_a3_jitabata_loop",
					"AS_em0300_233_AL_atk_a3_jitabata_loop",
					"AS_em0300_233_AL_atk_a3_jitabata_loop",
					"AS_em0300_233_AL_atk_a3_jitabata_loop",
					"AS_em0300_233_AL_atk_a3_jitabata_loop",
					"AS_em0300_207_AL_atk_a3_end" });
			})
			.Tick([this](_double)
			{
				Tick_AttackState();
			})
			.OnExit([this]
			{
				m_bCanSpecial = false;
			})
			.AddTransition("Jitabata to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead
					|| m_eCurAttackType == EAttackType::ATK_SPECIAL_END
					|| m_pASM->isSocketEmpty("FullBody");
				})



		.AddState("Down")
			.OnStart([this]
			{
				m_fWaterRegenTime = 10.f;
				m_pGlassMtrl->SetActive(false);
				m_pWaterMtrl->SetActive(false);
				m_pWaterMtrl->GetParam().Floats[1] = 0.f;
				m_pController->SetActive(false);
				m_eDeBuff = EDeBuffType::DEBUFF_OIL;

				Reset();
				m_pASM->AttachAnimSocketMany("FullBody", 
				{ "AS_em0300_425_AL_down_start",
					"AS_em0300_426_AL_down",
					"AS_em0300_207_AL_atk_a3_end"  });

				m_fWeakExplosionCnt = 8;
				m_fWeakExplosionTickTime = 0.3f;
			})
			.Tick([this](_double TimeDelta)
			{
				if (m_fWeakExplosionCnt == 0)
					return;

				m_fWeakExplosionTickTime -= (_float)TimeDelta;
				if (m_fWeakExplosionTickTime < 0.f)
				{
					--m_fWeakExplosionCnt;
					m_fWeakExplosionTickTime = 0.3f;
					CreateWeakExplosionEffect();
				}
			})
			.OnExit([this]
			{
				m_pController->SetActive(true);
			})
			.AddTransition("Down to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketEmpty("FullBody");
				})


		.AddState("Death")
			.OnStart([this]
			{
				Reset();
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0300_411_AL_WT_break");
			})



		.Build();
}

void CEM0320::SetUpIntroFSM()
{
	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")

			.AddTransition("Idle to JumpAtk", "JumpAtk")
				.Predicator([this] { return !m_bIntro; })

		.AddState("JumpAtk")
			.OnStart([this]
			{
				m_pController->SetActive(false);
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0300_204_AL_atk_a3_start");
				m_pModelCom->Find_Animation("AS_em0300_204_AL_atk_a3_start")->SetPlayRatio(0.3);
				m_bIntro = true;
			})
			.Tick([this](_double TimeDelta)
			{
					_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					vPos += XMVectorSet(0.f, -1.f, 0.f, 0.f) * 5.f * TimeDelta;

					m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);
			})
			.OnExit([this]
			{
				m_fGravity = 25.f;			
			})
			.AddTransition("JumpAtk to JumpEnd", "JumpEnd")
				.Predicator([this] { return m_bOnFloor; })

		.AddState("JumpEnd")
			.OnStart([this]
			{	
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0300_207_AL_atk_a3_end");
			})
			.AddTransition("JumpEnd to Idle", "Idle")
				.Predicator([this] { return m_pASM->isSocketPassby("FullBody", 0.95f); })


		.Build();

}

void CEM0320::BeginTick()
{
	CEnemy::BeginTick();

	m_pModelCom->FindMaterial(L"MI_em0320_GLASS_0")->SetActive(false);
	m_pGlassMtrl = m_pModelCom->FindMaterial(L"MI_em0320_GLASS_1");
	m_pWeakMtrl = m_pModelCom->FindMaterial(L"MI_em0320_WEAK_0");
	m_pWaterMtrl = m_pModelCom->FindMaterial(L"MI_em0300_WATER_0");
	for (auto pMtrl : m_pModelCom->GetMaterials())
	{
		if (lstrcmp(pMtrl->GetPrototypeTag(), L"MI_em0320_GLASS_0") == 0 
			|| lstrcmp(pMtrl->GetPrototypeTag(), L"MI_em0320_GLASS_1") == 0
			|| lstrcmp(pMtrl->GetPrototypeTag(), L"MI_em0300_WATER_0") == 0)
			continue;
		m_BodyMtrls.push_back(pMtrl);
	}

	// m_pASM->AttachAnimSocket("FullBody", { m_pModelCom->Find_Animation("AS_em0300_160_AL_threat") });

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPosition += XMVectorSet(0.f, 1.f, 0.f, 0.f) * 8.f;
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPosition);

	m_pEMUI->Create_BossUI();
	m_b2ndPhase = true;
}

void CEM0320::Tick(_double TimeDelta)
{
	CEnemy::Tick(TimeDelta);

	m_pController->SetTarget(m_pTarget);
	if (m_bDead == false)
		m_pController->Tick(TimeDelta);
	else
		m_pController->Invalidate();

	m_fTurnRemain = m_pController->GetTurnRemain();
	m_vMoveAxis = m_pController->GetMoveAxis();
	m_vMoveAxis.Normalize();
	m_eInput = m_pController->GetAIInput();

	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);


	if (m_fWeakHitFlash > 0.f)
	{
		m_fWeakHitFlash -= (_float)TimeDelta;
		m_pWeakMtrl->GetParam().Floats[1] = m_fWeakHitFlash;
	}

	if ((_float)m_iHP / (_float)m_iMaxHP < 0.5f)
	{
		m_b2ndPhase = true;
	}

	if (m_fWaterRegenTime > 0.f)
	{
		m_fWaterRegenTime -= (_float)TimeDelta;
		if (m_fWaterRegenTime <= 1.f)
		{
			m_pGlassMtrl->SetActive(true);
			m_pWaterMtrl->SetActive(true);
			m_pWaterMtrl->GetParam().Floats[1] = 1.f - m_fWaterRegenTime;
		}
	}
	else
	{
		m_pWaterMtrl->GetParam().Floats[1] = 1.f;
	}

	// Tick의 제일 마지막에서 실행한다.
	ResetHitData();
}

void CEM0320::Late_Tick(_double TimeDelta)
{
	CEnemy::Late_Tick(TimeDelta);
}

HRESULT CEM0320::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CEM0320::Imgui_RenderProperty()
{
	CEnemy::Imgui_RenderProperty();
	ImGui::InputFloat("angle", &fangle);

	static _bool tt = false;
	ImGui::Checkbox("Modify Pivot", &tt);

	if (tt)
	{
		static GUIZMO_INFO tInfo;
		CImguiUtils::Render_Guizmo(&pivot, tInfo, true, true);

		if (ImGui::Button("TestEffect"))
		{
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0320_LeftHand_Slash")
				->Start_AttachPivot(this, pivot, "Target", true, true, true);
		}

		if (ImGui::Button("TestEffect2"))
		{
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0320_RightHand_Slash")
				->Start_AttachPivot(this, pivot, "Target", true, true, true);
		}
	}

}

void CEM0320::AfterPhysX()
{
	CEnemy::AfterPhysX();

	const _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	m_pRange->Update_Tick(WorldMatrix);
	m_pWeak->Update_Tick(m_pModelCom->GetBoneMatrix("Water") * WorldMatrix);
	m_pHead->Update_Tick(m_pModelCom->GetBoneMatrix("HoseC") * WorldMatrix);
	// m_pLeftArm->Update_Tick(m_pModelCom->GetBoneMatrix("LeftElbow") * WorldMatrix);
	// m_pRightArm->Update_Tick(m_pModelCom->GetBoneMatrix("RightElbow") * WorldMatrix);
}

void CEM0320::TakeDamage(DAMAGE_PARAM tDamageParams)
{
	CEnemy::TakeDamage(tDamageParams);
	if (m_bHitWeak) 
		m_fWeakHitFlash = 1.f;
}

_float4 CEM0320::GetKineticTargetPos()
{
	_matrix WeakSocket = m_pModelCom->GetBoneMatrix("Water") * m_pTransformCom->Get_WorldMatrix();
	return WeakSocket.r[3];
}

_bool CEM0320::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

void CEM0320::Start_AttackState(EAttackStateType eState)
{
	ClearDamagedTarget();
	m_eEnemyAttackType = eState;
	switch (m_eEnemyAttackType)
	{
	case LEFT_SWEEP:
		{
			const _matrix BoneMatrix = m_pModelCom->GetBoneMatrix("LeftHandHelp") * m_pTransformCom->Get_WorldMatrix();
			m_vSweepPrePos = BoneMatrix.r[3];
		}
		break;
	case RIGHT_SWEEP:
		{
			const _matrix BoneMatrix = m_pModelCom->GetBoneMatrix("RightHandHelp") * m_pTransformCom->Get_WorldMatrix();
			m_vSweepPrePos = BoneMatrix.r[3];
		}
		break;
	case SPIN:
		break;
	case JUMP: 
		break;
	case STATE_END: 
		break;
	default: 
		NODEFAULT;
	}
}

void CEM0320::Tick_AttackState()
{
	_float4 vAttackPos;
	switch (m_eEnemyAttackType)
	{
	case LEFT_SWEEP:
		{
			const _matrix BoneMatrix = m_pModelCom->GetBoneMatrix("LeftHandHelp") * m_pTransformCom->Get_WorldMatrix();
			vAttackPos = BoneMatrix.r[3];
			vAttackPos.y -= 2.f;
		}
		break;
	case RIGHT_SWEEP:
		{
			const _matrix BoneMatrix = m_pModelCom->GetBoneMatrix("RightHandHelp") * m_pTransformCom->Get_WorldMatrix();
			vAttackPos = BoneMatrix.r[3];
			vAttackPos.y -= 2.f;
		}
		break;
	case SPIN:
		{
			SphereOverlapParams tParams;
			physx::PxOverlapHit hitBuffer[3];
			physx::PxOverlapBuffer overlapOut(hitBuffer, 3);
			tParams.fRadius = 6.5f;
			tParams.fVisibleTime = 0.1f;
			tParams.iTargetType = CTB_PLAYER;
			tParams.overlapOut = &overlapOut;
			tParams.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			if (CGameInstance::GetInstance()->OverlapSphere(tParams))
			{
				HitTargets(overlapOut, m_iAtkDamage * 1.5, EAttackType::ATK_HEAVY);
			}
		}
		break;
	case JUMP:
		{
			SphereOverlapParams tParams;
			physx::PxOverlapHit hitBuffer[3];
			physx::PxOverlapBuffer overlapOut(hitBuffer, 3);
			tParams.fRadius = 6.f;
			tParams.fVisibleTime = 0.1f;
			tParams.iTargetType = CTB_PLAYER;
			tParams.overlapOut = &overlapOut;
			tParams.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			//tParams.vPos.y += 2.f;
			if (CGameInstance::GetInstance()->OverlapSphere(tParams))
			{
				HitTargets(overlapOut, m_iAtkDamage * 2, EAttackType::ATK_HEAVY);
			}
		}
		break;
	case STATE_END: 
		break;
	default: 
		NODEFAULT;
	}

	if (m_eEnemyAttackType == LEFT_SWEEP || m_eEnemyAttackType == RIGHT_SWEEP)
	{
		SphereSweepParams tParams;

		physx::PxSweepHit hitBuffer[3];
		physx::PxSweepBuffer sweepOut(hitBuffer, 3);
		tParams.sweepOut = &sweepOut;
		tParams.fRadius = 2.5f;
		tParams.fVisibleTime = .1f;
		tParams.iTargetType = CTB_PLAYER;
		tParams.vPos = m_vSweepPrePos;
		const _vector vDiff = vAttackPos - m_vSweepPrePos;
		tParams.vUnitDir = XMVector3NormalizeEst(vDiff);
		tParams.fDistance = XMVectorGetX(XMVector3LengthEst(vDiff));

		if (CGameInstance::GetInstance()->SweepSphere(tParams))
		{
			HitTargets(sweepOut, m_iAtkDamage, EAttackType::ATK_HEAVY);
		}

		m_vSweepPrePos = vAttackPos;
	}
}

void CEM0320::End_AttackState()
{
	m_iJitabaCnt = 0;
	ClearDamagedTarget();
	m_eEnemyAttackType = STATE_END;
}

void CEM0320::Reset()
{
	End_AttackState();
	m_pController->ClearCommands();
}

void CEM0320::DeBuff_End()
{
	for (auto pMtrl : m_BodyMtrls)
	{
		pMtrl->GetParam().Ints[0] = 0;
	}
}

void CEM0320::DeBuff_Fire()
{
	m_fDeBuffTime = 8.f;
	for (auto pMtrl : m_BodyMtrls)
	{
		pMtrl->GetParam().Ints[0] = 1;
	}
}

void CEM0320::DeBuff_Oil()
{
	m_fDeBuffTime = 15.f;
	for (auto pMtrl : m_BodyMtrls)
	{
		pMtrl->GetParam().Ints[0] = 2;
	}
}

_bool CEM0320::IsWeak(CRigidBody* pHitPart)
{
	return pHitPart == m_pWeak;
}

void CEM0320::CheckHP(DAMAGE_PARAM& tDamageParams)
{
	if (m_bHitWeak)
		tDamageParams.iDamage =(_uint)(1.5f * (_float)tDamageParams.iDamage);
	CEnemy::CheckHP(tDamageParams);
}

void CEM0320::FireWaterBall()
{
	//auto pObj = CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_Bullet"), TEXT("Prototype_OilBullet"));		
	//if (COilBullet* pBullet = dynamic_cast<COilBullet*>(pObj))
	//{
	//	pBullet->Set_Owner(this);

	//	_matrix BoneMtx = m_pModelCom->GetBoneMatrix("Water") * m_pTransformCom->Get_WorldMatrix();
	//	_vector vSpawnPos = BoneMtx.r[3];

	//	pBullet->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vSpawnPos);
	//	pBullet->GetTransform()->LookAt(m_LastSpotTargetPos);
	//	pBullet->SetDamage(m_iAtkDamage);

	//	if (m_b2ndPhase)
	//	{
	//		auto pBulletLeft = dynamic_cast<COilBullet*>(CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_Bullet"), TEXT("Prototype_OilBullet")));
	//		pBulletLeft->Set_Owner(this);
	//		pBulletLeft->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vSpawnPos);
	//		pBulletLeft->GetTransform()->LookAt(m_LastSpotTargetPos);
	//		pBulletLeft->GetTransform()->Turn_Fixed(pBulletLeft->GetTransform()->Get_State(CTransform::STATE_UP), -XMConvertToRadians(fangle));
	//		pBulletLeft->SetDamage(m_iAtkDamage);

	//		auto pBulletRight = dynamic_cast<COilBullet*>(CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_Bullet"), TEXT("Prototype_OilBullet")));
	//		pBulletRight->Set_Owner(this);
	//		pBulletRight->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vSpawnPos);
	//		pBulletRight->GetTransform()->LookAt(m_LastSpotTargetPos);
	//		pBulletRight->GetTransform()->Turn_Fixed(pBulletRight->GetTransform()->Get_State(CTransform::STATE_UP), XMConvertToRadians(fangle));
	//		pBulletRight->SetDamage(m_iAtkDamage);
	//	}			
	//}	


	DAMAGE_PARAM eDamageParam;
	eDamageParam.eAttackType = EAttackType::ATK_HEAVY;
	eDamageParam.eDeBuff = EDeBuffType::DEBUFF_OIL;
	eDamageParam.iDamage = m_iAtkDamage;

	_matrix BoneMtx = m_pModelCom->GetBoneMatrix("Water") * m_pTransformCom->Get_WorldMatrix();
	_vector vSpawnPos = BoneMtx.r[3];

	CBulletBuilder()
		.CreateBullet()
			.Set_Owner(this)
			.Set_InitBulletEffect({ L"em0320_Bullet" })
			.Set_InitBulletParticle(L"em0320_Bullet_Trail_Particle")
			.Set_ShootSpeed(29.f)
			.Set_Life(5.f)
			.Set_DamageParam(eDamageParam)
			.Set_DeadBulletEffect({ L"em0320_Bullet_Dead_1", L"em0320_Bullet_Dead_2", L"em0320_Bullet_Dead_3" })
			.Set_Position(vSpawnPos)
			.Set_LookAt(m_LastSpotTargetPos)
		.Build();


	if (m_b2ndPhase)
	{
		CBulletBuilder()
			.CreateBullet()
				.Set_Owner(this)
				.Set_InitBulletEffect({ L"em0320_Bullet" })
				.Set_InitBulletParticle(L"em0320_Bullet_Trail_Particle")
				.Set_ShootSpeed(29.f)
				.Set_Life(5.f)
				.Set_DamageParam(eDamageParam)
				.Set_DeadBulletEffect({ L"em0320_Bullet_Dead_1", L"em0320_Bullet_Dead_2", L"em0320_Bullet_Dead_3" })
				.Set_Position(vSpawnPos)
				.Set_LookAt(m_LastSpotTargetPos)
				.Set_TurnFixed(XMConvertToRadians(fangle))
			.Build();

		CBulletBuilder()
			.CreateBullet()
				.Set_Owner(this)
				.Set_InitBulletEffect({ L"em0320_Bullet" })
				.Set_InitBulletParticle(L"em0320_Bullet_Trail_Particle")
				.Set_ShootSpeed(29.f)
				.Set_Life(5.f)
				.Set_DamageParam(eDamageParam)
				.Set_DeadBulletEffect({ L"em0320_Bullet_Dead_1", L"em0320_Bullet_Dead_2", L"em0320_Bullet_Dead_3" })
				.Set_Position(vSpawnPos)
				.Set_LookAt(m_LastSpotTargetPos)
				.Set_TurnFixed(XMConvertToRadians(fangle))
			.Build();
	}
}


void CEM0320::SmokeEffectCreate()
{
	CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0320_Smoke_Particle")->Start_Attach(this, "Reference", false);
}

void CEM0320::JitabataSmokeEffect()
{
	/*string Smoke_Decided;
	static vector<string> vecSmokePosition
	{
		"LeftCenterHandRing", "LeftHandRing", "RightBackHandThumb3", "RightHandMiddle1", "LeftBackHandRing", "WeakA7"
	};

	Smoke_Decided = vecSmokePosition[CMathUtils::RandomUInt(vecSmokePosition.size() - 1)];

	CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0320_Smoke_Particle")->Start_Attach(this, Smoke_Decided, false, true);*/

	_int RandomX = CMathUtils::RandomInt(-4, 4);
	_int RandomZ = CMathUtils::RandomInt(-4, 4);

	_float4x4 Pivot = XMMatrixTranslation(RandomX, 0.f, RandomZ);
	CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0320_Smoke_Particle")
		->Start_NoAttachPivot(this, Pivot, false, true);
}

void CEM0320::CreateWeakExplosionEffect()
{
	_float3 vWeakOrigin = m_pWeak->GetPxWorldMatrix().Translation();
	vWeakOrigin.y -= 0.5f;
	for (int i = 0; i < 15; ++i)
	{
		const _float3 vPos = CGameUtils::GetRandVector3Sphere(vWeakOrigin, 2.5f);
		const _float3 vDir = CGameUtils::GetRandVector3Sphere(_float3::Zero, 1.f);
		
		const wstring& HitBloodName = s_vecDefaultBlood[CMathUtils::RandomUInt(s_vecDefaultBlood.size() - 1)];
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_HIT, HitBloodName)
			->Start_AttachPosition(this, 
				_float4{vPos.x, vPos.y, vPos.z, 1.f}, 
				_float4{vDir.x, vDir.y, vDir.z, 0.f});
	}
}

CEM0320* CEM0320::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEM0320* pInstance = new CEM0320(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0320");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEM0320::Clone(void* pArg)
{
	CEM0320*		pInstance = new CEM0320(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0320");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0320::Free()
{
	CEnemy::Free();

	Safe_Release(m_pASM);
	Safe_Release(m_pController);
	Safe_Release(m_pWeak);
	Safe_Release(m_pHead);
	Safe_Release(m_pLeftArm);
	Safe_Release(m_pRightArm);
	Safe_Release(m_pRange);

	//for. BossUI 
	// 안녕하세요. 옥수현 입니다. 여기 걸리셨다구요? 
	// 보스를 다 잡고난 후에는 문제 없는 코드지만 보스를 잡기전 중간에 삭제 하실 경우에 객체 원본에서 Free() 가 돌고 난 후 여기 걸리신 것 입니다.
	//if (m_pUI_BossHP != nullptr)
	//	m_pUI_BossHP->SetDelete();
}
