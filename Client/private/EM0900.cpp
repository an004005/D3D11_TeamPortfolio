#include "stdafx.h"
#include "..\public\EM0900.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "EM0900_AnimInstance.h"
#include "EM0900_Controller.h"
#include "ImguiUtils.h"
#include "BulletBuilder.h"
#include "Material.h"
#include "RigidBody.h"
#include "ControlledRigidBody.h"
#include "EMBrain.h"

CEM0900::CEM0900(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
	m_eEnemyName = EEnemyName::EM1100;
}

CEM0900::CEM0900(const CEM0900 & rhs)
	: CEnemy(rhs)
{
	m_bSpawnEffect = false;
}

HRESULT CEM0900::Initialize(void * pArg)
{
	/*Json em900_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em0900/em0900Base.json");
	pArg = &em900_json;*/

	/*m_strDeathSoundTag = "mon_5_fx_death";
	m_strImpactVoiceTag = "mon_5_impact_voice";*/

	// 배치툴에서 조절할 수 있게 하기
	{
		m_iMaxHP = 3000;
		m_iHP = 3000; // ★

		m_iCrushGauge = 8000;
		m_iMaxCrushGauge = 8000;

		m_iAtkDamage = 50;
		iEemeyLevel = 2;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	m_eEnemyName = EEnemyName::EM0900;
	m_bHasCrushGauge = true;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(90.f));

	m_fGravity = 20.f;
	return S_OK;
}

void CEM0900::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em900", L"Model", (CComponent**)&m_pModelCom));

	m_pWeak = m_pModelCom->FindMaterial(L"MI_em0900_weak_0");
	m_pArmor = m_pModelCom->FindMaterial(L"MI_em0900_armor_0");

	assert(m_pWeak != nullptr);
	assert(m_pArmor != nullptr);

	//Json WeakCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em1100/em1100Weak.json");
	//Json BodyCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em0110/em1100Body.json");

	//Add_RigidBody("Weak", &WeakCol);
	//Add_RigidBody("Body", &BodyCol);

	// 컨트롤러, prototype안 만들고 여기서 자체생성하기 위함
	m_pController = CEM0900_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	// ASM
	m_pASM = CEM0900_AnimInstance::Create(m_pModelCom, this);
}

void CEM0900::SetUpSound()
{
	CEnemy::SetUpSound();
}

void CEM0900::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();
	

	m_pModelCom->Add_EventCaller("DeadFlower", [this]
		{
			CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em1100DeadFlower")
				->Start_NoAttach(this, false);
		});
}

void CEM0900::SetUpFSM()
{
	CEnemy::SetUpFSM();

	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.OnStart([this]
			{
				m_fGravity = 20.f;
			})
			/*.AddTransition("Idle to BrainCrushStart", "BrainCrushStart")
				.Predicator([this] { return m_bCrushStart; })*/
			.AddTransition("Idle to Death" , "Death")
				.Predicator([this] { return m_bDead; })
			.AddTransition("Idle to Down", "Down")
				.Predicator([this] { return m_bDestroyArmor; })

			.AddTransition("Idle to Hit_Heavy", "Hit_Heavy")
				.Predicator([this]
					{ 
						return m_eCurAttackType == EAttackType::ATK_HEAVY;
					})



			.AddTransition("Idle to 4Attack", "4Attack")
				.Predicator([this] { return m_eInput == CController::F; })

			.AddTransition("Idle to Spin", "Spin")
				.Predicator([this] { return m_eInput == CController::S; })

			.AddTransition("Idle to Throw_OmenStart", "Throw_OmenStart")
				.Predicator([this] { return m_eInput == CController::T; })
			
///////////////////////////////////////////////////////////////////////////////////////////
	
		.AddState("Hit_Heavy")
			.OnStart([this]
			{
				Play_LightHitAnim();
				HeavyAttackPushStart();
			})
			.Tick([this](_double TimeDelta)
			{
				if (m_eCurAttackType == EAttackType::ATK_HEAVY)
				{
					Play_LightHitAnim();
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
		.AddState("Death")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0900_424_AL_dead_down");
			})


///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Down")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0900_425_AL_down_start");
			})
			.AddTransition("Down to Down_Loop", "Down_Loop")
				.Predicator([this]
				{
						return m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Down_Loop")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0900_426_AL_down");
				m_dLoopTick = 0.0;
			})
			.Tick([this](_double TimeDelta)
			{
				m_dLoopTick += TimeDelta;
				if (m_eCurAttackType != EAttackType::ATK_END)
				{
					m_pASM->InputAnimSocketOne("FullBody", "AS_em0900_429_AL_damage_down_add");
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
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0900_427_AL_getup");
			})
			.AddTransition("Down to Down_Loop", "Idle")
				.Predicator([this]
				{
					return m_pASM->isSocketPassby("FullBody", 0.95f);
				})

///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("BrainCrushStart")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody",  "AS_em1100_485_AL_BCchance_start");
			})
			.AddTransition("BrainCrushStart to BrainCrushLoop", "BrainCrushLoop")
				.Predicator([this]
				{
					return m_bBrainCrush || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("BrainCrushLoop")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em1100_486_AL_BCchance_loop");
				m_pModelCom->Find_Animation("AS_em1100_486_AL_BCchance_loop")->SetLooping(true);

				//브레인 생성
				m_pBrain = dynamic_cast<CEMBrain*>(CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_EnemyBrain"), TEXT("Prototype_EMBrain")));

				m_BCLoopTime = 0.0;
			})
			.Tick([this](_double TimeDelta)
			{
					_matrix WeakBoneMatrix = GetBoneMatrix("Weak01") * m_pTransformCom->Get_WorldMatrix();
					m_pBrain->GetTransform()->Set_WorldMatrix(WeakBoneMatrix);
					m_BCLoopTime += TimeDelta;
			})

			.AddTransition("BrainCrushLoop to BrainCrushLoopDead", "BrainCrushLoopDead")
				.Predicator([this]
				{
					//브레인 크러쉬를 하지 않았을때는 부모쪽에서 3초 안에 죽임
					return m_BCLoopTime >= 3.0;
				})
			.AddTransition("BrainCrushLoop to BrainCrushEnd", "BrainCrushEnd")
				.Predicator([this]
					{
						return	m_bBrainCrush;
					})
		.AddState("BrainCrushLoopDead")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em1100_487_AL_BCchance_end");
				m_pBrain->SetDelete();
				SetDead();
			})
		
		.AddState("BrainCrushEnd")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_BC_em1100m_em1100");
				m_pBrain->BeginBC();
			})
			.Tick([this](_double)
			{
					SocketLocalMove(m_pASM);

					if (m_pBrain != nullptr)
					{
						_matrix WeakBoneMatrix = GetBoneMatrix("Weak01") * m_pTransformCom->Get_WorldMatrix();
						m_pBrain->GetTransform()->Set_WorldMatrix(WeakBoneMatrix);

						//const _float fPlayRatio = m_CanFullBC ? 0.8f : 0.2f;
						const _float fPlayRatio = m_CanFullBC ? 0.8f : 0.55f;

						if (m_pASM->isSocketPassby("FullBody", fPlayRatio))
						{
							m_pBrain->EndBC();
							m_pBrain->SetDelete();
							m_pBrain = nullptr;
							SetDead();
						}
					}
				
			})

///////////////////////////////////////////////////////////////////////////////////////////
		
		//때릴때마다 이벤트로 공격
		.AddState("4Attack")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0900_205_AL_atk_a5_4attack");
			})
			.Tick([this](_double TimeDelta)
			{
				m_pTransformCom->LookAt_Smooth(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION), TimeDelta);
			})
			.AddTransition("4Attack to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Spin")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0900_207_AL_atk_a7");
					
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
				if (m_bAttack)
					//공격
					;
			})
			.AddTransition("Spin to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Throw_OmenStart")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0900_218_AL_atk_a1_omenstart");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.AddTransition("Throw_OmenStart to Throw_OmenLoop", "Throw_OmenLoop")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Throw_OmenLoop")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0900_219_AL_atk_a1_omensloop");
				m_dLoopTick = 0.0;
			})
			.Tick([this](_double TimeDelta)
			{
				SocketLocalMove(m_pASM);
				m_dLoopTick += TimeDelta;
			})
			.AddTransition("Throw_OmenLoop to Feint", "Feint")
				.Predicator([this]
				{
					return m_bDead || (m_dLoopTick>= 0.5 && m_pController->GetGapDistance() < 7.f);
				})
			.AddTransition("Throw_OmenLoop to Throw_Start", "Throw_Start")
				.Predicator([this]
				{
					return m_bDead || (m_dLoopTick >= 0.5 && m_pController->GetGapDistance() >= 7.f);
				})

		.AddState("Feint")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0900_202_AL_atk_a2_feint");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.AddTransition("Feint to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Throw_Start")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0900_220_AL_atk_a1_start");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.AddTransition("Throw_Start to Throw_Loop", "Throw_Loop")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Throw_Loop")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0900_221_AL_atk_a1_loop");
			})
			.Tick([this](_double TimeDelta)
			{
					m_pTransformCom->LookAt_Smooth(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION), TimeDelta);
			})
			.AddTransition("Throw_Loop to Throw_End", "Throw_End")
				.Predicator([this]
				{
					return m_bDead || abs(m_pController->GetTurnRemain()) < 0.1f;
				})

		//이벤트에서 플레이어쪽으로 던짐
		.AddState("Throw_End")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0900_222_AL_atk_a1_end");
			})
			.AddTransition("Throw_End to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})
	
///////////////////////////////////////////////////////////////////////////////////////////

		.Build();
}

void CEM0900::BeginTick()
{
	CEnemy::BeginTick();
	m_pEMUI->Create_BossUI();
}

void CEM0900::Tick(_double TimeDelta)
{
	CEnemy::Tick(TimeDelta);

	//AIInstance tick
	m_pController->SetTarget(m_pTarget);

	if (m_bDead == false)
		m_pController->Tick(TimeDelta);
	else
		m_pController->Invalidate();

	//변수 업데이트
	m_vMoveAxis = m_pController->GetMoveAxis();
	m_vMoveAxis.Normalize();
	m_eInput = m_pController->GetAIInput();

	//ASM, FSM tick
	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);


	if (m_vMoveAxis.LengthSquared() > 0.f)
	{
		_float3 vVelocity;
		XMStoreFloat3(&vVelocity, 2.f /*MoveSpeed*/ * XMVector3Normalize(m_vMoveAxis));

		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}

	HitWeakProcess(TimeDelta);
	// Tick의 제일 마지막에서 실행한다.
	ResetHitData();
}

void CEM0900::Late_Tick(_double TimeDelta)
{
	CEnemy::Late_Tick(TimeDelta);
}

void CEM0900::AfterPhysX()
{
	CEnemy::AfterPhysX();

	/*_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	GetRigidBody("Weak")->Update_Tick(m_pModelCom->GetBoneMatrix("Shell3") * WorldMatrix);
	GetRigidBody("Body")->Update_Tick(m_pModelCom->GetBoneMatrix("Target") * WorldMatrix);*/
}

HRESULT CEM0900::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CEM0900::Imgui_RenderProperty()
{
	CEnemy::Imgui_RenderProperty();
	if (ImGui::CollapsingHeader("ASM"))
	{
		m_pASM->Imgui_RenderState();
	}
	m_pFSM->Imgui_RenderProperty();

	if (ImGui::Button("BrainCrashOn"))
	{
		m_bCrushStart = true;
	}

	//static _bool tt = false;
	//ImGui::Checkbox("Modify Pivot", &tt);

	//if (tt)
	//{
	//	static GUIZMO_INFO tInfo;
	//	CImguiUtils::Render_Guizmo(&pivot, tInfo, true, true);

	//	if (ImGui::Button("TetsEffect"))
	//	{

	//		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em1100_Elec_Bullet_Loop")
	//		->Start_NoAttachPivot(this, pivot, true, true);
	//	}
	//}
}

_bool CEM0900::IsWeak(CRigidBody* pHitPart)
{
	return 	pHitPart == GetRigidBody("Weak");
}

void CEM0900::PlayBC()
{
	__super::PlayBC();

	// 브레인 크러시 동작 중에는 모든 콜라이더를 사용하지 않도록 함
	m_pCollider->SetActive(false);
	for (auto& iter : m_pRigidBodies)
	{
		iter.second->Activate(false);
	}

	const _float fMinDist = 5.f; //플레이어에서 벽까지의 거리, 
	const _float fMaxDist = 7.f; //플레이어에서 몬스터까지의 거리

	m_CanFullBC = CanMove4BC(fMinDist);

	//가까우면 들어서 올림
	if (m_CanFullBC == false) return;

	_vector vTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vTargetLook = m_pTarget->GetTransform()->Get_State(CTransform::STATE_LOOK);

	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_float fDist = XMVectorGetX(XMVector4Length(vMyPos - vTargetPos));

	//너무 멀면 이동 안시킴
	if (fDist > fMaxDist)
	{
		m_CanFullBC = false;
		return;
	}
}

_bool CEM0900::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

void CEM0900::Play_LightHitAnim()
{
	if (m_eSimpleHitFrom == ESimpleAxis::NORTH)
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0900_401_AL_damage_l_F");
	else
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0900_402_AL_damage_l_B");
}

void CEM0900::HeavyAttackPushStart()
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

void CEM0900::Create_Bullet()
{
	DAMAGE_PARAM eDamageParam;

	//TODO : 상태이상 정리되면 감전상태로 변경

	eDamageParam.eAttackType = EAttackType::ATK_MIDDLE;
	eDamageParam.eDeBuff = EDeBuffType::DEBUFF_THUNDER;
	eDamageParam.iDamage = 100;

	_matrix BoneMtx = m_pModelCom->GetBoneMatrix("Shell1") * m_pTransformCom->Get_WorldMatrix();
	_vector vPrePos = BoneMtx.r[3];

	CBulletBuilder()
		.CreateBullet()
			.Set_Owner(this)
			.Set_Target(m_pTarget)
			.Set_InitBulletEffect({ L"em1100_Elec_Bullet_Loop" })
			.Set_BulletEffPivot(pivot)
			.Set_InitBulletParticle(L"em1100_Elec_Bullet_Particle")
			.Set_ShootSpeed(8.f)
			.Set_Life(7.f)
			.Set_DamageParam(eDamageParam)
			.Set_DeadBulletEffect({ L"em1100_Elec_Bullet_Dead" })
			.Set_DeadBulletParticle(L"em1100_Elec_Bullet_Dead_Particle")
			.Set_Position(vPrePos)
			.Set_Radius(1.2f)
		.Build();


}

void CEM0900::HitWeakProcess(_double TimeDelta)
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

_bool CEM0900::CanMove4BC(_float fMinDist)
{
	_vector vTargetPos = m_pTarget->GetColliderPosition();
	physx::PxRaycastHit hitBuffer[1];
	physx::PxRaycastBuffer t(hitBuffer, 1);

	RayCastParams params;
	params.rayOut = &t;
	params.vOrigin = vTargetPos;
	params.vDir = m_pTarget->GetTransform()->Get_State(CTransform::STATE_LOOK);
	params.fDistance = 10.f;
	params.iTargetType = CTB_STATIC;
	params.bSingle = true;

	if (CGameInstance::GetInstance()->RayCast(params))
	{
		for (int i = 0; i < t.getNbAnyHits(); ++i)
		{
			auto p = t.getAnyHit(i);

			_float4 vPos{ p.position.x, p.position.y , p.position.z, 1.f };

			_float fDist = XMVectorGetX(XMVector4Length(vTargetPos - XMLoadFloat4(&vPos)));

			//최소거리보다 멀면 이동시켜서 브레인 크러쉬 연출
			return fDist > fMinDist ? true : false;
		}
	}

	//장애물이 없으니 무조건 가능
	return true;
}

void CEM0900::Rush_SweepSphere()
{
	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);

	//Tail4가 꼬리 중앙에 있음
	_float4x4 BoneMatrix = GetBoneMatrix("Target") * m_pTransformCom->Get_WorldMatrix();
	_float4 vBonePos = _float4{ BoneMatrix.m[3][0], BoneMatrix.m[3][1], BoneMatrix.m[3][2], 1.f };

	_vector	vDir = vBonePos - m_BeforePos;

	SphereSweepParams tParams;
	tParams.fVisibleTime = 0.2f;
	tParams.iTargetType = CTB_PLAYER;
	tParams.fRadius = 3.f;
	tParams.fDistance = XMVectorGetX(XMVector4LengthEst(vDir));
	tParams.vPos = vBonePos;
	tParams.sweepOut = &sweepOut;
	tParams.vUnitDir = vDir;

	if (CGameInstance::GetInstance()->SweepSphere(tParams))
	{
		HitTargets(sweepOut, static_cast<_int>(m_iAtkDamage * 1.5f), EAttackType::ATK_TO_AIR);
	}

	m_BeforePos = vBonePos;
}

void CEM0900::TailSwing_SweepSphere()
{
	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);

	//Tail4가 꼬리 중앙에 있음
	_float4x4 TailMatrix = GetBoneMatrix("Tail4") * m_pTransformCom->Get_WorldMatrix();
	_float4 vTailPos = _float4{ TailMatrix.m[3][0], TailMatrix.m[3][1], TailMatrix.m[3][2], TailMatrix.m[3][3]};

	_vector	vTailDir = vTailPos - m_BeforePos;

	SphereSweepParams tParams;
	tParams.fVisibleTime = 0.2f;
	tParams.iTargetType = CTB_PLAYER;
	tParams.fRadius = 2.f;
	tParams.fDistance = 1.f;
	tParams.vPos = vTailPos;
	tParams.sweepOut = &sweepOut;
	tParams.vUnitDir = vTailDir;

	if (CGameInstance::GetInstance()->SweepSphere(tParams))
	{
		HitTargets(sweepOut, static_cast<_int>(m_iAtkDamage * 1.2f), EAttackType::ATK_LIGHT);
	}

	m_BeforePos = vTailPos;
}

void CEM0900::Stamp_Overlap()
{	
	_matrix BoneMatrix = m_pModelCom->GetBoneMatrix("Target") * m_pTransformCom->Get_WorldMatrix();

	_vector vBoneVector = BoneMatrix.r[3];
	_float3 fBone = vBoneVector;

	physx::PxOverlapHit hitBuffer[3];
	physx::PxOverlapBuffer overlapOut(hitBuffer, 3);

	SphereOverlapParams param;
	param.fVisibleTime = 0.1f;
	param.iTargetType = CTB_PLAYER;
	param.fRadius = 3.f;
	param.vPos = XMVectorSetW(fBone, 1.f);
	param.overlapOut = &overlapOut;

	if (CGameInstance::GetInstance()->OverlapSphere(param))
	{
		HitTargets(overlapOut, static_cast<_int>(m_iAtkDamage * 1.5f), EAttackType::ATK_DOWN);
	}
}



CEM0900 * CEM0900::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEM0900* pInstance = new CEM0900(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0900");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEM0900::Clone(void * pArg)
{
	CEM0900*		pInstance = new CEM0900(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0900");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0900::Free()
{
	CEnemy::Free();
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
}
