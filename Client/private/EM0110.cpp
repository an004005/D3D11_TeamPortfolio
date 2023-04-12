#include "stdafx.h"
#include "EM0110.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "RigidBody.h"
#include "EM0110_AnimInstance.h"
#include "EM0110_Controller.h"
#include "PhysX_Manager.h"
#include "CurveManager.h"
#include "CurveFloatMapImpl.h"
#include "ImguiUtils.h"
#include "Material.h"

CEM0110::CEM0110(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
	m_eEnemyName = EEnemyName::EM0110;
}

CEM0110::CEM0110(const CEM0110 & rhs)
	: CEnemy(rhs)
{
	m_SpawnEffectPivot = CImguiUtils::CreateMatrixFromImGuizmoData({0.f, 1.68f, 1.3f}, {0.f, 0.f, 0.f}, {1.2f,1.2f,1.2f});
	m_fSpawnDistortionDistancePivot = 1.7f;
}

HRESULT CEM0110::Initialize(void * pArg)
{
	Json em0110_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em0110/em0110Base.json");
	pArg = &em0110_json;

	m_strDeathSoundTag = "horse_fx_death";

	// 배치툴에서 조절할 수 있게 하기
	{
		m_iMaxHP = 1100;
		m_iHP = m_iMaxHP; // ★

		m_iCrushGauge = 3000;
		m_iMaxCrushGauge = m_iCrushGauge;

		m_iAtkDamage = 50;
		iEemeyLevel = 2;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	m_eEnemyName = EEnemyName::EM0110;
	m_bHasCrushGauge = true;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(120.f));

	return S_OK;
}
 
void CEM0110::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em110", L"Model", (CComponent**)&m_pModelCom));

	Json RangeCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em0110/em0110Range.json");
	Json WeakCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em0110/em0110Weak.json");

	Add_RigidBody("Range", &RangeCol);
	Add_RigidBody("Weak", &WeakCol);

	// 컨트롤러, prototype안 만들고 여기서 자체생성하기 위함
	m_pController = CEM0110_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	// ASM
	m_pASM = CEM0110_AnimInstance::Create(m_pModelCom, this);

	m_pArmor = m_pModelCom->FindMaterial(L"MI_em0110_armor_0");
	m_pWeak = m_pModelCom->FindMaterial(L"MI_em0110_weak_0");
}

void CEM0110::SetUpSound()
{
	CEnemy::SetUpSound();

	m_SoundStore.CloneSound("horse_attack_fieldoil");
	m_SoundStore.CloneSound("horse_attack_rush");
	m_SoundStore.CloneSound("horse_attack_spin");
	m_SoundStore.CloneSound("horse_runs_bip");
	m_SoundStore.CloneSound("horse_walks_bip");

	m_pModelCom->Add_EventCaller("horse_attack_fieldoil", [this] {m_SoundStore.PlaySound("horse_attack_fieldoil", m_pTransformCom); });
	m_pModelCom->Add_EventCaller("horse_attack_rush", [this] {m_SoundStore.PlaySound("horse_attack_rush", m_pTransformCom); });
	m_pModelCom->Add_EventCaller("horse_attack_spin", [this] {m_SoundStore.PlaySound("horse_attack_spin", m_pTransformCom); });
	m_pModelCom->Add_EventCaller("horse_runs_bip", [this] {m_SoundStore.PlaySound("horse_runs_bip", m_pTransformCom); });
	m_pModelCom->Add_EventCaller("horse_walks_bip", [this] {m_SoundStore.PlaySound("horse_walks_bip", m_pTransformCom); });
}

void CEM0110::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();

	// Event Caller
	
	m_pModelCom->Add_EventCaller("Turn_R_Start", [this]
	{ 
		ClearDamagedTarget();
		m_bAttack = true;
	});

	m_pModelCom->Add_EventCaller("Turn_R_End", [this] 
	{ 
		m_bAttack = false;	
	});

	m_pModelCom->Add_EventCaller("Turn_L_Start", [this]
	{ 
		ClearDamagedTarget();
		m_bAttack = true;
	});

	m_pModelCom->Add_EventCaller("Turn_L_End", [this] 
	{ 
		m_bAttack = false;	
	});

	//장판공격 시작(Area of Effect)
	m_pModelCom->Add_EventCaller("AOE_Start", [this] 
	{ 
		m_bAttack = true;
		ClearDamagedTarget();
		_matrix 	AOEPivotMatirx = CImguiUtils::CreateMatrixFromImGuizmoData(
			{ -0.4f, -2.2f, -1.f },
			{ 0.f, 0.f, 0.f, },
			{ 1.f, 1.f, 1.f }
		);

		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0110_Bubble_Decal")
				->Start_NoAttachPivot(this, AOEPivotMatirx, false);
		

		AOEPivotMatirx = CImguiUtils::CreateMatrixFromImGuizmoData(
			{ -0.4f, 0.f, -1.f },
			{ 0.f, 0.f,  0.f, },
			{ 0.3f, 0.3f, 0.3f }
		);

		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0110_Large_Bubbles")
			->Start_NoAttachPivot(this, AOEPivotMatirx, false);


		AOEPivotMatirx = CImguiUtils::CreateMatrixFromImGuizmoData(
			{ -0.473f, 0.f, -0.3f },
			{ 0.f, 0.f,  0.f, },
			{ 0.2f, 0.2f, 0.2f }
		);

		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0110_Little_Bubbles_A")
			->Start_NoAttachPivot(this, AOEPivotMatirx, false);


		AOEPivotMatirx = CImguiUtils::CreateMatrixFromImGuizmoData(
			{ -1.236f, 0.f, 1.572f },
			{ 0.f, 0.f, 0.f },
			{ 1.f, 1.f, 1.f }
		);

		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0110_Pop_Bubbles")
			->Start_NoAttachPivot(this , AOEPivotMatirx, false);

	});

	m_pModelCom->Add_EventCaller("AOE_End", [this] 
	{ 
		m_bAttack = false;
	});

	m_pModelCom->Add_EventCaller("Rush_Start", [this]
		{
			ClearDamagedTarget();
			m_bAttack = true;

			//위치 앞으로 옮기려면 x랑 y에 똑같은값을 더해주면 됨
			_matrix RushEffectPivotMatirx = CImguiUtils::CreateMatrixFromImGuizmoData(
				{ 0.f, 0.f, -3.5f },
				{ -180.f, 0.f, -180.f, },
				{ 1.f, 1.f, 1.f }
			);

			m_pRushEffect = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0110_Dash_Attack");
			m_pRushEffect->Start_AttachPivot(this, RushEffectPivotMatirx, "Target", true, true, false);
			Safe_AddRef(m_pRushEffect);
		});

	m_pModelCom->Add_EventCaller("Rush_End", [this] 
	{ 
		m_bAttack = false;

		if (m_pRushEffect != nullptr)
		{
			m_pRushEffect->SetDelete();
			Safe_Release(m_pRushEffect);
			m_pRushEffect = nullptr;
		}
	
	});

	m_pModelCom->Add_EventCaller("DeadFlower", [this]
		{
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0110DeadFlower")
				->Start_NoAttach(this, false);
		});

}

void CEM0110::SetUpFSM()
{
	CEnemy::SetUpFSM();

	/*
	R : Attack_turn (발차기)
	G : Attack_c1 (장판)
	C : Attack_b2 (돌진)
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
				.Predicator([this] { return m_bDestroyArmor; })
			.AddTransition("Idle to Hit_Heavy", "Hit_Heavy")
				.Predicator([this] { return m_eCurAttackType == EAttackType::ATK_HEAVY; })

			.AddTransition("Idle to Attack_turn", "Attack_turn")
				.Predicator([this] { return m_eInput == CController::R; })
			.AddTransition("Idle to Attack_c1", "Attack_c1")
				.Predicator([this] { return m_eInput == CController::G; })
			.AddTransition("Idle to Attack_b2_Start", "Attack_b2_Start")
				.Predicator([this] { return m_eInput == CController::C; })
		
///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Death")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0100_474_AL_dead_down02");
			})

///////////////////////////////////////////////////////////////////////////////////////////
		.AddState("Down")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0100_475_AL_down_start02");
			})
		.AddTransition("Down to Down_Loop", "Down_Loop")
			.Predicator([this]
			{
				return m_pASM->isSocketPassby("FullBody", 0.95f);
			})

		.AddState("Down_Loop")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0100_426_AL_down");
				m_dLoopTick = 0.0;
			})
			.Tick([this](_double TimeDelta)
			{
					m_dLoopTick += TimeDelta;
					if (m_eCurAttackType != EAttackType::ATK_END)
					{
						m_pASM->InputAnimSocketOne("FullBody", "AS_em0100_429_AL_damage_down_add");
					}
			})
			.AddTransition("Down_Loop to GetUp", "GetUp")
				.Predicator([this]
				{
						return m_dLoopTick >= 5.0;
				})

		.AddState("GetUp")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0100_477_AL_getup02");
			})
		.AddTransition("Down to Down_Loop", "Idle")
			.Predicator([this]
			{
				return m_pASM->isSocketPassby("FullBody", 0.95f);
			})
///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Hit_Heavy")
			.OnStart([this]
			{
				Play_HeavyHitAnim();
				HeavyAttackPushStart();
			})
			.Tick([this](_double TimeDelta)
			{
				if (m_eCurAttackType == EAttackType::ATK_HEAVY)
				{
					HeavyAttackPushStart();
					Play_HeavyHitAnim();
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
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

///////////////////////////////////////////////////////////////////////////////////////////

		//발차기
		.AddState("Attack_turn")
			.OnStart([this]
			{
				if (IsTargetFront())
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em0100_230_AL_atk_g1_turn_L");
				else
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em0100_241_AL_atk_h1_turn_R");
		
			})
			.Tick([this](_double) 
			{
				SocketLocalMove(m_pASM);

				if (m_bAttack)
					Kick_SweepSphere();
			})
			.OnExit([this]
			{
				//돌려차기를 했으면 회전한 방향으로 쳐다보게 함
				if (m_pASM->GetCurSocketAnimName() == "AS_em0100_230_AL_atk_g1_turn_L")
					AfterLocal180Turn();		
			
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Attack_turn to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_bDestroyArmor || m_pASM->isSocketPassby("FullBody", 0.95f);
				})
		//장판
		.AddState("Attack_c1")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0110_209_AL_atk_c1");
				m_dLoopTick = 0.0;
			})
			.Tick([this](_double TimeDelta) 
			{
					m_dLoopTick += TimeDelta;

					if (m_bAttack)
					{
						if (m_pTarget->IsOnFloor() && m_dLoopTick >= 1.f)
						{
							ClearDamagedTarget();
							AOE_Overlap();
							m_dLoopTick = 0.0;
						}
					}
				
			})
			.AddTransition("Attack_c1 to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_bDestroyArmor || m_pASM->isSocketPassby("FullBody", 0.95f);
				})
		//돌진
		.AddState("Attack_b2_Start")
			.OnStart([this]
			{
				//m_vRushDirection = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				m_bRush = true;
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0110_206_AL_atk_b2_start");
			})
			.Tick([this](_double) 
			{
				SocketLocalMove(m_pASM);

				if (m_bAttack)
				{
					Rush_Overlap();
					Rush_SweepSphere();
				}
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Attack_b2_Start to Attack_b2_Loop", "Attack_b2_Loop")
				.Predicator([this]
				{
					return m_bDead || m_bDestroyArmor || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Attack_b2_Loop")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0110_207_AL_atk_b2_loop");
				m_pModelCom->Find_Animation("AS_em0110_207_AL_atk_b2_loop")->SetLooping(true);
			})
			.Tick([this](_double TimeDelta)
			{
				SocketLocalMove(m_pASM);
				Rush_Overlap();
				Rush_SweepSphere();
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Attack_b2_Loop to Attack_b2_Stop", "Attack_b2_Stop")
				.Predicator([this]
				{
					return m_bDead || m_bDestroyArmor || !m_bRush;
				})

		.AddState("Attack_b2_Stop")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0110_208_AL_atk_b2_stop");
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
			.AddTransition("Attack_b2_Stop to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_bDestroyArmor || m_pASM->isSocketPassby("FullBody", 0.95f);
				})
///////////////////////////////////////////////////////////////////////////////////////////
	
		.Build();
}

void CEM0110::BeginTick()
{
	CEnemy::BeginTick();
	m_iArmorHp = m_iMaxHP * 0.2;

	//Create BugParticle
	m_pBugParticle = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0110_Bug_Particle");
	m_pBugParticle->Start_Attach(this, "Jaw", true);
	Safe_AddRef(m_pBugParticle);
}

void CEM0110::Tick(_double TimeDelta)
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

	Adjust_MoveAxis(TimeDelta);
	HitWeakProcess(TimeDelta);
	ResetHitData();
}

void CEM0110::Late_Tick(_double TimeDelta)
{
	CEnemy::Late_Tick(TimeDelta);
}

void CEM0110::AfterPhysX()
{
	CEnemy::AfterPhysX();
	
	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();

	GetRigidBody("Range")->Update_Tick(WorldMatrix);
	GetRigidBody("Weak")->Update_Tick(m_pModelCom->GetBoneMatrix("LeftFlower1") * WorldMatrix);
}

HRESULT CEM0110::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CEM0110::Imgui_RenderProperty()
{
	CEnemy::Imgui_RenderProperty();
	if (ImGui::CollapsingHeader("ASM"))
	{
		m_pASM->Imgui_RenderState();
	}

	m_pFSM->Imgui_RenderProperty();

	//static _bool tt = false;
	//ImGui::Checkbox("Modify Pivot", &tt);

	//if (tt)
	//{
	//	static GUIZMO_INFO tInfo;
	//	CImguiUtils::Render_Guizmo(&pivot, tInfo, true, true);

	//	if (ImGui::Button("TestEffect"))
	//	{
	//		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0110_Spin_Attack_Hit_EF")
	//			->Start_NoAttachPivot(this, pivot, false, true);
	//	}
	//}
}

_bool CEM0110::IsWeak(CRigidBody* pHitPart)
{
	return pHitPart == GetRigidBody("Weak");
}

void CEM0110::HitEffect(DAMAGE_PARAM& tDamageParams)
{
	if (m_bHitWeak && m_iArmorHp > 0)
	{
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0110_Dash_Attack_Hit_EF")
			->Start_Attach(this, "Jaw", false, true);
	}
	else
		__super::HitEffect(tDamageParams);
}

void CEM0110::CheckHP(DAMAGE_PARAM& tDamageParams)
{
	if (m_bHitWeak && m_iArmorHp > 0)
	{
		m_iArmorHp -= tDamageParams.iDamage;

		//hp가 0보다 작아지면 아머 삭제. 
		if (m_iArmorHp <= 0)
			m_bDestroyArmor = true;
	}
	else
		__super::CheckHP(tDamageParams);
}

_float4 CEM0110::GetKineticTargetPos()
{
	return XMLoadFloat4x4(&GetRigidBody("Weak")->GetPxWorldMatrix()).r[3];
}

_bool CEM0110::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

void CEM0110::AfterLocal180Turn()
{
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vPos -= XMVector3Normalize(vLook) * 10.f;
	m_pTransformCom->LookAt(vPos);
}


void CEM0110::Adjust_MoveAxis(_double TimeDelta)
{
	_float fMoveSpeed = 0.f;

	if (m_bRun)
		fMoveSpeed = 3.f;
	else
	{
		if (m_pModelCom->GetPlayAnimation() != nullptr)
		{
			fMoveSpeed = CCurveManager::GetInstance()->GetCurve("em0110_Walk")
				->GetValue(m_pModelCom->GetPlayAnimation()->GetPlayRatio());
		}
	}

	if (m_vMoveAxis.LengthSquared() > 0.f)
	{
		const _float fYaw = m_pTransformCom->GetYaw_Radian();
		_float3 vVelocity;
		XMStoreFloat3(&vVelocity, fMoveSpeed * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}

}


void CEM0110::Play_HeavyHitAnim()
{
	if (m_eSimpleHitFrom == ESimpleAxis::NORTH)
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0100_451_AL_damage_l_F02");
	else
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0100_452_AL_damage_l_B02");
}

void CEM0110::HeavyAttackPushStart()
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

void CEM0110::HitWeakProcess(_double TimeDelta)
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
			m_bDestroyArmor = false;
		}
	}
	
}


void CEM0110::Rush_Overlap()
{

	//Length로 Bone의 위치에서부터 바라보는 방향으로 원하는 지점까지의 거리를 지정
	_float fLength = 8.f;

	_matrix BoneMatrix = m_pModelCom->GetBoneMatrix("Target") * m_pTransformCom->Get_WorldMatrix();

	_vector vBoneVector = BoneMatrix.r[3];
	_float3 fBone = vBoneVector;

	_vector vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vMyLook = XMVectorSetY(vMyLook, 0.f);
	_float3 fDest = vMyLook;

	_float3 fFinish = { (fBone.x + fLength * fDest.x), fBone.y, (fBone.z + fLength * fDest.z) };

	physx::PxOverlapHit hitBuffer[3];
	physx::PxOverlapBuffer overlapOut(hitBuffer, 3);

	SphereOverlapParams param;
	param.fVisibleTime = 0.1f;
	param.iTargetType = CTB_STATIC;
	param.fRadius = 1.f;
	param.vPos = XMVectorSetW(fFinish, 1.f);
	param.overlapOut = &overlapOut;

	if (CGameInstance::GetInstance()->OverlapSphere(param))
	{
		m_bRush = false;
	}

}

void CEM0110::Rush_SweepSphere()
{
	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);

	//Tail4가 꼬리 중앙에 있음
	_float4x4 BoneMatrix = GetBoneMatrix("LeftFlower1") * m_pTransformCom->Get_WorldMatrix();
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
		HitTargets(sweepOut, static_cast<_int>(m_iAtkDamage * 1.2f), EAttackType::ATK_HEAVY);
	}

	m_BeforePos = vBonePos;
}

void CEM0110::Kick_SweepSphere()
{
	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);

	//Tail4가 꼬리 중앙에 있음
	_float4x4 BoneMatrix = GetBoneMatrix("RightToeBaseHelp") * m_pTransformCom->Get_WorldMatrix();
	_float4 vBonePos = _float4{ BoneMatrix.m[3][0], BoneMatrix.m[3][1], BoneMatrix.m[3][2], BoneMatrix.m[3][3] };

	_vector	vDir = vBonePos - m_BeforePos;

	SphereSweepParams tParams;
	tParams.fVisibleTime = 0.2f;
	tParams.iTargetType = CTB_PLAYER;
	tParams.fRadius = 1.5f;
	tParams.fDistance = XMVectorGetX(XMVector4LengthEst(vDir));
	tParams.vPos = vBonePos;
	tParams.sweepOut = &sweepOut;
	tParams.vUnitDir = vDir;

	if (CGameInstance::GetInstance()->SweepSphere(tParams))
	{
		//HitTargets(sweepOut, m_iAtkDamage * 1.3f, EAttackType::ATK_TO_AIR);

		for (int i = 0; i < sweepOut.getNbAnyHits(); ++i)
		{
			auto pTarget = dynamic_cast<CScarletCharacter*>(CPhysXUtils::GetOnwer(sweepOut.getAnyHit(i).actor));
			if (pTarget == nullptr)
				continue;

			if (CheckDamagedTarget(pTarget))
			{
				DAMAGE_PARAM tDamageParams;
				tDamageParams.iDamage = m_iAtkDamage * 1.3f;
				tDamageParams.eAttackType = EAttackType::ATK_TO_AIR;
				tDamageParams.eDeBuff = EDeBuffType::DEBUFF_END;
				tDamageParams.pCauser = this;
				tDamageParams.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				memcpy(&tDamageParams.vHitPosition, &sweepOut.getAnyHit(i).position, sizeof(_float3));
				memcpy(&tDamageParams.vHitNormal, &sweepOut.getAnyHit(i).normal, sizeof(_float3));
			
				//발차기에 맞았을때 추가 이펙트
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0110_Spin_Attack_Hit_EF")
					->Start_Attach(this, "RightToeBaseHelp", false, true);
			}
		}
	}

	m_BeforePos = vBonePos;
}

void CEM0110::AOE_Overlap()
{
	_matrix BoneMatrix = m_pModelCom->GetBoneMatrix("Target") * m_pTransformCom->Get_WorldMatrix();

	_vector vBoneVector = BoneMatrix.r[3];
	_float3 fBone = vBoneVector;

	physx::PxOverlapHit hitBuffer[3];
	physx::PxOverlapBuffer overlapOut(hitBuffer, 3);

	SphereOverlapParams param;
	param.fVisibleTime = 0.1f;
	param.iTargetType = CTB_PLAYER;
	param.fRadius = 15.f;
	param.vPos = XMVectorSetW(fBone, 1.f);
	param.overlapOut = &overlapOut;

	if (CGameInstance::GetInstance()->OverlapSphere(param))
	{
		HitTargets(overlapOut, static_cast<_int>(m_iAtkDamage * 1.5f), EAttackType::ATK_END);
	}
}


CEM0110 * CEM0110::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEM0110* pInstance = new CEM0110(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0110");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEM0110::Clone(void * pArg)
{
	CEM0110*		pInstance = new CEM0110(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0110");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0110::Free()
{
	CEnemy::Free();
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
	Safe_Release(m_pRushEffect);

	if (m_bCloned == true)
	{
		if (m_pBugParticle != nullptr)
		{
			m_pBugParticle->SetDelete();
			Safe_Release(m_pBugParticle);
			m_pBugParticle = nullptr;
		}
	}
}
