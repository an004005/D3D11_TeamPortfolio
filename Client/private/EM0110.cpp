#include "stdafx.h"
#include "EM0110.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "RigidBody.h"
#include "EM0110_AnimInstance.h"
#include "EM0110_Controller.h"
#include "EnemyBullet.h"
#include "PhysX_Manager.h"

CEM0110::CEM0110(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
	m_eMonsterName = EEnemyName::EM0110;
}

CEM0110::CEM0110(const CEM0110 & rhs)
	: CEnemy(rhs)
{
}

HRESULT CEM0110::Initialize(void * pArg)
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
		iMonsterLevel = 2;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	m_eMonsterName = EEnemyName::EM0110;
	m_bHasCrushGage = true;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(220.f));
	m_pTransformCom->SetSpeed(3.f);
	return S_OK;
}

void CEM0110::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW,
		L"Prototype_Model_em110", L"Model",
		(CComponent**)&m_pModelCom));

	//Create Collider
	//FAILED_CHECK(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("TailCol"),
	//	(CComponent**)&m_pTailCol, pArg));

	//FAILED_CHECK(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("WeaponCol"),
	//	(CComponent**)&m_pWeaponCol, pArg));

	// 컨트롤러, prototype안 만들고 여기서 자체생성하기 위함
	m_pController = CEM0110_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	// ASM
	m_pASM = CEM0110_AnimInstance::Create(m_pModelCom, this);
}

void CEM0110::SetUpSound()
{
	CEnemy::SetUpSound();
}

void CEM0110::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();

	// Event Caller

}

void CEM0110::SetUpFSM()
{
	CEnemy::SetUpFSM();

	/*
	R : Attack_turn (발차기)
	G : Attack_c1 (장판)
	C : Attack_b2 (돌진)

	CTRL : Threat
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

			.AddTransition("Idle to Hit_Heavy", "Hit_Heavy")
				.Predicator([this] { return m_eCurAttackType == EAttackType::ATK_HEAVY; })
			.AddTransition("Idle to Down", "Down")
				.Predicator([this] { return  m_iCrushGage <= 0; })

			.AddTransition("Idle to Attack_turn", "Attack_turn")
				.Predicator([this] { return m_eInput == CController::R; })
			.AddTransition("Idle to Attack_c1", "Attack_c1")
				.Predicator([this] { return m_eInput == CController::G; })
			.AddTransition("Idle to Attack_b2_Start", "Attack_b2_Start")
				.Predicator([this] { return m_eInput == CController::C; })
			.AddTransition("Idle to Threat", "Threat")
				.Predicator([this] { return m_eInput == CController::CTRL; })
		
///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Hit_Heavy")
			.OnStart([this]
			{
				Play_HeavbyHitAnim();
			})
			.Tick([this](_double)
			{
				if (m_eCurAttackType == EAttackType::ATK_HEAVY)
				{
					Play_HeavbyHitAnim();
				}
			})
			.AddTransition("Hit_Heavy to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Death")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0100_474_AL_dead_down02");
			})

		.AddState("Down")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0100_475_AL_down_start02");
			})
			.AddTransition("Down to OnFloorGetup", "OnFloorGetup")
				.Predicator([this]
			{
				return m_pASM->isSocketPassby("FullBody", 0.95f);
			})

		.AddState("OnFloorGetup")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketMany("FullBody", { "AS_em0100_480_AL_down_shock" "AS_em0100_477_AL_getup02" });
				m_fGravity = 20.f;
			})
				.AddTransition("OnFloorGetup to Idle", "Idle")
				.Predicator([this]
			{
				return  m_pASM->isSocketEmpty("FullBody");
			})
///////////////////////////////////////////////////////////////////////////////////////////

		//돌려차기
		.AddState("Attack_turn")
			.OnStart([this]
			{
				Define_TurningKickAnim();
			})
			.Tick([this](_double) 
			{
				SocketLocalMove(m_pASM);
			/*	if (m_bAttack)
					SweepCapsule(m_pTailCol);*/
			})
			.OnExit([this]
			{
				//돌려차기를 했으면 회전한 방향으로 쳐다보게 함
				AfterLocal180Turn();
				m_pASM->ClearSocketAnim("FullBody");

				//if (m_pASM->GetCurSocketAnimName() == "AS_em0100_230_AL_atk_g1_turn_L")
				//{
				//	//m_pModelCom->GetPlayAnimation()->SetInterpolation(false);
				//}
			})
			.AddTransition("Attack_turn to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.6667f);
				})
		//장판
		.AddState("Attack_c1")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0110_209_AL_atk_c1");
			})
			.Tick([this](_double) 
			{
					//이땐 데미지 어떻게?
			})
			.AddTransition("Attack_c1 to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})
		//돌진
		.AddState("Attack_b2_Start")
			.OnStart([this]
			{
				m_fRushDistance = 0.f;
				m_vRushDirection = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0110_206_AL_atk_b2_start");
			})
			.AddTransition("Attack_b2_Start to Attack_b2_Loop", "Attack_b2_Loop")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})
		.AddState("Attack_b2_Loop")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0110_207_AL_atk_b2_loop");
				m_pModelCom->Find_Animation("AS_em0110_207_AL_atk_b2_loop")->SetLooping(true);
			})
			.Tick([this](_double TimeDelta)
			{
				m_fRushDistance += m_pTransformCom->RushToTarget(m_vRushDirection, TimeDelta);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody");
			})
			.AddTransition("Attack_b2_Loop to Attack_b2_Stop", "Attack_b2_Stop")
				.Predicator([this]
				{
					return m_bDead || m_fRushDistance >= 15.f;
				})
		.AddState("Attack_b2_Stop")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0110_208_AL_atk_b2_stop");
			})
			.OnExit([this]
			{
				AfterLocal180Turn();
			})
			.AddTransition("Attack_b2_Stop to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})
///////////////////////////////////////////////////////////////////////////////////////////
	
		.AddState("Threat")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0100_160_AL_threat");
			})
			.AddTransition("Threat to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

///////////////////////////////////////////////////////////////////////////////////////////

		.Build();
}

void CEM0110::BeginTick()
{
	CEnemy::BeginTick();
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

	const _float fMoveSpeed = m_bRun ? 3.f : 1.f;

	if (m_vMoveAxis.LengthSquared() > 0.f)
	{
		const _float fYaw = m_pTransformCom->GetYaw_Radian();
		_float3 vVelocity;
		XMStoreFloat3(&vVelocity, fMoveSpeed * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}

	ResetHitData();
}

void CEM0110::Late_Tick(_double TimeDelta)
{
	CEnemy::Late_Tick(TimeDelta);
}

void CEM0110::AfterPhysX()
{
	CEnemy::AfterPhysX();
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
}

_bool CEM0110::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

void CEM0110::AfterLocal180Turn()
{
	/*IM_LOG("Turn");
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vPos -= XMVector3Normalize(vLook) * 10.f;
	m_pTransformCom->LookAt(vPos);*/
}

void CEM0110::Define_TurningKickAnim()
{
	_vector vTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	
	_float fAngle = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vMyLook), XMVector3Normalize(vTargetPos - vMyPos)));

	//0~ 90도 사이(정면)
	//if(fAngle > 0)
		m_pASM->AttachAnimSocketOne("FullBody", "AS_em0100_230_AL_atk_g1_turn_L");
	//else
	//	m_pASM->AttachAnimSocketOne("FullBody", "AS_em0100_241_AL_atk_h1_turn_R");

}


void CEM0110::Play_HeavbyHitAnim()
{
	if (m_eSimpleHitFrom == ESimpleAxis::NORTH)
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0100_451_AL_damage_l_F02");
	else
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0100_452_AL_damage_l_B02");
}

void CEM0110::SweepCapsule(CRigidBody * pCol)
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
	Safe_Release(m_pRange);
}
