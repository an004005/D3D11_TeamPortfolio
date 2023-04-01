#include "stdafx.h"
#include "EM0210.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "RigidBody.h"
#include "EM0210_AnimInstance.h"
#include "EM0210_Controller.h"
#include "PhysX_Manager.h"
#include "GameInstance.h"
#include "ImguiUtils.h"
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
	Json em0210_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em0210/em0210Base.json");
	pArg = &em0210_json;


	// 배치툴에서 조절할 수 있게 하기
	{
		m_iMaxHP = 1100;
		m_iHP = 1100; // ★

		m_iAtkDamage = 50;

		m_iCrushGauge = 1100;
		m_iMaxCrushGauge = 1100;
		iEemeyLevel = 2;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	m_eEnemyName = EEnemyName::EM0210;
	m_bHasCrushGauge = true;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(220.f));

	//시작부터 투명상태 적용
	m_IsInvisible = true;

	return S_OK;
}

void CEM0210::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW,
		L"Prototype_Model_em210", L"Model",
		(CComponent**)&m_pModelCom));

	Json RangeCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em0210/em0210Range.json");

	//Create Collider
	Add_RigidBody("Range", &RangeCol);

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

		_matrix SpinEffectPivotMatirx = CImguiUtils::CreateMatrixFromImGuizmoData(
			{ 1.65f, -0.55f, 1.121f },
			{ 180.f, 0.f, 0.f, },
			{ 1.f, 1.f, 1.f });

		 CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0210_Spin_Attack")
		->Start_AttachPivot(this, SpinEffectPivotMatirx, "Target", true, true);

	});

	m_pModelCom->Add_EventCaller("Spin_Effect1", [this]
		{
			_matrix SpinEffectPivotMatirx = CImguiUtils::CreateMatrixFromImGuizmoData(
				{ 1.65f, -0.55f, 1.121f },
				{ 180.f, 0.f, 0.f, },
				{ 1.f, 1.f, 1.f });

			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0210_Spin_Attack")
				->Start_AttachPivot(this, SpinEffectPivotMatirx, "Target", true, true);
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

		_matrix SpinEffectPivotMatirx = CImguiUtils::CreateMatrixFromImGuizmoData(
			{ 0.f, 0.f, 0.f },
			{ 180.f, 0.f, 0.f, },
			{ 1.f, 1.f, 1.f });

		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0210_Spin_Attack_A")
			->Start_AttachPivot(this, SpinEffectPivotMatirx, "RightToeBase", true, true);
		
	});

	//Somersault end
	m_pModelCom->Add_EventCaller("Somersault_End", [this]
	{
		m_bAttack = false;
	});

	m_pModelCom->Add_EventCaller("DeadFlower", [this]
		{
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0220DeadFlower")
				->Start_NoAttach(this, false);
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
			.AddTransition("Idle to BrainCrushStart", "BrainCrushStart")
				.Predicator([this] { return m_bCrushStart; })
			.AddTransition("Idle to Death", "Death")
				.Predicator([this] { return m_bDead; })
			.AddTransition("Idle to Down", "Down")
				.Predicator([this]	{ return m_bDown; })

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
				if (m_eCurAttackType == EAttackType::ATK_LIGHT
					|| m_eCurAttackType == EAttackType::ATK_SPECIAL_LOOP)
				{
					Play_LightHitAnim();
				}
			})
			.AddTransition("Hit_Light to Idle", "Idle")
				.Predicator([this]
				{
						return m_bCrushStart || m_bDead
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
						return m_bCrushStart || m_bDead
							|| m_pASM->isSocketPassby("FullBody", 0.95f)
							|| m_eCurAttackType == EAttackType::ATK_TO_AIR
							|| m_eCurAttackType == EAttackType::ATK_SPECIAL_LOOP;
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
					if (m_bAirToDown)
					{
						m_fYSpeed = -20.f;
					}
					else if (m_eCurAttackType != EAttackType::ATK_END)
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
				return m_bCrushStart || m_bDead || m_pASM->isSocketEmpty("FullBody");
			})

		.AddState("Down")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_425_AL_down_start");
				m_bDown = false;
			})
			.AddTransition("Down to OnFloorGetup", "OnFloorGetup")
				.Predicator([this]
				{
						return m_bCrushStart || m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("BrainCrushStart")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody",  "AS_em0200_485_AL_BCchance_start");
			})
			.AddTransition("BrainCrushStart to BrainCrushLoop", "BrainCrushLoop")
				.Predicator([this]
				{
					return m_bBrainCrush || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("BrainCrushLoop")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_486_AL_BCchance_loop");
				m_pModelCom->Find_Animation("AS_em0200_486_AL_BCchance_loop")->SetLooping(true);
			})
			.AddTransition("BrainCrushLoop to BrainCrushEnd", "BrainCrushEnd")
				.Predicator([this]
				{
					//브레인 크러쉬를 하지 않았을때는 부모쪽에서 3초 안에 죽임
					return m_bBrainCrush;
				})

		.AddState("BrainCrushEnd")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_487_AL_BCchance_end");
			})
			.Tick([this](_double)
			{
					if (m_pASM->isSocketPassby("FullBody", 0.95f))
						SetDead();
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
					return m_bCrushStart || m_bDead || (m_bDodge && m_bOnFloor);
				})
			.AddTransition("Dodge to Idle", "Idle")
				.Predicator([this]
				{
					return m_bCrushStart || m_bDead || m_bDown || m_eCurAttackType != EAttackType::ATK_END;
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
						return m_bCrushStart || m_bDead || m_bDown || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketPassby("FullBody", 0.99f);
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
					Swing_SweepSphere();
			})
			.AddTransition("Attack_Spin to Idle", "Idle")
				.Predicator([this]
				{
					return m_bCrushStart || m_bDead || m_bDown ||  m_eCurAttackType == EAttackType::ATK_TO_AIR || m_pASM->isSocketPassby("FullBody", 0.95f);
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
					Somersault_SweepSphere();
			})
			.AddTransition("Attack_Somersault to Idle", "Idle")
				.Predicator([this]
			{
				return m_bCrushStart || m_bDead || m_bDown || m_eCurAttackType == EAttackType::ATK_TO_AIR || m_pASM->isSocketPassby("FullBody", 0.95f);
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
					return m_bCrushStart || m_bDead || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

///////////////////////////////////////////////////////////////////////////////////////////

		.Build();
}

void CEM0210::SetUpUI()
{
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
	m_eSASType = CheckSASType(ESASType::SAS_PENETRATE);

	if (m_eSASType == true)
		m_dRenderChangeDelay += TimeDelta;
	else
		m_dRenderChangeDelay = 0.0;

	if (m_IsFirstHit == false 
		&& m_eSASType
		&& m_eCurAttackType != EAttackType::ATK_END)
	{
		m_IsInvisible = false;
		m_bDown = true;
		m_IsFirstHit = true;
	}


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

	if (m_IsInvisible)
	{
		if (m_dRenderChangeDelay >=0.5)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		else
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	}
	else
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}


void CEM0210::AfterPhysX()
{
	CEnemy::AfterPhysX();

	GetRigidBody("Range")->Update_Tick(m_pTransformCom->Get_WorldMatrix());
}

HRESULT CEM0210::Render()
{
	if (m_IsInvisible)
	{
		if (m_dRenderChangeDelay >= 0.7)
			m_pModelCom->Render(m_pTransformCom);
		else
			m_pModelCom->Render_Pass(m_pTransformCom, 5);
	}
	else
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

	/*static _bool tt = false;
	ImGui::Checkbox("Modify Pivot", &tt);
	
	if (tt)
	{
		static GUIZMO_INFO tInfo;
		CImguiUtils::Render_Guizmo(&pivot, tInfo, true, true);

		if (ImGui::Button("Create_Effect"))
		{
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0210_Spin_Attack")
				->Start_AttachPivot(this, pivot, "Target", true, true);
		}
	}*/
		
	ImGui::InputInt("HP", &m_iHP);
	ImGui::InputInt("Crush", &m_iCrushGauge);
}

_bool CEM0210::Exclude()
{
	return m_IsInvisible && CheckSASType(ESASType::SAS_PENETRATE) == false;
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

void CEM0210::Swing_SweepSphere()
{
	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);

	//Tail4가 꼬리 중앙에 있음
	_float4x4 BoneMatrix = GetBoneMatrix("RightToeBase") * m_pTransformCom->Get_WorldMatrix();
	_float4 vBonePos = _float4{ BoneMatrix.m[3][0], BoneMatrix.m[3][1], BoneMatrix.m[3][2], BoneMatrix.m[3][3] };

	_vector	vDir = vBonePos - m_BeforePos;

	SphereSweepParams tParams;
	tParams.fVisibleTime = 0.2f;
	tParams.iTargetType = CTB_PLAYER;
	tParams.fRadius = 2.f;
	tParams.fDistance = 1.f;
	tParams.vPos = vBonePos;
	tParams.sweepOut = &sweepOut;
	tParams.vUnitDir = vDir;

	if (CGameInstance::GetInstance()->SweepSphere(tParams))
	{
		HitTargets(sweepOut, static_cast<_int>(m_iAtkDamage * 1.2f), EAttackType::ATK_HEAVY);
	}

	m_BeforePos = vBonePos;
}

void CEM0210::Somersault_SweepSphere()
{
	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);

	//Tail4가 꼬리 중앙에 있음
	_float4x4 BoneMatrix = GetBoneMatrix("RightToeBase") * m_pTransformCom->Get_WorldMatrix();
	_float4 vBonePos = _float4{ BoneMatrix.m[3][0], BoneMatrix.m[3][1], BoneMatrix.m[3][2], BoneMatrix.m[3][3] };

	_vector	vDir = vBonePos - m_BeforePos;

	SphereSweepParams tParams;
	tParams.fVisibleTime = 0.2f;
	tParams.iTargetType = CTB_PLAYER;
	tParams.fRadius = 1.5f;
	tParams.fDistance = 1.f;
	tParams.vPos = vBonePos;
	tParams.sweepOut = &sweepOut;
	tParams.vUnitDir = vDir;

	if (CGameInstance::GetInstance()->SweepSphere(tParams))
	{
		HitTargets(sweepOut, static_cast<_int>(m_iAtkDamage * 1.f), EAttackType::ATK_TO_AIR);
	}

	m_BeforePos = vBonePos;
}

void CEM0210::HeavyAttackPushStart()
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

CGameObject* CEM0210::Clone(void* pArg)
{
	CEM0210* pInstance = new CEM0210(*this);

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

}
