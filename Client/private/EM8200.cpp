#include "stdafx.h"
#include "../public/EM8200.h"

#include <Camera_Manager.h>
#include <GameUtils.h>
#include <PhysX_Manager.h>

#include "BulletBuilder.h"
#include "EffectSystem.h"
#include "FSMComponent.h"
#include "RigidBody.h"
#include "JsonStorage.h"
#include "EM8200_AnimInstance.h"
#include "EM8200_Controller.h"
#include "ImguiUtils.h"
#include "MathUtils.h"
#include "Material.h"
#include "EMCable.h"
#include "MapKinetic_Object.h"
#include "PlayerInfoManager.h"
#include "EM8200_CopyRush.h"
#include "EM8200_BrainField.h"
#include "EM8200_BrainCrushCables.h"
#include "CurveManager.h"
#include "CurveFloatMapImpl.h"
#include "BrainField.h"
#include "UI_Manager.h"
#include "GameManager.h"
#include "Canvas_MainTalk.h"
#include "EnvironmentEffect.h"
#include "TestTarget.h"


CEM8200::CEM8200(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEnemy(pDevice, pContext)
{
	m_eEnemyName = EEnemyName::EM8200;
}

CEM8200::CEM8200(const CEM8200& rhs)
	: CEnemy(rhs)
{
}

HRESULT CEM8200::Initialize(void* pArg)
{
	Json em0200_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/em8200/em8200_Collider.json");
	pArg = &em0200_json;

	{
		m_iMaxHP = 30000;
		m_iHP = 30000; // ★
		m_iMaxCrushGauge = m_iMaxHP * 1.1f;
		m_iCrushGauge = 100; // m_iMaxCrushGauge;
		m_bHasCrushGauge = true;

		m_iAtkDamage = 50;
		m_bBoss = true;
		iEemeyLevel = 42;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	m_eEnemyName = EEnemyName::EM8200;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(180.f));

	Json KarenMask = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/EffectSystem/Monster_Effect/Karen_Mask.json");
	m_pKarenMaskEf = (CEffectSystem*) m_pGameInstance->Clone_GameObject_Get(L"Layer_KarenMask", L"ProtoVFX_EffectSystem", &KarenMask);
	Safe_AddRef(m_pKarenMaskEf);

	m_pKarenMaskEf->GetParams().Floats[1] = 1.f;


	m_CounterEFCoolTimeHelper.Initialize(0.1f, true);

	m_pKaren_AnimCam = dynamic_cast<CAnimCam*>(m_pGameInstance->FindCamera("KarenAnimCamaera"));
	if (m_pKaren_AnimCam == nullptr)
		m_pKaren_AnimCam = dynamic_cast<CAnimCam*>(m_pGameInstance->Add_Camera("KarenAnimCamaera", LEVEL_NOW, L"Layer_Camera", L"Prototype_AnimCam"));
	Safe_AddRef(m_pKaren_AnimCam);

	m_pEMUI->SetWeakBoneName("Spine1");

	m_bSpawnEffect = false;

	m_fFireResist = 1.f;
	m_fThunderResist = 1.f;

	return S_OK;
}

void CEM8200::SetUpComponents(void* pArg)
{
	CEnemy::SetUpComponents(pArg);
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em8200", L"Model", (CComponent**)&m_pModelCom));
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em8200", L"KineticModel", (CComponent**)&m_pKineticModel));

	m_pModelCom->FindMaterial(L"MI_em8200_HOOD_0")->SetActive(false);

	// 컨트롤러, prototype안 만들고 여기서 자체생성하기 위함
	m_pController = CEM8200_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	m_pController->SetDetectTarget(true); // 눈깔 스킵

	// ASM
	m_pASM = CEM8200_AnimInstance::Create(m_pModelCom, this);

	m_HeavyAttackPushTimeline.SetCurve("Simple_Decrease");

	m_TPStart.SetCurve("Simple_Increase_0.2x");
	m_TPEnd.SetCurve("Simple_Decrease_0.2x");

	m_CounterStart.SetCurve("Simple_Increase_0.2x");
	m_CounterEnd.SetCurve("Simple_Decrease_0.2x");

	m_CaptureStart.SetCurve("Simple_Increase_0.2x");
	m_CaptureEnd.SetCurve("Simple_Decrease_0.2x");

	m_KarenMaskStart.SetCurve("Simple_Decrease_0.2x");

	m_pLeftCopy = dynamic_cast<CEM8200_CopyRush*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, L"Monster_em8200_CopyRush"));
	m_pRightCopy = dynamic_cast<CEM8200_CopyRush*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, L"Monster_em8200_CopyRush"));

	m_pBrainField = dynamic_cast<CEM8200_BrainField*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, L"Monster_em8200_BrainField"));
	m_pBrainField->SetTargetInfo(this, m_pTransformCom, m_pModelCom);

	_double TickPerSec = m_pModelCom->Find_Animation("AS_em8200_BrainField_start")->GetTickPerSec();
	m_pModelCom->Find_Animation("AS_em8200_BrainField_start")->SetTickPerSec(TickPerSec * 2.0);

	m_pBrainCrushCables = dynamic_cast<CEM8200_BrainCrushCables*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, L"Monster_em8200_BrainCrushCables"));
	m_pBrainCrushCables->SetTargetInfo(m_pTransformCom, m_pModelCom);
	
	m_pBrainCrushRimLight = CCurveManager::GetInstance()->GetCurve("EM8200_BrainCrash_RimLight");
	m_pBrainCrushChromaticAberration = CCurveManager::GetInstance()->GetCurve("EM8200_BrainCrash_ChromaticAberration");
	m_pBrainCrushChromaticAberrationCrash = CCurveManager::GetInstance()->GetCurve("EM8200_BrainCrash_ChromaticAberration_Crash");
	m_pChromaticAberration = dynamic_cast<CPostVFX_ChromaticAberration*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayerNoBegin(LEVEL_NOW, L"Prototype_PostVFX_ChromaticAberration"));
}

void CEM8200::Detected_Attack()
{
}

void CEM8200::Clear_Socket()
{
	m_pASM->ClearSocketAnim("FullBody", 0.f);
}

void CEM8200::Set_KineticObject(CGameObject* pObject)
{
	if(CGameInstance::GetInstance()->Check_ObjectAlive(pObject))
	{
		m_pKineticObject = pObject;

		m_KineticObjectOrigionPos = m_pKineticObject->GetTransform()->Get_WorldMatrix();
	}
}

void CEM8200::Kinetic_Combo_AttachLerpObject()
{
	if (nullptr !=	m_pTarget &&
		CGameInstance::GetInstance()->Check_ObjectAlive(m_pKineticObject) &&
		!m_pASM->isSocketEmpty("FullBody"))
	{
		m_pKineticModel->GetPlayAnimation()->Update_Bones_SyncRatio(0.f);
		m_pKineticModel->Compute_CombindTransformationMatrix();

		_matrix	SocketMatrix = m_pKineticModel->GetBoneMatrix("Hips") * m_pTransformCom->Get_WorldMatrix();

		_float fScale_Right = XMVectorGetX(XMVector3Length(m_KineticObjectOrigionPos.r[0]));
		_float fScale_Up = XMVectorGetX(XMVector3Length(m_KineticObjectOrigionPos.r[1]));
		_float fScale_Look = XMVectorGetX(XMVector3Length(m_KineticObjectOrigionPos.r[2]));

		

		_float fRatio = m_pModelCom->Find_Animation("AS_em8200_272_AL_atk_throw1_wait")->GetPlayRatio() * 2.f;

		if (1.f < fRatio)
		{
			fRatio = 1.f;
		}

		_vector vLerpLook = XMVectorLerp(m_KineticObjectOrigionPos.r[2], SocketMatrix.r[2], fRatio);
		_vector vLerpRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLerpLook);
		_vector vLerpUp = XMVector3Cross(vLerpLook, vLerpRight);

		_vector vLerpPos = XMVectorLerp(m_KineticObjectOrigionPos.r[3], SocketMatrix.r[3], fRatio);

		vLerpRight = XMVector3Normalize(vLerpRight) * fScale_Right;
		vLerpUp = XMVector3Normalize(vLerpUp) * fScale_Up;
		vLerpLook = XMVector3Normalize(vLerpLook) * fScale_Look;

		_matrix LerpMatrix = { vLerpRight, vLerpUp, vLerpLook, vLerpPos };

		m_pKineticObject->GetTransform()->Set_WorldMatrix(LerpMatrix);
	}
}

void CEM8200::Kinetic_Combo_KineticAnimation()
{
	if (nullptr != m_pTarget &&
		CGameInstance::GetInstance()->Check_ObjectAlive(m_pKineticObject) &&
		!m_pASM->isSocketEmpty("FullBody"))
	{
		m_pKineticModel->GetPlayAnimation()->Update_Bones_SyncRatio(m_pASM->GetSocketAnimation("FullBody")->GetPlayTime());
		m_pKineticModel->Compute_CombindTransformationMatrix();

		_matrix	SocketMatrix = m_pKineticModel->GetBoneMatrix("Hips") * m_pTransformCom->Get_WorldMatrix();

		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

		m_pKineticObject->GetTransform()->Set_WorldMatrix(SocketMatrix);
	}
}

void CEM8200::SetUpSound()
{
	CEnemy::SetUpSound();
}

void CEM8200::Create_Bullet()
{
	DAMAGE_PARAM eDamageParam;

	//TODO : 상태이상 정리되면 감전상태로 변경

	eDamageParam.eAttackType = EAttackType::ATK_LIGHT;
	eDamageParam.eDeBuff = EDeBuffType::DEBUFF_THUNDER;
	eDamageParam.iDamage = 100;

	_matrix pivot = XMMatrixScaling(0.5f,0.5f,0.5f);

	_matrix BoneMtx = m_pModelCom->GetBoneMatrix("LeftWeapon") * m_pTransformCom->Get_WorldMatrix();
	_vector vPrePos = BoneMtx.r[3];

	CBulletBuilder()
		.CreateBullet()
		.Set_Owner(this)
		.Set_Target(m_pTarget)
		.Set_InitBulletEffect({ L"em8200_Elec_Bullet_Loop" })
		.Set_BulletEffPivot(pivot)
		.Set_InitBulletParticle(L"em8200_Elec_Bullet_Test", true)
		.Set_ShootSpeed(8.f)
		.Set_Life(7.f)
		.Set_DamageParam(eDamageParam)
		.Set_Radius(1.f)
		// .Set_DeadBulletEffect({ L"" })
		.Set_DeadBulletParticle(L"em8200_Elec_Bullet_Dead")
		.Set_Position(vPrePos)
		.Build();
}

void CEM8200::TakeDamage(DAMAGE_PARAM tDamageParams)
{
	if (m_pModelCom->GetMaterials().front()->GetParam().Floats[2] > 0.1f)
		return;

	CEnemy::TakeDamage(tDamageParams);
}

void CEM8200::CheckHP(DAMAGE_PARAM& tDamageParams)
{
	if (m_pModelCom->GetMaterials().front()->GetParam().Floats[1] >= 0.5f)
		tDamageParams.iDamage = 0;
	else
		m_iHP -= tDamageParams.iDamage;

	if (m_iHP < 0)
	{
		SetDead();
		m_iHP = 0;
	}
}

void CEM8200::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();

	m_pModelCom->Add_EventCaller("Connected_Cable", [this]
		{
			m_pGameInstance->PlayShake(0.2f, 0.02);
			CVFX_Manager::GetInstance()->GetParticle(PS_SAS, TEXT("Connected_Particles_Karen"))->Start_Attach(this, "Neck", false, true);
		});

	m_pModelCom->Add_EventCaller("KneeKick_A1_Start", [this]
		{
			m_bMeleeCollStart = true;
			_matrix Pivot;
			Pivot = XMMatrixRotationX(XMConvertToRadians(-40.f)) * XMMatrixRotationY(XMConvertToRadians(90.f)) * XMMatrixTranslation(-0.5f, -0.5f, 0.f);
			CVFX_Manager::GetInstance()->GetEffect(EF_MONSTER, TEXT("em8200_KneeKickAndFrontKick"))->Start_AttachPivot(this, Pivot, "LeftLeg", true, false, true);

		});

	m_pModelCom->Add_EventCaller("KneeKick_A1_End", [this]
		{
			m_bMeleeCollStart = false;
		});

	m_pModelCom->Add_EventCaller("FrontKick_A2_Start", [this]
		{
			_matrix Pivot;
			Pivot = XMMatrixRotationX(XMConvertToRadians(125.f)) * XMMatrixRotationY(XMConvertToRadians(45.f)) * XMMatrixRotationZ(XMConvertToRadians(-30.f)) * XMMatrixTranslation(0.5f, 0.5f, 0.5f);

			CVFX_Manager::GetInstance()->GetEffect(EF_MONSTER, TEXT("em8200_FrontKick"))->Start_AttachPivot(this, Pivot, "RightToeBase", true, false, true);

			m_bMeleeCollStart = true;
		});

	m_pModelCom->Add_EventCaller("FrontKick_A2_End", [this]
		{
			m_bMeleeCollStart = false;
		});

	m_pModelCom->Add_EventCaller("TurnKick_A3_Start", [this]
		{
			_matrix Pivot;
			Pivot = XMMatrixRotationX(XMConvertToRadians(50.f)) * XMMatrixRotationY(XMConvertToRadians(-10.f)) * XMMatrixRotationZ(XMConvertToRadians(180.f)) * XMMatrixTranslation(-0.7f, -2.5f, -3.5f);

			CVFX_Manager::GetInstance()->GetEffect(EF_MONSTER, TEXT("em8200_TurnKick"))->Start_AttachPivot(this, Pivot, "RightToeBase", true, false, true);
			m_bMeleeCollStart = true;
		});
	m_pModelCom->Add_EventCaller("TurnKick_A3_End", [this]
		{
			m_bMeleeCollStart = false;
		});

	m_pModelCom->Add_EventCaller("Air_Elec_Atk_Fire_Start", [this]
		{
			m_bRangeCollStart = true;
			_matrix Pivot;
			CGameInstance::GetInstance()->AddLifePointLight(1.f, m_vRangeOverlapPos, 10.f, _float4(0.8f, 0.8f, 0.5f, 1.f));

			Pivot = XMMatrixScaling(1.f, 0.8f, 1.f) *  XMMatrixRotationX(XMConvertToRadians(40.f)) * XMMatrixRotationY(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.5f, -0.5f, 0.f);
			CVFX_Manager::GetInstance()->GetEffect(EF_MONSTER, TEXT("em8200_Elec_Laser"))->Start_AttachPivot(this, Pivot, "LeftWeapon",true, false);
			Pivot = XMMatrixScaling(0.5f, 0.5f, 0.5f);
			CVFX_Manager::GetInstance()->GetParticle(PS_MONSTER, TEXT("em8200_Elec_Floor_Particle"))->Start_OnlyPosUsePivot(m_vRangeOverlapPos, Pivot);
			// 조명추가
		});

	m_pModelCom->Add_EventCaller("Air_Elec_Atk_Fire_End", [this]
		{
			m_bRangeCollStart = false;
		});

	m_pModelCom->Add_EventCaller("Rush_Start", [this]
		{
			m_bMeleeCollStart = true;
			m_bRushStart = true;

			_matrix Pivot;

			Pivot =XMMatrixTranslation(0.f, 1.f, -1.f);
			m_pDashEF = CVFX_Manager::GetInstance()->GetEffect(EF_MONSTER, TEXT("em8200_Dash_Attack"));
			m_pDashEF->Start_AttachPivot(this, Pivot, "Reference", true, true);

			
			//먼지 달기 
			// Pivot = XMMatrixScaling(0.5f, 0.5f, 0.5f);
			// CVFX_Manager::GetInstance()->GetParticle(PS_MONSTER, TEXT("em8200_Elec_Floor_Particle"))->Start_OnlyPosUsePivot(m_vRangeOverlapPos, Pivot);
		});

	m_pModelCom->Add_EventCaller("Rush_End", [this]
		{
			m_bRushStart = false;
			m_bMeleeCollStart = false;
		});


	m_pModelCom->Add_EventCaller("Ice_Needle_Start", [this]
		{
			m_bRangeCollStart = true;

		});

	m_pModelCom->Add_EventCaller("Ice_Needle_End", [this]
		{
			m_bRangeCollStart = false;
		});

	m_pModelCom->Add_EventCaller("Chase_Elec_Start", [this]
		{
			Create_Bullet();

			//총알 생성
			// m_bRangeCollStart = true;

			// CVFX_Manager::GetInstance()->GetEffect(EF_MONSTER, TEXT("em8200_Ice_Needle_Attack"))->Start_AttachOnlyPos(m_vRangeOverlapPos);
		});

	
	m_pModelCom->Add_EventCaller("Air_Elec_TP_Start", [this]
		{

			string CmpString = m_pFSM->GetCurStateName();

			if (CmpString == "Air_Elec_Atk_Charge_Start_Before_TP")
			{
				static _float2 RandomFloat2;
				_uint iRand = rand() % 4;
				if (iRand == 0)
				{
					RandomFloat2.x = CGameUtils::GetRandFloat(-5.5f, -3.f);
					RandomFloat2.y = CGameUtils::GetRandFloat(-5.5f, -3.f);
				}
				else if (iRand == 1)
				{
					RandomFloat2.x = CGameUtils::GetRandFloat(3.f, 5.5f);
					RandomFloat2.y = CGameUtils::GetRandFloat(3.f, 5.5f);
				}
				else if (iRand == 2)
				{
					RandomFloat2.x = CGameUtils::GetRandFloat(3.f, 5.5f);
					RandomFloat2.y = CGameUtils::GetRandFloat(-5.5f, -3.f);
				}
				else if (iRand == 3)
				{
					RandomFloat2.x = CGameUtils::GetRandFloat(-5.5f, -3.f);
					RandomFloat2.y = CGameUtils::GetRandFloat(3.f, 5.5f);
				}

				_float4 TargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION) + XMVectorSet(RandomFloat2.x, 3.f, RandomFloat2.y, 0.f);
				m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, TargetPos);

				m_pTransformCom->LookAt_NonY(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
			}

		});



	m_pModelCom->Add_EventCaller("Capture_Start", [this]
		{
			_float4 TargetPos = static_cast<CScarletCharacter*>(m_pTarget)->GetColliderPosition();

			static_cast<CMapKinetic_Object*>(m_pKineticObject)->Boss_Throw(TargetPos);

			m_pKineticObject = nullptr;

			m_CaptureEnd.PlayFromStart();
		});

	m_pModelCom->Add_EventCaller("Counter_Start", [this]
		{
			// 카운터 발찍을 때
			m_bMeleeCollStart = true;
		if(CPlayerInfoManager::GetInstance()->Get_Air() == false)
			Melee_Overlap("Reference", 50, 10.f, EAttackType::ATK_TO_AIR);
		    m_bMeleeCollStart = false;

			CVFX_Manager::GetInstance()->GetEffect(EF_MONSTER, TEXT("em8200_Counter_Stamp_Impact"))->Start_Attach(this, "LeftToeBase", false, true);
			CGameInstance::GetInstance()->AddLifePointLight(1.f, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 20.f, _float4(1.f, 1.f, 1.f, 1.f));

		});

	m_pModelCom->Add_EventCaller("Stamp_Start", [this]
		{
			// 발찍기 전 최고높이로 들어올린
			_matrix Pivot = XMMatrixRotationX(XMConvertToRadians(180.f));
			CVFX_Manager::GetInstance()->GetEffect(EF_MONSTER, TEXT("em8200_Counter_Stamp"))->Start_AttachPivot(this , Pivot, "LeftToeBase", true, true, true);

		});

	m_pModelCom->Add_EventCaller("See_Through_Start", [this]
		{
			// 번쩍 이펙트
			string CmpString = m_pFSM->GetCurStateName();

			if (CmpString == "See_Through_Start")
			{
				CPlayerInfoManager::GetInstance()->Release_SasEnergy_All();

				CVFX_Manager::GetInstance()->GetEffect(EF_MONSTER, TEXT("em8200_Seethrough"))->Start_Attach(this, "RightWeapon", false, true);
				CGameInstance::GetInstance()->AddLifePointLight(1.f, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 20.f, _float4(1.f, 1.f, 1.f, 1.f));
			}

			if(CmpString == "BattleStart")
			{
				m_KarenMaskStart.PlayFromStart();
				m_pKarenMaskEf->GetParams().Ints[0] = 1;
			}
		});


	m_pModelCom->Add_EventCaller("BrainField_Start_Ring", [this]
		{
			_matrix EffectMatrix = XMMatrixScaling(0.25f, 0.25f, 0.25f) * XMMatrixRotationX(-90.f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, L"BrainField_Before_Ring_EF")->Start_AttachPivot(this, EffectMatrix, "Eff01", true, true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, L"BrainField_Before_Ring_EF")->Start_AttachPivot(this, EffectMatrix, "Eff02", true, true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, L"BrainField_Before_Ring_EF")->Start_AttachPivot(this, EffectMatrix, "Eff03", true, true);

		});


	m_pModelCom->Add_EventCaller("BrainField_Start_Gear", [this]
		{
			_matrix EffectMatrix = XMMatrixScaling(0.25f, 0.25f, 0.25f) * XMMatrixRotationX(-90.f);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, L"BrainField_Before_Gear_EF")->Start_AttachPivot(this, EffectMatrix, "Eff01", true, true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, L"BrainField_Before_Gear_EF")->Start_AttachPivot(this, EffectMatrix, "Eff02", true, true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, L"BrainField_Before_Gear_EF")->Start_AttachPivot(this, EffectMatrix, "Eff03", true, true);

		});

	m_pModelCom->Add_EventCaller("BrainCrash_Slow", [&]() { CGameInstance::GetInstance()->SetTimeRatioCurve("BrainCrash_Slow"); });
}

void CEM8200::SetUpFSM()
{
	CEnemy::SetUpFSM();

	CFSMComponentBuilder Builder;
	Builder.InitState("Intro_00");
	AddState_Intro(Builder);
	AddState_Idle(Builder);
	AddState_Teleport(Builder);
	AddState_Attack_Kick(Builder);
	AddState_Attack_IceNeedle(Builder);
	AddState_Attack_ChaseElec(Builder);
	AddState_Attack_AirElec(Builder);
	AddState_Attack_Rush(Builder);
	AddState_Seethrough(Builder);
	AddState_Counter(Builder);
	AddState_CaptureKinetic(Builder);
	AddState_Damaged(Builder);
	AddState_BrainField(Builder);
	AddState_BrainCrush(Builder);

	m_pFSM = Builder.Build();
}

void CEM8200::BeginTick()
{
	CEnemy::BeginTick();

	m_pKineticModel->SetPlayAnimation("AS_em8200_208_AL_atk_pcon_b3_end_obj");
}

void CEM8200::Tick(_double TimeDelta)
{
	CEnemy::Tick(TimeDelta);

	if (m_bCrushStart && m_BrainCrushOnce.IsNotDo())
	{
		m_pFSM->SetState("Idle");
		m_pController->SetActive(false);
		m_pASM->ClearSocketAnim("FullBody");
	}

	if (GetHpRatio() < 0.7 && m_SecondPhase.IsNotDo())
	{
		m_bSecondPhase = true;
		m_iHP = m_iMaxHP;
	}

	m_CounterEFCoolTimeHelper.Tick(TimeDelta);

	m_pFSM->Tick(TimeDelta);

	m_pController->SetTarget(m_pTarget);
	if (m_bDead == false)
		m_pController->Tick(TimeDelta);
	else
		m_pController->Invalidate();

	m_bRun = m_pController->IsRun();
	m_fTurnRemain = m_pController->GetTurnRemain();
	m_vMoveAxis = m_pController->GetMoveAxis();
	m_vMoveAxis.Normalize();
	m_eInput = m_pController->GetAIInput();

	m_pASM->Tick(TimeDelta);

	const _float fMoveSpeed = m_bRun ? 4.f : 2.f;

	if (m_vMoveAxis.LengthSquared() > 0.f)
	{
		const _float fYaw = m_pTransformCom->GetYaw_Radian();
		_float3 vVelocity;
		XMStoreFloat3(&vVelocity, fMoveSpeed * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}

	{
		_float fTPStartOut;
		if (m_TPStart.Tick(TimeDelta, fTPStartOut))
		{
			for (auto pMtrl : m_pModelCom->GetMaterials())
			{
				pMtrl->GetParam().Floats[2] = fTPStartOut;
			}
			m_pKarenMaskEf->GetParams().Floats[1] = fTPStartOut;
			if (m_pBrainField->IsOpen())
			{
				m_pBrainField->SetCableTP(fTPStartOut);
			}
		}
	}

	{
		_float fTPEndOut;
		if (m_TPEnd.Tick(TimeDelta, fTPEndOut))
		{
			for (auto pMtrl : m_pModelCom->GetMaterials())
			{
				pMtrl->GetParam().Floats[2] = fTPEndOut;

			}
			m_pKarenMaskEf->GetParams().Floats[1] = fTPEndOut;
			if (m_pBrainField->IsOpen())
			{
				m_pBrainField->SetCableTP(fTPEndOut);
			}
		}
	}

	{
		_float fCounterStartOut;
		if (m_CounterStart.Tick(TimeDelta, fCounterStartOut))
		{
			for (auto pMtrl : m_pModelCom->GetMaterials())
			{
				pMtrl->GetParam().Floats[1] = fCounterStartOut;
				pMtrl->GetParam().Ints[0] = (_int)fCounterStartOut;

			}
			m_pKarenMaskEf->GetParams().Ints[0] = 1;
		}
	}

	{
		_float fCounterEndOut;
		if (m_CounterEnd.Tick(TimeDelta, fCounterEndOut))
		{
			for (auto pMtrl : m_pModelCom->GetMaterials())
			{
				pMtrl->GetParam().Floats[1] = fCounterEndOut;
				pMtrl->GetParam().Ints[0] = (_int)fCounterEndOut;

			}
			m_pKarenMaskEf->GetParams().Ints[0] = 0;
		}
	}

	{
		_float fCaptureStartOut;
		if (m_CaptureStart.Tick(TimeDelta, fCaptureStartOut))
		{
			for (auto pMtrl : m_pModelCom->GetMaterials())
			{
				pMtrl->GetParam().Floats[0] = fCaptureStartOut;
			}
		}
	}

	{
		_float fKarenMaskStartOut;
		if (m_KarenMaskStart.Tick(TimeDelta, fKarenMaskStartOut))
		{
			m_pKarenMaskEf->GetParams().Floats[1] = fKarenMaskStartOut;
		}
	}

	{
		_float fCaptureEndOut;
		if (m_CaptureEnd.Tick(TimeDelta, fCaptureEndOut))
		{
			for (auto pMtrl : m_pModelCom->GetMaterials())
			{
				pMtrl->GetParam().Floats[0] = fCaptureEndOut;
			}
		}
	}

	string StateName = m_pFSM->GetCurStateName();



	m_pLeftCopy->Tick(TimeDelta);
	m_pRightCopy->Tick(TimeDelta);

	m_pChromaticAberration->Tick(TimeDelta);

	// Tick의 제일 마지막에서 실행한다.
	ResetHitData();
}

void CEM8200::Late_Tick(_double TimeDelta)
{
	CScarletCharacter::Late_Tick(TimeDelta);
	m_pLeftCopy->Late_Tick(TimeDelta);
	m_pRightCopy->Late_Tick(TimeDelta);

	if (m_bVisible)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND_TOON, this);
	}

	m_pBrainField->Tick(TimeDelta);
	m_pBrainField->Late_Tick(TimeDelta);
	m_pBrainCrushCables->Tick(TimeDelta);
	m_pBrainCrushCables->Late_Tick(TimeDelta);
	m_pChromaticAberration->Late_Tick(TimeDelta);
}

void CEM8200::AfterPhysX()
{
	CEnemy::AfterPhysX();
	_matrix	SocketMatrix = GetBoneMatrix("FacialRoot") * m_pTransformCom->Get_WorldMatrix();
	m_pKarenMaskEf->GetTransform()->Set_WorldMatrix(SocketMatrix);

}

HRESULT CEM8200::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CEM8200::Imgui_RenderProperty()
{
	CEnemy::Imgui_RenderProperty();

	if(ImGui::Button("StoryEndBt"))
	{
		// m_bStoryEnd = true;

		auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("Karen_Intro_Walk");
		// m_pKaren_AnimCam->StartCamAnim_Return_Update(pCamAnim, CPlayerInfoManager::GetInstance()->Get_PlayerCam(), m_pTransformCom, 0.f, 0.f);
		
			for (auto iter : m_pGameInstance->GetLayer(LEVEL_NOW, L"Layer_FinalStage")->GetGameObjects())
			{
				if (auto pTestTarget = dynamic_cast<CTestTarget*>(iter))
				{
					pTestTarget->GetTransform()->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(-180.f));
					m_pKaren_AnimCam->StartCamAnim_Return_Update(pCamAnim, CPlayerInfoManager::GetInstance()->Get_PlayerCam(), pTestTarget->GetTransform(), 0.f, 0.f);
				}
			}
	}

	if (ImGui::CollapsingHeader("ASM"))
	{
		m_pASM->Imgui_RenderState();
	}
	if (ImGui::CollapsingHeader("KarenPivot"))
	{
		static GUIZMO_INFO tp1;
		CImguiUtils::Render_Guizmo(&Pivotmat, tp1, true, true);
	}

	if (ImGui::CollapsingHeader("KarenTP_EFCurve"))
	{
		// m_TPStart.Imgui_RenderEditor();
		// m_TPEnd.Imgui_RenderEditor();

	}

	if (ImGui::CollapsingHeader("KarenProgress"))
	{
		ImGui::InputFloat("LerpProgress_Karen", &m_fAnimProgress);
		ImGui::InputFloat("LerpTime_Karen", &m_fLerpTime);

		ImGui::InputFloat("ElecRange_Value", &m_fElecRangeValue);
	}

	if(ImGui::Button("KarenFSM_Recompile"))
	{
		CFSMComponent* pComponent = m_pFSM;

		SetUpFSM();

		m_pModelCom->Clear_mapEventFunc();

		SetUpAnimationEvent();

		Safe_Release(pComponent);
	}

	if (ImGui::CollapsingHeader("BrainField"))
	{
		m_pBrainField->Imgui_RenderProperty();
		m_pBrainField->Imgui_RenderComponentProperties();
	}
	if (ImGui::Button("CloseBrainField"))
	{
		m_pBrainField->CloseBrainField();
	}
	if (ImGui::CollapsingHeader("BrainCrush"))
	{
		m_pBrainCrushCables->Imgui_RenderProperty();
		m_pBrainCrushCables->Imgui_RenderComponentProperties();
	}
	if (ImGui::CollapsingHeader("Chromatic"))
	{
		m_pChromaticAberration->Imgui_RenderProperty();
		m_pChromaticAberration->Imgui_RenderComponentProperties();
	}
}

void CEM8200::SetUpUI()
{
	__super::SetUpUI();
	m_pEMUI->SetShieldUIPivotBoneName("HairRoot");
	m_pEMUI->SetShieldUIPivot(_float4x4::CreateTranslation({0.f, 0.4f, 0.f}));


	m_pEMUI->SetTargetBone("HairRoot");
	// m_pEMUI->Create_BossUI();
}

_bool CEM8200::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

void CEM8200::AddState_Idle(CFSMComponentBuilder& Builder)
{
	Builder

	.AddState("Idle")
		.OnStart([this]
			{
				// m_TPEnd.PlayFromStart();

				m_fGravity = 20.f;
			})
		.Tick([this] (_double TimeDelta)
		{
			if(m_pTarget != nullptr)
				m_pTransformCom->LookAt_Smooth(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION), TimeDelta);

			for (auto pMtrl : m_pModelCom->GetMaterials())
			{
				pMtrl->GetParam().Floats[2] = 0;
			}
		})

	.AddTransition("Idle to BrainCrushReady", "BrainCrushReady")
		.Predicator([this]
		{
			return m_bCrushStart;
		})
	.AddTransition("Idle to Death", "Death")
			.Predicator([this] {return m_bDead; })

	.AddTransition("Idle to BrainFieldStart", "BrainFieldStart")
		.Predicator([this]
		{
			 //return CGameInstance::GetInstance()->KeyDown(DIK_P);
				return m_eInput == CController::CTRL;
		})

	.AddTransition("Idle to Hit_Mid", "Hit_Mid_Heavy")
		.Predicator([this]
			{
				return	m_eCurAttackType == EAttackType::ATK_MIDDLE;
			})

	.AddTransition("Idle to Hit_Heavy", "Hit_Heavy")
		.Predicator([this]
			{
				return	m_eCurAttackType == EAttackType::ATK_HEAVY;
			})

	.AddTransition("Idle to Teleport_F_Start", "Teleport_F_Start")
		.Predicator([this]
			{
				return m_eInput == CController::NUM_1;
			})

	.AddTransition("Idle to Teleport_B_Start", "Teleport_B_Start")
		.Predicator([this]
			{
				return m_eInput == CController::NUM_4;
			})

	.AddTransition("Idle to Teleport_L_Start", "Teleport_L_Start")
		.Predicator([this]
			{
				return m_eInput == CController::NUM_3;
			})

	.AddTransition("Idle to Teleport_R_Start", "Teleport_R_Start")
		.Predicator([this]
			{
				return m_eInput == CController::NUM_2;
			})

	.AddTransition("Idle to Air_Elec_Atk_Charge_Start_Before_TP", "Air_Elec_Atk_Charge_Start_Before_TP")
		.Predicator([this]
			{
				return m_eInput == CController::F;
			})

	.AddTransition("Idle to Rush_Copy_Start_Before_TP", "Rush_Copy_Start_Before_TP")
		.Predicator([this]
			{
				return m_eInput == CController::G;
			})

	.AddTransition("Idle to Chase_Elec_Start_Before_TP", "Chase_Elec_Start_Before_TP")
		.Predicator([this]
			{
				return m_eInput == CController::R;
			})

	.AddTransition("Idle to Ice_Needle_Atk_Start", "Ice_Needle_Atk_Start")
		.Predicator([this]
			{
				return m_eInput == CController::Q;
			})

	.AddTransition("Idle to See_Through_Start", "See_Through_Start")
		.Predicator([this]
			{
				return m_eInput == CController::L;
			})

	.AddTransition("Idle to Counter_Start", "Counter_Start")
		.Predicator([this]
			{
				return m_eInput == CController::C;
			})

	.AddTransition("Idle to Capture_Wait", "Capture_Wait")
		.Predicator([this]
			{
				return m_eInput == CController::B;
			})

	.AddTransition("Idle to KneeKick_A1_Before_TP", "KneeKick_A1_Before_TP")
	.Predicator([this]
		{
			return m_eInput == CController::E;
		})

	;
}

void CEM8200::AddState_Teleport(CFSMComponentBuilder& Builder)
{
	Builder

	// Teleport_F_Start
	.AddState("Teleport_F_Start")
		.OnStart([this]
			{
				m_TPStart.PlayFromStart();

				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_130_AL_dodge_F_start");
				m_fTP_Range = CGameUtils::GetRandFloat(1.f, 2.5f);
				wstring strTeleportEffectName = m_vecRandomTeleportEffect[CMathUtils::RandomUInt(m_vecRandomTeleportEffect.size() - 1)];
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, strTeleportEffectName)->Start_Attach(this, "Spine", true);
			})
		.Tick([this](_double TimeDelta)
		{
				SocketLocalMove_Range(m_pASM, m_fTP_Range);
		})
	.AddTransition("Teleport_F_Start to Teleport_F_Stop", "Teleport_F_Stop")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Teleport_F_Start


	// Teleport_F_Stop
	.AddState("Teleport_F_Stop")
		.OnStart([this]
			{
				m_TPEnd.PlayFromStart();

				// m_pTransformCom->LookAt_NonY(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_131_AL_dodge_F_stop");
				m_fTP_Range = CGameUtils::GetRandFloat(1.f, 2.5f);

			})
		.Tick([this](_double TimeDelta)
			{
				SocketLocalMove_Range(m_pASM, m_fTP_Range);
			})

	.OnExit([this]
	{
			m_SetTPOnce.Reset();
			m_pTransformCom->LookAt_NonY(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
	})
	
	.AddTransition("Teleport_F_Stop to Idle", "Idle")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})

	// ~Teleport_F_Stop

	// Teleport_B_Start
	.AddState("Teleport_B_Start")
		.OnStart([this]
			{
				m_TPStart.PlayFromStart();

				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_135_AL_dodge_B_start");
				m_fTP_Range = CGameUtils::GetRandFloat(1.f, 2.5f);
				wstring strTeleportEffectName = m_vecRandomTeleportEffect[CMathUtils::RandomUInt(m_vecRandomTeleportEffect.size() - 1)];
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, strTeleportEffectName)->Start_Attach(this, "Spine", true);
			})
		.Tick([this](_double TimeDelta)
					{
						SocketLocalMove_Range(m_pASM, m_fTP_Range);

						if (CMathUtils::FloatCmp(m_pASM->GetSocketAnimation("FullBody")->GetPlayRatio(), 0.8f, 0.1f) && m_SetTPOnce.IsNotDo())
						{
							m_pTransformCom->LookAt_NonY(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
						}
					})
		.OnExit([this]
		{
			m_SetTPOnce.Reset();
		})
	.AddTransition("Teleport_B_Start to Idle", "Idle")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Teleport_B_Start
					
	// Teleport_L_Start
	.AddState("Teleport_L_Start")
		.OnStart([this]
			{
				m_TPStart.PlayFromStart();

				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_145_AL_dodge_L_start");
				m_fTP_Range = CGameUtils::GetRandFloat(1.f, 2.5f);
				wstring strTeleportEffectName = m_vecRandomTeleportEffect[CMathUtils::RandomUInt(m_vecRandomTeleportEffect.size() - 1)];
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, strTeleportEffectName)->Start_Attach(this, "Spine", true);
			})
		.Tick([this](_double TimeDelta)
			{
				SocketLocalMove_Range(m_pASM, m_fTP_Range);

				if (CMathUtils::FloatCmp(m_pASM->GetSocketAnimation("FullBody")->GetPlayRatio(), 0.8f, 0.1f) && m_SetTPOnce.IsNotDo())
				{
					m_pTransformCom->LookAt_NonY(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
				}
			})
		.OnExit([this] {m_SetTPOnce.Reset(); })

	.AddTransition("Teleport_L_Start to Idle", "Idle")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})

	
	// ~Teleport_L_Start

	//Teleport_R_Start
	.AddState("Teleport_R_Start")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_140_AL_dodge_R_start");
				m_fTP_Range = CGameUtils::GetRandFloat(1.f, 2.5f);
				wstring strTeleportEffectName = m_vecRandomTeleportEffect[CMathUtils::RandomUInt(m_vecRandomTeleportEffect.size() - 1)];
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, strTeleportEffectName)->Start_Attach(this, "Spine", true);
			})
		.Tick([this](_double TimeDelta)
			{
				SocketLocalMove_Range(m_pASM, m_fTP_Range);

				if (CMathUtils::FloatCmp(m_pASM->GetSocketAnimation("FullBody")->GetPlayRatio(), 0.8f, 0.1f) && m_SetTPOnce.IsNotDo())
				{
					m_pTransformCom->LookAt_NonY(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
				}
			})
		.OnExit([this] {m_SetTPOnce.Reset(); })

	.AddTransition("Teleport_R_Start to Idle", "Idle")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})

	// ~Teleport_R_Start

	;
}

void CEM8200::AddState_Attack_Kick(CFSMComponentBuilder& Builder)
{
	Builder
	// KneeKick_A1
	.AddState("KneeKick_A1_Before_TP")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_130_AL_dodge_F_start");

				m_TPStart.PlayFromStart();

				wstring strTeleportEffectName = m_vecRandomTeleportEffect[CMathUtils::RandomUInt(m_vecRandomTeleportEffect.size() - 1)];
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, strTeleportEffectName)->Start_Attach(this, "Spine", true);
			})
		.Tick([this](_double TimeDelta)
			{
				if (CMathUtils::FloatCmp(m_pASM->GetSocketAnimation("FullBody")->GetPlayRatio(), 0.9f, 0.1f) && m_SetTPOnce.IsNotDo())
				{
					m_vTeleportPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				
					if (nullptr != m_pTarget)
					{
						physx::PxRaycastHit hitBuffer[1];
						physx::PxRaycastBuffer rayOut(hitBuffer, 1);
				
						RayCastParams param;
						param.rayOut = &rayOut;
						param.vOrigin = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
						param.vDir = m_pTarget->GetColliderPosition() - param.vOrigin;
						param.fDistance = 12.f;
						param.iTargetType = CTB_PLAYER;
						param.bSingle = true;
						param.fVisibleTime = 1.f;
				
						if (CGameInstance::GetInstance()->RayCast(param))
						{
							for (int i = 0; i < rayOut.getNbAnyHits(); ++i)
							{
								auto pHit = rayOut.getAnyHit(i);
				
								_float4 vTargetOriginPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				
								m_vTeleportPos = _float4(pHit.position.x, m_vTeleportPos.y, pHit.position.z, 1.f);
								m_pTransformCom->LookAt_NonY(m_vTeleportPos);
							}
						}
						else
						{
							m_vTeleportPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 5.f);
						}
					}
					else
					{
						m_vTeleportPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 5.f);
					}
				
					m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, m_vTeleportPos);
				}
			})
			.OnExit([this]
			{
				m_SetTPOnce.Reset();
			})

	.AddTransition("KneeKick_A1_Before_TP to KneeKick_A1", "KneeKick_A1")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})


	.AddState("KneeKick_A1")
		.OnStart([this]
		{
				m_TPEnd.PlayFromStart();

				m_pASM->SetLerpDuration(0.5f);
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_201_AL_atk_kick_a1");
				ClearDamagedTarget();
		})
		.Tick([this](_double TimeDelta)
		{
				Melee_Overlap("LeftLeg", 50, 0.5, EAttackType::ATK_LIGHT);
		})
		.OnExit([this]
		{
				m_pASM->SetLerpDuration(m_fDefault_LerpTime);
		})
	.AddTransition("KneeKick_A1 to FrontKick_A2", "FrontKick_A2")
		.Predicator([this]
		{
				return m_pASM->isSocketPassby("FullBody", 0.6f) && !m_DamagedTargetList.empty();
		})

	.AddTransition("KneeKick_A1 to Idle", "Idle")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody") && m_DamagedTargetList.empty();
			})

	// ~KneeKick_A1

	// FrontKick_A2
	.AddState("FrontKick_A2")
		.OnStart([this]
			{
				m_pASM->SetLerpDuration(0.5f);
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_202_AL_atk_kick_a2");
				ClearDamagedTarget();
			})

	.Tick([this](_double TimeDelta)
		{
			Melee_Overlap("RightToeBase", 50, 0.5, EAttackType::ATK_MIDDLE);
		})

		.OnExit([this]
		{
				m_pASM->SetLerpDuration(m_fDefault_LerpTime);
		})

	.AddTransition("FrontKick_A2 to TurnKick_A3", "TurnKick_A3")
		.Predicator([this]
			{
				return m_pASM->isSocketPassby("FullBody", 0.65f) && !m_DamagedTargetList.empty();
			})

	.AddTransition("FrontKick_A2 to Idle", "Idle")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody") && m_DamagedTargetList.empty();
			})

	// ~FrontKick_A2

	// TurnKick_A3
	.AddState("TurnKick_A3")
		.OnStart([this]
			{
				m_pASM->SetLerpDuration(0.3f);
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_203_AL_atk_kick_a3");
				ClearDamagedTarget();
			})
	.Tick([this](_double TimeDelta)
	{
			Melee_Overlap("RightToeBase", 50, 1., EAttackType::ATK_HEAVY);
	})
		.OnExit([this]
		{
				m_pASM->SetLerpDuration(m_fDefault_LerpTime);
		})

	.AddTransition("TurnKick_A3 to Idle", "Idle")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~TurnKick_A3


		;


}

void CEM8200::AddState_Attack_IceNeedle(CFSMComponentBuilder& Builder)
{
	Builder
		// Ice Needle Ready 
	.AddState("Ice_Needle_Atk_Start")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_231_AL_atk_fire_chg_start");

				m_pTransformCom->LookAt_NonY(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
			})

	.AddTransition("Ice_Needle_Atk_Start to Ice_Needle_Atk_Loop", "Ice_Needle_Atk_Loop")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Ice Needle Ready

	// Ice Needle Ready Loop 
	.AddState("Ice_Needle_Atk_Loop")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_232_AL_atk_fire_chg_loop");
				// _matrix scale = XMMatrixScaling(0.5f, 1.f, 0.5f);

				_float4 vThisPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float4 vThisLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
				m_vRangeOverlapPos = vThisPos + vThisLook * 15.f;
				m_vRangeOverlapPos = _float4(m_vRangeOverlapPos.x, 0.f, m_vRangeOverlapPos.z, 1.f);

				CVFX_Manager::GetInstance()->GetEffect(EF_MONSTER, TEXT("em8200_Ice_Needle_Attack_S"))->Start_AttachOnlyPos(m_vRangeOverlapPos);
			})

	.AddTransition("Ice_Needle_Atk_Loop to Ice_Needle_Atk_Fire_Start", "Ice_Needle_Atk_Fire_Start")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Ice Needle Ready Loop

	// Ice Needle Fire Start
	.AddState("Ice_Needle_Atk_Fire_Start")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_233_AL_atk_fire_start");
				ClearDamagedTarget();
			})

	.AddTransition("Ice_Needle_Atk_Fire_Start to Ice_Needle_Atk_Fire_Loop", "Ice_Needle_Atk_Fire_Loop")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})

	.Tick([this](_double TimeDelta)
	{
			Range_Overlap(m_vRangeOverlapPos, 50.f, 13.f, EAttackType::ATK_TO_AIR);
	})
	// ~Ice Needle Fire Start

	// Ice Needle Fire Loop
	.AddState("Ice_Needle_Atk_Fire_Loop")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_234_AL_atk_fire_loop");
			})
		.Tick([this](_double TimeDelta)
		{
			Range_Overlap(m_vRangeOverlapPos, 50.f, 13.f, EAttackType::ATK_TO_AIR);
		})
	.AddTransition("Ice_Needle_Atk_Fire_Loop to Ice_Needle_Atk_Fire_End", "Ice_Needle_Atk_Fire_End")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Ice Needle Fire Loop



	// Ice Needle Fire End
	.AddState("Ice_Needle_Atk_Fire_End")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_235_AL_atk_fire_end");
			})

	.AddTransition("Ice_Needle_Atk_Fire_End to Idle", "Idle")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Ice Needle Fire End

		;
}

void CEM8200::AddState_Attack_ChaseElec(CFSMComponentBuilder& Builder)
{
	Builder
	// Chase_Elec_Start_Charge

		.AddState("Chase_Elec_Start_Before_TP")
		.OnStart([this]
			{
				m_TPStart.PlayFromStart();
				wstring strTeleportEffectName = m_vecRandomTeleportEffect[CMathUtils::RandomUInt(m_vecRandomTeleportEffect.size() - 1)];
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, strTeleportEffectName)->Start_Attach(this, "Spine", true);
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_135_AL_dodge_B_start");
				// 손에 이펙트 달기
			})
		.Tick([this](_double TimeDelta)
		{
			SocketLocalMove_Range(m_pASM, 1.5f);
		})

		.AddTransition("Chase_Elec_Start_Before_TP to Chase_Elec_Start", "Chase_Elec_Start")
				.Predicator([this]
					{
						return m_pASM->isSocketEmpty("FullBody");
					})

	.AddState("Chase_Elec_Start")
		.OnStart([this]
			{
				m_TPEnd.PlayFromStart();

				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_241_AL_atk_chase_charge");
			
			// 손에 이펙트 달기

			})

	.AddTransition("Chase_Elec_Start to Chase_Elec_Charge_Loop", "Chase_Elec_Charge_Loop")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Chase_Elec_Start_Charge

	// Chase_Elec_Start_Charge Loop 
	.AddState("Chase_Elec_Charge_Loop")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_242_AL_atk_chase_loop");

				_matrix Pivot = XMMatrixTranslation(0.2f, 0.f, 0.f);
				CVFX_Manager::GetInstance()->GetParticle(PS_MONSTER, TEXT("em8200_Chase_Elec_Start_Particle"))->Start_AttachPivot(this, Pivot, "LeftWeapon", true, false, true);
			})

	.AddTransition("Chase_Elec_Charge_Loop to Chase_Elec_Chase_Start_L", "Chase_Elec_Chase_Start_L")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Chase_Elec_Start_Charge Loop

	// Chase_Elec_Chase_Start_L
	.AddState("Chase_Elec_Chase_Start_L")
		.OnStart([this]
			{
				m_TPStart.PlayFromStart();

				wstring strTeleportEffectName = m_vecRandomTeleportEffect[CMathUtils::RandomUInt(m_vecRandomTeleportEffect.size() - 1)];
				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, strTeleportEffectName)->Start_Attach(this, "Spine", true);
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_243_AL_atk_chase_startL");

				
			})
		.Tick([this](_double TimeDelta)
		{
				_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
				vLook = XMVector3Normalize(vLook);
				m_pTransformCom->LocalMove(vLook, 0.2f);
		})

	.AddTransition("Chase_Elec_Chase_Start_L to Chase_Elec_Chase_End_L", "Chase_Elec_Chase_End_L")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Chase_Elec_Chase_Start_L

	// Chase_Elec_Chase_End_L
	.AddState("Chase_Elec_Chase_End_L")
		.OnStart([this]
			{
				m_TPEnd.PlayFromStart();

				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_244_AL_atk_chase_endL");
			})

	.AddTransition("Chase_Elec_Chase_End_L to Chase_Elec_Chase_Start_R", "Chase_Elec_Chase_Start_R")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Chase_Elec_Chase_End_L

	// Chase_Elec_Chase_Start_R
	.AddState("Chase_Elec_Chase_Start_R")
	.OnStart([this]
		{
			m_TPStart.PlayFromStart();

			m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_248_AL_atk_chase_startR");

			wstring strTeleportEffectName = m_vecRandomTeleportEffect[CMathUtils::RandomUInt(m_vecRandomTeleportEffect.size() - 1)];
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, strTeleportEffectName)->Start_Attach(this, "Spine", true);
		})
	.Tick([this](_double TimeDelta)
	{
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
			vLook = XMVector3Normalize(vLook);
			m_pTransformCom->LocalMove(-vLook, 0.2f);
	})
	.AddTransition("Chase_Elec_Chase_Start_R to Chase_Elec_Chase_End_R", "Chase_Elec_Chase_End_R")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Chase_Elec_Chase_Start_R

	// Chase_Elec_Chase_End_R
	.AddState("Chase_Elec_Chase_End_R")
		.OnStart([this]
			{
				m_TPEnd.PlayFromStart();

				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_249_AL_atk_chase_endR");
			})

	.AddTransition("Chase_Elec_Chase_End_R to Idle", "Idle")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Chase_Elec_Chase_End_R
	
		;
}

void CEM8200::AddState_Attack_AirElec(CFSMComponentBuilder& Builder)
{
	Builder

	// Air_Elec_Atk_Charge_Start 
	.AddState("Air_Elec_Atk_Charge_Start_Before_TP")
	.OnStart([this]
		{
			m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_130_AL_dodge_F_start");
			// 손에 이펙트 달기
			m_fGravity = 0.f;
			m_TPStart.PlayFromStart();

			m_fTP_Range = CGameUtils::GetRandFloat(1.f, 2.5f);

			wstring strTeleportEffectName = m_vecRandomTeleportEffect[CMathUtils::RandomUInt(m_vecRandomTeleportEffect.size() - 1)];
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, strTeleportEffectName)->Start_Attach(this, "Spine", true);
		})

		.Tick([this](_double TimeDelta)
		{
			// 순간이동 잡아주기
				SocketLocalMove_Range(m_pASM, m_fTP_Range);
		})
		.OnExit([this]
		{
			m_SetTPOnce.Reset();
			m_TPEnd.PlayFromStart();
		})

	.AddTransition("Air_Elec_Atk_Charge_Start_Before_TP to Air_Elec_Atk_Charge_Start", "Air_Elec_Atk_Charge_Start")
	.Predicator([this]
		{
			return m_pASM->isSocketEmpty("FullBody");
		})

	.AddState("Air_Elec_Atk_Charge_Start")
	.OnStart([this]
		{
			m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_211_AL_atk_air_chg_start");

			_matrix Pivot;
			Pivot = XMMatrixScaling(5.f, 5.f, 5.f);
			CVFX_Manager::GetInstance()->GetParticle(PS_MONSTER, TEXT("em8200_Elec_Detail_Particle"))->Start_AttachPivot(this, Pivot, "LeftWeapon", true, true, true);
			
			_float4 vThisPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			_float4 vThisLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			m_vRangeOverlapPos = vThisPos + vThisLook * 4.5f;
			m_vRangeOverlapPos = _float4(m_vRangeOverlapPos.x, 0.f, m_vRangeOverlapPos.z, 1.f);
		})
		.Tick([this](_double TimeDelta)
		{
			
		})
	.AddTransition("Air_Elec_Atk_Charge_Start to Air_Elec_Atk_Charge_Loop", "Air_Elec_Atk_Charge_Loop")
	.Predicator([this]
		{
			return m_pASM->isSocketEmpty("FullBody");
		})

	// ~Air_Elec_Atk_Charge_Start

	// Air_Elec_Start_Charge Loop 
	.AddState("Air_Elec_Atk_Charge_Loop")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_212_AL_atk_air_chg_loop");
				ClearDamagedTarget();
			})
	.Tick([this](_double TimeDelta)
		{
		})

	.AddTransition("Air_Elec_Atk_Charge_Loop to Air_Elec_Atk_Fire_Start", "Air_Elec_Atk_Fire_Start")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Air_Elec_Start_Charge Loop

	// Air_Elec_Atk_Fire_Start
	.AddState("Air_Elec_Atk_Fire_Start")
	.OnStart([this]
		{
			m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_213_AL_atk_air_start");
		})

	.AddTransition("Air_Elec_Atk_Fire_Start to Air_Elec_Atk_Fire_Start_Loop", "Air_Elec_Atk_Fire_Start_Loop")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Air_Elec_Atk_Fire_Start

	// Air_Elec_Atk_Fire_Start_Loop
	.AddState("Air_Elec_Atk_Fire_Start_Loop")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_214_AL_atk_air_loop");
			})
	.Tick([this](_double TimeDelta)
		{
			Range_Overlap(m_vRangeOverlapPos, 50, 3.f, EAttackType::ATK_LIGHT);
		})
	.AddTransition("Air_Elec_Atk_Fire_Start_Loop to Air_Elec_Atk_Fall", "Air_Elec_Atk_Fall")
		.Predicator([this]
			{
				return m_pASM->isSocketPassby("FullBody", 0.5f);
			})
	// ~Air_Elec_Atk_Fire_Start_Loop

	// Air_Elec_Atk_Fall
	.AddState("Air_Elec_Atk_Fall")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_215_AL_atk_air_fall");
				m_fGravity = 20.f;
				m_pASM->SetLerpDuration(0.f);
			})

	.AddTransition("Air_Elec_Atk_Fall to Air_Elec_Atk_Landing_Start", "Air_Elec_Atk_Landing_Start")
		.Predicator([this]
			{
				return !m_bPreOnFloor && m_bOnFloor;
			})
	// ~Air_Elec_Atk_Fall

	// Air_Elec_Atk_Landing_Start
	.AddState("Air_Elec_Atk_Landing_Start")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_216_AL_atk_air_landing_start");
			})

	.AddTransition("Air_Elec_Atk_Landing_Start to Air_Elec_Atk_Landing_Loop", "Air_Elec_Atk_Landing_Loop")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Air_Elec_Atk_Landing_Start


	// Air_Elec_Atk_Landing_Loop
	.AddState("Air_Elec_Atk_Landing_Loop")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_217_AL_atk_air_landing_loop");
			})

	.AddTransition("Air_Elec_Atk_Landing_Loop to Air_Elec_Atk_End", "Air_Elec_Atk_End")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Air_Elec_Atk_Landing_Loop

	// Air_Elec_Atk_End
	.AddState("Air_Elec_Atk_End")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_218_AL_atk_air_end");
				m_pASM->SetLerpDuration(m_fDefault_LerpTime);
			})

	.AddTransition("Air_Elec_Atk_End to Idle", "Idle")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Air_Elec_Atk_Landing_Loop

	;

}

void CEM8200::AddState_Attack_Rush(CFSMComponentBuilder& Builder)
{
	Builder

	// Rush_Copy_Start 

	.AddState("Rush_Copy_Start_Before_TP")
	.OnStart([this]
		{
			m_TPStart.PlayFromStart();

			m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_135_AL_dodge_B_start");
		})
	.Tick([this](_double TimeDelta)
		{
			SocketLocalMove_Range(m_pASM, 1.5f);

			if(CMathUtils::FloatCmp(m_pModelCom->Find_Animation("AS_em8200_135_AL_dodge_B_start")->GetPlayRatio(), 0.8f, 0.1f) && m_SetTPOnce.IsNotDo())
			{
				_float4 vTargetOriginPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);

				m_pTransformCom->LookAt_NonY(vTargetOriginPos);
			}
		})
	.OnExit([this]
		{
				m_SetTPOnce.Reset();
		})

	.AddTransition("Rush_Copy_Start_Before_TP to Rush_Copy_Start", "Rush_Copy_Start")
			.Predicator([this]
				{
					return m_pASM->isSocketEmpty("FullBody");
				})


	.AddState("Rush_Copy_Start")
		.OnStart([this]
			{
				m_TPEnd.PlayFromStart();
				_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
				_vector vRight = WorldMatrix.r[0];

				WorldMatrix.r[3] += vRight * 3.f;
				m_pLeftCopy->StartRush(this, m_pTarget, WorldMatrix, 50);
				WorldMatrix.r[3] -= vRight * 6.f;
				m_pRightCopy->StartRush(this, m_pTarget, WorldMatrix, 50);

				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_221_AL_atk_copy_start");
			})

	.AddTransition("Rush_Copy_Start to Rush_Copy_Loop", "Rush_Copy_Loop")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})


	// ~ Rush_Copy_Start

	// Rush_Copy_Loop Loop 
	.AddState("Rush_Copy_Loop")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_222_AL_atk_copy_loop");
			})

	.AddTransition("Rush_Copy_Loop to Rush_Crouch_Start", "Rush_Crouch_Start")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Rush_Copy_Loop Loop

	// Rush_Crouch_Start
	.AddState("Rush_Crouch_Start")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_223_AL_atk_crouch_start");
			})

	.AddTransition("Rush_Crouch_Start to Rush_Crouch_Loop", "Rush_Crouch_Loop")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Rush_Crouch_Start

	// Rush_Crouch_Loop
	.AddState("Rush_Crouch_Loop")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_224_AL_atk_crouch_loop");
			})

	.AddTransition("Rush_Crouch_Loop to Rush_Start", "Rush_Start")
			.Predicator([this]
				{
					return m_pASM->isSocketEmpty("FullBody");
				})
	// ~Rush_Crouch_Loop

	// Rush_Start
	.AddState("Rush_Start")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_225_AL_atk_rush_start");
				m_bMeleeCollStart = true;
				m_pTransformCom->LookAt_NonY(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
				ClearDamagedTarget();
			})
		.Tick([this](_double TimeDelta)
		{
				if (m_bRushStart == true)
				{
					_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
					
					m_pTransformCom->LocalMove(vLook, 0.5f);
				}
			Melee_Overlap("Reference", 50.f, 2.f, EAttackType::ATK_HEAVY);

		})
	.AddTransition("Rush_Start to Rush_Loop", "Rush_Loop")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})

	// .AddTransition("Rush_Start to Rush_End", "Rush_End")
	// 	.Predicator([this]
	// 		{
	// 			return !m_DamagedTargetList.empty();
	// 		})
	// ~Rush_Start

	// Rush_Loop
	.AddState("Rush_Loop")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_226_AL_atk_rush_loop");
			})
		.Tick([this](_double TimeDelta)
		{
				if (m_bRushStart == true)
				{
					_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
					m_pTransformCom->LocalMove(vLook, 0.5f);
				}

				Melee_Overlap("Reference", 50.f, 2.f, EAttackType::ATK_HEAVY);
		})
	.AddTransition("Rush_Loop to Rush_End", "Rush_End")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody") || !m_DamagedTargetList.empty();
			})
	// ~Rush_Loop

	// Rush_End
	.AddState("Rush_End")
		.OnStart([this]
		{
			m_bRushStart = false;
			m_bMeleeCollStart = false;
			m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_112_AL_brake_dash"); // 러시 앤드 애님 바꿈
			if (m_pDashEF != nullptr)
			{
				m_pDashEF->SetDelete();
				m_pDashEF = nullptr;
			}
		})
	.Tick([this](_double TimeDelta)
		{
			SocketLocalMove(m_pASM);
		})
	.OnExit([this]
		{
			
		})
	.AddTransition("Rush_End to Idle", "Idle")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Rush_End

				; 
}

void CEM8200::AddState_Seethrough(CFSMComponentBuilder& Builder)
{
	Builder
	.AddState("See_Through_Start")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_251_AL_atk_seethrough");

			// 이벤트 걸기 
			})

	.AddTransition("See_Through_Start to Idle", "Idle")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			});
}

void CEM8200::AddState_Counter(CFSMComponentBuilder& Builder)
{
	Builder
		.AddState("Counter_Start")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_261_AL_atk_counter_start");
			})

		.Tick([this](_double TimeDelta)
			{
					if (CMathUtils::FloatCmp(m_pASM->GetSocketAnimation("FullBody")->GetPlayRatio(), 0.4f, 0.2f) && m_SetTPOnce.IsNotDo())
					{
						m_CounterStart.PlayFromStart();
						CGameInstance::GetInstance()->AddLifePointLight(2.f, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 10.f, _float4(1.f, 1.f, 1.f, 1.f));
					}

					if (CMathUtils::FloatCmp(m_pASM->GetSocketAnimation("FullBody")->GetPlayRatio(), 0.6f, 0.4f) && m_CounterEFCoolTimeHelper.Use())
					{
						CVFX_Manager::GetInstance()->GetEffect(EF_MONSTER, TEXT("em8200_Counter_Start"))->Start_Attach(this, "Reference", false, true);
					}


			
			})
		.OnExit([this]
			{
				m_SetTPOnce.Reset();
			})
		.AddTransition("Counter_Start to Counter_Loop", "Counter_Loop")
				.Predicator([this]
					{
						return m_pASM->isSocketEmpty("FullBody");
					})

		.AddState("Counter_Loop")
			.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_262_AL_atk_counter_loop");

					// 이벤트 걸기 
				})

		.AddTransition("Counter_Loop to Counter_End", "Counter_End")
				.Predicator([this]
					{
						return m_pASM->isSocketEmpty("FullBody");
					})

		.AddState("Counter_End") // 실질 공격 애니메이션
			.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_263_AL_atk_counter_end");
					ClearDamagedTarget();
					
				})
			.Tick([this](_double TimeDelta)
				{
					if(CMathUtils::FloatCmp(m_pASM->GetSocketAnimation("FullBody")->GetPlayRatio(), 0.8f, 0.2f) && m_SetTPOnce.IsNotDo() )
						{
							m_CounterEnd.PlayFromStart();
						}
				})

			.OnExit([this]
				{
					m_SetTPOnce.Reset();
				})

		.AddTransition("Counter_End to Idle", "Idle")
			.Predicator([this]
				{
					return m_pASM->isSocketEmpty("FullBody");
				})

	;
}

void CEM8200::AddState_CaptureKinetic(CFSMComponentBuilder& Builder)
{
	Builder
		.AddState("Capture_Wait")
		.OnStart([this]
			{
				m_pASM->SetLerpDuration(0.3f);
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_272_AL_atk_throw1_wait");

				if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pKineticObject))
				{
					static_cast<CMapKinetic_Object*>(m_pKineticObject)->Set_Counter();
				}

				CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Kinetic_BaseCircle")->Start_Attach(this, "Reference", true);

				m_CaptureStart.PlayFromStart();
			})

		.Tick([this](_double TimeDelta)
		{
				Kinetic_Combo_AttachLerpObject();

				m_pTransformCom->LookAt_Smooth(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION), TimeDelta);
		})

		.AddTransition("Capture_Wait to Capture_Start", "Capture_Start")
			.Predicator([this]
				{
					return m_pASM->isSocketPassby("FullBody", 0.5f) && CGameInstance::GetInstance()->Check_ObjectAlive(m_pKineticObject);
				})

		.AddTransition("Capture_Wait to Idle", "Idle")
				.Predicator([this]
					{
						return m_pASM->isSocketPassby("FullBody", 0.5f) && CGameInstance::GetInstance()->Check_ObjectAlive(m_pKineticObject) == false;
					})

		.AddState("Capture_Start")
			.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_281_AL_atk_psycounter");
				})
		.Tick([this](_double TimeDelta)
			{
				m_pTransformCom->LookAt_Smooth(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION), TimeDelta);

				Kinetic_Combo_KineticAnimation();
			})

		.OnExit([this]
			{
				m_pASM->SetLerpDuration(m_fDefault_LerpTime);
				m_pController->Set_ResetKineticDoOnce();
			})

		.AddTransition("Capture_Wait_End to Idle", "Idle")
			.Predicator([this]
				{
					return m_pASM->isSocketEmpty("FullBody");
				})

	;

}

void CEM8200::AddState_BrainField(CFSMComponentBuilder& Builder)
{
	Builder
	.AddState("BrainFieldStart")
		.OnStart([this]
		{
			m_pModelCom->FindMaterial(L"MI_em8200_HOOD_0")->SetActive(true);
			m_pModelCom->FindMaterial(L"MI_em8200_HAIR_0")->SetActive(false);



			m_pBrainField->OpenBrainField();
			m_pASM->InputAnimSocketOne("FullBody", "AS_em8200_BrainField_start");

			auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("Karen_BrainField_Start");
			// m_pKaren_AnimCam->StartCamAnim_Return_Update(pCamAnim, CPlayerInfoManager::GetInstance()->Get_PlayerCam(), m_pTransformCom, 0.f, 0.f);
			m_pKaren_AnimCam->StartCamAnim_Return_Update(pCamAnim, m_pGameInstance->FindCamera("DynamicCamera"), m_pTransformCom, 0.f, 0.f);

			m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
			m_pController->ClearCommands();
			m_pController->SetActive(false);


		})
		.AddTransition("BrainFieldStart to BrainFieldTrans", "BrainFieldTrans")
			.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	.AddState("BrainFieldTrans")
		.OnStart([this]
		{
			m_pASM->InputAnimSocketOne("FullBody", "AS_em8200_BrainField_trans");

			auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("Karen_BrainField_Trans");
			// m_pKaren_AnimCam->StartCamAnim_Return_Update(pCamAnim, CPlayerInfoManager::GetInstance()->Get_PlayerCam(), m_pTransformCom, 0.f, 0.5f);
			m_pKaren_AnimCam->StartCamAnim_Return_Update(pCamAnim, m_pGameInstance->FindCamera("DynamicCamera"), m_pTransformCom, 0.f, 0.f);

			for( auto iter : m_pGameInstance->GetLayer(LEVEL_NOW, LAYER_MAP_DECO)->GetGameObjects())
			{
				if(auto pEnvEff = dynamic_cast<CEnvironmentEffect*>(iter))
				{
					pEnvEff->SetVisible(false);
				}
			}
		})
		.Tick([this](_double TimeDelta)
		{
			SocketLocalMove(m_pASM);
		})
		.OnExit([this]
		{
				m_pController->Set_SecondPhaseOnce();
		})
		.AddTransition("BrainFieldStart to InCombatIdle", "ImCombatIdle")
		.Predicator([this]
			{
				return m_pASM->isSocketPassby("FullBody", 0.9f);
			})

		.AddState("ImCombatIdle")
		.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody", "AS_em8200_004_Up_trans_battle");
			})
		.Tick([this](_double TimeDelta)
			{
				SocketLocalMove(m_pASM);
			})

	.OnExit([this]
	{
			// UI 보이기
			CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Item")->TempOff(false);
			CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_ItemMove")->TempOff(false);

			m_pController->SetActive(true);
	})
		.AddTransition("ImCombatIdle to Idle", "Idle")
			.Predicator([this]
				{
					return m_pASM->isSocketEmpty("FullBody");
				})
	;
}

void CEM8200::AddState_BrainCrush(CFSMComponentBuilder& Builder)
{
	Builder
	.AddState("BrainCrushReady")
		.OnStart([this]
		{
			m_pBrainField->CloseBrainField();
			m_pASM->InputAnimSocketOne("FullBody",  "AS_em8300_485_AL_BCchance_start");

			m_pModelCom->FindMaterial(L"MI_em8200_HOOD_0")->SetActive(false);
			m_pModelCom->FindMaterial(L"MI_em8200_HAIR_0")->SetActive(true);
		})
		.AddTransition("BrainCrushReady to BrainCrushReadyLoop", "BrainCrushReadyLoop")
			.Predicator([this]
			{
				return m_bBrainCrush || m_pASM->isSocketPassby("FullBody", 0.95f);
			})

	.AddState("BrainCrushReadyLoop")
		.OnStart([this]
		{
			m_pASM->InputAnimSocketOne("FullBody", "AS_em8300_486_AL_BCchance_loop");
			m_pModelCom->Find_Animation("AS_em8300_486_AL_BCchance_loop")->SetLooping(true);
		})
		.AddTransition("BrainCrushReadyLoop to BrainCrushStart_1", "BrainCrushStart_1")
			.Predicator([this]
				{
					return	m_bBrainCrush;
				})


	.AddState("BrainCrushStart_1")
		.OnStart([this]
		{
			// if (m_pTarget)
				// m_pTransformCom->LookAt_NonY(m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
			m_pASM->InputAnimSocketOne("FullBody", "AS_EnpcBC_dam_c02_em8200");
		})
		.Tick([this](_double TimeDelta)
		{
			_float fPlayRatio = m_pModelCom->Find_Animation("AS_EnpcBC_dam_c02_em8200")->GetPlayRatio(); 
			_float fValue = m_pBrainCrushRimLight->GetValue(fPlayRatio);
			for (auto pMtrl : m_pModelCom->GetMaterials())
				pMtrl->GetParam().Floats[0] = fValue;
			
		})
		.AddTransition("BrainCrushStart_1 to BrainCrushStart_2", "BrainCrushStart_2")
			.Predicator([this]
			{
				return m_bBrainCrashCommand;
			})

	.AddState("BrainCrushStart_2")
		.OnStart([this]
		{
			m_pBrainCrushCables->Activate(true);
			m_pASM->InputAnimSocketOne("FullBody", "AS_EnpcBC_dam_c03_em8200");
			m_bBrainCrashCommand = false;
		})
		.Tick([this](_double TimeDelta)
		{

		})
		.AddTransition("BrainCrushStart_2 to BrainCrushStart_3", "BrainCrushStart_3")
			.Predicator([this]
			{
				return m_bBrainCrashCommand;
			})

	.AddState("BrainCrushStart_3")
		.OnStart([this]
		{
			m_pModelCom->Find_Animation("AS_EnpcBC_fin_c05_em8200")->SetStay(true);
			m_pASM->InputAnimSocketOne("FullBody", "AS_EnpcBC_fin_c05_em8200");
			m_bBrainCrashCommand = false;
		})
		.Tick([this](_double TimeDelta)
		{
			_float fPlayRatio = m_pModelCom->Find_Animation("AS_EnpcBC_fin_c05_em8200")->GetPlayRatio(); 
			m_pChromaticAberration->GetParam().Floats[0] = m_pBrainCrushChromaticAberration->GetValue(fPlayRatio);;
			m_pChromaticAberration->GetParam().Ints[0] = (_int)m_pBrainCrushChromaticAberrationCrash->GetValue(fPlayRatio);
			if (fPlayRatio > 0.5f)
			{
				m_pBrainCrushCables->Activate(false);
				for (auto pMtrl : m_pModelCom->GetMaterials())
					pMtrl->GetParam().Floats[0] = 0.f;
			}
		})
	;

}

void CEM8200::AddState_Intro(CFSMComponentBuilder& Builder)
{
	Builder
		.AddState("Intro_00")
		.OnStart([this]
			{
				m_pASM->SetLerpDuration(0.3f);
				m_pASM->InputAnimSocketMany("FullBody", { "AS_em8200_001_AL_wait01" ,"AS_em8200_001_AL_wait01" ,"AS_em8200_002_AL_wait02"});
			})

		.Tick([this](_double TimeDelta)
			{
				
			})

		.AddTransition("Intro_00 to StoryWalk", "StoryWalk")
			.Predicator([this]
				{
					return m_pASM->isSocketPassby("FullBody", 0.95f)  || Check_PlayerDetected();;
				})

		.AddState("StoryWalk")
			.OnStart([this]
				{
					m_pASM->InputAnimSocketMany("FullBody", { "AS_em8200_105_AL_walk" ,"AS_em8200_105_AL_walk" ,"AS_em8200_105_AL_walk", "AS_em8200_105_AL_walk"});

					auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("Karen_Intro_Walk");

					m_pKaren_AnimCam->StartCamAnim(pCamAnim,
						_float4x4::Identity,
						_float4x4::Identity);
				})

		.Tick([this](_double TimeDelta)
		{
				_float4 TargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				_float4 ThisPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float3 Dir = XMVector3Normalize(TargetPos - ThisPos);


				m_pTransformCom->LocalMove(Dir, 0.025f);
		})

		.AddTransition("StoryWalk to Intro_01", "Intro_01")
			.Predicator([this]
				{
					return Check_PlayerDetected_Near();
				})


		
		.AddState("Intro_01")
			.OnStart([this]
				{

					m_pASM->InputAnimSocketMany("FullBody", { "AS_em8200_001_AL_wait01","AS_em8200_002_AL_wait02",
																						"AS_em8200_001_AL_wait01","AS_em8200_002_AL_wait02",
																						"AS_em8200_001_AL_wait01","AS_em8200_002_AL_wait02",
																							"AS_em8200_001_AL_wait01","AS_em8200_002_AL_wait02",
																						"AS_em8200_001_AL_wait01","AS_em8200_002_AL_wait02",
																						"AS_em8200_001_AL_wait01","AS_em8200_002_AL_wait02" });
				})

		.Tick([this](_double TimeDelta)
		{
				if (false == m_pCanvas_MainTalk->Get_End())
				{
					m_bStoryEnd = true;
				}
		})

		.OnExit([this]
		{
		})

		.AddTransition("Intro_01 to BattleStart", "BattleStart")
		.Predicator([this]
			{
				return m_bStoryEnd;
			})

		.AddState("BattleStart")
		.OnStart([this]
			{
				m_pASM->InputAnimSocketOne("FullBody",  "AS_em8200_251_AL_atk_seethrough" );

				Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MainTalk.json");
				m_pCanvas_MainTalk = dynamic_cast<CCanvas_MainTalk*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_NOW, PLAYERTEST_LAYER_FRONTUI, L"Canvas_MainTalk", &json));
				assert(m_pCanvas_MainTalk != nullptr && "Failed to Cloned : CCanvas_MainTalk");

				m_pCanvas_MainTalk->Add_Talk(42);
			})

	
		.OnExit([this]
			{
				m_pASM->SetLerpDuration(m_fDefault_LerpTime);
				m_pKarenMaskEf->GetParams().Ints[0] = 0;

			// 루카 대사 
			CGameManager::GetInstance()->Set_LeftTalk(104);
			CGameManager::GetInstance()->Set_LeftTalk(105);
			CGameManager::GetInstance()->Set_LeftTalk(106);
			CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Item")->TempOff(true);
			CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_ItemMove")->TempOff(true);
			})

				
		.AddTransition("BattleStart to Idle", "Idle")
			.Predicator([this]
				{
					return m_pASM->isSocketPassby("FullBody", 0.99f);
				})

				
				;
}


void CEM8200::AddState_Damaged(CFSMComponentBuilder& Builder)
{
	Builder

	.AddState("Hit_Mid_Heavy")
		.OnStart([this]
			{
				Play_MidHitAnim();
			})
		.Tick([this](_double TimeDelta)
			{
			})

	.AddTransition("Hit_Mid_Heavy to Idle", "Idle")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})

	.AddState("Hit_Heavy")
		.OnStart([this]
			{
				Play_HeavyHitAnim();
			})
		.Tick([this](_double TimeDelta)
			{
			})

	.AddTransition("Hit_Heavy to Idle", "Idle")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
				;
}

_bool CEM8200::Check_PlayerDetected()
{
	if (m_pGameInstance->Check_ObjectAlive(m_pTarget))
	{
		_vector vTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		_vector vThisPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vThisPos));

		if (fDistance < 25.f && m_bStoryModeStart.IsNotDo())
		{
			// Cam Start && Story Start

			return true;
		}
	}
	return false;
}

_bool CEM8200::Check_PlayerDetected_Near()
{
	if (m_pGameInstance->Check_ObjectAlive(m_pTarget))
	{
		_vector vTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
		_vector vThisPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		_float fDistance = XMVectorGetX(XMVector3Length(vTargetPos - vThisPos));

		if (fDistance < 5.f && m_DetectedOnce.IsNotDo())
		{
			Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MainTalk.json");
			m_pCanvas_MainTalk = dynamic_cast<CCanvas_MainTalk*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_NOW, PLAYERTEST_LAYER_FRONTUI, L"Canvas_MainTalk", &json));
			assert(m_pCanvas_MainTalk != nullptr && "Failed to Cloned : CCanvas_MainTalk");

			m_pCanvas_MainTalk->Add_Talk(35);
			m_pCanvas_MainTalk->Add_Talk(36);
			m_pCanvas_MainTalk->Add_Talk(37);
			// m_pCanvas_MainTalk->Add_Talk(38);
			m_pCanvas_MainTalk->Add_Talk(39);
			m_pCanvas_MainTalk->Add_Talk(40);
			m_pCanvas_MainTalk->Add_Talk(41);
			// m_pCanvas_MainTalk->Add_Talk(42);

			return true;
		}
	}

	return false;
}

_bool CEM8200::Check_StoryEnd()
{
	// 스토리 엔드 체크하고 끝나면 연출 캠 종료와 인트로 전투 캠 시작



		return false;
}

void CEM8200::Play_MidHitAnim()
{
	if (m_eSimpleHitFrom == ESimpleAxis::NORTH)
	{
		if(CMathUtils::RandomInt(0,1) == 0)
		{
			m_pASM->InputAnimSocketOne("FullBody", "AS_em8200_401_AL_damage_l_FL");
		}
		else
			m_pASM->InputAnimSocketOne("FullBody", "AS_em8200_401_AL_damage_l_FR");
	}
	else
		m_pASM->InputAnimSocketOne("FullBody", "AS_em8200_402_AL_damage_l_B");
}

void CEM8200::Play_HeavyHitAnim()
{
	switch (m_eHitFrom)
	{
	case EBaseAxis::NORTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em8200_411_AL_damage_h_F");
		break;
	case EBaseAxis::EAST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em8200_414_AL_damage_h_R");
		break;
	case EBaseAxis::SOUTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em8200_412_AL_damage_h_B");
		break;
	case EBaseAxis::WEST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em8200_413_AL_damage_h_L");
		break;
	case EBaseAxis::AXIS_END:
		FALLTHROUGH;
	default:
		NODEFAULT;
	}
}

void CEM8200::Melee_Overlap(const string& pBornName, _uint iDamage, _float fRad, EAttackType eAtkType)
{
	if (m_bMeleeCollStart == false)
		return;

	_float fLength = 0.5f;
	
	_matrix BoneMatrix = m_pModelCom->GetBoneMatrix(pBornName) * m_pTransformCom->Get_WorldMatrix();
	
	_vector vBoneVector = BoneMatrix.r[3];
	_float3 fBone = vBoneVector;
	
	_vector vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vMyLook = XMVectorSetY(vMyLook, 0.f);
	_float3 fDest = vMyLook;
	
	_float3 fFinish = { (fBone.x + fLength * fDest.x), fBone.y, (fBone.z + fLength * fDest.z) };
	
	physx::PxOverlapHit hitBuffer[3];
	physx::PxOverlapBuffer overlapOut(hitBuffer, 3);
	
	SphereOverlapParams param;
	param.fVisibleTime = 0.4f;
	param.iTargetType = CTB_PLAYER;
	param.fRadius = fRad;
	param.vPos = XMVectorSetW(fFinish, 1.f);
	param.overlapOut = &overlapOut;
	
	if (CGameInstance::GetInstance()->OverlapSphere(param))
	{
		HitTargets(overlapOut, iDamage * 0.6f, eAtkType);
	}
}

void CEM8200::Range_Overlap(_float4 vPos, _uint iDamage, _float fRad, EAttackType eAtkType)
{
	if (m_bRangeCollStart == false)
		return;

	physx::PxOverlapHit hitBuffer[3];
	physx::PxOverlapBuffer overlapOut(hitBuffer, 3);

	SphereOverlapParams param;
	param.fVisibleTime = 0.4f;
	param.iTargetType = CTB_PLAYER;
	param.fRadius = fRad;
	param.vPos = vPos;
	param.overlapOut = &overlapOut;

	if (CGameInstance::GetInstance()->OverlapSphere(param))
	{
		HitTargets(overlapOut, iDamage * 0.6f, eAtkType);
	}
}

CEM8200* CEM8200::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEM8200* pInstance = new CEM8200(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM8200");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEM8200::Clone(void* pArg)
{
	CEM8200* pInstance = new CEM8200(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM8200");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM8200::Free()
{
	CEnemy::Free();
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
	Safe_Release(m_pKineticModel);
	Safe_Release(m_pKaren_AnimCam);

	if (m_pDashEF != nullptr)
	{
		m_pDashEF->SetDelete();
		Safe_Release(m_pDashEF);
		m_pDashEF = nullptr;
	}

	if (m_pKarenMaskEf != nullptr)
		m_pKarenMaskEf->SetDelete();

	Safe_Release(m_pKarenMaskEf);

	Safe_Release(m_pLeftCopy);
	Safe_Release(m_pRightCopy);

	Safe_Release(m_pBrainField);
	Safe_Release(m_pBrainCrushCables);
	Safe_Release(m_pChromaticAberration);
}
