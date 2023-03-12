#include "stdafx.h"
#include "BronJon.h"
#include <random>

#include "GameInstance.h"
#include "MathUtils.h"
#include "GameUtils.h"
#include "FSMComponent.h"
#include "AnimationInstance.h"
#include "Animation.h"
#include "Model.h"
#include "JsonStorage.h"
#include "PhysX_Manager.h"

#include "BrJ_Controller.h"
#include "BrJ_AnimInstance.h"
#include "RigidBody.h"
#include "Player.h"

CBronJon::CBronJon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CBronJon::CBronJon(const CBronJon & rhs)
	: CMonster(rhs)
{
}

HRESULT CBronJon::Initialize_Prototype()
{
	return CMonster::Initialize_Prototype();
}

HRESULT CBronJon::Initialize(void * pArg)
{
	Json BronJonTrigger = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BronJon/BronJonTrigger.json");
	MoveTransformJson(BronJonTrigger, pArg);
	pArg = &BronJonTrigger;

	FAILED_CHECK(CMonster::Initialize(pArg));

	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Trigger"),
		(CComponent**)&m_pTrigger, &BronJonTrigger)))
		return E_FAIL;

	// Boss Monster HitBox

	Json BronJonJaw = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BronJon/BronJonJaw.json");
	Json BronJonLaser = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BronJon/BronJonEff02.json");
	Json BronJonArm_L = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BronJon/BronJonLeftArm.json");
	Json BronJonArm_R = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BronJon/BronJonRightArm.json");
	// m_pJawRBody : BiteAtk + Head HitBox
	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Jaw"),
		(CComponent**)&m_pJawRBody, &BronJonJaw)))
		return E_FAIL;
	// m_pLaserEffect : LaserAtk
	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Laser"),
		(CComponent**)&m_pLaserEffect, &BronJonLaser)))
		return E_FAIL;
	// m_pLeftArm : LeftArm HitBox
	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("LeftArm"),
		(CComponent**)&m_pLeftArm, &BronJonArm_L)))
		return E_FAIL;
	// m_pRightArm : RightArm HitBox
	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("RightArm"),
		(CComponent**)&m_pRightArm, &BronJonArm_R)))
		return E_FAIL;

	// ~Boss Monster HitBox

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"MonsterBronJon", L"Model", (CComponent**)&m_pModelCom));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Proto_BrJ_Controller"), TEXT("Com_Controller"), (CComponent**)&m_pController));

	// Event Caller

	m_pModelCom->Add_EventCaller("Bite_Start", [this] { m_bAtkBite = true; });
	m_pModelCom->Add_EventCaller("Bite_End", [this] 
	{
		m_bAtkBite = false; 
		ClearDamagedTarget();
	});

	m_pModelCom->Add_EventCaller("Laser_Create", [this] { m_bAtkLaser = true; });
	m_pModelCom->Add_EventCaller("Laser_Finish", [this]
	{
		m_bAtkLaser = false;
		ClearDamagedTarget();
	});

	m_pModelCom->Add_EventCaller("Groggy_End", [this] { m_bDown = false; });

	// ~Event Caller
	m_iHP = 5000;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(90.f));
	m_iGroggy_Able = 5;

	m_pASM = CBrJ_AnimInstance::Create(m_pModelCom, this);

	// 소켓 애니메이션 추가

	m_pAtk_LaserStart = m_pModelCom->Find_Animation("AS_em0800_214_AL_atk_a7_laser2_start");
	m_pAtk_LaserLoop = m_pModelCom->Find_Animation("AS_em0800_215_AL_atk_a7_laser2_loop");
	m_pAtk_LaserEnd = m_pModelCom->Find_Animation("AS_em0800_216_AL_atk_a7_laser2_end");
	m_pAtk_Bite = m_pModelCom->Find_Animation("AS_em0810_201_AL_atk_a1_bite");

	m_pThreat = m_pModelCom->Find_Animation("AS_em0800_160_AL_threat");

	m_pDamage_L_F = m_pModelCom->Find_Animation("AS_em0800_401_AL_damage_l_F");
	m_pDamage_L_B = m_pModelCom->Find_Animation("AS_em0800_402_AL_damage_l_B");

	m_pDamage_M_F = m_pModelCom->Find_Animation("AS_em0800_411_AL_damage_m_F");
	m_pDamage_M_B = m_pModelCom->Find_Animation("AS_em0800_412_AL_damage_m_B");
	m_pDamage_M_L = m_pModelCom->Find_Animation("AS_em0800_413_AL_damage_m_L");
	m_pDamage_M_R = m_pModelCom->Find_Animation("AS_em0800_414_AL_damage_m_R");

	m_pDownStart = m_pModelCom->Find_Animation("AS_em0800_425_AL_down_start");
	m_pDownLoop = m_pModelCom->Find_Animation("AS_em0800_480_AL_down_shock");
	m_pGetUp = m_pModelCom->Find_Animation("AS_em0800_427_AL_getup");

	m_pDeadAnim = m_pModelCom->Find_Animation("AS_em0800_424_AL_dead_down");

	// ~소켓 애니메이션 추가

	return S_OK;
}

void CBronJon::BeginTick()
{
	__super::BeginTick();
	m_pASM->AttachAnimSocket("BronJon", { m_pModelCom->Find_Animation("AS_em0800_495_AL_press_down_start") });

}

void CBronJon::Tick(_double TimeDelta)
{
	CMonster::Tick(TimeDelta);

	auto pPlayer = CGameInstance::GetInstance()->Find_ObjectByPredicator(LEVEL_NOW, [this](CGameObject* pObj)
	{
		return dynamic_cast<CPlayer*>(pObj) != nullptr;
	});
	m_pTarget = dynamic_cast<CScarletCharacter*>(pPlayer);

	// Controller
	m_pController->SetTarget(m_pTarget);

	if (m_bDead == false)
		m_pController->Tick(TimeDelta);

	_bool bOnfloor = IsOnFloor();
						// m_iGroggy_Able = 5, 1회 그로기마다 +1씩 되어 갈수록 그로기가 어려워진다.
	if (m_iGroggyCnt >= m_iGroggy_Able)
		m_bGroggy = true;

	// 소켓 키 세팅

	if (m_pController->KeyDown(CController::MOUSE_RB))
	{
		m_pASM->AttachAnimSocket("BronJon", { m_pAtk_Bite });
	}
	if (m_pController->KeyDown(CController::MOUSE_LB))
	{
		m_pASM->AttachAnimSocket("BronJon", { m_pAtk_LaserStart, m_pAtk_LaserLoop, m_pAtk_LaserLoop, m_pAtk_LaserLoop, m_pAtk_LaserEnd });
	}
	if (m_pController->KeyDown(CController::G))
	{
		m_pASM->AttachAnimSocket("BronJon", { m_pThreat });
	}

	if (m_bStruck || m_pController->KeyDown(CController::NUM_1))
	{
		m_bStruck = false;
		m_pController->ClearCommands();
		// Test
		if (m_eAtkType == EAttackType::ATK_LIGHT)
		{
			if (m_eHitDir == EBaseAxis::NORTH)
				m_pASM->InputAnimSocket("BronJon", { m_pDamage_L_F });

			else if (m_eHitDir == EBaseAxis::SOUTH)
				m_pASM->InputAnimSocket("BronJon", { m_pDamage_L_B });
		}
		// ~Test
		if (m_eAtkType == EAttackType::ATK_HEAVY)
		{
			if (m_eHitDir == EBaseAxis::NORTH)
				m_pASM->InputAnimSocket("BronJon", { m_pDamage_M_F });

			else if (m_eHitDir == EBaseAxis::SOUTH)
				m_pASM->InputAnimSocket("BronJon", { m_pDamage_M_B });

			else if (m_eHitDir == EBaseAxis::WEST)
				m_pASM->InputAnimSocket("BronJon", { m_pDamage_M_L });

			else if (m_eHitDir == EBaseAxis::EAST)
				m_pASM->InputAnimSocket("BronJon", { m_pDamage_M_R });
		}
	}
	
	// m_bGroggy : 그로기 상태 및 Down 애니메이션이 돌기 위한 조건이므로 한번 돌고 바로 false 
	if (m_bGroggy || m_pController->KeyDown(CController::NUM_4))
	{		
		m_bGroggy = false;
		m_iGroggyCnt = 0;
		m_iGroggy_Able += 1;
		m_bDown = true;
		m_pController->ClearCommands();

		m_pASM->AttachAnimSocket("BronJon", { m_pDownStart, m_pDownLoop, m_pDownLoop, m_pDownLoop, m_pGetUp });
	}

	// ~소켓 키 세팅

	m_pTrigger->Update_Tick(m_pTransformCom);

	m_fTurnRemain = m_pController->GetTurnRemain();
	m_vMoveAxis = m_pController->GetMoveAxis();

	m_pASM->Tick(TimeDelta);

	_float fMoveSpeed = 3.f;

	m_vMoveAxis.Normalize();

	const _float fYaw = m_pTransformCom->GetYaw_Radian();
	_float3 vVelocity;
	XMStoreFloat3(&vVelocity, fMoveSpeed * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
	m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);	
}

void CBronJon::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_bAtkBite)
		Atk_BiteSweep();

	if (m_bAtkLaser)
		Atk_LaserSweep();	

	if (nullptr != m_pRendererCom && m_bVisible)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CBronJon::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CBronJon::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
	m_pASM->Imgui_RenderState();
}

void CBronJon::AfterPhysX()
{
	__super::AfterPhysX();
	m_pTrigger->Update_AfterPhysX(m_pTransformCom);

	m_pJawRBody->Update_Tick(AttachCollider(m_pJawRBody));
	m_pJawRBody->Update_AfterPhysX(m_pTransformCom);

	m_pLaserEffect->Update_Tick(AttachCollider(m_pLaserEffect));
	m_pLaserEffect->Update_AfterPhysX(m_pTransformCom);

	m_pLeftArm->Update_Tick(AttachCollider(m_pLeftArm));
	m_pLeftArm->Update_AfterPhysX(m_pTransformCom);

	m_pRightArm->Update_Tick(AttachCollider(m_pRightArm));
	m_pRightArm->Update_AfterPhysX(m_pTransformCom);
}

void CBronJon::TakeDamage(DAMAGE_PARAM tDamageParams)
{
	if (m_bDead)
		return;

	EBaseAxis eHitFrom = CClientUtils::GetDamageFromAxis(m_pTransformCom, tDamageParams.vHitFrom);
	m_eHitDir = eHitFrom;

	m_eAtkType = tDamageParams.eAttackType;
	m_iHP -= tDamageParams.iDamage;

	if (m_eAtkType == EAttackType::ATK_HEAVY && !m_bAtkBite && !m_bAtkLaser && !m_bDown)
	{			
		m_bStruck = true;	// 체력 다는 조건으로 주면 될듯?
		++m_iGroggyCnt;
	}

	if (m_eAtkType == EAttackType::ATK_LIGHT) // ATK_MIDDLE
		m_bStruck = true;

	if (m_iHP <= 0)
	{
		m_pController->ClearCommands();
		m_DeathTimeline.PlayFromStart();
		m_pASM->InputAnimSocket("BronJon", { m_pDeadAnim });
		m_bDead = true;
	}
	
//	CGameInstance::GetInstance()->SetTimeRatioCurve("Simple_Increase");

	IM_LOG("yes");
}

_matrix CBronJon::AttachCollider(CRigidBody* pRigidBody)
{
	_matrix	SocketMatrix;

	if (pRigidBody == m_pJawRBody)
		SocketMatrix = m_pModelCom->GetBoneMatrix("Jaw") * m_pTransformCom->Get_WorldMatrix();

	else if (pRigidBody == m_pLaserEffect)
		SocketMatrix = m_pModelCom->GetBoneMatrix("Eff02") * m_pTransformCom->Get_WorldMatrix();

	else if (pRigidBody == m_pLeftArm)
		SocketMatrix = m_pModelCom->GetBoneMatrix("LeftForeArm") * m_pTransformCom->Get_WorldMatrix();

	else if (pRigidBody == m_pRightArm)
		SocketMatrix = m_pModelCom->GetBoneMatrix("RightForeArm") * m_pTransformCom->Get_WorldMatrix();

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	return SocketMatrix;
}

void CBronJon::Atk_BiteSweep()
{
	Matrix mJawMatrix = m_pJawRBody->GetPxWorldMatrix();
	_float4 vJawPos = _float4(mJawMatrix.Translation().x, mJawMatrix.Translation().y, mJawMatrix.Translation().z, 1.f);
	_float3 vLook = _float3(mJawMatrix.Up().x, mJawMatrix.Up().y, mJawMatrix.Up().z);
	
	physx::PxSweepHit hitBuffer[5];
	physx::PxSweepBuffer sweepOut(hitBuffer, 5);

	SphereSweepParams Sparam;
	Sparam.fVisibleTime = 0.f;
	Sparam.iTargetType = CTB_PLAYER;
	Sparam.fRadius = 1.2f;
	Sparam.fDistance = 1.5f;
	Sparam.vPos = vJawPos;
	Sparam.sweepOut = &sweepOut;
	Sparam.vUnitDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	
	if (CGameInstance::GetInstance()->SweepSphere(Sparam))
		HitTargets(sweepOut, (rand() % 80) + 25, EAttackType::ATK_HEAVY);
}

void CBronJon::Atk_LaserSweep()
{
	Matrix mLaserMatrix = m_pLaserEffect->GetPxWorldMatrix();
	_float4 vLaserPos = _float4(mLaserMatrix.Translation().x, mLaserMatrix.Translation().y, mLaserMatrix.Translation().z, 1.f);
	_float3 vLook = _float3(mLaserMatrix.Up().x, mLaserMatrix.Up().y, mLaserMatrix.Up().z);

	physx::PxSweepHit hitBuffer[5];
	physx::PxSweepBuffer sweepOut(hitBuffer, 5);

	SphereSweepParams Sparam;
	Sparam.fVisibleTime = 0.f;
	Sparam.iTargetType = CTB_PLAYER;
	Sparam.fRadius = 1.4f;
	Sparam.fDistance = 30.f;
	Sparam.vPos = vLaserPos;
	Sparam.sweepOut = &sweepOut;
	Sparam.vUnitDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	if (CGameInstance::GetInstance()->SweepSphere(Sparam))
		HitTargets(sweepOut, (rand() % 150) + 25, EAttackType::ATK_HEAVY);
}

_bool CBronJon::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("BronJon") == false;
}

CBronJon * CBronJon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBronJon* pInstance = new CBronJon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBronJon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBronJon::Clone(void * pArg)
{
	CBronJon*		pInstance = new CBronJon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBronJon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBronJon::Free()
{
	CMonster::Free();	
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
	Safe_Release(m_pTrigger);
	Safe_Release(m_pJawRBody);
	Safe_Release(m_pLaserEffect);
	Safe_Release(m_pLeftArm);
	Safe_Release(m_pRightArm);
}

