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
	Json SkummyPoolTrigger = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/Test.json");
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

	m_pModelCom->Add_EventCaller("Shoot", [this] 
	{
		_vector vTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);

		auto pObj = CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_Bullet"), TEXT("SkMpBullet"));
		if (CSkMpBullet* pBullet = dynamic_cast<CSkMpBullet*>(pObj))
		{
			pBullet->Set_Owner(this);

			_matrix BoneMtx = m_pModelCom->GetBoneMatrix("Alga_F_03") * m_pTransformCom->Get_WorldMatrix();
			_float4x4 fBoneMtx;
			XMStoreFloat4x4(&fBoneMtx, BoneMtx);

			_vector vPrePos = { fBoneMtx.m[3][0], fBoneMtx.m[3][1], fBoneMtx.m[3][2], fBoneMtx.m[3][3] };

			_vector vDest = XMVectorSetW(vTargetPos - vPrePos, 0.f);
			vDest = XMVector3Normalize(XMVectorSetY(vDest, 0.f));

			pBullet->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vPrePos);
			pBullet->Set_ShootDir(vDest);

			pBullet->GetTransform()->LookAt(vTargetPos);
		}

	});
	// ~Event Caller
	
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(90.f));

	m_pASM = CSkmP_AnimInstance::Create(m_pModelCom, this);

	// Socket 
	m_pAtk_Shoot = m_pModelCom->Find_Animation("AS_em0600_204_AL_atk_a3_shot");
	m_pThreat = m_pModelCom->Find_Animation("AS_em0600_160_AL_threat");

	m_pDamage_L_F = m_pModelCom->Find_Animation("AS_em0600_401_AL_damage_l_F");
	m_pDamage_L_B = m_pModelCom->Find_Animation("AS_em0600_402_AL_damage_l_B");

	m_pDamage_M_F = m_pModelCom->Find_Animation("AS_em0600_411_AL_damage_m_F");
	m_pDamage_M_B = m_pModelCom->Find_Animation("AS_em0600_412_AL_damage_m_B");
	m_pDamage_M_L = m_pModelCom->Find_Animation("AS_em0600_413_AL_damage_m_L");
	m_pDamage_M_R = m_pModelCom->Find_Animation("AS_em0600_414_AL_damage_m_R");

	m_pDeadAnim = m_pModelCom->Find_Animation("AS_em0600_424_AL_dead_down");

	return S_OK;
}

void CSkummyPool::BeginTick()
{
	__super::BeginTick();
	m_pASM->AttachAnimSocket("Pool", { m_pModelCom->Find_Animation("AS_em0600_160_AL_threat") });

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat3(&_float3(0.f, 0.f, 36.f)));
}

void CSkummyPool::Tick(_double TimeDelta)
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

	_bool bOnfloor = IsOnFloor();

	// Socket

	if (m_pController->KeyDown(CController::MOUSE_LB))
	{
		m_pASM->AttachAnimSocket("Pool", { m_pAtk_Shoot });
	}

	if (m_pController->KeyDown(CController::R))
	{
		m_pASM->AttachAnimSocket("Pool", { m_pThreat });
	}

	if (m_bStruck || m_pController->KeyDown(CController::Q))
	{
		m_bStruck = false;
		m_pController->ClearCommands();

		if (m_eAtkType == EAttackType::ATK_LIGHT)
		{
			if (m_eHitDir == EBaseAxis::NORTH)
				m_pASM->InputAnimSocket("Pool", { m_pDamage_L_F });

			else
				m_pASM->InputAnimSocket("Pool", { m_pDamage_L_B });
		}

		if (m_eAtkType == EAttackType::ATK_MIDDLE)
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

	m_pTrigger->Update_Tick(m_pTransformCom);

	m_fTurnRemain = m_pController->GetTurnRemain();
	m_vMoveAxis = m_pController->GetMoveAxis();

	m_pASM->Tick(TimeDelta);

	_float fMoveSpeed = 1.f;
	m_vMoveAxis.Normalize();

	const _float fYaw = m_pTransformCom->GetYaw_Radian();
	_float3 vVelocity;
	XMStoreFloat3(&vVelocity, fMoveSpeed * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
	m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
}

void CSkummyPool::Late_Tick(_double TimeDelta)
{
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
	EBaseAxis eHitFrom = CClientUtils::GetDamageFromAxis(m_pTransformCom, tDamageParams.vHitFrom);
	// ↑ 공격이 들어올 방향 
	m_eAtkType = tDamageParams.eAttackType;

	m_eHitDir = eHitFrom;

	if (m_eAtkType == EAttackType::ATK_TO_AIR)
	{
		// 공중 피격 정의 
	}

	else
		m_bStruck = true;
}

void CSkummyPool::AfterPhysX()
{
	__super::AfterPhysX();
	m_pTrigger->Update_AfterPhysX(m_pTransformCom);
}

_bool CSkummyPool::IsPlayingSocket() const
{
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

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
	Safe_Release(m_pTrigger);
}
