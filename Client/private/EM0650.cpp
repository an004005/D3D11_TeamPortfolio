#include "stdafx.h"
#include "EM0650.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "RigidBody.h"
#include "EM0650_AnimInstance.h"
#include "EM0650_Controller.h"
CEM0650::CEM0650(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
}

CEM0650::CEM0650(const CEM0650 & rhs)
	: CEnemy(rhs)
{
}

HRESULT CEM0650::Initialize(void * pArg)
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

void CEM0650::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em700", L"Model", (CComponent**)&m_pModelCom));

	// 범위 충돌체(플레이어가 몬스터 위로 못 올라가게한다.)
	/*Json FlowerLegRangeCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/FlowerLeg/FlowerLegRange.json");
	FAILED_CHECK(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("RangeCollider"),
		(CComponent**)&m_pRange, &FlowerLegRangeCol))
*/

	// 컨트롤러, prototype안 만들고 여기서 자체생성하기 위함
	m_pController = CEM0650_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	// ASM
	m_pASM = CEM0650_AnimInstance::Create(m_pModelCom, this);
}

void CEM0650::SetUpSound()
{
	CEnemy::SetUpSound();
}

void CEM0650::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();
}

void CEM0650::SetUpFSM()
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
			.AddTransition("Idle to Escape", "Escape")
				.Predicator([this] { return m_eInput == CController::SPACE; })
			.AddTransition("Idle to Threat", "Threat")
				.Predicator([this] { return m_eInput == CController::C; })
			.AddTransition("Idle to Attack_a1_Start", "Attack_a1_Start")
				.Predicator([this] { return m_eInput == CController::NUM_1; })
			.AddTransition("Idle to Attack_a3", "Attack_a3")
				.Predicator([this] { return m_eInput == CController::NUM_2; })
			.AddTransition("Idle to RandomMove_L", "RandomMove_L")
				.Predicator([this] { return m_eInput == CController::MOUSE_LB; })
			.AddTransition("Idle to RandomMove_R", "RandomMove_R")
				.Predicator([this] { return m_eInput == CController::MOUSE_RB; })

///////////////////////////////////////////////////////////////////////////////////////////
		.AddState("Attack_a1_Start")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_201_AL_atk_a1_start");
			})
			.AddTransition("Attack_a1_Start to Attack_a1_Loop", "Attack_a1_Loop")
				.Predicator([this]
			{
				return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
			})
		.AddState("Attack_a1_Loop")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_202_AL_atk_a1_loop");
				m_vDirection = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				m_fDistance = 0.f;
			})
			.Tick([this](_double TimeDelta)
			{
				Attack_a1_Loop(TimeDelta);
			})
			.AddTransition("Attack_a1_Loop to Attack_a1_End", "Attack_a1_End")
				.Predicator([this]
			{
				return m_bDead || m_fDistance >= 15.f;
			})
		.AddState("Attack_a1_End")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_203_AL_atk_a1_end");
			})
			.AddTransition("Attack_a1_End to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})
///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Attack_a3")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0710_207_AL_atk_a3");
			})
				.AddTransition("AS_em0710_207_AL_atk_a3 to Idle", "Idle")
				.Predicator([this]
			{
				return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
			})
///////////////////////////////////////////////////////////////////////////////////////////
		.AddState("Threat")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_160_AL_threat");
			})
			.AddTransition("Threat to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})


///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Escape")
			.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_120_AL_escape");
				})
					.AddTransition("Escape to Idle", "Idle")
					.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})
///////////////////////////////////////////////////////////////////////////////////////////
		.AddState("RandomMove_L")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_121_AL_randommove_L");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.AddTransition("RandomMove_L to Idle", "Idle")
				.Predicator([this]
			{
				return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
			})
		.AddState("RandomMove_R")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em0700_122_AL_randommove_R");
				})
				.Tick([this](_double)
				{
					SocketLocalMove(m_pASM);
				})
					.AddTransition("RandomMove_R to Idle", "Idle")
					.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})
		.Build();
}

void CEM0650::BeginTick()
{
	CEnemy::BeginTick();
}

void CEM0650::Tick(_double TimeDelta)
{
	CEnemy::Tick(TimeDelta);

	//AIInstance tick
	m_pController->SetTarget(m_pTarget);

	if (m_bDead == false)
		m_pController->Tick(TimeDelta);
	else
		m_pController->Invalidate();

	//변수 업데이트
	m_eInput = m_pController->GetAIInput();


	//ASM, FSM tick
	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);
}

void CEM0650::Late_Tick(_double TimeDelta)
{
	CEnemy::Late_Tick(TimeDelta);
}

void CEM0650::AfterPhysX()
{
	CEnemy::AfterPhysX();
}

HRESULT CEM0650::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CEM0650::Imgui_RenderProperty()
{
	CEnemy::Imgui_RenderProperty();
	if (ImGui::CollapsingHeader("ASM"))
	{
		m_pASM->Imgui_RenderState();
	}

	ImGui::InputFloat("Distance", &m_fDistance);

	m_pFSM->Imgui_RenderProperty();
}

_bool CEM0650::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

void CEM0650::Attack_a1_Loop(_double TimeDelta)
{	
	_vector vTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	m_fDistance += m_pTransformCom->RushToTarget(m_vDirection, TimeDelta);
}

CEM0650 * CEM0650::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEM0650* pInstance = new CEM0650(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0650");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEM0650::Clone(void * pArg)
{
	CEM0650*		pInstance = new CEM0650(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0650");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0650::Free()
{
	CEnemy::Free();
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
	Safe_Release(m_pRange);
}
