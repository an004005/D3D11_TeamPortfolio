#include "stdafx.h"
#include "FlowerLeg.h"
#include "GameInstance.h"
#include"Model.h"
#include "JsonStorage.h"
#include "PhysX_Manager.h"

#include "FL_Controller.h"
#include "FL_AnimInstance.h"
#include "RigidBody.h"
#include "Player.h"

#include "MonsterHpUI.h"
CFlowerLeg::CFlowerLeg(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CFlowerLeg::CFlowerLeg(const CFlowerLeg & rhs)
	: CMonster(rhs)
{
}

HRESULT CFlowerLeg::Initialize_Prototype()
{
	return CMonster::Initialize_Prototype();
}

HRESULT CFlowerLeg::Initialize(void * pArg)
{
	Json FlowerLegJson = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/FlowerLeg/FlowerLegTrigger.json");
	MoveTransformJson(FlowerLegJson, pArg);
	pArg = &FlowerLegJson;

	FAILED_CHECK(CMonster::Initialize(pArg));

	Json FlowerLegTrigger = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/FlowerLeg/FlowerLegTrigger.json");

	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Trigger"),
		(CComponent**)&m_pTrigger, &FlowerLegTrigger)))
		return E_FAIL;

	Json FlowerLegTailCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/FlowerLeg/FlowerLegTailCol.json");

	// 꼬리 충돌체
	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("TailCol"),
		(CComponent**)&m_pTailCol, &FlowerLegTailCol)))
		return E_FAIL;

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"MonsterFlowerLeg", L"Model", (CComponent**)&m_pModelCom));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Proto_FL_Controller"), TEXT("Com_Controller"), (CComponent**)&m_pController));

	m_pModelCom->Add_EventCaller("JumpAttackStart", [this]
	{	
		m_fGravity = 14.f;
		m_fYSpeed = 8.f;
		m_bJumpAttack = true;

		if (m_pTarget)
		{
			// MyPos
			_vector vOrigin = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			vOrigin = XMVectorSetY(vOrigin, 0.f); // ~MyPos

			// TargetPos
			_vector vDest = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
			vDest = XMVectorSetY(vDest, 0.f); // ~TargetPos
			m_fJumpMoveTime = (1.4 * m_fYSpeed) / m_fGravity; // 점프로 이동하는 시간

			const _vector vDiff = vDest - vOrigin; // 나 -> 대상까지의 방향
			const _float fDistance = XMVectorGetX(XMVector3LengthEst(vDiff)); // 나 -> 대상까지의 거리
			const _vector vDirection = XMVector3NormalizeEst(vDiff);

			m_vOnJumpMoveVelocity = vDirection * (fDistance / m_fJumpMoveTime);
		}
		else	// 예외 처리
		{
			m_fJumpMoveTime = 0.f;
			m_vOnJumpMoveVelocity = _float3::Zero;
		}
	});

	m_pModelCom->Add_EventCaller("DodgeL_Start", [this] 
	{		
		m_fGravity = 27.f;
		m_fYSpeed = 8.f;

		_float fLength = 3.8f; // 회피 이동하고자 하는 고정 거리
		
		if (m_pTarget)
		{
			_vector vOrigin = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			vOrigin = XMVectorSetY(vOrigin, 0.f); 
			_float3 fOrigin = vOrigin;
			
			_vector vDest = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vDest = XMVectorSetY(vDest, 0.f);
			_float3 fDest = vDest * -1.f;
									// Right 는 -1.f 를 빼자.
			_float3 fFinish = { (fOrigin.x + fLength * fDest.x), fOrigin.y, (fOrigin.z + fLength * fDest.z) };

			m_fDodgeMoveTime = (1.1 * m_fYSpeed) / m_fGravity;

			const _vector vDiff = fFinish - fOrigin;
			const _float fDistance = XMVectorGetX(XMVector3LengthEst(vDiff));
			const _vector vDirection = XMVector3NormalizeEst(vDiff);

			m_vOnDodgeMoveVelocity = vDirection * (fDistance / m_fDodgeMoveTime);
		}
		else
		{
			m_fDodgeMoveTime = 0.f;
			m_vOnDodgeMoveVelocity = _float3::Zero;
		}	
	});

	m_pModelCom->Add_EventCaller("DodgeR_Start", [this]
	{
		m_fGravity = 27.f;
		m_fYSpeed = 6.f;

		_float fLength = 3.8f; // 회피 이동하고자 하는 고정 거리

		if (m_pTarget)
		{
			_vector vOrigin = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			vOrigin = XMVectorSetY(vOrigin, 0.f);
			_float3 fOrigin = vOrigin;

			_vector vDest = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vDest = XMVectorSetY(vDest, 0.f);
			_float3 fDest = vDest;
			
			_float3 fFinish = { (fOrigin.x + fLength * fDest.x), fOrigin.y, (fOrigin.z + fLength * fDest.z) };

			m_fDodgeMoveTimeRev = (1.1 * m_fYSpeed) / m_fGravity;

			const _vector vDiff = fFinish - fOrigin;
			const _float fDistance = XMVectorGetX(XMVector3LengthEst(vDiff));
			const _vector vDirection = XMVector3NormalizeEst(vDiff);

			m_vOnDodgeMoveVelocityRev = vDirection * (fDistance / m_fDodgeMoveTimeRev);
		}
		else
		{
			m_fDodgeMoveTimeRev = 0.f;
			m_vOnDodgeMoveVelocityRev = _float3::Zero;
		}
	});


	m_pModelCom->Add_EventCaller("DodgeB_Start", [this]
	{
		m_fGravity = 27.f;
		m_fYSpeed = 6.f;

		_float fLength = 2.8f; // 회피 이동하고자 하는 고정 거리

		if (m_pTarget)
		{
			_vector vOrigin = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			vOrigin = XMVectorSetY(vOrigin, 0.f);
			vOrigin = XMVector3NormalizeEst(vOrigin);
			_float3 fOrigin = vOrigin;

			/*_vector vDest = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vDest = XMVectorSetY(vDest, 0.f);*/
			_float3 fDest = vOrigin * -1.f;

			_float3 fFinish = { (fOrigin.x + fLength * fDest.x), fOrigin.y, (fOrigin.z + fLength * fDest.z) };

			m_fDodgeMoveTimeBack = (1.1 * m_fYSpeed) / m_fGravity;

			const _vector vDiff = fFinish - fOrigin;
			const _float fDistance = XMVectorGetX(XMVector3LengthEst(vDiff));
			const _vector vDirection = XMVector3NormalizeEst(vDiff);

			m_vOnDodgeMoveVelocityBack = vDirection * (fDistance / m_fDodgeMoveTimeBack);
		}
		else
		{
			m_fDodgeMoveTimeBack = 0.f;
			m_vOnDodgeMoveVelocityBack = _float3::Zero;
		}
	});

	m_pModelCom->Add_EventCaller("Spin_Atk", [this] { m_bAtkSwitch = true; });
	m_pModelCom->Add_EventCaller("Spin_AtkEnd", [this] { m_bOneHit = true; });

	m_pModelCom->Add_EventCaller("Invincible_Start", [this] { m_bInvisible = true; });
	m_pModelCom->Add_EventCaller("OverLap", [this] { Strew_Overlap(); });
	m_pModelCom->Add_EventCaller("Invincible_End", [this] { m_bInvisible = false; });

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
	m_pModelCom->Add_EventCaller("Damage_End", [this] { m_bHitMove = false; });
	m_pModelCom->Add_EventCaller("Untouch", [this] { m_bUntouchable = true; });
	m_pModelCom->Add_EventCaller("Untouch_End", [this] { m_bUntouchable = false; });

	m_iMaxHP = 1200;
	m_iHP = m_iMaxHP; // ★
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(180.f));
	m_vFinDir = { 0.f, 0.f, 0.f, 0.f };

	m_pASM = CFL_AnimInstance::Create(m_pModelCom, this);

	// 소켓 애니메이션 추가
	m_pAtk_Spin = m_pModelCom->Find_Animation("AS_em0200_202_AL_atk_a2");
	m_pAtk_Strew = m_pModelCom->Find_Animation("AS_em0200_203_AL_atk_a3");
	m_pJumpStart = m_pModelCom->Find_Animation("AS_em0200_204_AL_atk_a4_start");
	m_pJumpLanding = m_pModelCom->Find_Animation("AS_em0200_205_AL_atk_a4_landing");

	m_pDodgeL_Start = m_pModelCom->Find_Animation("AS_em0200_140_AL_dodge_L_start");
	m_pDodgeL_Stop = m_pModelCom->Find_Animation("AS_em0200_141AL_dodge_L_stop");

	m_pDodgeR_Start = m_pModelCom->Find_Animation("AS_em0200_145_AL_dodge_R_start");
	m_pDodgeR_Stop = m_pModelCom->Find_Animation("AS_em0200_146_AL_dodge_R_stop");

	m_pDodgeB_Start = m_pModelCom->Find_Animation("AS_em0200_135_AL_dodge_B_start");
	m_pDodgeB_Stop = m_pModelCom->Find_Animation("AS_em0200_136_AL_dodge_B_stop");

	m_pThreat = m_pModelCom->Find_Animation("AS_em0200_160_AL_threat");
	
	// Light Damage
	m_pDamage_L_F = m_pModelCom->Find_Animation("AS_em0200_401_AL_damage_l_F");
	m_pDamage_L_B = m_pModelCom->Find_Animation("AS_em0200_402_AL_damage_l_B");

	// Middle Damage
	m_pDamage_M_F = m_pModelCom->Find_Animation("AS_em0200_411_AL_damage_m_F");
	m_pDamage_M_B = m_pModelCom->Find_Animation("AS_em0200_412_AL_damage_m_B");
	m_pDamage_M_L = m_pModelCom->Find_Animation("AS_em0200_413_AL_damage_m_L");
	m_pDamage_M_R = m_pModelCom->Find_Animation("AS_em0200_414_AL_damage_m_R");

	// Air Damage
	m_pBlowStart = m_pModelCom->Find_Animation("AS_em0200_432_AL_blow_start_F");
	m_pBlowLand = m_pModelCom->Find_Animation("AS_em0200_433_AL_blow_landing_F");
	m_pGetUp = m_pModelCom->Find_Animation("AS_em0200_427_AL_getup");
	m_pRiseStart = m_pModelCom->Find_Animation("AS_em0200_455_AL_rise_start");

	// Dead
	m_pDeadAnim = m_pModelCom->Find_Animation("AS_em0200_424_AL_dead_down");
	// ~소켓 애니메이션 추가


	//HP UI
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pUI_Hp = dynamic_cast<CMonsterHpUI*>(pGameInstance->Clone_GameObject_Get(TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonsterHP")));
	
	//실험중이라 생성 안되면 터트림
	assert(m_pUI_Hp != nullptr);
	m_pUI_Hp->Set_Owner(this);
	m_pUI_Hp->SetPivotMatrix(m_UI_PivotMatrix[HP]);
	return S_OK;
}

void CFlowerLeg::BeginTick()
{
	__super::BeginTick();
}

void CFlowerLeg::Tick(_double TimeDelta)
{
	if (!m_bActive)
		return;

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
	
	m_bRun = m_pController->IsRun();	
	_bool bOnfloor = IsOnFloor();
	
	if (m_fJumpMoveTime > 0.f)
	{
		m_pTransformCom->MoveVelocity(TimeDelta, m_vOnJumpMoveVelocity);
		m_fJumpMoveTime -= (_float)TimeDelta;
	}
	if (m_fDodgeMoveTime > 0.f)
	{
		m_pTransformCom->MoveVelocity((TimeDelta * 1.5), m_vOnDodgeMoveVelocity);
		m_fDodgeMoveTime -= (_float)TimeDelta;
	}
	if (m_fDodgeMoveTimeRev > 0.f)
	{
		m_pTransformCom->MoveVelocity((TimeDelta * 1.5), m_vOnDodgeMoveVelocityRev);
		m_fDodgeMoveTimeRev -= (_float)TimeDelta;
	}
	if (m_fDodgeMoveTimeBack > 0.f)
	{
		m_pTransformCom->MoveVelocity((TimeDelta * 1.5), m_vOnDodgeMoveVelocityBack);
		m_fDodgeMoveTimeBack -= (_float)TimeDelta;
	}

	if (m_pController->KeyDown(CController::R))
	{
		m_pASM->AttachAnimSocket("UsingControl", { m_pAtk_Spin });
	}

	if (m_pController->KeyDown(CController::G))
	{
		m_pASM->AttachAnimSocket("UsingControl", { m_pAtk_Strew });
		m_bAtkSwitch = false;
		m_bOneHit = false;
	}

	if (m_pController->KeyDown(CController::NUM_1))
	{
		m_pASM->AttachAnimSocket("UsingControl", { m_pDodgeL_Start, m_pDodgeL_Stop });
		m_bAtkSwitch = false;
		m_bOneHit = false;
	}

	if (m_pController->KeyDown(CController::NUM_2))
	{
		m_pASM->AttachAnimSocket("UsingControl", { m_pDodgeR_Start, m_pDodgeR_Stop });
		m_bAtkSwitch = false;
		m_bOneHit = false;
	}

	if (m_pController->KeyDown(CController::NUM_3))
	{
		m_pASM->AttachAnimSocket("UsingControl", { m_pDodgeB_Start, m_pDodgeB_Stop });
		m_bAtkSwitch = false;
		m_bOneHit = false;
	}

	if (m_pController->KeyDown(CController::MOUSE_RB))
	{
		m_pASM->AttachAnimSocket("UsingControl", { m_pJumpStart, m_pJumpLanding });
		m_bAtkSwitch = false;
		m_bOneHit = false;
	}

	if (m_pController->KeyDown(CController::C))
	{
		m_pASM->AttachAnimSocket("UsingControl", { m_pThreat });
		m_bAtkSwitch = false;
		m_bOneHit = false;
	}

	if (!m_bAirStruck && m_bStruck && !m_bAirMaintain || m_pController->KeyDown(CController::Q))
	{
		m_bStruck = false;
		m_pController->ClearCommands();

		if (m_eAtkType == EAttackType::ATK_LIGHT || m_eAtkType == EAttackType::ATK_MIDDLE)
		{
			if (m_eHitDir == EBaseAxis::NORTH)
			{
				m_pASM->InputAnimSocket("UsingControl", { m_pDamage_L_F });			
				m_bHitMove = true;
			}
			else
			{
				m_pASM->InputAnimSocket("UsingControl", { m_pDamage_L_B });		
				m_bHitMove = true;
			}
		}

		if (m_eAtkType == EAttackType::ATK_HEAVY)
		{
			if(m_eHitDir == EBaseAxis::NORTH)
				m_pASM->InputAnimSocket("UsingControl", { m_pDamage_M_F });

			else if(m_eHitDir == EBaseAxis::SOUTH)
				m_pASM->InputAnimSocket("UsingControl", { m_pDamage_M_B });

			else if(m_eHitDir == EBaseAxis::WEST)
				m_pASM->InputAnimSocket("UsingControl", { m_pDamage_M_L });

			else if(m_eHitDir == EBaseAxis::EAST)
				m_pASM->InputAnimSocket("UsingControl", { m_pDamage_M_R });
		}
	}	

	if ((!m_bStruck && m_bAirStruck && !m_bAirMaintain) || m_pController->KeyDown(CController::X))
	{
  		m_bHitMove = false;
		m_bAirStruck = false;
		m_pController->ClearCommands();

		m_pASM->AttachAnimSocket("UsingControl", { m_pBlowStart });
		m_bAirMaintain = true;
	}

	if (m_bAirMaintain && (m_bStruck || m_bAirStruck))
	{
		m_bAirStruck = false;
		m_bStruck = false;
		m_pASM->InputAnimSocket("UsingControl", { m_pRiseStart });
	}

	if (m_bAirMaintain)
	{
		if (m_pASM->isSocketPassby("UsingControl", 0.5f))
		{
			if (bOnfloor)
			{				
				m_pASM->InputAnimSocket("UsingControl", { m_pBlowLand, m_pGetUp });
				m_bAirMaintain = false;
			}
		}
	}

	m_pTrigger->Update_Tick(m_pTransformCom);

	m_fTurnRemain = m_pController->GetTurnRemain();
	m_vMoveAxis = m_pController->GetMoveAxis();
	
	m_pASM->Tick(TimeDelta);

	_float fMoveSpeed = 0.f;

	if (m_bRun)
		fMoveSpeed = 3.6f;

	else
		fMoveSpeed = 1.8f;

	m_vMoveAxis.Normalize();

	if (!m_bHitMove)
	{
		const _float fYaw = m_pTransformCom->GetYaw_Radian();
		_float3 vVelocity;
		XMStoreFloat3(&vVelocity, fMoveSpeed * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
		m_bOneTick = false;

		m_vPreDir = { 0.f, 0.f, 0.f, 0.f };
		m_vCurDir = { 0.f, 0.f, 0.f, 0.f };
		m_vFinDir = { 0.f, 0.f, 0.f, 0.f };
	}
	else
		HitDir(TimeDelta);		

}

void CFlowerLeg::Late_Tick(_double TimeDelta)
{
	if (!m_bActive)
		return;

	__super::Late_Tick(TimeDelta);

	if (m_bAtkSwitch)	
		Spin_SweepCapsule(m_bOneHit);

	if (m_bVisible)
	{
		if (m_bInvisible)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
		}
		else
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		}
	}
}

HRESULT CFlowerLeg::Render()
{
	if (m_bInvisible)
	{
		m_pModelCom->Render_Pass(m_pTransformCom, 5);
	}
	else
	{
		m_pModelCom->Render(m_pTransformCom);
	}

	return S_OK;
}

void CFlowerLeg::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
	m_pASM->Imgui_RenderState();
}

void CFlowerLeg::AfterPhysX()
{
	if (!m_bActive)
		return;

	__super::AfterPhysX();
	m_pTrigger->Update_AfterPhysX(m_pTransformCom);

	m_pTailCol->Update_Tick(AttachCollider(m_pTailCol));
	m_pTailCol->Update_AfterPhysX(m_pTransformCom);
}

void CFlowerLeg::TakeDamage(DAMAGE_PARAM tDamageParams)
{
	if (m_bDead)
		return;

	EBaseAxis eHitFrom = CClientUtils::GetDamageFromAxis(m_pTransformCom, tDamageParams.vHitFrom);
	m_eHitDir = eHitFrom;
	
	m_eAtkType = tDamageParams.eAttackType;
	m_iHP -= tDamageParams.iDamage;
	
	if (m_eAtkType == EAttackType::ATK_TO_AIR)
	{
		m_bAirStruck = true;
	}

	if(m_eAtkType != EAttackType::ATK_TO_AIR && !m_bAtkSwitch && !m_bInvisible && !m_bUntouchable)
		m_bStruck = true;

	if (m_iHP <= 0 && !m_bDead)
	{
		m_pController->ClearCommands();
		m_DeathTimeline.PlayFromStart();
		m_pASM->InputAnimSocket("UsingControl", { m_pDeadAnim });
		m_bDead = true;
	}
}

void CFlowerLeg::SetActive()
{
	CMonster::SetActive();
	m_pASM->AttachAnimSocket(("UsingControl"), {m_pModelCom->Find_Animation("AS_em0200_160_AL_threat")});
}


void CFlowerLeg::Strew_Overlap()
{	
	_float fLength = 1.f;

	_matrix BoneMatrix = m_pModelCom->GetBoneMatrix("TopFlower4") * m_pTransformCom->Get_WorldMatrix();
	
	_vector vBoneVector = BoneMatrix.r[3];
	_float3 fBone = vBoneVector;

	_vector vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vMyLook = XMVectorSetY(vMyLook, 0.f);
	_float3 fDest = vMyLook;
	
//	const _vector vDirection = XMVector3NormalizeEst(vMyLook);
//	const _float fDistance = XMVectorGetX(XMVector3LengthEst(vDirection));

	_float3 fFinish = { (fBone.x + fLength * fDest.x), fBone.y, (fBone.z + fLength * fDest.z) };
	
	physx::PxOverlapHit hitBuffer[5];
	physx::PxOverlapBuffer overlapOut(hitBuffer, 5);

	SphereOverlapParams param;
	param.fVisibleTime = 0.4f;
	param.iTargetType = CTB_PLAYER;
	param.fRadius = 1.5f;
	param.vPos = XMVectorSetW(fFinish, 1.f);
	param.overlapOut = &overlapOut;

	_float3 paramPos = param.vPos;

	if (CGameInstance::GetInstance()->OverlapSphere(param))
	{
		for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
		{
			auto pHit = overlapOut.getAnyHit(i);
			CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(pHit.actor);
			if (auto pTarget = dynamic_cast<CScarletCharacter*>(pCollidedObject))
			{
				DAMAGE_PARAM tParam;
				tParam.iDamage = (rand() % 30) + 20; 
				tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				tParam.pCauser = this;
				tParam.vHitPosition = paramPos;
				tParam.eAttackType = EAttackType::ATK_LIGHT;
				pTarget->TakeDamage(tParam);
			}
		}
	}
}

void CFlowerLeg::Spin_SweepCapsule(_bool bCol)
{
	if (bCol)
	{
		m_CollisionList.clear();
		return;
	}

	Matrix mTailMatrix = m_pTailCol->GetPxWorldMatrix();
	_float4 vTailPos = _float4(mTailMatrix.Translation().x, mTailMatrix.Translation().y, mTailMatrix.Translation().z, 1.f);
	_float3 vLook = _float3(mTailMatrix.Up().x, mTailMatrix.Up().y, mTailMatrix.Up().z);

	physx::PxSweepHit hitBuffer[5];
	physx::PxSweepBuffer overlapOut(hitBuffer, 5);

	PxCapsuleSweepParams param;
	param.sweepOut = &overlapOut;
	param.CapsuleGeo = m_pTailCol->Get_CapsuleGeometry();
	param.pxTransform = m_pTailCol->Get_PxTransform();

	_float4	vWeaponDir = vTailPos - m_BeforePos;

	param.vUnitDir = _float3(vWeaponDir.x, vWeaponDir.y, vWeaponDir.z);
	param.fDistance = param.vUnitDir.Length();
	param.iTargetType = CTB_PLAYER;
	param.fVisibleTime = 0.f;

	if (CGameInstance::GetInstance()->PxSweepCapsule(param))
	{
		for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
		{
			auto pHit = overlapOut.getAnyHit(i);
			CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(pHit.actor);
			if (auto pTarget = dynamic_cast<CScarletCharacter*>(pCollidedObject))
			{
				_bool bDamagedTarget = true;
				for (auto& iter : m_CollisionList)
				{
					if (iter == pTarget)
					{
						bDamagedTarget = false;
						break;
					}
				}
				if (bDamagedTarget)
				{
					DAMAGE_PARAM tParam;

					tParam.pCauser = this;
					tParam.vHitNormal = _float3(pHit.normal.x, pHit.normal.y, pHit.normal.z);
					tParam.vHitPosition = _float3(pHit.position.x, pHit.position.y, pHit.position.z);
					tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					tParam.iDamage = (rand() % 80) + 40;
					tParam.eAttackType = EAttackType::ATK_MIDDLE;

					pTarget->TakeDamage(tParam);

					m_CollisionList.push_back(pTarget);
					m_bOneHit = true;
				}
			}
		}
	}

	m_BeforePos = vTailPos;
}

void CFlowerLeg::Kick_SweepSphere()
{
	physx::PxSweepHit hitBuffer[5];
	physx::PxSweepBuffer sweepOut(hitBuffer, 5);

	SphereSweepParams Sparam;
	Sparam.fVisibleTime = 0.4f;
	Sparam.iTargetType = CTB_PLAYER;
	Sparam.fRadius = 1.f;
	Sparam.fDistance = 1.f;
	Sparam.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	Sparam.sweepOut = &sweepOut;
	Sparam.vUnitDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_float3 paramPos = Sparam.vPos;
	
	if (CGameInstance::GetInstance()->SweepSphere(Sparam))
	{
		for (int i = 0; i < sweepOut.getNbAnyHits(); ++i)
		{
			auto pHit = sweepOut.getAnyHit(i);
			CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(pHit.actor);
			if (auto pTarget = dynamic_cast<CScarletCharacter*>(pCollidedObject))
			{
				DAMAGE_PARAM param;
				param.iDamage = (rand() % 120) + 30;
				param.eAttackType = EAttackType::ATK_HEAVY;
				param.pCauser = this;
				param.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				param.vHitPosition = paramPos;
				
				pTarget->TakeDamage(param);
			}
		}
	}
}

_matrix CFlowerLeg::AttachCollider(CRigidBody * pRigidBody)
{
	_matrix SocketMatrix;

	if (pRigidBody == m_pTailCol)
		SocketMatrix = m_pModelCom->GetBoneMatrix("Tail4") * m_pTransformCom->Get_WorldMatrix();
	
	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	return SocketMatrix;
}

void CFlowerLeg::HitDir(_double TimeDelta)
{
	// 몸의 중점을 잡는 뼈
	_matrix matRef = m_pModelCom->GetBoneMatrix("Reference") * m_pTransformCom->Get_WorldMatrix();
	_vector vRef = matRef.r[3];

	// 밀려나는 거리만큼의 뼈
	_matrix matWeak = m_pModelCom->GetBoneMatrix("Weak01") * m_pTransformCom->Get_WorldMatrix();
	_vector vWeak = matWeak.r[3];

	// 현재 위치
	_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	if (!m_bOneTick)
		m_vPreDir = vPosition;

	// 방향
	_vector vDest = vRef - vWeak;

	m_vCurDir = vDest;

	if (m_bOneTick)
		m_vFinDir = m_vCurDir - m_vPreDir;

	_float fRange = XMVectorGetX(m_vFinDir);

	m_pTransformCom->LocalMove(m_vFinDir);

	m_vPreDir = m_vCurDir;

	m_bOneTick = true;		
}

_bool CFlowerLeg::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("UsingControl") == false;
}

CFlowerLeg * CFlowerLeg::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFlowerLeg* pInstance = new CFlowerLeg(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFlowerLeg");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFlowerLeg::Clone(void * pArg)
{
	CFlowerLeg*		pInstance = new CFlowerLeg(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFlowerLeg");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFlowerLeg::Free()
{
	CMonster::Free();
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
	Safe_Release(m_pTrigger);	
	Safe_Release(m_pTailCol);
}

/*	기존의 initialize
if (FAILED(__super::Initialize(pArg)))
return E_FAIL;

if (FAILED(SetUp_Components(pArg)))
return E_FAIL;

Json FlowerLegTrigger = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/FlowerLegTrigger.json");

if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Trigger"),
(CComponent**)&m_pTrigger, &FlowerLegTrigger)))
return E_FAIL;

m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat3(&_float3(0.f, 0.f, 0.f)));

m_pTransformCom->SetSpeed(1.f);

m_strObjectTag = "Flower_Leg";*/
