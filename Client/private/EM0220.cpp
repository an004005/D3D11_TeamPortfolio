#include "stdafx.h"
#include "EM0220.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "RigidBody.h"
#include "EM0220_AnimInstance.h"
#include "EM0220_Controller.h"
#include "PhysX_Manager.h"
#include "EM0221.h"
#include "ImguiUtils.h"
#include "BulletBuilder.h"
#include "Material.h"

CEM0220::CEM0220(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
	m_eEnemyName = EEnemyName::EM0220;
}

CEM0220::CEM0220(const CEM0220 & rhs)
	: CEnemy(rhs)
{
	m_SpawnEffectPivot = CImguiUtils::CreateMatrixFromImGuizmoData({0.f, 1.5f, 0.f}, {0.f, 0.f, 0.f}, {1.f, 1.f, 1.f});
	m_fSpawnDistortionDistancePivot = 1.f;
}

HRESULT CEM0220::Initialize(void * pArg)
{
	Json em0220_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em0220/em0220Base.json");
	pArg = &em0220_json;

	/*m_strDeathSoundTag = "mon_5_fx_death";
	m_strImpactVoiceTag = "mon_5_impact_voice";*/

	// 배치툴에서 조절할 수 있게 하기
	{
		m_iMaxHP = 1100;
		m_iHP = 1100; // ★

		m_iCrushGauge = 700;
		m_iMaxCrushGauge = m_iCrushGauge;

		m_iAtkDamage = 50;
		iEemeyLevel = 2;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	m_eEnemyName = EEnemyName::EM0220;
	m_bHasCrushGauge = true;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(220.f));

	SetUp_Lantern();

	return S_OK;
}

void CEM0220::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW,
		L"Prototype_Model_em220", L"Model",
		(CComponent**)&m_pModelCom));

	m_pWeak = m_pModelCom->FindMaterial(L"MI_em0220_WEAK_0");

	// 컨트롤러, prototype안 만들고 여기서 자체생성하기 위함
	m_pController = CEM0220_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	// ASM
	m_pASM = CEM0220_AnimInstance::Create(m_pModelCom, this);
}

void CEM0220::SetUpSound()
{
	CEnemy::SetUpSound();
}

void CEM0220::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();

	// Event Caller

	//Attack_Shot 중일때 앞으로 뱉는 모션에서 계속 Bullet생성
	m_pModelCom->Add_EventCaller("Shot", [this]
	{
		Create_Bullet();
	});
}

void CEM0220::SetUpFSM()
{
	CEnemy::SetUpFSM();

	/*
	C : Attack_Shot

	SHIFT : Guard
	*/

	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.OnStart([this]
			{
				m_fGravity = 20.f;
			})
			.AddTransition("Idle to BrainCrushStart", "BrainCrushStart")
				.Predicator([this] { return m_bCrushStart; })
			.AddTransition("Idle to Death", "Death")
				.Predicator([this] { return m_bDead; })

		//얘는 때리면 그냥 넘어져야함
		/*	.AddTransition("Idle to Hit", "Hit")
				.Predicator([this] { return !m_Unbeatable &&
					m_eCurAttackType != EAttackType::ATK_END; })*/

			.AddTransition("Idle to Down", "Down")
				.Predicator([this] { return !m_Unbeatable && m_eCurAttackType != EAttackType::ATK_END; })
			
			.AddTransition("Idle to Attack_Shot", "Attack_Shot")
				.Predicator([this] { return m_eInput == CController::C; })
			.AddTransition("Idle to Guard_Start", "Guard_Start")
				.Predicator([this] { return m_eInput == CController::SHIFT; })
		
///////////////////////////////////////////////////////////////////////////////////////////

		.AddState("Hit")
			.OnStart([this]
			{
				Play_HeavbyHitAnim();
			})
			.AddTransition("Hit_Heavy to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Death")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_424_AL_dead_down");
			})

		.AddState("Down")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_425_AL_down_start");
			})
			.AddTransition("Down to OnFloorGetup", "OnFloorGetup")
				.Predicator([this]
			{
				return m_pASM->isSocketPassby("FullBody", 0.95f);
			})

		.AddState("OnFloorGetup")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketMany("FullBody", { "AS_em0200_426_AL_down", "AS_em0200_427_AL_getup" });
				m_fGravity = 20.f;
			})
			.AddTransition("OnFloorGetup to Idle", "Idle")
				.Predicator([this]
			{
				return m_bDead || m_pASM->isSocketEmpty("FullBody");
			})

		.AddState("BrainCrushStart")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_485_AL_BCchance_start");
			})
			.AddTransition("BrainCrushStart to BrainCrushLoop", "BrainCrushLoop")
				.Predicator([this]
				{
					return m_bBrainCrush || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("BrainCrushLoop")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_486_AL_BCchance_loop");
				m_pModelCom->Find_Animation("AS_em0200_486_AL_BCchance_loop")->SetLooping(true);
			})
			.AddTransition("BrainCrushLoop to BrainCrushEnd", "BrainCrushEnd")
				.Predicator([this]
				{
					return m_bBrainCrush;
				})

		.AddState("BrainCrushEnd")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_487_AL_BCchance_end");
			})
			.Tick([this](_double)
			{
				if (m_pASM->isSocketPassby("FullBody", 0.95f))
					SetDead();
			})
///////////////////////////////////////////////////////////////////////////////////////////

		//Guard loop 중일때 어택이 되면 Guard_End 모션이 끝나고 공격을 하기때문에
		// 공격시작은 무조건 일어서 있는 상태일꺼임
		.AddState("Attack_Shot")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0220_215_AL_Shot2");
				m_pModelCom->Find_Animation("AS_em0220_215_AL_Shot2")->SetLooping(true);
			})
			.Tick([this](_double TimeDelta) 
			{
				_vector vTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				m_pTransformCom->LookAt_Smooth(vTargetPos, TimeDelta);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Attack_Shot to Idle", "Idle")
				.Predicator([this]
				{
				//루프 모션일때는 컨트롤러가 작동하지 않기때문에 여기서 위치를 계산해줘야함
					return m_bDead || m_pController->Check_TargetDist();
				})
		
///////////////////////////////////////////////////////////////////////////////////////////
		
		// FIXME : count 실행조건이 기본 공격이랑 키네틱 둘다 실행되는데 아니면 나중에 수정!


		//Guard start 모션이 끝나면 Loop 상태, 
		//loop로 가는 도중 때릴수 있으니 Count로 바로 가게 해둠
		.AddState("Guard_Start")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0220_111_AL_to_guard");
				m_pModelCom->Find_Animation("AS_em0220_111_AL_to_guard")->SetTickPerSec(10.f);
				m_Unbeatable = false;
				
			})
			.Tick([this](_double TimeDelta)
			{
					if (!m_Unbeatable && m_pModelCom->Find_Animation("AS_em0220_111_AL_to_guard")->GetPlayTime() >= 3.f)
					{
						m_pModelCom->Find_Animation("AS_em0220_111_AL_to_guard")->SetTickPerSec(60.f);
						m_Unbeatable = true;
					}
			})

			.AddTransition("Guard_Start to Guard_Loop", "Guard_Loop")
				.Predicator([this]
				{
					return m_bDead || (m_Unbeatable &&m_pASM->isSocketPassby("FullBody", 0.95f));
				})

			.AddTransition("Guard_Start to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || (!m_Unbeatable && m_eCurAttackType != EAttackType::ATK_END);
				})

		// near 에서 멀어지면 일어서는건데 무조건 공격으로 변하기 때문에 이 조건이면 될것같음
		.AddState("Guard_Loop")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0220_112_AL_guard");
				m_pModelCom->Find_Animation("AS_em0220_112_AL_guard")->SetLooping(true);
				
			})
			.AddTransition("Guard_Loop to Guard_End", "Guard_End")
				.Predicator([this]
				{		
					return m_bDead || m_pController->Check_TargetDist(true); 
				})
			.AddTransition("Guard_Loop to Counter", "Counter")
				.Predicator([this]
				{
					return m_eCurAttackType != EAttackType::ATK_END;
				})

		.AddState("Counter")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0220_217_AL_Counter");
				m_dFallCount = 0.0;
			})
			.Tick([this](_double TimeDelta)
			{
				//폭탄 떨구기
				CounterAttack(TimeDelta);
			})
			.AddTransition("Counter to Guard_End", "Guard_End")
				.Predicator([this]
			{
				return m_bDead || (m_pController->Check_TargetDist(true) && m_pASM->isSocketPassby("FullBody", 0.95f));
			})
			.AddTransition("Counter to Guard_Loop", "Guard_Loop")
				.Predicator([this]
			{
				return m_bDead || (m_pController->Check_TargetDist() && m_pASM->isSocketPassby("FullBody", 0.95f));
			})

		.AddState("Guard_End")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0220_113_AL_to_wait03");
			})
			.OnExit([this]
			{
				m_Unbeatable = false;
			})
			.AddTransition("Guard_End to Idle", "Idle")
				.Predicator([this]
			{
				return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
			})
			
///////////////////////////////////////////////////////////////////////////////////////////

		.Build();
}

void CEM0220::BeginTick()
{
	CEnemy::BeginTick();
}

void CEM0220::Tick(_double TimeDelta)
{
	CEnemy::Tick(TimeDelta);

	//AIInstance tick
	m_pController->SetTarget(m_pTarget);

	if (m_bDead == false)
		m_pController->Tick(TimeDelta);
	else
		m_pController->Invalidate();

	//변수 업데이트
	m_fTurnRemain = m_pController->GetTurnRemain();
	m_eInput = m_pController->GetAIInput();

	//m_bTeleport = CheckSASType(ESASType::SAS_TELEPORT);

	//ASM, FSM tick
	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);

	for (auto it : m_pLanterns)
		it->Tick(TimeDelta);

	Update_LanternMatrix();
	HitWeakProcess(TimeDelta);
	ResetHitData();
}

void CEM0220::Late_Tick(_double TimeDelta)
{
	CEnemy::Late_Tick(TimeDelta);

	for (auto it : m_pLanterns)
		it->Late_Tick(TimeDelta);
}

void CEM0220::AfterPhysX()
{
	CEnemy::AfterPhysX();
}

HRESULT CEM0220::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CEM0220::Imgui_RenderProperty()
{
	CEnemy::Imgui_RenderProperty();
	if (ImGui::CollapsingHeader("ASM"))
	{
		m_pASM->Imgui_RenderState();
	}

	m_pFSM->Imgui_RenderProperty();
}

_bool CEM0220::IsWeak(CRigidBody* pHitPart)
{
	return true;
}

void CEM0220::HitEffect(DAMAGE_PARAM& tDamageParams)
{
	if (m_Unbeatable == true) return;
	
	__super::HitEffect(tDamageParams);
}

void CEM0220::CheckHP(DAMAGE_PARAM & tDamageParams)
{
	_int iDamage = tDamageParams.iDamage;
	
	if(m_Unbeatable == false)
		m_iHP -= iDamage;

	if (m_iHP < 0)
	{
		SetDead();
		m_iHP = 0;
	}
}

void CEM0220::SetUp_Lantern()
{
	//lantern01~13
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	for (_uint i = 1; i <= 13; ++i)
	{
		string boneName = "";

		if (i < 10)
			boneName = "lantern0" + to_string(i);
		else
			boneName = "lantern" + to_string(i);

		CEM0221* pLantern = dynamic_cast<CEM0221*>(pGameInstance->Clone_GameObject_NoLayer(LEVEL_NOW, TEXT("Monster_em221")));
		//CEM0221* pLantern = dynamic_cast<CEM0221*>(pGameInstance->Clone_GameObject_Get(TEXT("Layer_Lantern"), TEXT("Monster_em221")));

		assert(pLantern != nullptr);

		pLantern->Set_TargetBoneName(boneName);
		m_pLanterns.emplace_back(pLantern);
	}

}

_bool CEM0220::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}


void CEM0220::Play_HeavbyHitAnim()
{
	if (m_eSimpleHitFrom == ESimpleAxis::NORTH)
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_401_AL_damage_l_F");
	else
		m_pASM->InputAnimSocketOne("FullBody", "AS_em0200_402_AL_damage_l_B");
}

void CEM0220::Update_LanternMatrix()
{
	for (auto it : m_pLanterns)
		it->SetUpMatrix(m_pModelCom, m_pTransformCom->Get_WorldMatrix());
}

void CEM0220::CounterAttack(_double TimeDelta)
{
	if (m_dFallCount >= 0.1)
	{
		m_iFallIndex = (m_iFallIndex + 7) % m_pLanterns.size();
		m_pLanterns[m_iFallIndex]->Set_Fall();
		m_dFallCount = 0.0;
		return;
	}
	
	m_dFallCount += TimeDelta;
}

void CEM0220::Create_Bullet()
{
	DAMAGE_PARAM eDamageParam;


	eDamageParam.eAttackType = EAttackType::ATK_END;
	eDamageParam.eDeBuff = EDeBuffType::DEBUFF_END;
	eDamageParam.iDamage = 50;

	_vector vBonePos = (m_pModelCom->GetBoneMatrix("Target") * m_pTransformCom->Get_WorldMatrix()).r[3];
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vBonePos += XMVector3Normalize(vLook) * 3.f;

	_vector vTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vUp = m_pTarget->GetTransform()->Get_State(CTransform::STATE_UP);
	vTargetPos += XMVector3Normalize(vUp) * 0.5f;

	CBulletBuilder()
		.CreateBullet()
			.Set_Owner(this)
			.Set_InitBulletEffect({ L"em0650_Bullet_Birth", L"em0220_Bullet" })
			.Set_ShootSpeed(22.f)
			.Set_Life(2.f)
			.Set_DamageParam(eDamageParam)
			.Set_DeadBulletEffect({ L"em0320_Bullet_Dead_1", L"em0320_Bullet_Dead_2", L"em0320_Bullet_Dead_3" })
			.Set_Position(vBonePos)
			.Set_LookAt(vTargetPos)
		.Build();
}

void CEM0220::HitWeakProcess(_double TimeDelta)
{
	if (m_bHitWeak)
	{
		m_bWeakProcess = true;
		m_pWeak->GetParam().Floats[1] = 1.f;
	}

	//1번째가 약점 맞았을때 반짝거리는거
	//2번째가 아머 삭제될때 디졸브
	if (m_bWeakProcess)
	{
		m_pWeak->GetParam().Floats[1] -= static_cast<_float>(TimeDelta);

		if (m_pWeak->GetParam().Floats[1] <= 0.f)
		{
			m_pWeak->GetParam().Floats[1] = 0.f;
			m_bWeakProcess = false;
		}
	}

}




CEM0220 * CEM0220::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEM0220* pInstance = new CEM0220(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM0220");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEM0220::Clone(void * pArg)
{
	CEM0220*		pInstance = new CEM0220(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM0220");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM0220::Free()
{
	CEnemy::Free();
	Safe_Release(m_pASM);
	Safe_Release(m_pController);

	for (auto it : m_pLanterns)
		Safe_Release(it);

	m_pLanterns.clear();
}
