#include "stdafx.h"
#include "../public/EM8200.h"

#include <GameUtils.h>

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
	{
		m_iMaxHP = 10000;
		m_iHP = 10000; // ��
		m_iCrushGauge = 10000;
		m_iMaxCrushGauge = 10000;
		m_bHasCrushGauge = false;

		m_iAtkDamage = 50;
		iEemeyLevel = 2;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	m_eEnemyName = EEnemyName::EM8200;
	m_bHasCrushGauge = false;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(180.f));

	Json KarenMask = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/EffectSystem/Monster_Effect/Karen_Mask.json");
	m_pKarenMaskEf = (CEffectSystem*) m_pGameInstance->Clone_GameObject_Get(L"Layer_KarenMask", L"ProtoVFX_EffectSystem", &KarenMask);
	Safe_AddRef(m_pKarenMaskEf);


	return S_OK;
}

void CEM8200::SetUpComponents(void* pArg)
{
	CEnemy::SetUpComponents(pArg);
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em8200", L"Model", (CComponent**)&m_pModelCom));

	// ��Ʈ�ѷ�, prototype�� ����� ���⼭ ��ü�����ϱ� ����
	m_pController = CEM8200_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	// ASM
	m_pASM = CEM8200_AnimInstance::Create(m_pModelCom, this);


	m_HeavyAttackPushTimeline.SetCurve("Simple_Decrease");
}

void CEM8200::SetUpSound()
{
	CEnemy::SetUpSound();
}

void CEM8200::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();
}

void CEM8200::SetUpFSM()
{
	CEnemy::SetUpFSM();

	CFSMComponentBuilder Builder;
	Builder.InitState("Idle");

	AddState_Idle(Builder);
	AddState_Teleport(Builder);
	AddState_Attack_Kick(Builder);
	AddState_Attack_IceNeedle(Builder);
	AddState_Attack_ChaseElec(Builder);
	AddState_Attack_AirElec(Builder);
	AddState_Attack_Rush(Builder);
	AddState_Seethrough(Builder);

	m_pFSM = Builder.Build();
}

void CEM8200::BeginTick()
{
	CEnemy::BeginTick();
}

void CEM8200::Tick(_double TimeDelta)
{
	CEnemy::Tick(TimeDelta);

	

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

	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);

	const _float fMoveSpeed = m_bRun ? 4.f : 2.f;

	if (m_vMoveAxis.LengthSquared() > 0.f)
	{
		const _float fYaw = m_pTransformCom->GetYaw_Radian();
		_float3 vVelocity;
		XMStoreFloat3(&vVelocity, fMoveSpeed * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}

	// Tick�� ���� ���������� �����Ѵ�.
	ResetHitData();
}

void CEM8200::Late_Tick(_double TimeDelta)
{
	CScarletCharacter::Late_Tick(TimeDelta);

	if (m_bVisible)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND_TOON, this);
	}
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
	if (ImGui::CollapsingHeader("ASM"))
	{
		m_pASM->Imgui_RenderState();
	}
	if (ImGui::CollapsingHeader("KarenPivot"))
	{
		static GUIZMO_INFO tp1;
		CImguiUtils::Render_Guizmo(&pivot1, tp1, true, true);
	}

	if (ImGui::CollapsingHeader("KarenProgress"))
	{
		ImGui::InputFloat("LerpProgress_Karen", &m_fAnimProgress);
	}

	if(ImGui::Button("KarenFSM_Recompile"))
	{
		CFSMComponent* pComponent = m_pFSM;

		SetUpFSM();

		Safe_Release(pComponent);
	}
}

void CEM8200::SetUpUI()
{
	__super::SetUpUI();
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
				m_fGravity = 20.f;
			})

	.AddTransition("Idle to Death", "Death")
			.Predicator([this] {return m_bDead; })

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
				return m_eInput == CController::NUM_2;
			})

	.AddTransition("Idle to Teleport_L_Start", "Teleport_L_Start")
		.Predicator([this]
			{
				return m_eInput == CController::NUM_3;
			})

	.AddTransition("Idle to Teleport_R_Start", "Teleport_R_Start")
		.Predicator([this]
			{
				return m_eInput == CController::NUM_4;
			})

	.AddTransition("Idle to KneeKick_A1", "KneeKick_A1")
		.Predicator([this]
			{
				return m_eInput == CController::E;
			})

	.AddTransition("Idle to Air_Elec_Atk_Charge_Start", "Air_Elec_Atk_Charge_Start")
		.Predicator([this]
			{
				return m_eInput == CController::F;
			})

	.AddTransition("Idle to Rush_Copy_Start", "Rush_Copy_Start")
		.Predicator([this]
			{
				return m_eInput == CController::G;
			})

	.AddTransition("Idle to Chase_Elec_Start", "Chase_Elec_Start")
		.Predicator([this]
			{
				return m_eInput == CController::R;
			})

	.AddTransition("Idle to Ice_Needle_Atk_Start", "Ice_Needle_Atk_Start")
		.Predicator([this]
			{
				return m_eInput == CController::Q;
			})

	.AddTransition("Idle to Zero_Sas", "Zero_Sas")
		.Predicator([this]
			{
				return m_eInput == CController::C;
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
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_130_AL_dodge_F_start");
			})
		.Tick([this](_double TimeDelta)
		{
				SocketLocalMove(m_pASM);
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
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_131_AL_dodge_F_stop");
			})
		.Tick([this](_double TimeDelta)
			{
				SocketLocalMove(m_pASM);
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
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_135_AL_dodge_B_start");
			})
		.Tick([this](_double TimeDelta)
					{
						SocketLocalMove(m_pASM);
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
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_145_AL_dodge_L_start");
			})
		.Tick([this](_double TimeDelta)
			{
				SocketLocalMove(m_pASM);
			})

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
			})
		.Tick([this](_double TimeDelta)
			{
				SocketLocalMove(m_pASM);
			})

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
	.AddState("KneeKick_A1")
		.OnStart([this]
		{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_201_AL_atk_kick_a1");
		})

	.AddTransition("KneeKick_A1 to FrontKick_A2", "FrontKick_A2")
		.Predicator([this]
		{
				return m_pASM->isSocketPassby("FullBody", m_fAnimProgress);

			// A1 Ÿ���� �����ߴ��� ���� ���� 
		})
	// ~KneeKick_A1

	// FrontKick_A2
	.AddState("FrontKick_A2")
		.OnStart([this]
			{
				m_pASM->SetLerpDuration(m_fLerpTime);
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_202_AL_atk_kick_a2");
			})
		.OnExit([this]
		{
				m_pASM->SetLerpDuration(m_fDefault_LerpTime);
		})

	.AddTransition("FrontKick_A2 to TurnKick_A3", "TurnKick_A3")
		.Predicator([this]
			{
				return m_pASM->isSocketPassby("FullBody", m_fAnimProgress);
			})
	// ~FrontKick_A2

	// TurnKick_A3
	.AddState("TurnKick_A3")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_203_AL_atk_kick_a3");
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
			})

	.AddTransition("Ice_Needle_Atk_Fire_Start to Ice_Needle_Atk_Fire_Loop", "Ice_Needle_Atk_Fire_Loop")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Ice Needle Fire Start

	// Ice Needle Fire Loop
	.AddState("Ice_Needle_Atk_Fire_Loop")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_234_AL_atk_fire_loop");
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
	.AddState("Chase_Elec_Start")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_241_AL_atk_chase_charge");

			// �տ� ����Ʈ �ޱ�
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
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_243_AL_atk_chase_startL");
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
			m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_248_AL_atk_chase_startR");
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
	.AddState("Air_Elec_Atk_Charge_Start")
	.OnStart([this]
		{
			m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_211_AL_atk_air_chg_start");

			// �տ� ����Ʈ �ޱ�
		})

	.AddTransition("Air_Elec_Atk_Charge_Start to Air_Elec_Atk_Charge_Loop", "Air_Elec_Atk_Charge_Loop")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Air_Elec_Atk_Charge_Start

	// Chase_Elec_Start_Charge Loop 
	.AddState("Air_Elec_Atk_Charge_Loop")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_212_AL_atk_air_chg_loop");
			})

	.AddTransition("Air_Elec_Atk_Charge_Loop to Air_Elec_Atk_Fire_Start", "Air_Elec_Atk_Fire_Start")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Chase_Elec_Start_Charge Loop

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

	.AddTransition("Air_Elec_Atk_Fire_Start_Loop to Air_Elec_Atk_Fall", "Air_Elec_Atk_Fall")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Air_Elec_Atk_Fire_Start_Loop

	// Air_Elec_Atk_Fall
	.AddState("Air_Elec_Atk_Fall")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_215_AL_atk_air_fall");
			})

	.AddTransition("Air_Elec_Atk_Fall to Air_Elec_Atk_Landing_Start", "Air_Elec_Atk_Landing_Start")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
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
	.AddState("Rush_Copy_Start")
		.OnStart([this]
			{
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

	.AddTransition("Rush_Crouch_Loop to Rush_Rush_Start", "Rush_Rush_Start")
			.Predicator([this]
				{
					return m_pASM->isSocketEmpty("FullBody");
				})
	// ~Rush_Crouch_Loop

	// Rush_Rush_Start
	.AddState("Rush_Rush_Start")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_225_AL_atk_rush_start");
			})

	.AddTransition("Rush_Rush_Start to Rush_Rush_Loop", "Rush_Rush_Loop")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Rush_Rush_Start

	// Rush_Rush_Loop
	.AddState("Rush_Rush_Loop")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_226_AL_atk_rush_loop");
			})

	.AddTransition("Rush_Rush_Loop to Rush_Rush_End", "Rush_Rush_End")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Rush_Rush_Loop

	// Rush_Rush_End
	.AddState("Rush_Rush_End")
		.OnStart([this]
		{
			m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_227_AL_atk_rush_end");
		})

	.AddTransition("Rush_Rush_End to Idle", "Idle")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			})
	// ~Rush_Rush_End

				; 
}

void CEM8200::AddState_Seethrough(CFSMComponentBuilder& Builder)
{
	Builder
	.AddState("Zero_Sas")
		.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em8200_251_AL_atk_seethrough");

			// �̺�Ʈ �ɱ� 
			})

	.AddTransition("Zero_Sas to Idle", "Idle")
		.Predicator([this]
			{
				return m_pASM->isSocketEmpty("FullBody");
			});
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
				if (m_eCurAttackType == EAttackType::ATK_MIDDLE)
				{
					Play_MidHitAnim();
				}

				// _float fPower;
				// if (m_HeavyAttackPushTimeline.Tick(TimeDelta, fPower))
				// {
				// 	_float3 vVelocity = { m_vPushVelocity.x, m_vPushVelocity.y, m_vPushVelocity.z };
				// 	m_pTransformCom->MoveVelocity(TimeDelta, vVelocity * fPower);
				// }
			})
	.AddTransition("Hit_Mid_Heavy to Idle", "Idle")
		.Predicator([this]
			{
				return m_bDead || m_pASM->isSocketEmpty("FullBody");
			})



	.AddState("Hit_Heavy")
		.OnStart([this]
			{
				Play_HeavyHitAnim();
			})
		.Tick([this](_double TimeDelta)
			{
				if (m_eCurAttackType == EAttackType::ATK_HEAVY)
				{
					Play_HeavyHitAnim();
				}

				// _float fPower;
				// if (m_HeavyAttackPushTimeline.Tick(TimeDelta, fPower))
				// {
				// 	_float3 vVelocity = { m_vPushVelocity.x, m_vPushVelocity.y, m_vPushVelocity.z };
				// 	m_pTransformCom->MoveVelocity(TimeDelta, vVelocity * fPower);
				// }
			})

	.AddTransition("Hit_Mid_Heavy to Idle", "Idle")
		.Predicator([this]
			{
				return m_bDead || m_pASM->isSocketEmpty("FullBody");
			})
				;
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
	Safe_Release(m_pKarenMaskEf);
}
