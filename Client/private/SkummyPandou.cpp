#include "stdafx.h"
#include "SkummyPandou.h"
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

#include "SkPd_Controller.h"
#include "SkPd_AnimInstance.h"
#include "RigidBody.h"
#include "Player.h"
#include "VFX_Manager.h"


CSkummyPandou::CSkummyPandou(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CSkummyPandou::CSkummyPandou(const CSkummyPandou & rhs)
	: CMonster(rhs)
{
}

HRESULT CSkummyPandou::Initialize_Prototype()
{
	return CMonster::Initialize_Prototype();
}

HRESULT CSkummyPandou::Initialize(void * pArg)
{
	Json SkummyPandouTrigger = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/SkummyPandou/SkummyPandouTrigger.json");
	MoveTransformJson(SkummyPandouTrigger, pArg);
	pArg = &SkummyPandouTrigger;
	
	FAILED_CHECK(CMonster::Initialize(pArg));

	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Trigger"),
		(CComponent**)&m_pTrigger, &SkummyPandouTrigger)))
		return E_FAIL;	
	
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"MonsterSkummyPandou", L"Model", (CComponent**)&m_pModelCom));

	// Controller
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Proto_SkPd_Controller"), TEXT("Com_Controller"), (CComponent**)&m_pController));

	// Event Caller
	m_pModelCom->Add_EventCaller("Rush_Start", [this]
	{
		// Effect 생성
		if (!m_bDead)
		{
			m_pDash_Effect = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0750_Dash_Attack");
			m_pDash_Effect->Start_Attach(this, "Hips", true);
			Safe_AddRef(m_pDash_Effect);
		}
		m_bAtkToggle = true;
	});

	m_pModelCom->Add_EventCaller("Rush_End", [this]
	{
		//// Effect 해제
		//if (m_bCloned == true)
		//{
		//	if (m_pDash_Effect->IsDeleted() == false)
		//		m_pDash_Effect->SetDelete();

		//	Safe_Release(m_pDash_Effect);		
		//}
		//m_bAtkToggle = false;
	});
	
	m_pModelCom->Add_EventCaller("UpperCut", [this]
	{
//		m_bActiveGravity = true;
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
	// ~Event Caller

	m_iHP = 900; // ★
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(190.f));	
	m_vFinDir = { 0.f, 0.f, 0.f, 0.f };
//	m_bActiveGravity = false;

	m_pASM = CSkPd_AnimInstance::Create(m_pModelCom, this);

	// Socket
	m_pAtk_RushStart = m_pModelCom->Find_Animation("AS_em0700_201_AL_atk_a1_start");
	m_pAtk_RushLoop = m_pModelCom->Find_Animation("AS_em0710_205_AL_atk_a2_loop");
	m_pAtk_RushEnd = m_pModelCom->Find_Animation("AS_em0710_207_AL_atk_a3");
	m_pThreat = m_pModelCom->Find_Animation("AS_em0700_160_AL_threat");

	m_pDamage_L_F = m_pModelCom->Find_Animation("AS_em0700_401_AL_damage_l_F");
	m_pDamage_L_B = m_pModelCom->Find_Animation("AS_em0700_402_AL_damage_l_B");

	m_pDamage_M_F = m_pModelCom->Find_Animation("AS_em0700_411_AL_damage_m_F");
	m_pDamage_M_B = m_pModelCom->Find_Animation("AS_em0700_412_AL_damage_m_B");
	m_pDamage_M_L = m_pModelCom->Find_Animation("AS_em0700_413_AL_damage_m_L");
	m_pDamage_M_R = m_pModelCom->Find_Animation("AS_em0700_414_AL_damage_m_R");

	m_pBlowStart = m_pModelCom->Find_Animation("AS_em0700_432_AL_blow_start_F");
	m_pBlowLand = m_pModelCom->Find_Animation("AS_em0700_433_AL_blow_landing_F");
	m_pGetUp = m_pModelCom->Find_Animation("AS_em0700_427_AL_getup");
	m_pRiseStart = m_pModelCom->Find_Animation("AS_em0700_455_AL_rise_start");

	m_pDeadAnim = m_pModelCom->Find_Animation("AS_em0700_424_AL_dead_down01");
	// ~Socket
	
	return S_OK;
}


void CSkummyPandou::BeginTick()
{
	__super::BeginTick();
	m_pASM->AttachAnimSocket("Bee", { m_pModelCom->Find_Animation("AS_em0700_160_AL_threat") });
}

void CSkummyPandou::Tick(_double TimeDelta)
{
	/*if (!m_bActive)
		return;*/

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
	
	m_bRush = m_pController->IsRush();

	if (m_bAtkToggle)
	{
		RushSweep(m_bOneHit);
	}

	/*if (!m_bActiveGravity)
	{
		m_fYSpeed -= m_fGravity * (_float)TimeDelta;
	}
	else
	{
		if (m_pASM->CheckSocketAnim("Bee", "AS_em0700_427_AL_getup"))
		{
			if (m_pASM->isSocketPassby("Bee", 0.75f))
				m_bActiveGravity = false;
		}
		else
			m_bActiveGravity = true;
	}*/

	_bool bOnfloor = IsOnFloor();
	// Socket

	if (!m_bDead && m_pController->KeyDown(CController::MOUSE_LB))
	{
		m_pASM->AttachAnimSocket("Bee", { m_pAtk_RushStart });
	}

	if (m_pController->KeyDown(CController::R))
	{
		m_pASM->AttachAnimSocket("Bee", { m_pAtk_RushLoop, m_pAtk_RushLoop, m_pAtk_RushEnd });	
	}

	if (!m_bDead && m_pController->KeyDown(CController::G))
	{
		m_pASM->AttachAnimSocket("Bee", { m_pThreat });
		m_bOneHit = false;		
	}

	if (!m_bAirStruck && m_bStruck && !m_bAirMaintain || m_pController->KeyDown(CController::NUM_1))
	{	
		m_bOneHit = false;
		m_bStruck = false;
		m_pController->ClearCommands();

		if (m_eAtkType == EAttackType::ATK_LIGHT)
		{
			if (m_eHitDir == EBaseAxis::NORTH)
			{
				m_pASM->InputAnimSocket("Bee", { m_pDamage_L_F });
				m_bHitMove = true;
			}

			else
			{
				m_pASM->InputAnimSocket("Bee", { m_pDamage_L_B });
				m_bHitMove = true;
			}
		}

		if (m_eAtkType == EAttackType::ATK_MIDDLE || m_eAtkType == EAttackType::ATK_HEAVY)
		{
			if (m_eHitDir == EBaseAxis::NORTH)
				m_pASM->InputAnimSocket("Bee", { m_pDamage_M_F });

			else if (m_eHitDir == EBaseAxis::SOUTH)
				m_pASM->InputAnimSocket("Bee", { m_pDamage_M_B });

			else if (m_eHitDir == EBaseAxis::WEST)
				m_pASM->InputAnimSocket("Bee", { m_pDamage_M_L });

			else if (m_eHitDir == EBaseAxis::EAST)
				m_pASM->InputAnimSocket("Bee", { m_pDamage_M_R });
		}
	}

	if ((!m_bStruck && m_bAirStruck && !m_bAirMaintain) || m_pController->KeyDown(CController::NUM_3))
	{
		m_bHitMove = false;
		m_bAirStruck = false;
		m_bOneHit = false;
		m_pController->ClearCommands();
	
		m_pASM->AttachAnimSocket("Bee", { m_pBlowStart });
		m_bAirMaintain = true;
	}

	if(m_bAirMaintain && (m_bStruck || m_bAirStruck))
	{		
		m_bAirStruck = false;
		m_bStruck = false;
		m_pASM->InputAnimSocket("Bee", { m_pRiseStart });
	}

	if (m_bAirMaintain)
	{
		if (m_pASM->isSocketPassby("Bee", 0.5f))
		{
			if (bOnfloor)
			{
				m_pASM->InputAnimSocket("Bee", { m_pBlowLand, m_pGetUp });
				m_bAirMaintain = false;
			}
		}
	}

	m_pTrigger->Update_Tick(AttachCollider(m_pTrigger));

	m_fTurnRemain = m_pController->GetTurnRemain();
	m_vMoveAxis = m_pController->GetMoveAxis();

	
	m_pASM->Tick(TimeDelta);

	_float fMoveSpeed = 0.f;

	if (m_bRush)
		fMoveSpeed = 15.f;

	else
		fMoveSpeed = 2.5f;

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

void CSkummyPandou::Late_Tick(_double TimeDelta)
{
	/*if (!m_bActive)
		return;*/

	__super::Late_Tick(TimeDelta);

	if(nullptr != m_pRendererCom && m_bVisible)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CSkummyPandou::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CSkummyPandou::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();	
	m_pASM->Imgui_RenderState();
}

void CSkummyPandou::TakeDamage(DAMAGE_PARAM tDamageParams)
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

	if (m_eAtkType != EAttackType::ATK_TO_AIR && !m_bAtkToggle && !m_bDead)
	{
		m_bStruck = true;
	}

	if (m_iHP <= 0 && !m_bDead)
	{
		m_pController->ClearCommands();
		m_DeathTimeline.PlayFromStart();
		m_pASM->InputAnimSocket("Bee", { m_pDeadAnim });
		m_bDead = true;
	}

	__super::TakeDamage(tDamageParams);
}

void CSkummyPandou::AfterPhysX()
{
	/*if (!m_bActive)
		return;*/

	__super::AfterPhysX();	
	m_pTrigger->Update_AfterPhysX(m_pTransformCom);
}

_matrix CSkummyPandou::AttachCollider(CRigidBody* pRigidBody)
{
	_matrix SocketMatrix; 
	
	if(pRigidBody == m_pTrigger)
		SocketMatrix = m_pModelCom->GetBoneMatrix("Spine1") * m_pTransformCom->Get_WorldMatrix();

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	return SocketMatrix;
}

void CSkummyPandou::RushSweep(_bool bCol)
{	
	if (bCol)
	{
		m_CollisionList.clear();
		return;
	}

	Matrix matWeapon = m_pTrigger->GetPxWorldMatrix();
	_float4 vWeaponPos = _float4(matWeapon.Translation().x, matWeapon.Translation().y, matWeapon.Translation().z, 1.f);
	
	physx::PxSweepHit hitBuff[5];
	physx::PxSweepBuffer overlapOut(hitBuff, 5);

	SphereSweepParams params;
	params.sweepOut = &overlapOut;
	params.fRadius = 1.2f;
	params.vPos = vWeaponPos;

	_float4	vWeaponDir = vWeaponPos - m_BeforePos;

	params.vUnitDir = _float3(vWeaponDir.x, vWeaponDir.y, vWeaponDir.z);
	params.fDistance = params.vUnitDir.Length() + 2.f;
	params.iTargetType = CTB_PLAYER;
	params.fVisibleTime = 0.f; // 보여지는 시간
	if (CGameInstance::GetInstance()->SweepSphere(params))	// 조건
	{
		for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
		{
			auto pHit = overlapOut.getAnyHit(i);
			CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(pHit.actor);
			if (auto pTargetCol = dynamic_cast<CPlayer*>(pCollidedObject))
			{
				_bool bDamagedTarget = true;
				for (auto& iter : m_CollisionList)
				{
					if (iter == pTargetCol)
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
					tParam.iDamage = (rand() % 60) + 35;
					tParam.eAttackType = EAttackType::ATK_HEAVY;

					pTargetCol->TakeDamage(tParam);
					// 러쉬 공격시에만 데미지 전달
					m_CollisionList.push_back(pTargetCol);
					m_bOneHit = true;
				}
			}
		}
	}

	m_BeforePos = vWeaponPos;
}

void CSkummyPandou::HitDir(_double TimeDelta)
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

//void CSkummyPandou::SetActive()
//{
//	CMonster::SetActive();
//	m_pASM->AttachAnimSocket("Bee", { m_pModelCom->Find_Animation("AS_em0700_160_AL_threat") });
//}

_bool CSkummyPandou::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("Bee") == false;
}

CSkummyPandou * CSkummyPandou::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkummyPandou* pInstance = new CSkummyPandou(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSkummyPandou");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSkummyPandou::Clone(void * pArg)
{
	CSkummyPandou*		pInstance = new CSkummyPandou(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSkummyPandou");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkummyPandou::Free()
{
	CMonster::Free();

	if (m_bCloned == true)
	{
		if (m_pDash_Effect != nullptr && m_pDash_Effect->IsDeleted() == false)
			m_pDash_Effect->SetDelete();

		Safe_Release(m_pDash_Effect);
	}

	Safe_Release(m_pASM);
	Safe_Release(m_pController);
	Safe_Release(m_pTrigger);
}
