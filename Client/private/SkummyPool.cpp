#include "stdafx.h"
#include "SkummyPool.h"
#include "GameInstance.h"
#include "MathUtils.h"
#include "GameUtils.h"
#include "FSMComponent.h"
#include "AnimationInstance.h"
#include "Animation.h"
#include "Model.h"
#include "JsonStorage.h"
#include "PhysX_Manager.h"

#include "SkmP_Controller.h"
#include "SkmP_AnimInstance.h"
#include "FlowerLeg.h"
#include "Player.h"
#include "SkMpBullet.h"

#include "RigidBody.h"
#include "VFX_Manager.h"
#include "MonsterHpUI.h"
// TODO : 소켓 작업, Turn

CSkummyPool::CSkummyPool(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CSkummyPool::CSkummyPool(const CSkummyPool & rhs)
	: CMonster(rhs)
{
}

HRESULT CSkummyPool::Initialize_Prototype()
{
	return  CMonster::Initialize_Prototype();
}

HRESULT CSkummyPool::Initialize(void * pArg)
{
	Json SkummyPoolTrigger = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/SkummyPool/Test.json");
	MoveTransformJson(SkummyPoolTrigger, pArg);
	pArg = &SkummyPoolTrigger;

	FAILED_CHECK(CMonster::Initialize(pArg));

	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Trigger"), 
		(CComponent**)&m_pTrigger, &SkummyPoolTrigger)))
		return E_FAIL;

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"MonsterSkummyPool", L"Model", (CComponent**)&m_pModelCom));

	// Controller
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Proto_SkmP_Controller"), TEXT("Com_Controller"), (CComponent**)&m_pController));

	// Event Caller

	m_pModelCom->Add_EventCaller("Muzzle", [this] 
	{
		if (!m_bDead)
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0650_Bullet_Birth")->Start_Attach(this, "Eff02");	
	});

	m_pModelCom->Add_EventCaller("Shoot", [this] 
	{
		_vector vTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		_vector vTargetColPos = dynamic_cast<CScarletCharacter*>(m_pTarget)->GetColliderPosition();
		m_pBullet = dynamic_cast<CSkMpBullet*>(CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_Bullet"), TEXT("SkMpBullet")));	
		
//		auto pObj = CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_Bullet"), TEXT("SkMpBullet"));

		if(CSkMpBullet* pBullet = dynamic_cast<CSkMpBullet*>(m_pBullet))
		{
			pBullet->Set_Owner(this);
			// Effect
			pBullet->Set_TrailParticle(CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0650_Bullet_Loop"));
			pBullet->Get_TrailParticle()->Start_NoAttach(pBullet, false);
			pBullet->Set_BulletEffect(CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"Em0650_Bullet_Loop"));
			pBullet->Get_BulletEffect()->Start_NoAttach(pBullet, true);
			// ~Effect

			_matrix BoneMtx = m_pModelCom->GetBoneMatrix("Alga_F_03") * m_pTransformCom->Get_WorldMatrix();
			_vector vPrePos = BoneMtx.r[3];

			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

			pBullet->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vPrePos);
			_vector vDest = vTargetColPos - vPrePos;
			pBullet->Set_ShootDir(vDest); // vLook

			pBullet->GetTransform()->LookAt(vTargetColPos); // vPrePos + vLook);
		}
	});
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
	// ~Event Caller
	
	m_iHP = 800; // ★
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(180.f));
	m_vFinDir = { 0.f, 0.f, 0.f, 0.f };

	m_pASM = CSkmP_AnimInstance::Create(m_pModelCom, this);

	// Socket 
	m_pAtk_Shoot = m_pModelCom->Find_Animation("AS_em0600_204_AL_atk_a3_shot");
//	m_pThreat = m_pModelCom->Find_Animation("AS_em0600_160_AL_threat");

	m_pDamage_L_F = m_pModelCom->Find_Animation("AS_em0600_401_AL_damage_l_F");
	m_pDamage_L_B = m_pModelCom->Find_Animation("AS_em0600_402_AL_damage_l_B");

	m_pDamage_M_F = m_pModelCom->Find_Animation("AS_em0600_411_AL_damage_m_F");
	m_pDamage_M_B = m_pModelCom->Find_Animation("AS_em0600_412_AL_damage_m_B");
	m_pDamage_M_L = m_pModelCom->Find_Animation("AS_em0600_413_AL_damage_m_L");
	m_pDamage_M_R = m_pModelCom->Find_Animation("AS_em0600_414_AL_damage_m_R");

	m_pBlowStart = m_pModelCom->Find_Animation("AS_em0600_432_AL_blow_start_F");
	m_pBlowLand = m_pModelCom->Find_Animation("AS_em0600_433_AL_blow_landing_F");
	m_pGetUp = m_pModelCom->Find_Animation("AS_em0600_427_AL_getup");
	m_pRiseStart = m_pModelCom->Find_Animation("AS_em0600_455_AL_rise_start");

	m_pDeadAnim = m_pModelCom->Find_Animation("AS_em0600_424_AL_dead_down");

	return S_OK;
}

void CSkummyPool::BeginTick()
{
	__super::BeginTick();
	m_pASM->AttachAnimSocket("Pool", { m_pModelCom->Find_Animation("AS_em0600_160_AL_threat") });
}

void CSkummyPool::Tick(_double TimeDelta)
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

	_bool bOnfloor = IsOnFloor();

	// Socket
	if (!m_bDead && m_pController->KeyDown(CController::MOUSE_LB))
	{
		m_pASM->AttachAnimSocket("Pool", { m_pAtk_Shoot });
	}

	/*if (m_pController->KeyDown(CController::R))
	{
		m_pASM->AttachAnimSocket("Pool", { m_pThreat });
	}*/

	if (!m_bAirStruck && m_bStruck && !m_bAirMaintain || m_pController->KeyDown(CController::Q))
	{
		m_bStruck = false;
		m_pController->ClearCommands();

		if (m_eAtkType == EAttackType::ATK_LIGHT)
		{
			if (m_eHitDir == EBaseAxis::NORTH)
			{
				m_pASM->InputAnimSocket("Pool", { m_pDamage_L_F });
				m_bHitMove = true;
			}

			else
			{
				m_pASM->InputAnimSocket("Pool", { m_pDamage_L_B });
				m_bHitMove = true;
			}
		}

		if (m_eAtkType == EAttackType::ATK_MIDDLE || m_eAtkType == EAttackType::ATK_HEAVY)
		{
			if (m_eHitDir == EBaseAxis::NORTH)
				m_pASM->InputAnimSocket("Pool", { m_pDamage_M_F });

			else if (m_eHitDir == EBaseAxis::SOUTH)
				m_pASM->InputAnimSocket("Pool", { m_pDamage_M_B });

			else if (m_eHitDir == EBaseAxis::WEST)
				m_pASM->InputAnimSocket("Pool", { m_pDamage_M_L });

			else if (m_eHitDir == EBaseAxis::EAST)
				m_pASM->InputAnimSocket("Pool", { m_pDamage_M_R });
		}
	}

	if ((!m_bStruck && m_bAirStruck && !m_bAirMaintain) || m_pController->KeyDown(CController::X))
	{
		m_bHitMove = false;
		m_bAirStruck = false;
		m_pController->ClearCommands();
		
		m_pASM->AttachAnimSocket("Pool", { m_pBlowStart });
		m_bAirMaintain = true;
	}

	if (m_bAirMaintain && (m_bStruck || m_bAirStruck))
	{
		m_bAirStruck = false;
		m_bStruck = false;
		m_pASM->AttachAnimSocket("Pool", { m_pRiseStart });
	}

	if (m_bAirMaintain)
	{
		if (m_pASM->isSocketPassby("Pool", 0.5f))
		{
			if (bOnfloor)
			{
				m_pASM->InputAnimSocket("Pool", { m_pBlowLand, m_pGetUp });				
				m_bAirMaintain = false;
			}
		}
	}

	m_pTrigger->Update_Tick(m_pTransformCom);

	m_fTurnRemain = m_pController->GetTurnRemain();
	m_vMoveAxis = m_pController->GetMoveAxis();

	
	m_pASM->Tick(TimeDelta);

	_float fMoveSpeed = 1.f;
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

void CSkummyPool::Late_Tick(_double TimeDelta)
{
	/*if (!m_bActive)
		return;*/
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom && m_bVisible)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CSkummyPool::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CSkummyPool::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
	m_pASM->Imgui_RenderState();
}

void CSkummyPool::TakeDamage(DAMAGE_PARAM tDamageParams)
{
	IM_LOG("Coll");

	EBaseAxis eHitFrom = CClientUtils::GetDamageFromAxis(m_pTransformCom, tDamageParams.vHitFrom);
	// ↑ 공격이 들어올 방향 
	m_eHitDir = eHitFrom;
	
	m_eAtkType = tDamageParams.eAttackType;
	m_iHP -= tDamageParams.iDamage;
	
	if (m_eAtkType == EAttackType::ATK_TO_AIR)
	{
		// 공중 피격 정의 
		m_bAirStruck = true;

	}
	if (m_eAtkType != EAttackType::ATK_TO_AIR)	
		m_bStruck = true;
	
	if (m_iHP <= 0 && !m_bDead)
	{
		m_pController->ClearCommands();
		m_DeathTimeline.PlayFromStart();
		m_pASM->InputAnimSocket("Pool", { m_pDeadAnim });
		m_bDead = true;
	}

	// 타격 이펙트 잡아주는 부분, 몬스터 상위에서 동작
	__super::TakeDamage(tDamageParams);
}

void CSkummyPool::AfterPhysX()
{
	/*if (!m_bActive)
		return;*/
	__super::AfterPhysX();
	m_pTrigger->Update_AfterPhysX(m_pTransformCom);
}

void CSkummyPool::HitDir(_double TimeDelta)
{
	// 몸의 중점을 잡는 뼈
	_matrix matRef = m_pModelCom->GetBoneMatrix("Weak01") * m_pTransformCom->Get_WorldMatrix();
	_vector vRef = matRef.r[3];

	// 밀려나는 거리만큼의 뼈
	_matrix matWeak = m_pModelCom->GetBoneMatrix("Tail_F") * m_pTransformCom->Get_WorldMatrix();
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

//void CSkummyPool::SetActive()
//{
//	CMonster::SetActive();
//	m_pASM->AttachAnimSocket("Pool", { m_pModelCom->Find_Animation("AS_em0600_160_AL_threat") });
//}

_bool CSkummyPool::IsPlayingSocket() const
{
	_bool bTest = (m_pASM->isSocketEmpty("Pool") == false);

	return m_pASM->isSocketEmpty("Pool") == false;
}

CSkummyPool * CSkummyPool::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkummyPool* pInstance = new CSkummyPool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSkummyPool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSkummyPool::Clone(void * pArg)
{
	CSkummyPool*		pInstance = new CSkummyPool(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSkummyPool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkummyPool::Free()
{
	CMonster::Free();
	Safe_Release(m_pBullet);
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
	Safe_Release(m_pTrigger);
}
