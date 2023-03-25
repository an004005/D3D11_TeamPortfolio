#include "stdafx.h"
#include "..\public\EM1100.h"
#include <FSMComponent.h>
#include "JsonStorage.h"
#include "RigidBody.h"
#include "EM1100_AnimInstance.h"
#include "EM1100_Controller.h"
#include "EnemyBullet.h"
CEM1100::CEM1100(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
	m_eMonsterName = EEnemyName::EM0700;
}

CEM1100::CEM1100(const CEM1100 & rhs)
	: CEnemy(rhs)
{
}

HRESULT CEM1100::Initialize(void * pArg)
{
	//Json em0200_json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/FlowerLeg/FlowerLegTrigger.json");
	//pArg = &em0200_json;

	/*m_strDeathSoundTag = "mon_5_fx_death";
	m_strImpactVoiceTag = "mon_5_impact_voice";*/

	// ��ġ������ ������ �� �ְ� �ϱ�
	{
		m_iMaxHP = 3000;
		m_iHP = 3000; // ��

		m_iAtkDamage = 50;
		iMonsterLevel = 2;
	}

	FAILED_CHECK(CEnemy::Initialize(pArg));

	m_eMonsterName = EEnemyName::EM1100;
	m_bHasCrushGage = true;
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(120.f));

	m_fGravity = 0.f;
	return S_OK;
}

void CEM1100::SetUpComponents(void * pArg)
{
	CEnemy::SetUpComponents(pArg);
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_em1100", L"Model", (CComponent**)&m_pModelCom));

	// ���� �浹ü(�÷��̾ ���� ���� �� �ö󰡰��Ѵ�.)
	/*Json FlowerLegRangeCol = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/FlowerLeg/FlowerLegRange.json");*/

	FAILED_CHECK(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("RangeColl"),
		(CComponent**)&m_pRange, pArg));

	FAILED_CHECK(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("BodyColl"),
		(CComponent**)&m_pBody))
		

	// ��Ʈ�ѷ�, prototype�� ����� ���⼭ ��ü�����ϱ� ����
	m_pController = CEM1100_Controller::Create();
	m_pController->Initialize(nullptr);
	m_Components.emplace(L"Controller", m_pController);
	Safe_AddRef(m_pController);
	m_pController->SetOwner(this);

	// ASM
	m_pASM = CEM1100_AnimInstance::Create(m_pModelCom, this);
}

void CEM1100::SetUpSound()
{
	CEnemy::SetUpSound();
}

void CEM1100::SetUpAnimationEvent()
{
	CEnemy::SetUpAnimationEvent();
}

void CEM1100::SetUpFSM()
{
	CEnemy::SetUpFSM();

	/*
	W : waterattack
	E : Electricball
	S : Stamp
	R : Rush
	T : TailSwing

	SHIFT : Dodge
	SPACE : DodgeTurn
	*/

	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.OnStart([this]
			{
				m_fGravity = 20.f;
			})
			.AddTransition("Idle to Death" , "Death")
				.Predicator([this] { return m_bDead; })

			.AddTransition("Idle to Hit_Mid_Heavy", "Hit_Mid_Heavy")
				.Predicator([this] { return
					m_eCurAttackType == EAttackType::ATK_HEAVY
					|| m_eCurAttackType == EAttackType::ATK_MIDDLE
					|| m_eCurAttackType == EAttackType::ATK_SPECIAL_END; })
	
			.AddTransition("Idle to Dodge_Start", "Dodge_Start")
				.Predicator([this] { return m_eInput == CController::SHIFT; })

			.AddTransition("Idle to WaterAttack_Omen", "WaterAttack_Omen")
				.Predicator([this] { return m_eInput == CController::W; })

			.AddTransition("Idle to ElectricBall_Omen", "ElectricBall_Omen")
				.Predicator([this] { return m_eInput == CController::E; })

			.AddTransition("Idle to Stamp_Omen", "Stamp_Omen")
				.Predicator([this] { return m_eInput == CController::S; })

			.AddTransition("Idle to Rush_Omen", "Rush_Omen")
				.Predicator([this] { return m_eInput == CController::R; })

			.AddTransition("Idle to TailSwing", "TailSwing")
				.Predicator([this] { return m_eInput == CController::T; })
			
///////////////////////////////////////////////////////////////////////////////////////////
	
			.AddState("Hit_Mid_Heavy")
				.OnStart([this]
			{
				Play_MidHitAnim();
				HeavyAttackPushStart();
			})
			.Tick([this](_double TimeDelta)
			{
				if (m_eCurAttackType == EAttackType::ATK_HEAVY
					|| m_eCurAttackType == EAttackType::ATK_MIDDLE
					|| m_eCurAttackType == EAttackType::ATK_SPECIAL_END)
				{
					HeavyAttackPushStart();
					Play_MidHitAnim();
				}

				_float fPower;
				if (m_HeavyAttackPushTimeline.Tick(TimeDelta, fPower))
				{
					_float3 vVelocity = { m_vPushVelocity.x, m_vPushVelocity.y, m_vPushVelocity.z };
					m_pTransformCom->MoveVelocity(TimeDelta, vVelocity * fPower);
					//m_pTransformCom->MoveVelocity(TimeDelta, m_vPushVelocity * fPower);
				}
			})
			.AddTransition("Hit_Mid_Heavy to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead
						|| m_pASM->isSocketPassby("FullBody", 0.95f)
						|| m_eCurAttackType == EAttackType::ATK_TO_AIR
						|| m_eCurAttackType == EAttackType::ATK_SPECIAL_LOOP;
				})
		.AddState("Death")
				.OnStart([this]
				{
					m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_474_AL_dead_down02");
				})


		.AddState("OnFloorGetup")
			.OnStart([this]
			{
				m_pASM->InputAnimSocketMany("FullBody", { "AS_em0700_433_AL_blow_landing_F", "AS_em0700_427_AL_getup" });
				m_fGravity = 20.f;
			})
			.AddTransition("OnFloorGetup to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_eCurAttackType != EAttackType::ATK_END || m_pASM->isSocketEmpty("FullBody");
				})

///////////////////////////////////////////////////////////////////////////////////////////

		//���۰� ���� ������ ����
		.AddState("Dodge_Start")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_135_AL_dodge_B_start");
			})
			.Tick([this](_double TimeDelta)
			{
				//�ڷ� �پ����
			})
			.OnExit([this] 
			{
			})
			.AddTransition("Dodge_Start to Dodge_End", "Dodge_End")
				.Predicator([this]
				{
					return  m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

				//����. ������x
		.AddState("Dodge_End")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_136_AL_dodge_B_stop");
				})
	
				.AddTransition("Dodge_End to Idle", "Idle")
					.Predicator([this]
					{
						return  m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

///////////////////////////////////////////////////////////////////////////////////////////

		//�´� ����
		.AddState("WaterAttack_Omen")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_215_AL_atk_a5_drainage_omen");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("WaterAttack_Omen to WaterAttack_Start", "WaterAttack_Start")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("WaterAttack_Start")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_215_AL_atk_a5_drainage_start");
				})
				.Tick([this](_double)
				{
					SocketLocalMove(m_pASM);
				})
				.OnExit([this]
				{
					m_pASM->ClearSocketAnim("FullBody", 0.f);
				})
				.AddTransition("WaterAttack_Start to WaterAttack_End", "WaterAttack_End")
					.Predicator([this]
					{
						return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

			.AddState("WaterAttack_End")
				.OnStart([this]
				{
					m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_215_AL_atk_a5_drainage_end");
				})
				.Tick([this](_double)
				{
					SocketLocalMove(m_pASM);
				})
				.OnExit([this]
				{
					m_pASM->ClearSocketAnim("FullBody", 0.f);
				})
				.AddTransition("WaterAttack_End to Idle", "Idle")
					.Predicator([this]
					{
						return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
					})

		//�´� �������� , ���⺼�� �÷��̾ �Ѿư�����
		.AddState("ElectricBall_Omen")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_208_AL_atk_a3_guidanceshot_omen");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("ElectricBall_Omen to ElectricBall_Start", "ElectricBall_Start")
				.Predicator([this]
			{
				return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
			})

		.AddState("ElectricBall_Start")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_208_AL_atk_a3_guidanceshot_start");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("ElectricBall_Start to ElectricBall_End", "ElectricBall_End")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("ElectricBall_End")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_208_AL_atk_a3_guidanceshot_end");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("ElectricBall_End to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})


		//�´� ������ ���ݾ� ����
		.AddState("Stamp_Omen")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_202_AL_atk_a1_bringdown_omen");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Stamp_Omen to Stamp_Start", "Stamp_Start")
				.Predicator([this]
			{
				return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
			})

		.AddState("Stamp_Start")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_202_AL_atk_a1_bringdown_start");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Stamp_Start to Stamp_End", "Stamp_End")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Stamp_End")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_202_AL_atk_a1_bringdown_end");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Stamp_End to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})


		//start�� end�� ��������
		.AddState("Rush_Omen")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1120_211_AL_atk_a4_sliding_omen");
			})

			.AddTransition("Rush_Omen to Rush_Start", "Rush_Start")
				.Predicator([this]
			{
				return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
			})

		.AddState("Rush_Start")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1120_211_AL_atk_a4_sliding_start");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Rush_Start to Rush_End", "Rush_End")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		.AddState("Rush_End")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1120_211_AL_atk_a4_sliding_end");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("Rush_End to Idle", "Idle")
				.Predicator([this]
				{
					return m_bDead || m_pASM->isSocketPassby("FullBody", 0.95f);
				})

		//����ġ�� �� ���¸�Ǳ��� ���� ���� ����
		.AddState("TailSwing")
			.OnStart([this]
			{
				m_pASM->AttachAnimSocketOne("FullBody", "AS_em1100_204_AL_atk_a2_tailall");
			})
			.Tick([this](_double)
			{
				SocketLocalMove(m_pASM);
			})
			.OnExit([this]
			{
				m_pASM->ClearSocketAnim("FullBody", 0.f);
			})
			.AddTransition("TailSwing to Idle", "Idle")
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

		.Build();
}

void CEM1100::BeginTick()
{
	CEnemy::BeginTick();
}

void CEM1100::Tick(_double TimeDelta)
{
	CEnemy::Tick(TimeDelta);

	//AIInstance tick
	m_pController->SetTarget(m_pTarget);

	if (m_bDead == false)
		m_pController->Tick(TimeDelta);
	else
		m_pController->Invalidate();

	//���� ������Ʈ
	m_vMoveAxis = m_pController->GetMoveAxis();
	m_vMoveAxis.Normalize();
	m_eInput = m_pController->GetAIInput();
	m_bRun = m_pController- IsRun();

	//ASM, FSM tick
	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);


	if (m_vMoveAxis.LengthSquared() > 0.f)
	{
		_float3 vVelocity;

		if (m_bRun)
		{
			const _float fYaw = m_pTransformCom->GetYaw_Radian();
			XMStoreFloat3(&vVelocity, 6.f /*MoveSpeed*/ * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
		}
		else
			XMStoreFloat3(&vVelocity, 2.f /*MoveSpeed*/ * XMVector3Normalize(m_vMoveAxis));

		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}

	// Tick�� ���� ���������� �����Ѵ�.
	ResetHitData();
}

void CEM1100::Late_Tick(_double TimeDelta)
{
	CEnemy::Late_Tick(TimeDelta);
}

void CEM1100::AfterPhysX()
{
	CEnemy::AfterPhysX();
}

HRESULT CEM1100::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CEM1100::Imgui_RenderProperty()
{
	CEnemy::Imgui_RenderProperty();
	if (ImGui::CollapsingHeader("ASM"))
	{
		m_pASM->Imgui_RenderState();
	}
	m_pFSM->Imgui_RenderProperty();
}

_bool CEM1100::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("FullBody") == false;
}

//void CEM1100::Play_LightHitAnim()
//{
//	if (m_eSimpleHitFrom == ESimpleAxis::NORTH)
//		m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_401_AL_damage_l_F");
//	else
//		m_pASM->InputAnimSocketOne("FullBody", "AS_em0700_402_AL_damage_l_B");
//}

void CEM1100::Play_MidHitAnim()
{
	switch (m_eHitFrom)
	{
	case EBaseAxis::NORTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em1100_411_AL_damage_m_F");
		break;
	case EBaseAxis::EAST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em1100_414_AL_damage_m_R");
		break;
	case EBaseAxis::SOUTH:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em1100_412_AL_damage_m_B");
		break;
	case EBaseAxis::WEST:
		m_pASM->InputAnimSocketOne("FullBody", "AS_em1100_413_AL_damage_m_L");
		break;
	case EBaseAxis::AXIS_END:
		FALLTHROUGH;
	default:
		NODEFAULT;
	}
}

void CEM1100::HeavyAttackPushStart()
{
	if (m_eCurAttackType == EAttackType::ATK_MIDDLE || m_eCurAttackType == EAttackType::ATK_HEAVY || m_eCurAttackType == EAttackType::ATK_SPECIAL_END)
	{
		m_HeavyAttackPushTimeline.PlayFromStart();
		m_vPushVelocity = CClientUtils::GetDirFromAxis(m_eHitFrom);
		m_vPushVelocity *= -4.f; // ���� �� ������ �ݴ�� �̵�

		const _float fYaw = m_pTransformCom->GetYaw_Radian();
		m_vPushVelocity = XMVector3TransformNormal(m_vPushVelocity, XMMatrixRotationY(fYaw));
	}
}

_bool CEM1100::IsTargetFront()
{
	_vector vTargetPos = m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	_vector vMyLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	_float fAngle = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vMyLook), XMVector3Normalize(vTargetPos - vMyPos)));

	//0~ 90�� ����(����)
	if (fAngle > 0)
		return true;
	else
		return false;

}


CEM1100 * CEM1100::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEM1100* pInstance = new CEM1100(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM1100");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEM1100::Clone(void * pArg)
{
	CEM1100*		pInstance = new CEM1100(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM1100");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEM1100::Free()
{
	CEnemy::Free();
	Safe_Release(m_pASM);
	Safe_Release(m_pController);
	Safe_Release(m_pRange);
	Safe_Release(m_pBody);
}
