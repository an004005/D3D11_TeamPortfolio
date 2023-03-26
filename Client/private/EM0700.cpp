#include "stdafx.h"
#include "..\public\EM0700.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "RigidBody.h"
#include "EM0700_AnimInstance.h"
#include "EM0700_Controller.h"
#include "EnemyBullet.h"
CEM0700::CEM0700(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
	m_eMonsterName = EEnemyName::EM0700;
}

CEM0700::CEM0700(const CEM0700 & rhs)
	: CEnemy(rhs)
{
}

HRESULT CEM0700::Initialize(void * pArg)
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

	m_eMonsterName = EEnemyName::EM0700;
	m_bHasCrushGage = true;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(180.f));
	m_pTransformCom->SetSpeed(7.f);

	m_fGravity = 0.f;
	return S_OK;
}

void CEM0700::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em700", L"Model", (CComponent**)&m_pModelCom));

	// 범위 충돌체(플레이어가 몬스터 위로 못 올라가게한다.)
	/*Json FlowerLegRangeCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/FlowerLeg/FlowerLegRange.json");
*/

	FAILED_CHECK(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("RangeColl"),
		(CComponent**)&m_pRange, pArg));

	FAILED_CHECK(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("BodyColl"),
		(CComponent**)&m_pBody))
		

	// 컨트롤러, prototype안 만들고 여기서 자체생성하기 위함
	m_pController = CEM0700_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	// ASM
	m_pASM = CEM0700_AnimInstance::Create(m_pModelCom, this);
}

void CEM0700::SetUpSound()
{
	CEnemy::SetUpSound();
}

void CEM0700::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();
}

void CEM0700::SetUpFSM()
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

			.AddTransition("Idle to Escape", "Escape")
				.Predicator([this] { return m_eInput == CController::SPACE; })

			.AddTransition("Idle to Threat", "Threat")
				.Predicator([this] { return m_eInput == CController::SHIFT; })

			.AddTransition("Idle to Rush_Start", "Rush_Start")
				.Predicator([this] { return m_eInput == CController::C; })

			.AddTransition("Idle to Shot", "Shot")
				.Predicator([this] { return m_eInput == CController::R; })
			

///////////////////////////////////////////////////////////////////////////////////////////
		.AddState("Hit_Light")
			.OnStart([this]
			{
				Play_LightHitAnim();
			})
			.Tick([this](_double)
			{
				if (m_eCurAttackType == EAttackType::ATK_LIGHT || m_eCurAttackType == EAttackType::ATK_SPECIAL_LOOP)
				{
					Play_LightHitAnim();
				}
			})
			.AddTransition("Hit_Light to Idle", "Idle")
				.Predicator([this]
			{
				return m_bDead
					|| m_pASM->isSocketPassby("FullBody", 0.95f)
					|| (m_eCurAttackType != EAttackType::ATK_LIGHT && m_eCurAttackType != EAttackType::ATK_END);
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
					return m_bDead
						|| m_pASM->isSocketPassby("FullBody", 0.95f)
						|| m_eCurAttackType == EAttackType::ATK_TO_AIR
						|| m_eCurAttackType == EAttackType::ATK_SPECIAL_LOOP;
				})
		.AddState("Death")
				.OnStart([this]
				{
					m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_474_AL_dead_down02");
				})



		.AddState("Hit_ToAir")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_432_AL_blow_start_F");
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
					m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_455_AL_rise_start");
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
				m_pASM->InputAnimSocketMany("FullBody", { "AS_em0700_433_AL_blow_landing_F", "AS_em0700_427_AL_getup" });
				m_fGravity = 20.f;
			})
			.AddTransition("OnFloorGetup to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketEmpty("FullBody");
				})

///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Escape")
			.OnStart([this]
			{
				SelectEscapeAnim_Overlap();
			})
			.Tick([this](_double) {	
				SocketLocalMove(m_pASM);
			})
			.AddTransition("Escape to Idle", "Idle")
				.Predicator([this]
			{
				return m_bDead || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketPassby("FullBody", 0.95f);
			})

///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Threat")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_160_AL_threat");
			})
			.AddTransition("Threat to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

///////////////////////////////////////////////////////////////////////////////////////////

		//시작과 끝은 로컬이 있음
		.AddState("Rush_Start")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_202_AL_atk_a1_loop");
				Rush_Start();
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
			.AddTransition("Rush_Start to Rush_Loop", "Rush_Loop")
				.Predicator([this]
			{
				return  m_pASM->isSocketPassby("FullBody", 0.95f);
			})
			.AddTransition("Rush_Start to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || (m_eCurAttackType != EAttackType::ATK_LIGHT && m_eCurAttackType != EAttackType::ATK_END);
				})

		.AddState("Rush_Loop")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_202_AL_atk_a1_loop");
				m_pModelCom->GetPlayAnimation()->SetLooping(true);	
				m_bRush = true;
			})
			.Tick([this](_double TimeDelta)
			{
				Rush(TimeDelta);
				Rush_Overlap();
				Rush_SweepCapsule();
			})
			.OnExit([this] {
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Rush_Loop to Rush_End", "Rush_End")
				.Predicator([this]
				{
					return m_bDead || !m_bRush;
				})

		.AddState("Rush_End")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_203_AL_atk_a1_end");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Rush_End to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Shot")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0710_207_AL_atk_a3");
			})
			.OnExit([this] 
			{
				Shot();
			})
				.AddTransition("AS_em0710_207_AL_atk_a3 to Idle", "Idle")
				.Predicator([this]
			{
				return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
			})


				///////////////////////////////////////////////////////////////////////////////////////////

		.Build();
}

void CEM0700::BeginTick()
{
	CEnemy::BeginTick();
}

void CEM0700::Tick(_double TimeDelta)
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

	const _float fMoveSpeed = 2.f;

	if (m_vMoveAxis.LengthSquared() > 0.f)
	{
		const _float fYaw = m_pTransformCom->GetYaw_Radian();
		_float3 vVelocity;
		XMStoreFloat3(&vVelocity, fMoveSpeed * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}

	// Tick의 제일 마지막에서 실행한다.
	ResetHitData();
}

void CEM0700::Late_Tick(_double TimeDelta)
{
	CEnemy::Late_Tick(TimeDelta);
}

void CEM0700::AfterPhysX()
{
	CEnemy::AfterPhysX();
}

HRESULT CEM0700::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CEM0700::Imgui_RenderProperty()
{
	CEnemy::Imgui_RenderProperty();
	if (ImGui::CollapsingHeader("ASM"))
	{
		m_pASM->Imgui_RenderState();
	}
	m_pFSM->Imgui_RenderProperty();
}

_bool CEM0700::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

void CEM0700::Play_LightHitAnim()
{
	if (m_eSimpleHitFrom == ESimpleAxis::NORTH)
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_401_AL_damage_l_F");
	else
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_402_AL_damage_l_B");
}

void CEM0700::Play_MidHitAnim()
{
	switch (m_eHitFrom)
	{
	case EBaseAxis::NORTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_411_AL_damage_m_F");
		break;
	case EBaseAxis::EAST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_414_AL_damage_m_R");
		break;
	case EBaseAxis::SOUTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_412_AL_damage_m_B");
		break;
	case EBaseAxis::WEST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_413_AL_damage_m_L");
		break;
	case EBaseAxis::AXIS_END:
		FALLTHROUGH;
	default:
		NODEFAULT;
	}
}

void CEM0700::HeavyAttackPushStart()
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

void CEM0700::SelectRandomMoveAnim()
{
	_vector vTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	_float fAngle = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vMyRight), XMVector3Normalize(vTargetPos - vMyPos)));

	//오른쪽
	if (fAngle > 0)
		m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_122_AL_randommove_R");
	else
		m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_121_AL_randommove_L");
}

void CEM0700::Rush_Start()
{
	//한틱만 실행시켜줘야함
	_matrix MyBone = XMLoadFloat4x4(&GetBoneMatrix("Target"));
	_vector vMyBonePos = (MyBone * m_pTransformCom->Get_WorldMatrix()).r[3];

	_matrix TargetBone = XMLoadFloat4x4(&m_pTarget->GetBoneMatrix("Waist"));
	_vector TargetBonePos = (TargetBone * m_pTarget->GetTransform()->Get_WorldMatrix()).r[3];

	//공격하는 벡터
	_vector vStartDir = TargetBonePos - vMyBonePos;
	XMStoreFloat4(&m_vStartDir, vStartDir);

	_vector vInterval = XMVectorSetY(TargetBonePos, XMVectorGetY(vMyBonePos) - XMVectorGetY(TargetBonePos)) * 2.f;

	//공격 끝나고 올라가는 벡터
	_vector vEndDir = vStartDir + vInterval;
	XMStoreFloat4(&m_vEndDir, vEndDir);
	//
}

void CEM0700::Rush(_double TimeDelta)
{
	_matrix MyBone = XMLoadFloat4x4(&GetBoneMatrix("Target"));
	_vector vMyBonePos = (MyBone * m_pTransformCom->Get_WorldMatrix()).r[3];

	if (m_bChangeDir == false)
	{
		_matrix TargetBone = XMLoadFloat4x4(&m_pTarget->GetBoneMatrix("Waist"));
		_vector TargetBonePos = (TargetBone * m_pTarget->GetTransform()->Get_WorldMatrix()).r[3];

		_float fDist = XMVectorGetX(XMVector4Length(TargetBonePos- vMyBonePos));

		if (fDist <= 0.2f)
			m_bChangeDir = true;
	}
	else
	{
		_float fDist = XMVectorGetX(XMVector4Length(vMyBonePos - XMLoadFloat4(&m_vEndDir)));

		if (fDist <= 0.2f)
			m_bRush = false;
	}

	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vMyPos += XMVector3Normalize(m_vStartDir) * 5.f * TimeDelta;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vMyPos);

}

void CEM0700::Shot()
{
	auto pObj = CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_Bullet"), TEXT("EnemyBullet"));

	if (CEnemyBullet* pBullet = dynamic_cast<CEnemyBullet*>(pObj))
	{
		pBullet->Set_Owner(this);

		_vector vBonePos = (m_pModelCom->GetBoneMatrix("Target") * m_pTransformCom->Get_WorldMatrix()).r[3];
		_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

		//입에 달려있는 뼈가 없어서 몬스터 입 뒤쪽에있는 Target뼈에서 앞쪽으로 당겨준다.
		vBonePos += XMVector3Normalize(vLook) * 3.f;
		pBullet->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vBonePos);
		pBullet->GetTransform()->LookAt(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
	}
}



void CEM0700::SelectEscapeAnim_Overlap()
{
	//뒤에 벽이 있는지 체크
	_float fLength = -5.f;

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
	param.fRadius = 3.f;
	param.vPos = XMVectorSetW(fFinish, 1.f);
	param.overlapOut = &overlapOut;

	if (CGameInstance::GetInstance()->OverlapSphere(param))
		m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_120_AL_escape");
	else
		SelectRandomMoveAnim();
}

void CEM0700::Rush_Overlap()
{
	//Length로 Bone의 위치에서부터 바라보는 방향으로 원하는 지점까지의 거리를 지정
	_float fLength = 7.f;

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
	param.fRadius = 3.f;
	param.vPos = XMVectorSetW(fFinish, 1.f);
	param.overlapOut = &overlapOut;

	if (CGameInstance::GetInstance()->OverlapSphere(param))
	{
		m_bRush = false;
	}
}

void CEM0700::Rush_SweepCapsule()
{
	_float4x4 BodyMatrix = m_pBody->GetPxWorldMatrix();
	_float4 vBodyPos = _float4{ BodyMatrix.m[3][0], BodyMatrix.m[3][1], BodyMatrix.m[3][2], BodyMatrix.m[3][3] };

	physx::PxSweepHit hitBuffer[3];
	physx::PxSweepBuffer sweepOut(hitBuffer, 3);

	PxCapsuleSweepParams tParams;
	tParams.sweepOut = &sweepOut;
	tParams.CapsuleGeo = m_pBody->Get_CapsuleGeometry();
	tParams.pxTransform = m_pBody->Get_PxTransform();

	_float4	vDir = vBodyPos - m_BeforePos;
	tParams.vUnitDir = _float3(vDir.x, vDir.y, vDir.z);
	tParams.fDistance = tParams.vUnitDir.Length();
	tParams.iTargetType = CTB_PLAYER;
	tParams.fVisibleTime = 0.2f;

	if (CGameInstance::GetInstance()->PxSweepCapsule(tParams))
	{
		HitTargets(sweepOut, m_iAtkDamage * 1.5f, EAttackType::ATK_TO_AIR);
	}

	m_BeforePos = vBodyPos;
}


CEM0700 * CEM0700::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEM0700* pInstance = new CEM0700(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0700");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEM0700::Clone(void * pArg)
{
	CEM0700*		pInstance = new CEM0700(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0700");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0700::Free()
{
	CEnemy::Free();
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
	Safe_Release(m_pRange);
	Safe_Release(m_pBody);
}
