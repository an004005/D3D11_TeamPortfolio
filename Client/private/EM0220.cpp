#include "stdafx.h"
#include "EM0220.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "RigidBody.h"
#include "EM0220_AnimInstance.h"
#include "EM0220_Controller.h"
#include "EnemyBullet.h"
#include "PhysX_Manager.h"
#include "EM0221.h"
#include "ImguiUtils.h"

CEM0220::CEM0220(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
	m_eMonsterName = EEnemyName::EM0220;
}

CEM0220::CEM0220(const CEM0220 & rhs)
	: CEnemy(rhs)
{
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

		m_iAtkDamage = 50;
		iMonsterLevel = 2;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	m_eMonsterName = EEnemyName::EM0220;
	m_bHasCrushGage = true;
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

	//Create Collider
	//FAILED_CHECK(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("TailCol"),
	//	(CComponent**)&m_pTailCol, pArg));

	//FAILED_CHECK(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("WeaponCol"),
	//	(CComponent**)&m_pWeaponCol, pArg));

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
		auto pObj = CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_Bullet"), TEXT("EnemyBullet"));

		if (CEnemyBullet* pBullet = dynamic_cast<CEnemyBullet*>(pObj))
		{
			pBullet->Set_Owner(this);

			_vector vBonePos = (m_pModelCom->GetBoneMatrix("Target") * m_pTransformCom->Get_WorldMatrix()).r[3];
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

			//입에 달려있는 뼈가 없어서 몬스터 입 뒤쪽에있는 Target뼈에서 앞쪽으로 당겨준다.
			vBonePos += XMVector3Normalize(vLook) * 3.f;
			pBullet->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vBonePos);
			pBullet->GetTransform()->LookAt(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)); 
		}
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
			.AddTransition("Idle to Death", "Death")
				.Predicator([this] { return m_bDead; })

			.AddTransition("Idle to Hit", "Hit")
				.Predicator([this] { return m_eCurAttackType == EAttackType::ATK_LIGHT || m_eCurAttackType == EAttackType::ATK_MIDDLE; })
			.AddTransition("Idle to Down", "Down")
				.Predicator([this] { return m_eCurAttackType == EAttackType::ATK_HEAVY; })
			
			.AddTransition("Idle to Attack_Shot", "Attack_Shot")
				.Predicator([this] { return m_eInput == CController::C; })
			.AddTransition("Idle to Guard_Start", "Guard_Start")
				.Predicator([this] { return m_eInput == CController::SHIFT || m_bTeleport == false; })
		
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
				m_pASM->InputAnimSocketMany("FullBody", { "AS_em0200_426_AL_down" "AS_em0200_427_AL_getup" });
				m_fGravity = 20.f;
			})
			.AddTransition("OnFloorGetup to Idle", "Idle")
				.Predicator([this]
			{
				return m_bDead || m_pASM->isSocketEmpty("FullBody");
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
			//공격중 가까이오면 Guard상태로 감
			.AddTransition("Attack_Shot to Guard_Start", "Guard_Start")
				.Predicator([this]
				{
				//루프 모션일때는 컨트롤러가 작동하지 않기때문에 여기서 위치를 계산해줘야함
					return m_pController->Check_TargetDist(10.f);
				})
			//순간이동이나, 키네틱으로 맞았을때는 Idle상태로 보내서 hit모션 뜨게 함
			.AddTransition("Attack_Shot to Idle", "Idle" )
				.Predicator([this]
				{
					return m_bDead || m_eCurAttackType != EAttackType::ATK_END;
				})

///////////////////////////////////////////////////////////////////////////////////////////
		
		// FIXME : count 실행조건이 기본 공격이랑 키네틱 둘다 실행되는데 아니면 나중에 수정!


		//Guard start 모션이 끝나면 Loop 상태, 
		//loop로 가는 도중 때릴수 있으니 Count로 바로 가게 해둠
		.AddState("Guard_Start")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0220_111_AL_to_guard");
			})
			.Tick([this](_double)
			{
			
			})
			.AddTransition("Guard_Start to Guard_Loop", "Guard_Loop")
				.Predicator([this]
				{
					return m_bDead || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		// near 에서 멀어지면 일어서는건데 무조건 공격으로 변하기 때문에 이 조건이면 될것같음
		.AddState("Guard_Loop")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0220_112_AL_guard");
				m_pModelCom->Find_Animation("AS_em0220_112_AL_guard")->SetLooping(true);
				m_IsBeat = true;
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
				m_IsBeat = false;
			})
			.AddTransition("Guard_Loop to Guard_End", "Guard_End")
				.Predicator([this]
				{		
					return m_bDead || m_pController->Check_TargetDist(10.f, true); 
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
				return m_bDead || (m_pController->Check_TargetDist(10.f, true) && m_pASM->isSocketPassby("FullBody", 0.95f));
			})
			.AddTransition("Counter to Guard_Loop", "Guard_Loop")
				.Predicator([this]
			{
				return m_bDead || (m_pController->Check_TargetDist(10.f) && m_pASM->isSocketPassby("FullBody", 0.95f));
			})

		.AddState("Guard_End")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em0220_113_AL_to_wait03");
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
	m_bTeleport = CheckSASType(ESASType::SAS_TELEPORT);

	//ASM, FSM tick
	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);

	for (auto it : m_pLanterns)
		it->Tick(TimeDelta);

	Update_LanternMatrix();

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

void CEM0220::CheckHP(DAMAGE_PARAM & tDamageParams)
{
	_int iDamage = tDamageParams.iDamage;
	
	if(m_IsBeat == false)
		m_iHP -= iDamage;

	if (m_iHP < 0)
	{
		if (m_iCrushGage > 0)
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
	Safe_Release(m_pRange);

	for (auto it : m_pLanterns)
		Safe_Release(it);

	m_pLanterns.clear();
}
