#include "stdafx.h"
#include "..\public\Boss1.h"

#include <PhysX_Manager.h>

#include "Model.h"
#include "JsonStorage.h"
#include "Boss1_AnimationInstance.h"
#include "GameInstance.h"
#include "Boss1_AIController.h"
#include "Player.h"
#include "JsonStorage.h"
#include "RigidBody.h"

CBoss1::CBoss1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CBoss1::CBoss1(const CBoss1& rhs)
	: CMonster(rhs)
{
}

HRESULT CBoss1::Initialize_Prototype()
{
	return CMonster::Initialize_Prototype();
}

HRESULT CBoss1::Initialize(void* pArg)
{
	Json BossJson = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/Boss1_en320/Boss1_en320.json");
	pArg = &BossJson;

	FAILED_CHECK(CMonster::Initialize(pArg));

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"MonsterBoss1", L"Model", (CComponent**)&m_pModelCom));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_MonsterBoss1_Controller"), TEXT("Com_Controller"), (CComponent**)&m_pController));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Com_Weak"), (CComponent**)&m_pWeak, pArg));


	m_fGravity = 25.f;

	m_pModelCom->Add_EventCaller("JumpAttackStart", [this]
	{
		//start damage
		m_fYSpeed = 17.f;
		m_bJumpAttack = true;

		if (m_pTarget)
		{
			_vector vOrigin = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			vOrigin = XMVectorSetY(vOrigin, 0.f);
			_vector vDest = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
			vDest = XMVectorSetY(vDest, 0.f);
			m_fJumpMoveTime = (2 * m_fYSpeed) / m_fGravity;

			const _vector vDiff = vDest - vOrigin;
			const _float fDistance = XMVectorGetX(XMVector3LengthEst(vDiff));
			const _vector vDirection = XMVector3NormalizeEst(vDiff);

			m_vOnJumpMoveVelocity = vDirection * (fDistance / m_fJumpMoveTime);
		}
		else
		{
			m_fJumpMoveTime = 0.f;
			m_vOnJumpMoveVelocity = _float3::Zero;
		}
	});
	m_pModelCom->Add_EventCaller("Jitabata", [this]
	{
		++m_iJitabaCnt;
		ClearDamagedTarget(); // 매 jitabata마다 초기화해서 다시 공격 가능
		if (m_iJitabaCnt == 5)
		{
			m_bJumpAttack = false;
			End_AttackState();
		}
	});

	m_pModelCom->Add_EventCaller("Start_SweepRight", [this]
	{
		Start_AttackState(RIGHT_SWEEP);
	});
	m_pModelCom->Add_EventCaller("Start_SweepLeft", [this]
	{
		Start_AttackState(LEFT_SWEEP);
	});
	m_pModelCom->Add_EventCaller("Start_Spin", [this]
	{
		Start_AttackState(SPIN);
	});
	m_pModelCom->Add_EventCaller("AttackEnd", [this]
	{
		End_AttackState();
	});


	m_pTransformCom->SetRotPerSec(XMConvertToRadians(100.f));

	m_pASM = CBoss1_AnimationInstance::Create(m_pModelCom, this);
	
	m_pAtk_R = m_pModelCom->Find_Animation("AS_em0300_202_AL_atk_a2_R");
	m_pAtk_L = m_pModelCom->Find_Animation("AS_em0300_201_AL_atk_a1_L");
	m_pAtk_Spin = m_pModelCom->Find_Animation("AS_em0320_225_AL_atk7_spin");
	m_pAtk_WaterBall = m_pModelCom->Find_Animation("AS_em0320_228_AL_atk8_waterball_F");
	m_pMiddleDown = m_pModelCom->Find_Animation("AS_em0300_415_AL_damage_m_F");
	m_pDownStart = m_pModelCom->Find_Animation("AS_em0300_425_AL_down_start");
	m_pDown = m_pModelCom->Find_Animation("AS_em0300_426_AL_down");

	m_pJumpStart = m_pModelCom->Find_Animation("AS_em0300_204_AL_atk_a3_start");
	m_pJumpEnd = m_pModelCom->Find_Animation("AS_em0300_207_AL_atk_a3_end");
	m_pJumpLand = m_pModelCom->Find_Animation("AS_em0300_205_AL_atk_a3_landing");
	m_pJumpJitabata = m_pModelCom->Find_Animation("AS_em0300_233_AL_atk_a3_jitabata_loop");
		
	return S_OK;
}

void CBoss1::BeginTick()
{
	CMonster::BeginTick();
	m_pASM->AttachAnimSocket("FullBody", { m_pModelCom->Find_Animation("AS_em0300_160_AL_threat") });
}

void CBoss1::Tick(_double TimeDelta)
{
	CMonster::Tick(TimeDelta);

	// 타겟 가져오기 임시 코드
	auto pPlayer = CGameInstance::GetInstance()->Find_ObjectByPredicator(LEVEL_NOW, [this](CGameObject* pObj)
   {
	   return dynamic_cast<CPlayer*>(pObj) != nullptr;
   });
	m_pTarget = dynamic_cast<CScarletCharacter*>(pPlayer);


	m_pController->SetTarget(m_pTarget);

	m_pController->Tick(TimeDelta);

	if (m_fJumpMoveTime > 0.f)
	{
		m_pTransformCom->MoveVelocity(TimeDelta, m_vOnJumpMoveVelocity);
		m_fJumpMoveTime -= (_float)TimeDelta;
	}

	if (m_pController->KeyDown(CController::MOUSE_LB))
	{
		const _vector vLookAt = pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		EBaseTurn eTurn = CClientUtils::TurnDeltaToEnum(m_fTurnRemain);

		if (eTurn == EBaseTurn::TURN_LEFT)
			m_pASM->AttachAnimSocket("FullBody", { m_pAtk_L });
		else
			m_pASM->AttachAnimSocket("FullBody", { m_pAtk_R });
	}
	if (m_pController->KeyDown(CController::G))
	{
		m_pASM->AttachAnimSocket("FullBody", { m_pAtk_Spin });
	}
	if (m_pController->KeyDown(CController::MOUSE_RB))
	{
		m_pASM->AttachAnimSocket("FullBody", { m_pAtk_WaterBall });
	}
	if (m_pController->KeyDown(CController::E))
	{
		Start_AttackState(JUMP);
		m_pASM->AttachAnimSocket("FullBody", { m_pJumpStart, m_pJumpLand, m_pJumpJitabata, m_pJumpJitabata, m_pJumpJitabata, m_pJumpJitabata, m_pJumpJitabata, m_pJumpEnd });
	}

	if (m_bDown || m_pController->KeyDown(CController::C))
	{
		Reset();
		m_bDown = false;
		m_pASM->AttachAnimSocket("FullBody", { m_pDownStart, m_pDown, m_pJumpEnd  });
	}
	if (m_bMiddleDown || m_pController->KeyDown(CController::Q))
	{
		if (m_bJumpAttack == false) // jump 어택중에는 경직 무시
		{
			Reset();
			m_bMiddleDown = false;
			m_pASM->AttachAnimSocket("FullBody", { m_pMiddleDown });
		}
	}

	m_fTurnRemain = m_pController->GetTurnRemain();
	m_vMoveAxis = m_pController->GetMoveAxis();
	m_pASM->Tick(TimeDelta);

	const _matrix WeakBoneMatrix = m_pModelCom->GetBoneMatrix("Water") * m_pTransformCom->Get_WorldMatrix();
	m_pWeak->Update_Tick(WeakBoneMatrix);
	Tick_AttackState();
}

void CBoss1::Late_Tick(_double TimeDelta)
{
	CMonster::Late_Tick(TimeDelta);
	if (m_bVisible)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

void CBoss1::AfterPhysX()
{
	CMonster::AfterPhysX();
}

HRESULT CBoss1::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CBoss1::Imgui_RenderProperty()
{
	CMonster::Imgui_RenderProperty();
	m_pASM->Imgui_RenderState();
}

_bool CBoss1::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

void CBoss1::Start_AttackState(CBoss1_AttackStateType eState)
{
	m_eAttackType = eState;
	switch (m_eAttackType)
	{
	case LEFT_SWEEP:
		{
			const _matrix BoneMatrix = m_pModelCom->GetBoneMatrix("LeftHandMiddle1") * m_pTransformCom->Get_WorldMatrix();
			m_vSweepPrePos = BoneMatrix.r[3];
		}
		break;
	case RIGHT_SWEEP:
		{
			const _matrix BoneMatrix = m_pModelCom->GetBoneMatrix("RightHandMiddle1") * m_pTransformCom->Get_WorldMatrix();
			m_vSweepPrePos = BoneMatrix.r[3];
		}
		break;
	case SPIN:
		break;
	case JUMP: 
		break;
	case STATE_END: 
		break;
	default: 
		NODEFAULT;
	}
}

void CBoss1::Tick_AttackState()
{
	_float4 vAttackPos;
	switch (m_eAttackType)
	{
	case LEFT_SWEEP:
		{
			const _matrix BoneMatrix = m_pModelCom->GetBoneMatrix("LeftHandMiddle1") * m_pTransformCom->Get_WorldMatrix();
			vAttackPos = BoneMatrix.r[3];
		}
		break;
	case RIGHT_SWEEP:
		{
			const _matrix BoneMatrix = m_pModelCom->GetBoneMatrix("RightHandMiddle1") * m_pTransformCom->Get_WorldMatrix();
			vAttackPos = BoneMatrix.r[3];
		}
		break;
	case SPIN:
		{
			SphereOverlapParams tParams;
			physx::PxOverlapHit hitBuffer[3];
			physx::PxOverlapBuffer overlapOut(hitBuffer, 3);
			tParams.fRadius = 3.f;
			tParams.fVisibleTime = 0.1f;
			tParams.iTargetType = CTB_PLAYER;
			tParams.overlapOut = &overlapOut;
			tParams.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			if (CGameInstance::GetInstance()->OverlapSphere(tParams))
			{
				HitTargets(overlapOut, 1, EAttackType::ATK_HEAVY);
			}
		}
		break;
	case JUMP:
		{
			SphereOverlapParams tParams;
			physx::PxOverlapHit hitBuffer[3];
			physx::PxOverlapBuffer overlapOut(hitBuffer, 3);
			tParams.fRadius = 3.f;
			tParams.fVisibleTime = 0.1f;
			tParams.iTargetType = CTB_PLAYER;
			tParams.overlapOut = &overlapOut;
			tParams.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			if (CGameInstance::GetInstance()->OverlapSphere(tParams))
			{
				HitTargets(overlapOut, 1, EAttackType::ATK_HEAVY);
			}
		}
		break;
	case STATE_END: 
		break;
	default: 
		NODEFAULT;
	}

	if (m_eAttackType == LEFT_SWEEP || m_eAttackType == RIGHT_SWEEP)
	{
		SphereSweepParams tParams;

		physx::PxSweepHit hitBuffer[3];
		physx::PxSweepBuffer sweepOut(hitBuffer, 3);
		tParams.sweepOut = &sweepOut;
		tParams.fRadius = 1.f;
		tParams.fVisibleTime = .5f;
		tParams.iTargetType = CTB_PLAYER;
		tParams.vPos = m_vSweepPrePos;
		const _vector vDiff = vAttackPos - m_vSweepPrePos;
		tParams.vUnitDir = XMVector3NormalizeEst(vDiff);
		tParams.fDistance = XMVectorGetX(XMVector3LengthEst(vDiff));

		if (CGameInstance::GetInstance()->SweepSphere(tParams))
		{
			HitTargets(sweepOut, 1, EAttackType::ATK_HEAVY);
		}

		m_vSweepPrePos = vAttackPos;
	}
}

void CBoss1::End_AttackState()
{
	m_iJitabaCnt = 0;
	ClearDamagedTarget();
	m_eAttackType = STATE_END;
}

void CBoss1::Reset()
{
	CMonster::Reset();
	End_AttackState();
	m_pController->ClearCommands();
}

CBoss1* CBoss1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss1* pInstance = new CBoss1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBoss1");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBoss1::Clone(void* pArg)
{
	CBoss1*		pInstance = new CBoss1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss1");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss1::Free()
{
	CMonster::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
	Safe_Release(m_pWeak);
}
