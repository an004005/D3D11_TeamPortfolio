#include "stdafx.h"
#include "BuddyLumi.h"
#include "GameInstance.h"
#include "MathUtils.h"
#include "GameUtils.h"
#include "FSMComponent.h"
#include "AnimationInstance.h"
#include "Animation.h"
#include "Model.h"
#include "JsonStorage.h"
#include "PhysX_Manager.h"

#include "BdLm_Controller.h"
#include "BdLm_AnimInstance.h"
#include "RigidBody.h"
#include "Player.h"

#include "ScarletWeapon.h"

CBuddyLumi::CBuddyLumi(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CBuddyLumi::CBuddyLumi(const CBuddyLumi & rhs)
	: CMonster(rhs)
{
}

HRESULT CBuddyLumi::Initialize_Prototype()
{
	return CMonster::Initialize_Prototype();
}

HRESULT CBuddyLumi::Initialize(void * pArg)
{
	Json BuddyLumiTrigger = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BuddyLumiTrigger.json");
	pArg = &BuddyLumiTrigger;

	FAILED_CHECK(CMonster::Initialize(pArg));

	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Trigger"),
		(CComponent**)&m_pTrigger, &BuddyLumiTrigger)))
		return E_FAIL;
	
	Json BuddyLumiWeapon = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BuddyLumiWeapon.json");

	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Weapon"),
		(CComponent**)&m_pWeaponCollider, &BuddyLumiWeapon)))
		return E_FAIL;	

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"MonsterBuddyLumi", L"Model", (CComponent**)&m_pModelCom));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Proto_BdLm_Controller"), TEXT("Com_Controller"), (CComponent**)&m_pController));

	// Event Caller
	
	m_pModelCom->Add_EventCaller("Swing_Start", [this] {  });
	m_pModelCom->Add_EventCaller("Swing_End", [this] {});

	// ~Event Caller

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat3(&_float3(1.f, 0.f, 29.f)));

	m_pTransformCom->SetRotPerSec(XMConvertToRadians(90.f));

	m_pASM = CBdLm_AnimInstance::Create(m_pModelCom, this);

	// 소켓 애니메이션 추가

	m_pAtk_Swing = m_pModelCom->Find_Animation("AS_em0400_200_AL_atk_a1");

	m_pThreat = m_pModelCom->Find_Animation("AS_em0400_160_AL_threat");

	m_pDodgeL = m_pModelCom->Find_Animation("AS_em0400_140_AL_dodge_L");
	m_pDodgeR = m_pModelCom->Find_Animation("AS_em0400_145_AL_dodge_R");
	m_pDodgeB = m_pModelCom->Find_Animation("AS_em0400_135_AL_dodge_B");

	m_pDamage_L_F = m_pModelCom->Find_Animation("AS_em0400_401_AL_damage_l_F");
	m_pDamage_L_B = m_pModelCom->Find_Animation("AS_em0400_402_AL_damage_l_B");

	m_pDamage_M_F = m_pModelCom->Find_Animation("AS_em0400_411_AL_damage_m_F");
	m_pDamage_M_B = m_pModelCom->Find_Animation("AS_em0400_412_AL_damage_m_B");
	m_pDamage_M_L = m_pModelCom->Find_Animation("AS_em0400_413_AL_damage_m_L");
	m_pDamage_M_R = m_pModelCom->Find_Animation("AS_em0400_414_AL_damage_m_R");

	m_pBlowStart = m_pModelCom->Find_Animation("AS_em0400_432_AL_blow_start_F");
	m_pBlowLand = m_pModelCom->Find_Animation("AS_em0400_433_AL_blow_landing_F");
	m_pGetUp = m_pModelCom->Find_Animation("AS_em0400_427_AL_getup_01");
	m_pRiseStart = m_pModelCom->Find_Animation("AS_em0400_455_AL_rise_start");

	m_pDeadAnim = m_pModelCom->Find_Animation("AS_em0400_424_AL_dead_down");

	// ~소켓 애니메이션 추가

	return S_OK;
}

void CBuddyLumi::BeginTick()
{
	__super::BeginTick();
	m_pASM->AttachAnimSocket("Buddy", { m_pModelCom->Find_Animation("AS_em0400_160_AL_threat") });

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat3(&_float3(1.f, 0.f, 29.f)));	
}

void CBuddyLumi::Tick(_double TimeDelta)
{	
	CMonster::Tick(TimeDelta);

	auto pPlayer = CGameInstance::GetInstance()->Find_ObjectByPredicator(LEVEL_NOW, [this](CGameObject* pObj)
	{
		return dynamic_cast<CPlayer*>(pObj) != nullptr;
	});
	m_pTarget = dynamic_cast<CScarletCharacter*>(pPlayer);

	// Controller
	m_pController->SetTarget(m_pTarget);

	m_pController->Tick(TimeDelta);
	m_bRun = m_pController->IsRun();
	_bool bOnfloor = IsOnFloor();

	if (m_pController->KeyDown(CController::NUM_1)) 
	{
		m_pASM->AttachAnimSocket("Buddy", { m_pDodgeL });
	}

	if (m_pController->KeyDown(CController::NUM_2))
	{
		m_pASM->AttachAnimSocket("Buddy", { m_pDodgeR });
	}

	if (m_pController->KeyDown(CController::NUM_3))
	{
		m_pASM->AttachAnimSocket("Buddy", { m_pDodgeB });
	}

	if (m_pController->KeyDown(CController::MOUSE_LB))
	{
		m_pASM->AttachAnimSocket("Buddy", { m_pAtk_Swing });
	}

	if (m_pController->KeyDown(CController::MOUSE_RB))
	{
		m_pASM->AttachAnimSocket("Buddy", { m_pThreat });
	}

	if (m_bStruck || m_pController->KeyDown(CController::Q))
	{
		m_bStruck = false;
		m_pController->ClearCommands();

		if (m_eAtkType == EAttackType::ATK_LIGHT)
		{
			if (m_eHitDir == EBaseAxis::NORTH)
				m_pASM->InputAnimSocket("Buddy", { m_pDamage_L_F });

			else
				m_pASM->InputAnimSocket("Buddy", { m_pDamage_L_B });
		}

		if (m_eAtkType == EAttackType::ATK_MIDDLE)
		{
			if (m_eHitDir == EBaseAxis::NORTH)
				m_pASM->InputAnimSocket("Buddy", { m_pDamage_M_F });

			else if (m_eHitDir == EBaseAxis::SOUTH)
				m_pASM->InputAnimSocket("Buddy", { m_pDamage_M_B });

			else if (m_eHitDir == EBaseAxis::WEST)
				m_pASM->InputAnimSocket("Buddy", { m_pDamage_M_L });

			else if (m_eHitDir == EBaseAxis::EAST)
				m_pASM->InputAnimSocket("Buddy", { m_pDamage_M_R });
		}
	}

	if (m_bAirStruck || m_pController->KeyDown(CController::X))
	{
		m_bAirStruck = false;
		m_pController->ClearCommands();
		// 추가타 X
		if (m_iAirDamage < 2)
		{
			if (!m_bMaintain)
			{
				m_pASM->AttachAnimSocket("Buddy", { m_pBlowStart });
				m_bMaintain = true;
			}
		}

		else if (m_iAirDamage >= 2)
		{
			if (m_iAirDamage > m_iPreAirDamageCnt)
				m_pASM->AttachAnimSocket("Buddy", { m_pRiseStart });

			m_iPreAirDamageCnt = m_iAirDamage;
		}
	}

	if (m_bMaintain)
	{
		if (m_pASM->isSocketPassby("Buddy", 0.5f))
		{
			if (bOnfloor)
			{
				m_pASM->InputAnimSocket("Buddy", { m_pBlowLand, m_pGetUp });
				m_iAirDamage = 0;
				m_bMaintain = false;
			}
		}
	}

	m_pTrigger->Update_Tick(m_pTransformCom);

	m_fTurnRemain = m_pController->GetTurnRemain();
	m_vMoveAxis = m_pController->GetMoveAxis();

	m_pASM->Tick(TimeDelta);

	_float fMoveSpeed = 0.f;

	if (m_bRun)
		fMoveSpeed = 3.f;

	else
		fMoveSpeed = 1.8f;

	m_vMoveAxis.Normalize();

	const _float fYaw = m_pTransformCom->GetYaw_Radian();
	_float3 vVelocity;
	XMStoreFloat3(&vVelocity, fMoveSpeed * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
	m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);

}

void CBuddyLumi::Late_Tick(_double TimeDelta)
{	
	__super::Late_Tick(TimeDelta);

	if (m_iHP <= 0)
		m_bDead = true;

	if (m_bDead && m_pController->KeyDown(CController::X))
	{
		m_pController->ClearCommands();
		m_pASM->AttachAnimSocket("Buddy", { m_pDeadAnim });
	}
		
	if (nullptr != m_pRendererCom && m_bVisible)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CBuddyLumi::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CBuddyLumi::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
	m_pASM->Imgui_RenderState();
}

void CBuddyLumi::TakeDamage(DAMAGE_PARAM tDamageParams)
{
	/*
	// 예제 코드
	_vector tmp = _float4{ tDamageParams.vHitFrom.x, tDamageParams.vHitFrom.y , tDamageParams.vHitFrom.z, 1.f };
	_float4 vBackDir = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) - tmp;
	vBackDir.Normalize();

	m_tmp = _float3{ vBackDir.x, vBackDir.y ,vBackDir.z } *2.f;

	*/
	EBaseAxis eHitFrom = CClientUtils::GetDamageFromAxis(m_pTransformCom, tDamageParams.vHitFrom);
	// ↑ 공격이 들어올 방향 
	m_eHitDir = eHitFrom;
	
	m_eAtkType = tDamageParams.eAttackType;

	if (m_eAtkType == EAttackType::ATK_TO_AIR)
	{
		m_bAirStruck = true;
		++m_iAirDamage;
	}
	else
		m_bStruck = true;
}

void CBuddyLumi::AfterPhysX()
{
	__super::AfterPhysX();
	// 무기 콜라이더의 Update_Tick(매트릭스) 
	m_pTrigger->Update_AfterPhysX(m_pTransformCom);

	m_pWeaponCollider->Update_Tick(AttachCollider(m_pWeaponCollider));
	m_pWeaponCollider->Update_AfterPhysX(m_pTransformCom);

}

void CBuddyLumi::Collision()
{
	DAMAGE_PARAM damageParam;

	_float3 fWeaponHitPosition = { m_pWeaponCollider->GetPxWorldMatrix().m[3][0], m_pWeaponCollider->GetPxWorldMatrix().m[3][1], m_pWeaponCollider->GetPxWorldMatrix().m[3][2] };

	damageParam.eAttackSAS = ESASType::SAS_END;
	damageParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	damageParam.vHitPosition = fWeaponHitPosition;
	damageParam.pCauser = this;
	damageParam.iDamage = 1;

	Collision_Check_Capsule(m_pWeaponCollider, damageParam);
}

_matrix CBuddyLumi::AttachCollider(CRigidBody* pRigidBody)
{
	_matrix	SocketMatrix;

	if (pRigidBody == m_pWeaponCollider)
		SocketMatrix = m_pModelCom->GetBoneMatrix("RightWeapon") * m_pTransformCom->Get_WorldMatrix();

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	return SocketMatrix;
}

_bool CBuddyLumi::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("Buddy") == false;
}

CBuddyLumi * CBuddyLumi::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBuddyLumi* pInstance = new CBuddyLumi(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBuddyLumi");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBuddyLumi::Clone(void * pArg)
{
	CBuddyLumi*		pInstance = new CBuddyLumi(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBuddyLumi");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBuddyLumi::Free()
{
	CMonster::Free();	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
	Safe_Release(m_pTrigger);
	Safe_Release(m_pWeaponCollider);
}
