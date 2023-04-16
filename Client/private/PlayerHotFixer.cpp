#include "stdafx.h"
#include "PlayerHotFixer.h"
#include "Player.h"
#include "FSMComponent.h"
#include "BaseAnimInstance.h"
#include "SAS_Portrait.h"
#include "PlayerInfoManager.h"
#include "Enemy.h"
#include "AnimCam.h"
#include "Camera_Player.h"
#include "EM0210.h"
#include "EM0700.h"
#include "EM0800.h"
#include "Channel.h"
#include "Bone.h"
#include "CamSpot.h"
#include "ScarletWeapon.h"
#include "Material.h"
#include "MapKinetic_Object.h"
#include "BrainField.h"
#include "Weapon_Player.h"
#include "Sheath_Player.h"
#include "Transform.h"
#include "BaseAnimInstance.h"
#include "ImguiUtils.h"
#include "Camera_Player.h"
#include "EM8200.h"

CPlayerHotFixer::CPlayerHotFixer()
{
}

HRESULT CPlayerHotFixer::Initialize(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;

	m_pPlayer->m_pModel->Set_AdditiveAnim("AS_ch0100_490_AL_damage_add");

	m_ActionCamStart.SetCurve("ActionCam_4to2");

	return S_OK;
}

void CPlayerHotFixer::Tick()
{
	if (ImGui::CollapsingHeader("HotFixer"))
	{
		//ImGui::Checkbox("Hanabi", &m_bHanabiActive);
		//ImGui::Checkbox("Tsugumi", &m_bTsugumiActive);
		//CPlayerInfoManager::GetInstance()->Hanabi_Active(m_bHanabiActive);
		//CPlayerInfoManager::GetInstance()->Tsugumi_Active(m_bTsugumiActive);

		if (ImGui::Button("MaskEffect"))
		{
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, L"Sas_DriveMode_Effect")->Start_Attach(m_pPlayer, "Mask", true);
		}

		if (ImGui::Button("ActionCam_Escape"))
		{
			m_pPlayer->m_pCamSpot->Reset_CamMod();
		}

		ImGui::SliderFloat("CamDistance", &m_fCamDistance, 1.f, 10.f);
		if (1.f <= m_fCamDistance && 10.f >= m_fCamDistance)
		{
			CPlayerInfoManager::GetInstance()->SetPlayerCamDistance(m_fCamDistance);
		}

		if (ImGui::Button("BrainCrash_Recompile"))
		{
			BrainCrashStateMachine_ReCompoile();
		}
		if (ImGui::Button("BrainField_Recompile"))
		{
			BrainFieldStateMachine_ReCompile();
		}
		if (ImGui::Button("Debuff_Recompile"))
		{
			DeBuffStateMachine_ReCompile();
		}
		if (ImGui::Button("Hit_Recompile"))
		{
			HitStateMachine_ReCompile();
		}
		if (ImGui::Button("Something_Recompile"))
		{
			Player_Something_Update();
		}

		if (ImGui::Button("wp0106"))
		{
			for (auto& iter : m_pPlayer->m_vecWeapon)
			{
				static_cast<CWeapon_Player*>(iter)->Change_Weapon(WP_0106);
			}
			for (auto& iter : m_pPlayer->m_vecSheath)
			{
				static_cast<CSheath_Player*>(iter)->Change_Sheath(WP_0106);
			}
		}
		if (ImGui::Button("wp0126"))
		{
			for (auto& iter : m_pPlayer->m_vecWeapon)
			{
				static_cast<CWeapon_Player*>(iter)->Change_Weapon(WP_0126);
			}
			for (auto& iter : m_pPlayer->m_vecSheath)
			{
				static_cast<CSheath_Player*>(iter)->Change_Sheath(WP_0126);
			}
		}
		if (ImGui::Button("wp0190"))
		{
			for (auto& iter : m_pPlayer->m_vecWeapon)
			{
				static_cast<CWeapon_Player*>(iter)->Change_Weapon(WP_0190);
			}
			for (auto& iter : m_pPlayer->m_vecSheath)
			{
				static_cast<CSheath_Player*>(iter)->Change_Sheath(WP_0190);
			}
		}

		if (ImGui::Button("DMG_Light"))
		{
			m_pPlayer->m_bHit = true;
			m_pPlayer->m_DamageDesc.m_iDamage = 1;
			m_pPlayer->m_DamageDesc.m_iDamageType = EAttackType::ATK_LIGHT;
			m_pPlayer->m_DamageDesc.m_vHitDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
			m_pPlayer->m_DamageDesc.m_eHitDir = CClientUtils::GetDamageFromAxis(m_pPlayer->m_pTransformCom, XMVectorSet(0.f, 0.f, 1.f, 0.f));
		}
		if (ImGui::Button("DMG_Middle"))
		{
			m_pPlayer->m_bHit = true;
			m_pPlayer->m_DamageDesc.m_iDamage = 1;
			m_pPlayer->m_DamageDesc.m_iDamageType = EAttackType::ATK_MIDDLE;
			m_pPlayer->m_DamageDesc.m_vHitDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
			m_pPlayer->m_DamageDesc.m_eHitDir = CClientUtils::GetDamageFromAxis(m_pPlayer->m_pTransformCom, XMVectorSet(0.f, 0.f, 1.f, 0.f));
		}
		if (ImGui::Button("DMG_Heavy"))
		{
			m_pPlayer->m_bHit = true;
			m_pPlayer->m_DamageDesc.m_iDamage = 1;
			m_pPlayer->m_DamageDesc.m_iDamageType = EAttackType::ATK_HEAVY;
			m_pPlayer->m_DamageDesc.m_vHitDir = XMVectorSet(0.f, 0.f, 1.f, 0.f);
			m_pPlayer->m_DamageDesc.m_eHitDir = CClientUtils::GetDamageFromAxis(m_pPlayer->m_pTransformCom, XMVectorSet(0.f, 0.f, 1.f, 0.f));
		}

		if (ImGui::Button("FSM_Escape"))
		{
			m_pPlayer->m_bHit = false;
			ZeroMemory(&m_pPlayer->m_DamageDesc, sizeof(m_pPlayer->m_DamageDesc));

			m_pPlayer->m_pHitStateMachine->SetState("NON_HIT");
			m_pPlayer->m_pKineticComboStateMachine->SetState("KINETIC_COMBO_NOUSE");
			m_pPlayer->m_pKineticStataMachine->SetState("NO_USE_KINETIC");
			m_pPlayer->m_pJustDodgeStateMachine->SetState("JUSTDODGE_NONUSE");
			m_pPlayer->m_pTrainStateMachine_Left->SetState("TRAIN_LEFT_NOUSE");
			m_pPlayer->m_pTelephonePoleStateMachine_Left->SetState("TELEPHONEPOLE_LEFT_NOUSE");
			m_pPlayer->m_pHBeamStateMachine_Left->SetState("HBEAM_LEFT_NOUSE");
			m_pPlayer->m_pDropObjectStateMachine->SetState("DROP_NOUSE");
			m_pPlayer->m_pTeleportStateMachine->SetState("TELEPORTATTACK_NOUSE");
			m_pPlayer->m_pTankLorryStateMachine->SetState("TANKLORRY_NOUSE");
			m_pPlayer->m_pIronBarsStateMachine->SetState("IRONBARS_NOUSE");
			m_pPlayer->m_pContainerStateMachine->SetState("CONTAINER_NOUSE");
			m_pPlayer->m_pTeleportStateMachine->SetState("TELEPORTATTACK_NOUSE");
			m_pPlayer->m_pDriveModeProductionStateMachine->SetState("DRIVEMODE_NOUSE");
			m_pPlayer->m_pBrainFieldKineticStateMachine->SetState("BF_NO_USE_KINETIC");
			m_pPlayer->m_pBrainFieldKineticComboStateMachine->SetState("BF_NO_USE_KINETIC_COMBO");
			m_pPlayer->m_pBrainFieldAttackStateMachine->SetState("NO_USE_BRAINFIELD");
			m_pPlayer->m_pBrainFieldFallStateMachine->SetState("BF_NO_USE_KINETIC_FALL");
			m_pPlayer->m_pBrainFieldProductStateMachine->SetState("BRAINFIELD");

			m_pPlayer->m_pASM->ClearAnimSocket();
		}

		if (ImGui::Button("Fire"))
			m_pPlayer->m_eDeBuff = EDeBuffType::DEBUFF_FIRE;

		if (ImGui::Button("Water"))
			m_pPlayer->m_eDeBuff = EDeBuffType::DEBUFF_WATER;

		if (ImGui::Button("Oil"))
			m_pPlayer->m_eDeBuff = EDeBuffType::DEBUFF_OIL;

		if (ImGui::Button("Thunder"))
			m_pPlayer->m_eDeBuff = EDeBuffType::DEBUFF_THUNDER;

		if (ImGui::Button("Heal"))
			CPlayerInfoManager::GetInstance()->Change_PlayerHP(CHANGE_INCREASE, 100);

		if (ImGui::Button("Arrange_Cam"))
		{
			m_pPlayer->m_pCamSpot->Arrange_Cam();
		}

		if (ImGui::CollapsingHeader("Pivot"))
		{
			static GUIZMO_INFO tp1;
			CImguiUtils::Render_Guizmo(&m_Pivot, tp1, true, true);
		}

		if (ImGui::Button("ArmEffect"))
		{
			//CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, L"BrainField_Before_Gear_EF")->Start_AttachPivot(m_pPlayer, m_Pivot, "LeftForeArmRoll1", true, true);

			/*CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, L"BrainField_Before_Gear_EF")->Start_AttachPivot(m_pPlayer, m_Pivot, "LeftForeArmRoll1", true, true);
			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, L"BrainField_Before_Ring_EF")->Start_AttachPivot(m_pPlayer, m_Pivot, "LeftForeArmRoll1", true, true);*/
		}

		//LeftForeArmRoll1

		ImGui::Checkbox("BrainMap_Combo4", &m_bBrainMap_Combo4);
		ImGui::Checkbox("BrainMap_Air", &m_bBrainMap_Air);
		ImGui::Checkbox("BrainMap_BF", &m_bBrainMap_BF);
		CPlayerInfoManager::GetInstance()->Set_BrainMap(EBRAINMAP::BRAINMAP_KINETIC_COMBO_4, m_bBrainMap_Combo4);
		CPlayerInfoManager::GetInstance()->Set_BrainMap(EBRAINMAP::BRAINMAP_KINETIC_COMBO_AIR, m_bBrainMap_Air);
		CPlayerInfoManager::GetInstance()->Set_BrainMap(EBRAINMAP::BRAINMAP_BRAINFIELD_HARDBODY, m_bBrainMap_BF);


		//if (ImGui::Button("PointLightTest"))
		//{
		//	CGameInstance::GetInstance()->AddLifePointLight(1.f, m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + XMVectorSet(0.f, 1.f, 0.f, 0.f), 10.f, { 0.145098f, 0.01f, 0.2f, 1.f });
		//}

		m_pPlayer->m_pBrainFieldProductStateMachine->Imgui_RenderProperty();


		static _bool bAiLock = false;
		ImGui::Checkbox("AILOCK", &bAiLock);
		CPlayerInfoManager::GetInstance()->SetAILock(bAiLock);

	}
	ImGui::CollapsingHeader("~HotFixer");
}

void CPlayerHotFixer::BrainCrashStateMachine_ReCompoile()
{
	CFSMComponent* pOriginFSM = m_pPlayer->m_pBrainCrashStateMachine;

	m_pPlayer->m_pBrainCrashStateMachine = CFSMComponentBuilder()
		.InitState("BRAINCRASH_NOUSE")

		.AddState("BRAINCRASH_NOUSE")
		.OnStart([&]()
		{
			m_pPlayer->m_pASM->ClearAnimSocket("BrainCrash_AnimSocket");
			//m_pPlayer->m_bBrainCrash = false;
		})
		.Tick([&](double fTimeDelta) 
		{
			//m_pPlayer->m_bBrainCrash = false;
		})
		.OnExit([&]()
		{
			m_pPlayer->m_pASM->SetCurState("IDLE");
			m_pPlayer->SetAbleState({ false, false, false, false, false, true, true, true, true, false });
		})
			.AddTransition("BRAINCRASH_NOUSE to BRAINCRASH_EM8200_CUTSCENE", "BRAINCRASH_EM8200_CUTSCENE")
			.Predicator([&]()->_bool 
			{ 
				//Monster_em8200

				/*if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster()) return false;
				if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetPrototypeTag() != L"Monster_em8200") return false;
				return m_pPlayer->m_bBrainCrashInput && static_cast<CEnemy*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->CanBC();*/

				return m_pPlayer->m_bBrainCrashInput;
			})
			.Priority(0)

			.AddTransition("BRAINCRASH_NOUSE to BRAINCRASH_CUTSCENE", "BRAINCRASH_CUTSCENE")
			.Predicator([&]()->_bool 
			{ 
				if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster()) return false;
				return m_pPlayer->m_bBrainCrashInput && static_cast<CEnemy*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->CanBC();
			})
			.Priority(1)

		.AddState("BRAINCRASH_CUTSCENE")
		.OnStart([&]()
		{
			m_pPlayer->m_bBrainCrash = true;
			m_pPlayer->m_pSasPortrait->Start_SAS(ESASType::SAS_NOT);
		})
		.Tick([&](double fTimeDelta) 
		{

		})
		.OnExit([&]()
		{
		})
		.AddTransition("BRAINCRASH_CUTSCENE to BRAINCRASH_ACTIVATE", "BRAINCRASH_ACTIVATE")
			.Predicator([&]()->_bool { return m_pPlayer->m_pSasPortrait->isFinish(); })
			.Priority(0)

		.AddState("BRAINCRASH_ACTIVATE")
		.OnStart([&]()
		{
			CGameObject* pTarget = nullptr;

			if (pTarget = CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				_float4 vTargetPos = pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				_float4 vDistance = XMLoadFloat4(&vTargetPos) - m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float fDistance = vDistance.Length();

				if (pTarget->GetPrototypeTag() == L"Monster_em210")
				{
					m_pPlayer->m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_pPlayer->m_BrandCrash_em0200);
					static_cast<CEM0210*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->PlayBC();

					if (5.f >= fDistance)
					{
						auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("em0210_brainCrash");
						m_pPlayer->m_pPlayer_AnimCam->StartCamAnim_Return_Update(pCamAnim, m_pPlayer->m_pPlayerCam, m_pPlayer->m_pTransformCom, 0.f, 0.f);
						//m_pPlayer_AnimCam->StartCamAnim_Return_Update(pCamAnim, CPlayerInfoManager::GetInstance()->Get_PlayerCam(), m_pTransformCom, 0.f, 0.f);

						_vector BC_Pos = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (XMVector3Normalize(m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 3.f);
						_vector vPlayerPos = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
						pTarget->GetTransform()->LookAt_NonY(vPlayerPos);
						pTarget->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, BC_Pos);
					}
					else
					{
						auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("BrainCrush01");
						m_pPlayer->m_pPlayer_AnimCam->StartCamAnim_Return_Update(pCamAnim, m_pPlayer->m_pPlayerCam, m_pPlayer->m_pTransformCom, 0.f, 0.f);
						m_pPlayer->m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_pPlayer->m_BrainCrash_Activate);
					}
				}
				else if (pTarget->GetPrototypeTag() == L"Monster_em700")
				{
					m_pPlayer->m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_pPlayer->m_BrainCrash_em0700);
					static_cast<CEM0700*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->PlayBC();

					if (5.f >= fDistance)
					{
						_vector BC_Pos = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (XMVector3Normalize(m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 3.f);
						_vector vPlayerPos = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
						pTarget->GetTransform()->LookAt_NonY(BC_Pos);
						pTarget->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, BC_Pos);
					}
					else
					{
						auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("BrainCrush01");
						m_pPlayer->m_pPlayer_AnimCam->StartCamAnim_Return_Update(pCamAnim, m_pPlayer->m_pPlayerCam, m_pPlayer->m_pTransformCom, 0.f, 0.f);
						m_pPlayer->m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_pPlayer->m_BrainCrash_Activate);
					}
				}
				else if (pTarget->GetPrototypeTag() == L"Monster_em800")
				{
					m_pPlayer->m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_pPlayer->m_BrainCrash_em0800);
					static_cast<CEM0800*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->PlayBC();

					_vector BC_Pos = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (XMVector3Normalize(m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 8.5f);
					_vector vPlayerPos = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					pTarget->GetTransform()->LookAt_NonY(vPlayerPos);
					pTarget->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, BC_Pos);
				}

				else if (pTarget->GetPrototypeTag() == L"Monster_em1100")
				{
					m_pPlayer->m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_pPlayer->m_BrainCrash_em1100);
					static_cast<CEM0800*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->PlayBC();

					_vector BC_Pos = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (XMVector3Normalize(m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 8.f);
					BC_Pos += (XMVector3Normalize(m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_RIGHT)) * 3.f);
					_vector vPlayerPos = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					pTarget->GetTransform()->LookAt_NonY(vPlayerPos);
					pTarget->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, BC_Pos);
				}
			}
		})
		.Tick([&](double fTimeDelta)
		{

		})
		.OnExit([&]()
		{

		})
			.AddTransition("BRAINCRASH_ACTIVATE to BRAINCRASH_NOUSE", "BRAINCRASH_NOUSE")
			.Predicator([&]()->_bool { return m_pPlayer->m_pASM->isSocketEmpty("BrainCrash_AnimSocket"); })
			.Priority(0)

		// 카렌 전용 브레인크러시
		.AddState("BRAINCRASH_EM8200_CUTSCENE")
		.OnStart([&]()
		{
			m_pPlayer->m_bBrainCrash = true;
			m_pPlayer->m_pSasPortrait->Start_SAS(ESASType::SAS_NOT);

			_vector BC_Pos = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (XMVector3Normalize(m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 5.f);
			BC_Pos += (XMVector3Normalize(m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_RIGHT)) * -0.5f);
			_vector vPlayerPos = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->LookAt_NonY(vPlayerPos);
			CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, BC_Pos);
		})
		.Tick([&](double fTimeDelta) 
		{

		})
		.OnExit([&]()
		{
		})
		.AddTransition("BRAINCRASH_EM8200_CUTSCENE to BRAINCRASH_EM8200_SCENE01", "BRAINCRASH_EM8200_SCENE01")
		.Predicator([&]()->_bool { return m_pPlayer->m_pSasPortrait->isFinish(); })
		.Priority(0)

		.AddState("BRAINCRASH_EM8200_SCENE01")
		.OnStart([&]()
		{
			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				static_cast<CEnemy*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->PlayBC();
			}

			auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("em8200_BrainCrash_01");
			m_pPlayer->m_pPlayer_AnimCam->StartCamAnim_Return_Update(pCamAnim, m_pPlayer->m_pPlayerCam, m_pPlayer->m_pTransformCom, 0.f, 0.f);
			m_pPlayer->m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_pPlayer->m_BrainCrash_em8200_01);
		})
		.Tick([&](double fTimeDelta) 
		{

		})
		.OnExit([&]()
		{
		})
		.AddTransition("BRAINCRASH_EM8200_SCENE01 to BRAINCRASH_EM8200_SCENE02", "BRAINCRASH_EM8200_SCENE02")
		.Predicator([&]()->_bool { return m_pPlayer->m_pASM->isSocketEmpty("BrainCrash_AnimSocket"); })
		.Priority(0)

		.AddState("BRAINCRASH_EM8200_SCENE02")
		.OnStart([&]()
		{
			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				static_cast<CEM8200*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->SetBrainCrashCommand();
			}

			auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("em8200_BrainCrash_Boss1");
			m_pPlayer->m_pPlayer_AnimCam->StartCamAnim_Return_Update(pCamAnim, m_pPlayer->m_pPlayerCam, CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform(), 0.f, 0.f);
		})
		.Tick([&](double fTimeDelta)
		{

		})
		.OnExit([&]()
		{
		})
		.AddTransition("BRAINCRASH_EM8200_SCENE02 to BRAINCRASH_EM8200_SCENE03", "BRAINCRASH_EM8200_SCENE03")
		.Predicator([&]()->_bool { return CGameInstance::GetInstance()->GetCamAnim("em8200_BrainCrash_Boss1")->IsFinished(); })
		.Priority(0)

		.AddState("BRAINCRASH_EM8200_SCENE03")
		.OnStart([&]()
		{
			auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("em8200_BrainCrash_02");
			m_pPlayer->m_pPlayer_AnimCam->StartCamAnim_Return_Update(pCamAnim, m_pPlayer->m_pPlayerCam, m_pPlayer->m_pTransformCom, 0.f, 0.f);

			m_pPlayer->m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_pPlayer->m_BrainCrash_em8200_02);
		})
		.Tick([&](double fTimeDelta) 
		{

		})
		.OnExit([&]()
		{
		})
		.AddTransition("BRAINCRASH_EM8200_SCENE03 to BRAINCRASH_EM8200_SCENE04", "BRAINCRASH_EM8200_SCENE04")
		.Predicator([&]()->_bool { return m_pPlayer->m_pASM->isSocketEmpty("BrainCrash_AnimSocket"); })
		.Priority(0)

		.AddState("BRAINCRASH_EM8200_SCENE04")
		.OnStart([&]()
		{
			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
			{
				static_cast<CEM8200*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->SetBrainCrashCommand();
			}

			auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("em8200_BrainCrash_Boss2");
			m_pPlayer->m_pPlayer_AnimCam->StartCamAnim_Return_Update(pCamAnim, m_pPlayer->m_pPlayerCam, CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform(), 0.f, 0.f);
		})
		.Tick([&](double fTimeDelta) 
		{

		})
		.OnExit([&]()
		{
		})
		.AddTransition("BRAINCRASH_EM8200_SCENE04 to BRAINCRASH_NOUSE", "BRAINCRASH_NOUSE")
		.Predicator([&]()->_bool { return CGameInstance::GetInstance()->GetCamAnim("em8200_BrainCrash_Boss1")->IsFinished(); })
		.Priority(0)

		.Build();

	Safe_Release(pOriginFSM);
}

void CPlayerHotFixer::BrainFieldStateMachine_ReCompile()
{
	CFSMComponent* pOriginFSM = m_pPlayer->m_pBrainFieldProductStateMachine;

	m_pPlayer->m_pBrainFieldProductStateMachine =
		CFSMComponentBuilder().InitState("BRAINFIELD")

		.AddState("BRAINFIELD")
		.OnStart([&]() 
		{ 
			m_pPlayer->m_bZoomIsFinish = false;
			m_pPlayer->m_pASM->ClearAnimSocket("Common_AnimSocket");
		})
		.Tick([&](double fTimeDelta) 
		{
			m_pPlayer->m_bBrainField_Prod = false;

			if (m_pPlayer->m_bBrainField)
			{
				CPlayerInfoManager::GetInstance()->Change_BrainFieldMaintain(CHANGE_DECREASE, (_float)fTimeDelta);
			}
		})
		.OnExit([&]() {})
			.AddTransition("BRAINFIELD to BRAINFIELD_START", "BRAINFIELD_START")
			.Predicator([&]()->_bool { return (!m_pPlayer->m_bBrainField) && (m_pPlayer->m_bBrainFieldStart); })
			.Priority(0)

			.AddTransition("BRAINFIELD to BRAINFIELD_FINISH_BF", "BRAINFIELD_FINISH_BF")
			.Predicator([&]()->_bool { return (m_pPlayer->m_bBrainField) && (m_pPlayer->m_bBrainFieldStart || 0.f >= CPlayerInfoManager::GetInstance()->Get_PlayerStat().fBrainFieldMaintain); })
			.Priority(0)

		.AddState("BRAINFIELD_START")
		.OnStart([&]() 
		{
			m_pPlayer->m_bBrainField_Prod = true;

			list<CAnimation*> TestAnim;
			TestAnim.push_back(m_pPlayer->m_pModel->Find_Animation("AS_ch0100_BrainField_start"));
			m_pPlayer->m_pASM->AttachAnimSocket("Common_AnimSocket", TestAnim);

			for (_uint i = 0; i < SAS_CNT; ++i)
			{
				CPlayerInfoManager::GetInstance()->Finish_SasType((ESASType)i);
			}
			m_pPlayer->SasStateCheck();
			m_pPlayer->Visible_Check();

			m_ActionCamStart.PlayFromStart();
		})
		.Tick([&](double fTimeDelta) 
		{
			if(m_ActionCamStart.Tick(fTimeDelta, m_fActionCamOut))
			{
				static_cast<CCamera_Player*>(m_pPlayer->m_pPlayerCam)->Set_CamDistance(m_fActionCamOut);
			}
		})
		.OnExit([&]() {})
			.AddTransition("BRAINFIELD_START to BRAINFIELD_CAM_CLOSER", "BRAINFIELD_CAM_CLOSER")
			.Predicator([&]()->_bool { return (0.9f <= m_pPlayer->m_pModel->Find_Animation("AS_ch0100_BrainField_start")->GetPlayRatio()); })
			.Priority(0)

		.AddState("BRAINFIELD_CAM_CLOSER")
		.OnStart([&]() {})
		.Tick([&](double fTimeDelta) { m_pPlayer->m_bZoomIsFinish = static_cast<CCamSpot*>(m_pPlayer->m_pCamSpot)->Cam_Closer(fTimeDelta, 0.3f); })
		.OnExit([&]() {})
			.AddTransition("BRAINFIELD_CAM_CLOSER to BRAINFIELD_ACTIONCAM_01", "BRAINFIELD_ACTIONCAM_01")
			.Predicator([&]()->_bool { return m_pPlayer->m_bZoomIsFinish; })
			.Priority(0)

		.AddState("BRAINFIELD_ACTIONCAM_01")
		.OnStart([&]() 
		{
			list<CAnimation*> TestAnim;
			TestAnim.push_back(m_pPlayer->m_pModel->Find_Animation("AS_BrainFieldOpen_c01_ch0100"));
			m_pPlayer->m_pASM->AttachAnimSocket("Common_AnimSocket", TestAnim);

			for (auto& iter : m_pPlayer->m_vecWeapon)
				iter->SetVisible(false);

			m_pPlayer->m_pBrainField->OpenBrainField();

			auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("BrainField_01");
			m_pPlayer->m_pPlayer_AnimCam->StartCamAnim_Return_Update(pCamAnim, m_pPlayer->m_pPlayerCam, m_pPlayer->m_pTransformCom, m_pPlayer->m_fCameraLerpTime_In, m_pPlayer->m_fCameraLerpTime_Out);
			// m_pBrainField->CloseBrainField();  끄는거
		})
		.Tick([&](double fTimeDelta) 
		{
			static_cast<CCamSpot*>(m_pPlayer->m_pCamSpot)->Cam_Away(fTimeDelta, 0.3f);

			if (m_pPlayer->m_pModel->Find_Animation("AS_BrainFieldOpen_c01_ch0100")->GetPlayRatio() >= 0.8f && m_pPlayer->BF_Start.IsNotDo())
			{
				CPlayerInfoManager::GetInstance()->Set_BrainFieldMaintain(70.f);
				m_pPlayer->m_bBrainField = true;
				CPlayerInfoManager::GetInstance()->Set_BrainField(m_pPlayer->m_bBrainField);
			}

		})
		.OnExit([&]() {})
			.AddTransition("BRAINFIELD_ACTIONCAM_01 to BRAINFIELD_ACTIONCAM_02", "BRAINFIELD_ACTIONCAM_02")
			.Predicator([&]()->_bool { return m_pPlayer->m_pModel->Find_Animation("AS_BrainFieldOpen_c01_ch0100")->IsFinished(); })
			.Priority(0)

		.AddState("BRAINFIELD_ACTIONCAM_02")
		.OnStart([&]() 
		{
			m_pPlayer->BF_Start.Reset();
			list<CAnimation*> TestAnim;
			TestAnim.push_back(m_pPlayer->m_pModel->Find_Animation("AS_BrainFieldOpen_c02_ch0100"));
			m_pPlayer->m_pASM->AttachAnimSocket("Common_AnimSocket", TestAnim);

			//m_pPlayer->CMap_KineticBatchPreset::GetInstance()->Set_RespawnTime(1.f);

			// 브레인필드 키면 키네틱 오브젝트들 하늘로 떠오름
			for (auto& iter : CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, L"Layer_MapKineticObject")->GetGameObjects())
			{
				if (false == static_cast<CMapKinetic_Object*>(iter)->GetThrow() && true == static_cast<CMapKinetic_Object*>(iter)->Usable())
				{
					static_cast<CMapKinetic_Object*>(iter)->SetRefloat();
				}
			}

			static_cast<CCamera_Player*>(m_pPlayer->m_pPlayerCam)->Set_CamDistance(4.f);
		})
		.Tick([&](double fTimeDelta) {})
		.OnExit([&]() 
		{
		})
			.AddTransition("BRAINFIELD_ACTIONCAM_02 to BRAINFIELD", "BRAINFIELD")
			.Predicator([&]()->_bool { return m_pPlayer->m_pModel->Find_Animation("AS_BrainFieldOpen_c02_ch0100")->IsFinished(); })
			.Priority(0)

		.AddState("BRAINFIELD_FINISH_BF")
		.OnStart([&]() 
		{
			m_pPlayer->m_bBrainField_Prod = true;

			list<CAnimation*> TestAnim;
			TestAnim.push_back(m_pPlayer->m_pModel->Find_Animation("AS_ch0100_BrainField_close_BF"));
			m_pPlayer->m_pASM->AttachAnimSocket("Common_AnimSocket", TestAnim);

			//m_pPlayer->CMap_KineticBatchPreset::GetInstance()->Set_RespawnTime(5.f);
		})
		.Tick([&](double fTimeDelta) 
		{
			if (m_pPlayer->m_pASM->GetSocketAnimation("Common_AnimSocket") != m_pPlayer->m_pModel->Find_Animation("AS_ch0100_BrainField_close_BF"))
			{
				list<CAnimation*> TestAnim;
				TestAnim.push_back(m_pPlayer->m_pModel->Find_Animation("AS_ch0100_BrainField_close_BF"));
				m_pPlayer->m_pASM->AttachAnimSocket("Common_AnimSocket", TestAnim);
			}
		})
		.OnExit([&]() { m_pPlayer->m_bBrainField = false; })
			.AddTransition("BRAINFIELD_FINISH_BF to BRAINFIELD_FINISH_NF", "BRAINFIELD_FINISH_NF")
			.Predicator([&]()->_bool { return m_pPlayer->m_pModel->Find_Animation("AS_ch0100_BrainField_close_BF")->IsFinished(); })
			.Priority(0)

		.AddState("BRAINFIELD_FINISH_NF")
		.OnStart([&]() 
		{
			list<CAnimation*> TestAnim;
			TestAnim.push_back(m_pPlayer->m_pModel->Find_Animation("AS_ch0100_BrainField_close_NF"));
			m_pPlayer->m_pASM->AttachAnimSocket("Common_AnimSocket", TestAnim);

			m_pPlayer->m_pBrainField->CloseBrainField();

			for (auto& iter : m_pPlayer->m_vecWeapon)
				iter->SetVisible(true);

			// 브레인필드 키면 키네틱 오브젝트들 하늘로 떠오름
			for (auto& iter : CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, L"Layer_MapKineticObject")->GetGameObjects())
			{
				if (false == static_cast<CMapKinetic_Object*>(iter)->GetThrow() && true == static_cast<CMapKinetic_Object*>(iter)->Usable())
				{
					static_cast<CMapKinetic_Object*>(iter)->Set_Dynamic();
				}
			}
		})
		.Tick([&](double fTimeDelta) {})
		.OnExit([&]() {})
			.AddTransition("BRAINFIELD_FINISH_NF to BRAINFIELD", "BRAINFIELD")
			.Predicator([&]()->_bool { return m_pPlayer->m_pModel->Find_Animation("AS_ch0100_BrainField_close_NF")->IsFinished(); })
			.Priority(0)

		.Build();

	Safe_Release(pOriginFSM);
}

void CPlayerHotFixer::DeBuffStateMachine_ReCompile()
{
	CFSMComponent* pOriginFSM = m_pPlayer->m_pDeBuffStateMachine;

	m_pPlayer->m_pDeBuffStateMachine =
		CFSMComponentBuilder().InitState("DEBUFF_END")

		.AddState("DEBUFF_END")
		.OnStart([&]() {})
		.Tick([&](double fTimeDelta) {})
		.OnExit([&]() {})

			.AddTransition("DEBUFF_END to DEBUFF_FIRE", "DEBUFF_FIRE")
			.Predicator([&]()->_bool {return m_pPlayer->GetDeBuffType() == EDeBuffType::DEBUFF_FIRE; })
			.Priority(0)

			.AddTransition("DEBUFF_END to DEBUFF_WATER", "DEBUFF_WATER")
			.Predicator([&]()->_bool {return m_pPlayer->GetDeBuffType() == EDeBuffType::DEBUFF_WATER; })
			.Priority(1)

			.AddTransition("DEBUFF_END to DEBUFF_OIL", "DEBUFF_OIL")
			.Predicator([&]()->_bool {return m_pPlayer->GetDeBuffType() == EDeBuffType::DEBUFF_OIL; })
			.Priority(2)

			.AddTransition("DEBUFF_END to DEBUFF_ELEC", "DEBUFF_ELEC")
			.Predicator([&]()->_bool {return m_pPlayer->GetDeBuffType() == EDeBuffType::DEBUFF_THUNDER; })
			.Priority(3)

		.AddState("DEBUFF_FIRE")
		.OnStart([&]() {})
		.Tick([&](double fTimeDelta) 
		{
			m_pPlayer->m_NoticeTick.Tick(fTimeDelta);

			if (m_pPlayer->m_NoticeTick.Use())
			{
				DAMAGE_PARAM tParam;
				ZeroMemory(&tParam, sizeof(DAMAGE_DESC));
				tParam.eAttackType = EAttackType::ATK_END;
				tParam.eDeBuff = EDeBuffType::DEBUFF_FIRE;
				tParam.iDamage = 1;
				tParam.pCauser = m_pPlayer;
				m_pPlayer->TakeDamage(tParam);
			}
		})
		.OnExit([&]() {})
			
			/*.AddTransition("DEBUFF_FIRE to DEBUFF_END", "DEBUFF_END")
			.Predicator([&]()->_bool {return m_pPlayer->GetDeBuffType() == EDeBuffType::DEBUFF_END; })
			.Priority(0)*/

			.AddTransition("DEBUFF_FIRE to DEBUFF_END", "DEBUFF_END")
			.Predicator([&]()->_bool {return m_pPlayer->GetDeBuffType() == EDeBuffType::DEBUFF_END; })
			.Priority(100)

		.AddState("DEBUFF_WATER")
		.OnStart([&]() {})
		.Tick([&](double fTimeDelta) { })
		.OnExit([&]() {})

			.AddTransition("DEBUFF_WATER to DEBUFF_ELEC", "DEBUFF_ELEC")
			.Predicator([&]()->_bool {return m_pPlayer->m_eDeBuff == EDeBuffType::DEBUFF_THUNDER; })
			.Priority(0)

			.AddTransition("DEBUFF_WATER to DEBUFF_END", "DEBUFF_END")
			.Predicator([&]()->_bool {return m_pPlayer->GetDeBuffType() == EDeBuffType::DEBUFF_END; })
			.Priority(100)

		.AddState("DEBUFF_OIL")
		.OnStart([&]() {})
		.Tick([&](double fTimeDelta) { })
		.OnExit([&]() {})

			.AddTransition("DEBUFF_OIL to DEBUFF_FIRE", "DEBUFF_FIRE")
			.Predicator([&]()->_bool {return m_pPlayer->GetDeBuffType() == EDeBuffType::DEBUFF_FIRE; })
			.Priority(0)

			.AddTransition("DEBUFF_OIL to DEBUFF_END", "DEBUFF_END")
			.Predicator([&]()->_bool {return m_pPlayer->GetDeBuffType() == EDeBuffType::DEBUFF_END; })
			.Priority(100)

		.AddState("DEBUFF_ELEC")
		.OnStart([&]() 
		{
			m_pPlayer->m_pASM->SetCurState("IDLE");
			m_pPlayer->m_pASM->SetCurState_BrainField("IDLE");
			m_pPlayer->SetAbleState({ false, false, false, false, false, true, true, true, true, false });

			m_pPlayer->m_pASM->AttachAnimSocket("Hit_AnimSocket", m_pPlayer->m_ElecShock_Start);
		})
		.Tick([&](double fTimeDelta) { })
		.OnExit([&]() {})

			/*.AddTransition("DEBUFF_OIL to DEBUFF_END", "DEBUFF_END")
			.Predicator([&]()->_bool {return m_pPlayer->GetDeBuffType() == EDeBuffType::DEBUFF_END; })
			.Priority(0)*/

			.AddTransition("DEBUFF_ELEC to DEBUFF_ELEC_END", "DEBUFF_ELEC_END")
			.Predicator([&]()->_bool {return m_pPlayer->GetDeBuffType() == EDeBuffType::DEBUFF_END; })
			.Priority(100)

		.AddState("DEBUFF_ELEC_END")
		.OnStart([&]() 
		{
			m_pPlayer->m_pASM->AttachAnimSocket("Hit_AnimSocket", m_pPlayer->m_ElecShock_WakeUp);
		})
		.Tick([&](double fTimeDelta) { })
		.OnExit([&]() { m_pPlayer->m_pASM->ClearAnimSocket("Hit_AnimSocket"); })

			.AddTransition("DEBUFF_ELEC_END to DEBUFF_END", "DEBUFF_END")
			.Predicator([&]()->_bool 
			{
				return m_pPlayer->m_pModel->Find_Animation("AS_ch0100_445_AL_wakeup_B")->GetPlayRatio() > 0.8f &&
					(m_pPlayer->m_bLeftClick || m_pPlayer->m_bWalk || m_pPlayer->m_bDash || m_pPlayer->m_bJump);
			})
			.Priority(0)

			.AddTransition("DEBUFF_ELEC_END to DEBUFF_END", "DEBUFF_END")
			.Predicator([&]()->_bool 
			{
				return m_pPlayer->m_pASM->isSocketEmpty("Hit_AnimSocket");
			})
			.Priority(100)

		.Build();

	if (nullptr != pOriginFSM)
		Safe_Release(pOriginFSM);
}

void CPlayerHotFixer::HitStateMachine_ReCompile()
{
	CFSMComponent* pOriginFSM = m_pPlayer->m_pHitStateMachine;

	m_pPlayer->m_pHitStateMachine = 
		CFSMComponentBuilder().InitState("NON_HIT")
		.AddState("NON_HIT")
			.OnStart([&]() 
			{ 
				//SetAbleState({ false, false, false, false, false, true, true, true, true, false });
				m_pPlayer->m_pModel->Reset_LocalMove(true);

				if (!m_pPlayer->m_pASM->isSocketEmpty("Hit_AnimSocket"))
					m_pPlayer->m_pASM->GetSocketAnimation("Hit_AnimSocket")->Reset();

				m_pPlayer->m_pASM->ClearAnimSocket("Hit_AnimSocket");
			})
			.Tick([&](double fTimeDelta) {m_pPlayer->m_bHit = false; })
			.OnExit([&]() 
			{ 
				m_pPlayer->m_pCamSpot->Reset_CamMod();
				m_pPlayer->m_pASM->SetCurState("IDLE");
				m_pPlayer->m_pASM->SetCurState_BrainField("IDLE");
				m_pPlayer->m_bSeperateAnim = false; m_pPlayer->m_bHit = true;
				m_pPlayer->SetAbleState({ false, false, false, false, false, true, true, true, true, false });
			})
				.AddTransition("NON_HIT to AIRBORNE", "AIRBORNE")
				.Predicator([&]()->_bool {return m_pPlayer->m_bHit && (m_pPlayer->m_DamageDesc.m_iDamageType == EAttackType::ATK_TO_AIR); })
				.Priority(0)

				.AddTransition("NON_HIT to KNUCKBACK", "KNUCKBACK")
				.Predicator([&]()->_bool {return m_pPlayer->m_bHit && (m_pPlayer->m_DamageDesc.m_iDamageType == EAttackType::ATK_HEAVY); })
				.Priority(0)

				.AddTransition("NON_HIT to HIT_LIGHT", "HIT_LIGHT")
				.Predicator([&]()->_bool {return m_pPlayer->m_bHit && !m_pPlayer->m_bAir && (m_pPlayer->m_DamageDesc.m_iDamageType == EAttackType::ATK_LIGHT); })
				.Priority(0)

				.AddTransition("NON_HIT to HIT_MIDIUM", "HIT_MIDIUM")
				.Predicator([&]()->_bool {return m_pPlayer->m_bHit && !m_pPlayer->m_bAir && (m_pPlayer->m_DamageDesc.m_iDamageType == EAttackType::ATK_MIDDLE); })
				.Priority(0)

		.AddState("KNUCKBACK")
			.OnStart([&]() 
			{ 
				CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.1f, 0.2f);

				m_pPlayer->m_pASM->InputAnimSocket("Hit_AnimSocket", m_pPlayer->m_Knuckback);
			})
			.Tick([&](double TimeDelta) 
			{ 
				m_pPlayer->m_bWalk = false;
				_vector vOpTest = m_pPlayer->m_pModel->GetOptionalMoveVector(m_pPlayer->m_pTransformCom->Get_WorldMatrix(), "AS_ch0100_455_AL_rise_start");
				m_pPlayer->m_pTransformCom->LocalMove(vOpTest);
			})
				.AddTransition("KNUCKBACK to FALLDOWN", "FALLDOWN")
				.Predicator([&]()->_bool {return m_pPlayer->m_pASM->isSocketAlmostFinish("Hit_AnimSocket"); })
				.Priority(0)

		.AddState("AIRBORNE")
			.OnStart([&]() 
			{
				CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.1f, 0.2f);

				m_pPlayer->m_pASM->InputAnimSocket("Hit_AnimSocket", m_pPlayer->m_Airborne);
				m_pPlayer->Jump();
			})
			.Tick([&](double TimeDelta) 
			{ 
				m_pPlayer->m_bWalk = false;
				m_pPlayer->m_bAir = true;
			})
				.AddTransition("AIRBORNE to FALL", "FALL")
				.Predicator([&]()->_bool {return m_pPlayer->m_pASM->isSocketAlmostFinish("Hit_AnimSocket"); })
				.Priority(1)

				.AddTransition("AIRBORNE to FALLDOWN", "FALLDOWN")
				.Predicator([&]()->_bool {return m_pPlayer->m_bAir && m_pPlayer->m_pASM->isSocketAlmostFinish("Hit_AnimSocket");; })
				.Priority(0)

		.AddState("FALL")
			.OnStart([&]() { m_pPlayer->m_pASM->AttachAnimSocket("Hit_AnimSocket", m_pPlayer->m_Fall); })
			.Tick([&](double TimeDelta) 
			{
				m_pPlayer->m_bWalk = false;
				m_pPlayer->m_bAir = true;

				if (m_pPlayer->m_pASM->isSocketAlmostFinish("Hit_AnimSocket"))
				{
					m_pPlayer->m_pASM->AttachAnimSocket("Hit_AnimSocket", m_pPlayer->m_Fall);
				}
			})
				.AddTransition("FALL to FALLDOWN", "FALLDOWN")
				.Predicator([&]()->_bool {return m_pPlayer->m_bOnFloor; })
				.Priority(0)

				.AddTransition("FALL to BREAKFALL", "BREAKFALL")
				.Predicator([&]()->_bool {return m_pPlayer->m_bBreakFall; })
				.Priority(0)

		.AddState("FALLDOWN")
			.OnStart([&]() { m_pPlayer->m_pASM->InputAnimSocket("Hit_AnimSocket", m_pPlayer->m_FallDown_Back); })
			.Tick([&](double TimeDelta) 
			{
				m_pPlayer->m_bWalk = false;
				//m_bAir = false;
			})
			.OnExit([&]()
			{
				ZeroMemory(&m_pPlayer->m_DamageDesc, sizeof(DAMAGE_DESC));
				m_pPlayer->m_bHit = false;
			})
				.AddTransition("FALLDOWN to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pPlayer->m_pASM->isSocketEmpty("Hit_AnimSocket"); })
				.Priority(0)

				.AddTransition("FALLDOWN to BREAKFALL", "BREAKFALL")
				.Predicator([&]()->_bool {return !m_pPlayer->m_pASM->isSocketPassby("Hit_AnimSocket", 0.f) && m_pPlayer->m_bBreakFall; })
				.Priority(0)

		.AddState("HIT_LIGHT")
			.OnStart([&]() { CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.05f, 0.1f); })
			.AddTransition("HIT_LIGHT to FRONT", "HIT_LIGHT_FRONT")
			.Predicator([&]()->_bool 
			{
				_float fDot = XMVectorGetX(XMVector3Dot(m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_pPlayer->m_DamageDesc.m_vHitDir));
				return (fDot < 0.f) ? true : false; 
			})
			.Priority(0)

			.AddTransition("HIT_LIGHT to BACK", "HIT_LIGHT_BACK")
			.Predicator([&]()->_bool
			{
				_float fDot = XMVectorGetX(XMVector3Dot(m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_pPlayer->m_DamageDesc.m_vHitDir));
				return (fDot > 0.f) ? true : false;
			})
			.Priority(0)

			.AddTransition("HIT_LIGHT to NON_HIT", "NON_HIT")
			.Predicator([&]()->_bool {return m_pPlayer->m_pASM->isSocketEmpty("Hit_AnimSocket"); })
			.Priority(100)

		.AddState("HIT_LIGHT_FRONT")
			.OnStart([&]() 
			{ 
				m_pPlayer->m_pASM->InputAnimSocket("Hit_AnimSocket", m_pPlayer->m_Hit_F_Level01);
				ZeroMemory(&m_pPlayer->m_DamageDesc, sizeof(DAMAGE_DESC));
				m_pPlayer->m_bHit = false;
			})
			.Tick([&](double fTimeDelta)
			{
				//string szCurAnimName = m_pASM->GetSocketAnimation("Hit_AnimSocket")->GetName();
				//_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				//m_pTransformCom->LocalMove(vLocal);
			})

				.AddTransition("HIT_LIGHT_FRONT to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pPlayer->m_pASM->isSocketEmpty("Hit_AnimSocket"); })
				.Priority(0)

				.AddTransition("HIT_LIGHT_FRONT to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pPlayer->m_bHit && m_pPlayer->m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

		.AddState("HIT_LIGHT_BACK")
			.OnStart([&]() 
			{
				m_pPlayer->m_pASM->InputAnimSocket("Hit_AnimSocket", m_pPlayer->m_Hit_B_Level01);
				ZeroMemory(&m_pPlayer->m_DamageDesc, sizeof(DAMAGE_DESC));
				m_pPlayer->m_bHit = false;
			})
			.Tick([&](double fTimeDelta)
			{
				//string szCurAnimName = m_pASM->GetSocketAnimation("Hit_AnimSocket")->GetName();
				//_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				//m_pTransformCom->LocalMove(vLocal);
			})

				.AddTransition("HIT_LIGHT_BACK to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pPlayer->m_pASM->isSocketEmpty("Hit_AnimSocket"); })
				.Priority(0)

				.AddTransition("HIT_LIGHT_BACK to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pPlayer->m_bHit && m_pPlayer->m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

		.AddState("HIT_MIDIUM")
			.OnStart([&]() { CPlayerInfoManager::GetInstance()->Camera_Random_Shake_Maintain(0.05f, 0.2f); })
			.AddTransition("HIT_MIDIUM to FRONT", "HIT_MIDIUM_FRONT")
			.Predicator([&]()->_bool 
			{
				return m_pPlayer->m_DamageDesc.m_eHitDir == EBaseAxis::NORTH ? true : false;
			})
			.Priority(0)

			.AddTransition("HIT_MIDIUM to BACK", "HIT_MIDIUM_BACK")
			.Predicator([&]()->_bool
			{
				return m_pPlayer->m_DamageDesc.m_eHitDir == EBaseAxis::SOUTH ? true : false;
			})
			.Priority(0)

			.AddTransition("HIT_MIDIUM to LEFT", "HIT_MIDIUM_LEFT")
			.Predicator([&]()->_bool 
			{
				return m_pPlayer->m_DamageDesc.m_eHitDir == EBaseAxis::WEST ? true : false;
			})
			.Priority(0)

			.AddTransition("HIT_MIDIUM to RIGHT", "HIT_MIDIUM_RIGHT")
			.Predicator([&]()->_bool
			{
				return m_pPlayer->m_DamageDesc.m_eHitDir == EBaseAxis::EAST ? true : false;
			})
			.Priority(0)

			.AddTransition("HIT_MIDIUM to NON_HIT", "NON_HIT")
			.Predicator([&]()->_bool {return true; })
			.Priority(100)

		.AddState("HIT_MIDIUM_FRONT")
			.OnStart([&]() 
			{ 
				m_pPlayer->m_pASM->InputAnimSocket("Hit_AnimSocket", m_pPlayer->m_Hit_F_Level02);
				ZeroMemory(&m_pPlayer->m_DamageDesc, sizeof(DAMAGE_DESC));
				m_pPlayer->m_bHit = false;
			})
			.Tick([&](double fTimeDelta)
			{
				//string szCurAnimName = m_pASM->GetSocketAnimation("Hit_AnimSocket")->GetName();
				//_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				//m_pTransformCom->LocalMove(vLocal);
			})

				.AddTransition("HIT_MIDIUM_FRONT to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pPlayer->m_pASM->isSocketEmpty("Hit_AnimSocket"); })
				.Priority(0)

				.AddTransition("HIT_MIDIUM_FRONT to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pPlayer->m_bHit && m_pPlayer->m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

		.AddState("HIT_MIDIUM_BACK")
			.OnStart([&]() 
			{ 
				m_pPlayer->m_pASM->InputAnimSocket("Hit_AnimSocket", m_pPlayer->m_Hit_B_Level02);
				ZeroMemory(&m_pPlayer->m_DamageDesc, sizeof(DAMAGE_DESC));
				m_pPlayer->m_bHit = false;
			})
			.Tick([&](double fTimeDelta)
			{
				//string szCurAnimName = m_pASM->GetSocketAnimation("Hit_AnimSocket")->GetName();
				//_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				//m_pTransformCom->LocalMove(vLocal);
			})

				.AddTransition("HIT_MIDIUM_BACK to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pPlayer->m_pASM->isSocketEmpty("Hit_AnimSocket"); })
				.Priority(0)

				.AddTransition("HIT_MIDIUM_BACK to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pPlayer->m_bHit && m_pPlayer->m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

		.AddState("HIT_MIDIUM_LEFT")
			.OnStart([&]() 
			{ 
				m_pPlayer->m_pASM->InputAnimSocket("Hit_AnimSocket", m_pPlayer->m_Hit_L_Level02);
				ZeroMemory(&m_pPlayer->m_DamageDesc, sizeof(DAMAGE_DESC));
				m_pPlayer->m_bHit = false;
			})
			.Tick([&](double fTimeDelta)
			{
				//string szCurAnimName = m_pASM->GetSocketAnimation("Hit_AnimSocket")->GetName();
				//_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				//m_pTransformCom->LocalMove(vLocal);
			})

				.AddTransition("HIT_MIDIUM_LEFT to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pPlayer->m_pASM->isSocketEmpty("Hit_AnimSocket"); })
				.Priority(0)

				.AddTransition("HIT_MIDIUM_LEFT to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pPlayer->m_bHit && m_pPlayer->m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

		.AddState("HIT_MIDIUM_RIGHT")
			.OnStart([&]() 
			{ 
				m_pPlayer->m_pASM->InputAnimSocket("Hit_AnimSocket", m_pPlayer->m_Hit_R_Level02);
				ZeroMemory(&m_pPlayer->m_DamageDesc, sizeof(DAMAGE_DESC));
				m_pPlayer->m_bHit = false;
			})
			.Tick([&](double fTimeDelta)
			{
				//string szCurAnimName = m_pASM->GetSocketAnimation("Hit_AnimSocket")->GetName();
				//_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
				//m_pTransformCom->LocalMove(vLocal);
			})

				.AddTransition("HIT_MIDIUM_RIGHT to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pPlayer->m_pASM->isSocketEmpty("Hit_AnimSocket"); })
				.Priority(0)

				.AddTransition("HIT_MIDIUM_RIGHT to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pPlayer->m_bHit && m_pPlayer->m_pASM->isSocketPassby("Hit_AnimSocket", 0.1f); })
				.Priority(1)

		.AddState("BREAKFALL")
			.OnStart([&]()
			{
				m_pPlayer->m_bAir = true;
				m_pPlayer->m_pASM->InputAnimSocket("Hit_AnimSocket", m_pPlayer->m_BreakFall_Front);
				ZeroMemory(&m_pPlayer->m_DamageDesc, sizeof(DAMAGE_DESC));
				m_pPlayer->m_bHit = false;
				m_pPlayer->m_fYSpeed = 5.f;
			})
				.AddTransition("BREAKFALL to BREAKFALL_LANDING", "BREAKFALL_LANDING")
				.Predicator([&]()->_bool {return m_pPlayer->m_pASM->isSocketPassby("Hit_AnimSocket", 0.2f) && m_pPlayer->m_bOnFloor; })
				.Priority(0)

				.AddTransition("BREAKFALL to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pPlayer->m_pASM->isSocketEmpty("Hit_AnimSocket"); })
				.Priority(1)

		.AddState("BREAKFALL_LANDING")
			.OnStart([&]()
			{
				m_pPlayer->m_bHit = false;
				m_pPlayer->m_pASM->InputAnimSocket("Hit_AnimSocket", m_pPlayer->m_BreakFall_Landing);
			})

				.AddTransition("BREAKFALL_LANDING to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pPlayer->m_pASM->isSocketPassby("Hit_AnimSocket", 0.5f) && 
					(m_pPlayer->m_bLeftClick || m_pPlayer->m_bJump || m_pPlayer->m_bDash || m_pPlayer->m_bWalk || m_pPlayer->m_bKineticG || m_pPlayer->m_bKineticRB); })
				.Priority(0)

				.AddTransition("BREAKFALL_LANDING to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pPlayer->m_pASM->isSocketAlmostFinish("Hit_AnimSocket"); })
				.Priority(1)

				.AddTransition("BREAKFALL_LANDING to NON_HIT", "NON_HIT")
				.Predicator([&]()->_bool {return m_pPlayer->m_pASM->isSocketEmpty("Hit_AnimSocket"); })
				.Priority(1)

		.Build();

	Safe_Release(pOriginFSM);
}

void CPlayerHotFixer::Player_Something_Update()
{
	// 플레이어에 추가할 내용이나 변경할 내용 있으면 해당 부분에서 추가 컴파일 가능

	/*m_pPlayer->m_BrandCrash_em0200.clear();
	m_pPlayer->m_BrandCrash_em0200.push_back(m_pPlayer->m_pModel->Find_Animation("AS_BC_em0200m_ch0100"));*/
	_float4x4	NoticeNeonPivot = XMMatrixScaling(0.5f, 0.5f, 0.5f) * XMMatrixTranslation(0.1f, 0.f, 0.f);
	
	/*if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pPlayer->m_pCautionNeon.first))*/
	if (m_pPlayer->m_pCautionNeon != nullptr)
	{
		m_pPlayer->m_pCautionNeon->SetDelete();
		m_pPlayer->m_pCautionNeon = nullptr;
	}

	if (m_pPlayer->m_pCautionNeon == nullptr)
	{
		m_pPlayer->m_pCautionNeon = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_UI, L"NoticeNeon_HP");
		m_pPlayer->m_pCautionNeon->Start_AttachPivot(m_pPlayer, NoticeNeonPivot, "String2", true, true);
	}
}

void CPlayerHotFixer::Additive_Test()
{
	m_pPlayer->m_pModel->Play_Animation_Additive(g_fTimeDelta * 3.f, 0.1f);
}

CPlayerHotFixer* CPlayerHotFixer::Create(CPlayer* pPlayer)
{
	CPlayerHotFixer* pInstance = new CPlayerHotFixer();

	if (FAILED(pInstance->Initialize(pPlayer)))
	{
		MSG_BOX("Failed to Created : CPlayerHotFixer");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CPlayerHotFixer::Free()
{
}
