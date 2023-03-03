#include "stdafx.h"
#include "..\public\Boss1.h"
#include "Model.h"
#include "JsonStorage.h"
#include "Boss1_AnimationInstance.h"
#include "FSMComponent.h"
#include "GameInstance.h"
#include "Boss1_AIController.h"
#include "Player.h"

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
	FAILED_CHECK(CMonster::Initialize(pArg));

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"MonsterBoss1", L"Model", (CComponent**)&m_pModelCom));
	m_pModelCom->ActivateLocalRotation(true);

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_MonsterBoss1_Controller"), TEXT("Com_Controller"), (CComponent**)&m_pController));
	// FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Component_TestCamera"), TEXT("Com_Controller"), (CComponent**)&m_pController));


	m_pTransformCom->SetRotPerSec(XMConvertToRadians(100.f));

	m_pASM = CBoss1_AnimationInstance::Create(m_pModelCom, this);
	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.AddTransition("Idle to MeleeAttack", "MeleeAttack")
				.Predicator([this]
				{
					return m_bAttack;
				})
			.AddTransition("Idle to RangeAttack", "RangeAttack")
				.Predicator([this]
				{
					return m_bRange;
				})
			.AddTransition("Idle to JumpAttack", "JumpAttack")
				.Predicator([this]
				{
					return m_bJump;
				})
			.AddTransition("Idle to Down", "Down")
				.Predicator([this]
				{
					return m_bDown;
				})
			.AddTransition("Idle to MiddleDown", "MiddleDown")
				.Predicator([this]
				{
					return m_bMiddleDown;
				})
			
		.AddState("Move")
		.AddState("MeleeAttack")
			.OnStart([this]
			{
				// todo 플레이어 위치에 따라 공격 정하기
				m_bAttack = false;
				m_pASM->AttachAnimSocket("FullBody", { m_pAtk_R });
			})
			.AddTransition("MeleeAttack to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDown || m_bMiddleDown || m_pASM->isSocketEmpty("FullBody");
				})
			
		.AddState("RangeAttack")
			.OnStart([this]
			{
				m_bRange = false;
				m_pASM->AttachAnimSocket("FullBody", { m_pAtk_WaterBall });
			})
			.AddTransition("RangeAttack to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDown || m_bMiddleDown || m_pASM->isSocketEmpty("FullBody");
				})

		.AddState("JumpAttack")
			.OnStart([this]
			{
				m_bJump = false;
				m_pASM->AttachAnimSocket("FullBody", { m_pJumpStart, m_pJumpLand, m_pJumpJitabata, m_pJumpJitabata, m_pJumpJitabata, m_pJumpJitabata, m_pJumpJitabata, m_pJumpEnd });
			})
			.AddTransition("JumpAttack to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDown || m_pASM->isSocketEmpty("FullBody");
				})

		.AddState("Down")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocket("FullBody", { m_pDownStart, m_pDown, m_pJumpEnd  });
				m_bDown = false;
			})
			.AddTransition("Down to Idle", "Idle")
				.Predicator([this]
				{
					return m_pASM->isSocketEmpty("FullBody");
				})

		.AddState("MiddleDown")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocket("FullBody", { m_pMiddleDown });
				m_bMiddleDown = false;
			})
			.AddTransition("MiddleDown to Idle", "Idle")
				.Predicator([this]
				{
					return m_pASM->isSocketEmpty("FullBody");
				})
		.Build();
	
	m_pAtk_R = m_pModelCom->Find_Animation("AS_em0300_202_AL_atk_a2_R");
	m_pAtk_L = m_pModelCom->Find_Animation("AS_em0300_201_AL_atk_a1_L");
	m_pAtk_Spin = m_pModelCom->Find_Animation("AS_em0320_225_AL_atk7_spin");
	m_pAtk_WaterBall = m_pModelCom->Find_Animation("AS_em0320_228_AL_atk8_waterball_F");
	m_pThreat = m_pModelCom->Find_Animation("AS_em0300_160_AL_threat");
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
	m_pModelCom->SetPlayAnimation("AS_em0300_101_AL_wait01");

	// m_pASM->AttachAnimSocket("FullBody", { m_pThreat });
}

void CBoss1::Tick(_double TimeDelta)
{
	CMonster::Tick(TimeDelta);

	auto pPlayer = CGameInstance::GetInstance()->Find_ObjectByPredicator(LEVEL_NOW, [this](CGameObject* pObj)
	{
		return dynamic_cast<CPlayer*>(pObj) != nullptr;
	});
	m_pController->SetTarget(dynamic_cast<CScarletCharacter*>(pPlayer));

	m_pController->Tick(TimeDelta);


	if (m_pController->KeyDown(CController::MOUSE_LB))
	{
		m_bAttack = true;
	}
	if (m_pController->KeyDown(CController::MOUSE_RB))
	{
		m_bRange = true;
	}
	if (m_pController->KeyDown(CController::C))
	{
		m_bDown = true;
	}
	if (m_pController->KeyDown(CController::G))
	{
		m_bRange = true;
	}
	if (m_pController->KeyDown(CController::Q))
	{
		m_bMiddleDown = true;
	}

	m_fTurnRemain = m_pController->GetTurnRemain();
	m_vMoveAxis = m_pController->GetMoveAxis();
	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);



	// _vector vCamLook = XMVectorSetY(m_pCam->GetTransform()->Get_State(CTransform::STATE_LOOK), 0.f) ;
	//
	// const char* pStateName = m_pFSM->GetCurStateName();
	// if (0 == strcmp(pStateName, "Move") || 0 == strcmp(pStateName, "Idle"))
	// {
	// 	_vector vLookAt = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + XMVector3Normalize(vCamLook) * 5.f;
	// 	m_fTurnRemain = m_pTransformCom->LookAt_SmoothYaw(vLookAt, TimeDelta);
	// }
	// else if (0 == strcmp(pStateName, "RangeAttack"))
	// {
	// 	_vector vLookAt = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + XMVector3Normalize(vCamLook) * 5.f;
	// 	m_fTurnRemain = m_pTransformCom->LookAt_SmoothYaw(vLookAt, TimeDelta * 0.2f);
	// }
}

void CBoss1::Late_Tick(_double TimeDelta)
{
	CMonster::Late_Tick(TimeDelta);
	if (m_bVisible)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	// m_pCam->AttachTo(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
}

HRESULT CBoss1::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CBoss1::Imgui_RenderProperty()
{
	CMonster::Imgui_RenderProperty();
	m_pFSM->Imgui_RenderProperty();
	m_pASM->Imgui_RenderState();
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
	Safe_Release(m_pFSM);
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
}
