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

CPlayerHotFixer::CPlayerHotFixer()
{
}

HRESULT CPlayerHotFixer::Initialize(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;

	return S_OK;
}

void CPlayerHotFixer::Tick()
{
	if (ImGui::CollapsingHeader("HotFixer"))
	{
		if (ImGui::Button("BrainCrash_Recompile"))
		{
			BrainCrashStateMachine_ReCompoile();
		}
		if (ImGui::Button("BrainField_Recompile"))
		{
			BrainFieldStateMachine_ReCompile();
		}
		if (ImGui::Button("Something_Recompile"))
		{
			Player_Something_Update();
		}
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
			m_pPlayer->m_bBrainCrash = false;
		})
		.Tick([&](double fTimeDelta) 
		{
			m_pPlayer->m_bBrainCrash = false;
		})
		.OnExit([&]()
		{
			m_pPlayer->m_pASM->SetCurState("IDLE");
			m_pPlayer->SetAbleState({ false, false, false, false, false, true, true, true, true, false });
		})
			.AddTransition("BRAINCRASH_NOUSE to BRAINCRASH_CUTSCENE", "BRAINCRASH_CUTSCENE")
			.Predicator([&]()->_bool 
			{ 
				//if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster()) return false;
				return m_pPlayer->m_bBrainCrashInput/* && static_cast<CEnemy*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->CanBC()*/;
			})
			.Priority(0)

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
				//static_cast<CEnemy*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->PlayBC();
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
				// Å¸°Ù Á¾·ù¿¡ µû¶ó ¾Ö´Ï¸ÞÀÌ¼Ç°ú ¿¬Ãâ Ä·À» Àç»ý½ÃÅ°µµ·Ï ÇÑ´Ù

				_float4 vTargetPos = pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				_float4 vDistance = XMLoadFloat4(&vTargetPos) - m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
				_float fDistance = vDistance.Length();

				if (pTarget->GetPrototypeTag() == L"Monster_em210")
				{
					m_pPlayer->m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_pPlayer->m_BrandCrash_em0200);
					static_cast<CEM0210*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->PlayBC();

					if (5.f >= fDistance)
					{
						_vector BC_Pos = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (XMVector3Normalize(m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 3.f);
						_vector vPlayerPos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
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
						_vector vPlayerPos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
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
					_vector vPlayerPos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					pTarget->GetTransform()->LookAt_NonY(vPlayerPos);
					pTarget->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, BC_Pos);
				}
			}
			else
			{
				//static_cast<CEM0210*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->PlayBC();

				/*auto pCamAnim = CGameInstance::GetInstance()->GetCamAnim("BrainCrush01");
				m_pPlayer->m_pPlayer_AnimCam->StartCamAnim_Return_Update(pCamAnim, m_pPlayer->m_pPlayerCam, m_pPlayer->m_pTransformCom, 0.f, 0.f);
				m_pPlayer->m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_pPlayer->m_BrainCrash_Activate);*/

				m_pPlayer->m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_pPlayer->m_BrainCrash_em0100);
			}
		})
		.Tick([&](double fTimeDelta) 
		{
			string szCurAnimName = m_pPlayer->m_pASM->GetSocketAnimation("BrainCrash_AnimSocket")->GetName();
			_float4 vLocal = m_pPlayer->m_pModel->GetLocalMove(m_pPlayer->m_pTransformCom->Get_WorldMatrix(), szCurAnimName);

			// ÇØ´ç ºÎºÐµµ Å¸°Ù Á¾·ù¿¡ µû¶ó ·ÎÄÃ¹«ºê¸¦ ´Ù¸£°Ô Àû¿ë½ÃÅ²´Ù.
			CAnimation* pAnimation = m_pPlayer->m_pModel->Find_Animation("AS_BC_em0200m_ch0100");
			_float fRatio = pAnimation->GetPlayRatio();

			//if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetPrototypeTag() == L"Monster_em800")
			//{
			
			//}

			/*if (fRatio >= 0.285f && fRatio <= 0.3714f)
			{
				m_pPlayer->m_pTransformCom->LocalMove(vLocal, 2.f);
			}
			else
			{*/
				//m_pPlayer->m_pTransformCom->LocalMove(vLocal);
			//}

			// em0110 Àü¿ë
			/*_float4 vMinusCheck = m_pPlayer->m_pModel->Find_Animation("AS_BC_em0100m_ch0100")->GetLocalMove();

			if (vMinusCheck.z > 0.f) 
				vLocal.y *= -1.f;

			m_pPlayer->m_pTransformCom->LocalMove({vLocal.x, vLocal.y, vLocal.z});*/
		})
		.OnExit([&]()
		{

		})
			.AddTransition("BRAINCRASH_ACTIVATE to BRAINCRASH_NOUSE", "BRAINCRASH_NOUSE")
			.Predicator([&]()->_bool { return m_pPlayer->m_pASM->isSocketEmpty("BrainCrash_AnimSocket"); })
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
		.OnStart([&]() { m_pPlayer->m_bZoomIsFinish = false; })
		.Tick([&](double fTimeDelta) {})
		.OnExit([&]() {})
			.AddTransition("BRAINFIELD to BRAINFIELD_START", "BRAINFIELD_START")
			.Predicator([&]()->_bool { return (!m_pPlayer->m_bBrainField) && (m_pPlayer->m_pController->KeyDown(CController::B)); })
			.Priority(0)

		.AddState("BRAINFIELD_START")
		.OnStart([&]() 
		{
			list<CAnimation*> TestAnim;
			TestAnim.push_back(m_pPlayer->m_pModel->Find_Animation("AS_ch0100_BrainField_start"));
			m_pPlayer->m_pASM->AttachAnimSocket("Common_AnimSocket", TestAnim);

			for (_uint i = 0; i < SAS_CNT; ++i)
			{
				CPlayerInfoManager::GetInstance()->Finish_SasType((ESASType)i);
			}
			m_pPlayer->SasStateCheck();
			m_pPlayer->Visible_Check();
		})
		.Tick([&](double fTimeDelta) {})
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
		})
		.Tick([&](double fTimeDelta) {static_cast<CCamSpot*>(m_pPlayer->m_pCamSpot)->Cam_Away(fTimeDelta, 0.3f); })
		.OnExit([&]() {})
			.AddTransition("BRAINFIELD_ACTIONCAM_01 to BRAINFIELD_ACTIONCAM_02", "BRAINFIELD_ACTIONCAM_02")
			.Predicator([&]()->_bool { return m_pPlayer->m_pModel->Find_Animation("AS_BrainFieldOpen_c01_ch0100")->IsFinished(); })
			.Priority(0)

		.AddState("BRAINFIELD_ACTIONCAM_02")
		.OnStart([&]() 
		{
			list<CAnimation*> TestAnim;
			TestAnim.push_back(m_pPlayer->m_pModel->Find_Animation("AS_BrainFieldOpen_c02_ch0100"));
			m_pPlayer->m_pASM->AttachAnimSocket("Common_AnimSocket", TestAnim);

			for (auto& iter : m_pPlayer->m_vecWeapon)
				iter->SetVisible(false);
		})
		.Tick([&](double fTimeDelta) {})
		.OnExit([&]() { m_pPlayer->m_bBrainField = true; })
			.AddTransition("BRAINFIELD_ACTIONCAM_02 to BRAINFIELD", "BRAINFIELD")
			.Predicator([&]()->_bool { return m_pPlayer->m_pModel->Find_Animation("AS_BrainFieldOpen_c02_ch0100")->IsFinished(); })
			.Priority(0)

		.Build();

	Safe_Release(pOriginFSM);
}

void CPlayerHotFixer::KineticComboStateMachine_ReCompile()
{
//	CFSMComponent* pOriginFSM = m_pPlayer->m_pKineticComboStateMachine;
//
//	m_pPlayer->m_pKineticComboStateMachine = CFSMComponentBuilder()
//
//		.InitState("KINETIC_COMBO_NOUSE")
//
//		.AddState("KINETIC_COMBO_NOUSE")
//		.OnStart([&]() 
//		{
//			m_pPlayer->m_bKineticCombo = false;
//
//			m_pPlayer->m_fKineticCombo_Kinetic = 0.f;
//			m_pPlayer->m_fKineticCombo_Slash = 0.f;
//
//			m_pPlayer->m_pASM->ClearAnimSocket("Kinetic_Combo_AnimSocket");
//
//			if(!m_pPlayer->m_bAir)
//				m_pPlayer->m_pASM->SetCurState("IDLE");
//			else
//				m_pPlayer->m_pASM->SetCurState("JUMP_FALL");
//
//			// ë¦¼ë¼?´íŠ¸ ì¢…ë£Œ
//			m_pPlayer->m_pCurve = nullptr;
//
//			for (auto& iter : m_pPlayer->m_pModel->GetMaterials())
//			{
//				iter->GetParam().Floats[0] = 0.f;
//				iter->GetParam().Float4s[0] = { 0.f, 0.f, 0.f, 0.f };
//			}
//
//			m_pPlayer->Event_Trail(false);
//			static_cast<CScarletWeapon*>(m_pPlayer->m_vecWeapon.front())->Set_Bright(ESASType::SAS_NOT, false);
//
//			IM_LOG("Kinetic No Use");
//		})
//		.Tick([&](double fTimeDelta)
//		{
//
//		})
//		.OnExit([&]()
//		{
//			m_pPlayer->m_bKineticMove = false;
//			m_pPlayer->m_bSeperateAnim = false;
//			m_pPlayer->SetAbleState({ false, false, false, false, false, true, true, true, true, false });
//
//			m_pPlayer->m_pASM->SetCurState("IDLE");
//			m_pPlayer->m_pKineticStataMachine->SetState("NO_USE_KINETIC");
//
//			for (auto& iter : m_pPlayer->m_pModel->GetMaterials())
//			{
//				iter->GetParam().Float4s[0] = { 0.945098f, 0.4f, 1.f, 1.f };
//			}
//
//			m_pPlayer->Event_Trail(false);
//			static_cast<CScarletWeapon*>(m_pPlayer->m_vecWeapon.front())->Set_Bright(ESASType::SAS_NOT, true);
//		})
//
//			.AddTransition("KINETIC_COMBO_NOUSE to KINETIC_COMBO_SLASH01", "KINETIC_COMBO_SLASH01")
//			.Predicator([&]()->_bool { return !m_pPlayer->m_bHit && m_pPlayer->m_bLeftClick && (m_pPlayer->m_fKineticCombo_Kinetic > 0.f) && !m_pPlayer->m_bAir && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_NOUSE to KINETIC_COMBO_KINETIC01_CHARGE", "KINETIC_COMBO_KINETIC01_CHARGE")
//			.Predicator([&]()->_bool { return !m_pPlayer->m_bHit && m_pPlayer->m_bKineticRB && (m_pPlayer->m_fKineticCombo_Slash > 0.f) && !m_pPlayer->m_bAir && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_NOUSE to KINETIC_COMBO_AIR_CAP", "KINETIC_COMBO_AIR_CAP")
//			.Predicator([&]()->_bool { return !m_pPlayer->m_bHit && m_pPlayer->m_bKineticRB && (m_pPlayer->m_fKineticCombo_Slash > 0.f) && m_pPlayer->m_bAir && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
//			.Priority(0)
//
//#pragma region ?¬ëž˜??ì½¤ë³´ 1
//
//		.AddState("KINETIC_COMBO_SLASH01")
//		.OnStart([&]() 
//		{
//			m_pPlayer->m_bSeperateAnim = false;
//			m_pPlayer->m_bKineticMove = false;
//			m_pPlayer->m_bKineticCombo = true;
//
//			m_pPlayer->KineticObject_Targeting();
//
//			if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//				m_pPlayer->Enemy_Targeting(true);
//
//			m_pPlayer->m_fKineticCombo_Kinetic = 0.f;
//			m_pPlayer->m_fKineticCombo_Slash = 10.f;
//
//			if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//			{
//				m_pPlayer->m_pTransformCom->LookAt_NonY(CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
//			}
//
//			m_pPlayer->m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_pPlayer->m_KineticCombo_Slash01);
//		})
//		.Tick([&](double fTimeDelta)
//		{
//				m_pPlayer->KineticObject_Targeting();
//
//			if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//			{
//				_vector EnemyPos = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
//				_vector vDirEnemy = EnemyPos - m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
//
//				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));
//
//				if (2 == m_pPlayer->m_pASM->GetSocketSize("Kinetic_Combo_AnimSocket"))
//					m_pPlayer->m_pTransformCom->Chase(EnemyPos, fDistance * 0.02f, 2.f);
//			}
//		})
//
//			.AddTransition("KINETIC_COMBO_SLASH01 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_pPlayer->m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
//			.Priority(0)
//		
//			.AddTransition("KINETIC_COMBO_SLASH01 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool {return m_pPlayer->m_pASM->isSocketAlmostFinish("Kinetic_Combo_AnimSocket"); })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_SLASH01 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool {return (m_pPlayer->m_bLeftClick || m_pPlayer->m_bWalk || m_pPlayer->m_bJump || m_pPlayer->m_bDash) && m_pPlayer->m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.2f); })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_SLASH01 to KINETIC_COMBO_KINETIC01_CHARGE", "KINETIC_COMBO_KINETIC01_CHARGE")
//			.Predicator([&]()->_bool {return m_pPlayer->m_bKineticRB && m_pPlayer->m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.2f) && (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster()) && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
//			.Priority(0)
//
//#pragma endregion ?¬ëž˜??ì½¤ë³´ 1
//
//#pragma region ?¤ë„¤??ì½¤ë³´ 1
//
//		.AddState("KINETIC_COMBO_KINETIC01_CHARGE")
//		.OnStart([&]() 
//		{
//			m_pPlayer->m_bSeperateAnim = false;
//			m_pPlayer->m_bKineticMove = false;
//			m_pPlayer->m_bKineticCombo = true;
//
//			m_pPlayer->KineticObject_Targeting();
//
//			if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//				m_pPlayer->Enemy_Targeting(true);
//
//			if (CPlayerInfoManager::GetInstance()->Get_KineticObject())
//			{
//				m_pPlayer->m_KineticObjectOrigionPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_WorldMatrix();
//				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_Trigger(true);
//			}
//
//			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//			{
//				m_pPlayer->LookTarget();
//			}
//
//			m_pPlayer->m_fKineticCombo_Slash = 0.f;
//			m_pPlayer->m_fKineticCharge = 0.f;
//			m_pPlayer->m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_pPlayer->m_KineticCombo_sLcLeR_Start);
//
//			m_pPlayer->m_pKineticAnimModel->SetPlayAnimation("AS_no0000_245_AL_Pcon_cLeR_Lv1");
//			m_pPlayer->Kinetic_Combo_MoveToKineticPoint();
//
//			// ?„ì‹œë¡??¤ë¡œ ë¹ ì?ê²??˜ëŠ” ì½”ë“œ
//			m_pPlayer->m_vKineticComboRefPoint = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -5.f);
//		})
//		.Tick([&](double fTimeDelta)
//		{
//			if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//			{
//				_vector vMyPos = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
//				_vector vDirEnemy = m_pPlayer->m_vKineticComboRefPoint - vMyPos;
//
//				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));
//
//				_vector vLerpPos = XMVectorLerp(vMyPos, m_pPlayer->m_vKineticComboRefPoint, min(m_pPlayer->m_fKineticCharge, 1.f));
//				m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vLerpPos);
//
//				//m_pTransformCom->Chase(m_vKineticComboRefPoint, fDistance * 0.01f, 0.f);
//
//				m_pPlayer->Kinetic_Combo_AttachLerpObject();
//			}
//
//			m_pPlayer->m_fKineticCharge += (_float)fTimeDelta;
//		})
//
//			.AddTransition("KINETIC_COMBO_KINETIC01_CHARGE to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
//			.Priority(0)
//		
//			.AddTransition("KINETIC_COMBO_KINETIC01_CHARGE to KINETIC_COMBO_KINETIC01_CANCEL", "KINETIC_COMBO_KINETIC01_CANCEL")
//			.Predicator([&]()->_bool {return !m_bKineticRB; })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_KINETIC01_CHARGE to KINETIC_COMBO_KINETIC01_THROW", "KINETIC_COMBO_KINETIC01_THROW")
//			.Predicator([&]()->_bool {return (m_fKineticCharge > 1.f); })
//			.Priority(1)
//
//		.AddState("KINETIC_COMBO_KINETIC01_CANCEL")
//		.OnStart([&]()
//		{
//			m_fKineticCharge = 0.f;
//			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_sLcLeR_End);
//		})
//		.Tick([&](double fTimeDelta)
//		{
//			if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
//			{
//				string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
//				_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
//				m_pTransformCom->LocalMove(vLocal);
//			}
//		})
//
//			.AddTransition("KINETIC_COMBO_KINETIC01_CANCEL to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_bWalk || m_bDash || m_bLeftClick || m_bKineticRB || m_bJump; })
//			.Priority(0)
//
//		.AddState("KINETIC_COMBO_KINETIC01_THROW")
//		.OnStart([&]()
//		{
//			// ë¦¼ë¼?´íŠ¸ ì»¤ë¸Œ ?ì„±
//			Start_RimLight("Kinetic_Combo_01_RimLight");
//
//			m_fKineticCharge = 0.f;
//			m_fKineticCombo_Kinetic = 10.f;
//			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Pcon_cLeR_Lv1);
//
//			Kinetic_Combo_KineticAnimation();
//		})
//		.Tick([&](double fTimeDelta)
//		{
//			// ë¦¼ë¼?´íŠ¸ ì»¤ë¸Œ ?™ìž‘ (? ë‹ˆë©”ì´???¬ìƒ ?ë„??ë§žì¶°??
//			_float fRatio = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetPlayRatio();
//			Tick_RimLight(fRatio);
//
//
//			if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
//			{
//				string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
//				_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
//				m_pTransformCom->LocalMove(vLocal);
//			}
//
//			Kinetic_Combo_KineticAnimation();
//		})
//		.OnExit([&]()
//		{
//			m_pCurve = nullptr;
//
//			// ë¦¼ë¼?´íŠ¸ ì¢…ë£Œ
//			End_RimLight();
//
//		})
//
//			.AddTransition("KINETIC_COMBO_KINETIC01_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_KINETIC01_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.7f) && (m_bDash || m_bJump || m_bKineticRB || m_bWalk); })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_KINETIC01_THROW to KINETIC_COMBO_SLASH02", "KINETIC_COMBO_SLASH02")
//			.Predicator([&]()->_bool {return m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.5f) && m_bLeftClick; })
//			.Priority(0)
//
//#pragma endregion ?¤ë„¤??ì½¤ë³´ 1
//
//#pragma region ?¬ëž˜??ì½¤ë³´ 2
//
//		.AddState("KINETIC_COMBO_SLASH02")
//		.OnStart([&]() 
//		{
//			m_fKineticCombo_Kinetic = 0.f;
//			m_fKineticCombo_Slash = 10.f;
//
//			if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//				Enemy_Targeting(true);
//
//			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//			{
//				LookTarget();
//			}
//
//			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Slash02);
//		})
//		.Tick([&](double fTimeDelta)
//		{
//			KineticObject_Targeting();
//
//			if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//			{
//				_vector EnemyPos = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
//				_vector vDirEnemy = EnemyPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
//
//				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));
//
//				if (2 == m_pASM->GetSocketSize("Kinetic_Combo_AnimSocket"))
//					m_pTransformCom->Chase(EnemyPos, fDistance * 0.02f, 2.f);
//			}
//		})
//
//			.AddTransition("KINETIC_COMBO_SLASH02 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
//			.Priority(0)
//		
//			.AddTransition("KINETIC_COMBO_SLASH02 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_Combo_AnimSocket"); })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_SLASH02 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool {return (m_bLeftClick || m_bDash || m_bJump || m_bWalk) && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.2f); })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_SLASH02 to KINETIC_COMBO_KINETIC02_CHARGE", "KINETIC_COMBO_KINETIC02_CHARGE")
//			.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.2f) && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
//			.Priority(0)
//
//
//#pragma endregion ?¬ëž˜??ì½¤ë³´ 2
//
//#pragma region ?¤ë„¤??ì½¤ë³´ 2
//
//		.AddState("KINETIC_COMBO_KINETIC02_CHARGE")
//		.OnStart([&]() 
//		{
//			KineticObject_Targeting();
//
//			if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//				Enemy_Targeting(true);
//
//			if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
//			{
//				m_KineticObjectOrigionPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_WorldMatrix();
//				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_Trigger(true);
//			}
//
//			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//			{
//				LookTarget();
//			}
//
//			m_bKineticCombo = true;
//			m_fKineticCombo_Slash = 0.f;
//			m_fKineticCharge = 0.f;
//			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_sLcLeR_Start);
//
//			m_pKineticAnimModel->SetPlayAnimation("AS_no0000_252_AL_Pcon_cLeR_Lv2");
//			Kinetic_Combo_MoveToKineticPoint();
//
//			m_vKineticComboRefPoint = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -5.f);
//		})
//		.Tick([&](double fTimeDelta)
//		{
//			if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//			{
//				_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
//				_vector vDirEnemy = m_vKineticComboRefPoint - vMyPos;
//
//				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));
//
//				_vector vLerpPos = XMVectorLerp(vMyPos, m_vKineticComboRefPoint, min(m_fKineticCharge, 1.f));
//				m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vLerpPos);
//
//				//m_pTransformCom->Chase(m_vKineticComboRefPoint, fDistance * 0.01f, 0.f);
//
//				Kinetic_Combo_AttachLerpObject();
//			}
//
//			m_fKineticCharge += (_float)fTimeDelta;
//		})
//
//			.AddTransition("KINETIC_COMBO_KINETIC02_CHARGE to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
//			.Priority(0)
//		
//			.AddTransition("KINETIC_COMBO_KINETIC02_CHARGE to KINETIC_COMBO_KINETIC02_CANCEL", "KINETIC_COMBO_KINETIC02_CANCEL")
//			.Predicator([&]()->_bool {return !m_bKineticRB; })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_KINETIC02_CHARGE to KINETIC_COMBO_KINETIC02_THROW", "KINETIC_COMBO_KINETIC02_THROW")
//			.Predicator([&]()->_bool {return (m_fKineticCharge > 1.f); })
//			.Priority(1)
//
//		.AddState("KINETIC_COMBO_KINETIC02_CANCEL")
//		.OnStart([&]()
//		{
//			m_fKineticCharge = 0.f;
//			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_sLcLeR_End);
//		})
//		.Tick([&](double fTimeDelta)
//		{
//			if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
//			{
//				string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
//				_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
//				m_pTransformCom->LocalMove(vLocal);
//			}
//		})
//
//			.AddTransition("KINETIC_COMBO_KINETIC02_CANCEL to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_bWalk || m_bDash || m_bLeftClick || m_bKineticRB || m_bJump; })
//			.Priority(0)
//
//		.AddState("KINETIC_COMBO_KINETIC02_THROW")
//		.OnStart([&]()
//		{
//			Start_RimLight("Kinetic_Combo_02_RimLight");
//
//			m_fKineticCharge = 0.f;
//			m_fKineticCombo_Kinetic = 10.f;
//			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Pcon_cLeR_Lv2);
//		})
//		.Tick([&](double fTimeDelta)
//		{
//			_float fRatio = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetPlayRatio();
//			Tick_RimLight(fRatio);
//
//			if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
//			{
//				string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
//				_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
//				m_pTransformCom->LocalMove(vLocal);
//			}
//
//			Kinetic_Combo_KineticAnimation();
//		})
//		.OnExit([&]()
//		{
//			End_RimLight();
//		})
//
//			.AddTransition("KINETIC_COMBO_KINETIC02_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_KINETIC02_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.7f) && (m_bDash || m_bJump || m_bKineticRB || m_bWalk); })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_KINETIC02_THROW to KINETIC_COMBO_SLASH03", "KINETIC_COMBO_SLASH03")
//			.Predicator([&]()->_bool {return m_bLeftClick; })
//			.Priority(0)
//
//#pragma endregion ?¤ë„¤??ì½¤ë³´ 2
//
//#pragma region ?¬ëž˜??ì½¤ë³´ 3
//
//		.AddState("KINETIC_COMBO_SLASH03")
//		.OnStart([&]() 
//		{
//			m_fKineticCombo_Kinetic = 0.f;
//			m_fKineticCombo_Slash = 10.f;
//
//			if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//				Enemy_Targeting(true);
//
//			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//			{
//				LookTarget();
//			}
//
//			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Slash03);
//		})
//		.Tick([&](double fTimeDelta)
//		{
//			KineticObject_Targeting();
//
//			if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//			{
//				_vector EnemyPos = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
//				_vector vDirEnemy = EnemyPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
//
//				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));
//
//				if (2 == m_pASM->GetSocketSize("Kinetic_Combo_AnimSocket"))
//					m_pTransformCom->Chase(EnemyPos, fDistance * 0.02f, 2.f);
//			}
//		})
//
//			.AddTransition("KINETIC_COMBO_SLASH03 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
//			.Priority(0)
//		
//			.AddTransition("KINETIC_COMBO_SLASH03 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_Combo_AnimSocket"); })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_SLASH03 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool {return (m_bLeftClick || m_bWalk || m_bJump || m_bDash) && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.25f); })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_SLASH03 to KINETIC_COMBO_KINETIC03_CHARGE", "KINETIC_COMBO_KINETIC03_CHARGE")
//			.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.25f) && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
//			.Priority(0)
//
//#pragma endregion ?¬ëž˜??ì½¤ë³´ 3
//
//#pragma region ?¤ë„¤??ì½¤ë³´ 3
//
//		.AddState("KINETIC_COMBO_KINETIC03_CHARGE")
//		.OnStart([&]() 
//		{
//			KineticObject_Targeting();
//
//			if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//				Enemy_Targeting(true);
//
//			if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
//			{
//				m_KineticObjectOrigionPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_WorldMatrix();
//				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_Trigger(true);
//			}
//
//			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//			{
//				LookTarget();
//			}
//
//			m_bKineticCombo = true;
//			m_fKineticCombo_Slash = 0.f;
//			m_fKineticCharge = 0.f;
//			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_sLcLeR_Start);
//
//			m_pKineticAnimModel->SetPlayAnimation("AS_no0000_259_AL_Pcon_cLeR_Lv3");
//			Kinetic_Combo_MoveToKineticPoint();
//
//			m_vKineticComboRefPoint = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -5.f);
//		})
//		.Tick([&](double fTimeDelta)
//		{
//			if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//			{
//				_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
//				_vector vDirEnemy = m_vKineticComboRefPoint - vMyPos;
//
//				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));
//
//				_vector vLerpPos = XMVectorLerp(vMyPos, m_vKineticComboRefPoint, min(m_fKineticCharge, 1.f));
//				m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vLerpPos);
//
//				//m_pTransformCom->Chase(m_vKineticComboRefPoint, fDistance * 0.01f, 0.f);
//
//				Kinetic_Combo_AttachLerpObject();
//			}
//
//			m_fKineticCharge += (_float)fTimeDelta;
//		})
//
//			.AddTransition("KINETIC_COMBO_KINETIC03_CHARGE to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
//			.Priority(0)
//		
//			.AddTransition("KINETIC_COMBO_KINETIC03_CHARGE to KINETIC_COMBO_KINETIC03_CANCEL", "KINETIC_COMBO_KINETIC03_CANCEL")
//			.Predicator([&]()->_bool {return !m_bKineticRB; })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_KINETIC03_CHARGE to KINETIC_COMBO_KINETIC03_THROW", "KINETIC_COMBO_KINETIC03_THROW")
//			.Predicator([&]()->_bool {return (m_fKineticCharge > 1.f) && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
//			.Priority(1)
//
//		.AddState("KINETIC_COMBO_KINETIC03_CANCEL")
//		.OnStart([&]()
//		{
//			m_fKineticCharge = 0.f;
//			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_sLcLeR_End);
//		})
//		.Tick([&](double fTimeDelta)
//		{
//			if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
//			{
//				string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
//				_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
//				m_pTransformCom->LocalMove(vLocal);
//			}
//		})
//
//			.AddTransition("KINETIC_COMBO_KINETIC03_CANCEL to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_bWalk || m_bDash || m_bLeftClick || m_bKineticRB || m_bJump; })
//			.Priority(0)
//
//		.AddState("KINETIC_COMBO_KINETIC03_THROW")
//		.OnStart([&]()
//		{
//			Start_RimLight("Kinetic_Combo_03_RimLight");
//
//			m_fKineticCharge = 0.f;
//			m_fKineticCombo_Kinetic = 10.f;
//			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Pcon_cLeR_Lv3);
//		})
//		.Tick([&](double fTimeDelta)
//		{
//			_float fRatio = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetPlayRatio();
//			Tick_RimLight(fRatio);
//
//			if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
//			{
//				string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
//				_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
//				m_pTransformCom->LocalMove(vLocal);
//			}
//
//			Kinetic_Combo_KineticAnimation();
//		})
//		.OnExit([&]()
//		{
//			End_RimLight();
//		})
//
//			.AddTransition("KINETIC_COMBO_KINETIC03_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_KINETIC03_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.7f) && (m_bDash || m_bJump || m_bKineticRB || m_bWalk); })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_KINETIC03_THROW to KINETIC_COMBO_SLASH04", "KINETIC_COMBO_SLASH04")
//			.Predicator([&]()->_bool {return m_bLeftClick; })
//			.Priority(0)
//
//
//#pragma endregion ?¤ë„¤??ì½¤ë³´ 3
//
//#pragma region ?¬ëž˜??ì½¤ë³´ 4
//
//		.AddState("KINETIC_COMBO_SLASH04")
//		.OnStart([&]() 
//		{
//			m_fKineticCombo_Kinetic = 0.f;
//			m_fKineticCombo_Slash = 10.f;
//
//			if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//				Enemy_Targeting(true);
//
//			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//			{
//				LookTarget();
//			}
//
//			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Slash04);
//		})
//		.Tick([&](double fTimeDelta)
//		{
//			KineticObject_Targeting();
//
//			if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//			{
//				_vector EnemyPos = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
//				_vector vDirEnemy = EnemyPos - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
//
//				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));
//
//				if (2 == m_pASM->GetSocketSize("Kinetic_Combo_AnimSocket"))
//					m_pTransformCom->Chase(EnemyPos, fDistance * 0.02f, 2.f);
//			}
//		})
//
//			.AddTransition("KINETIC_COMBO_SLASH04 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
//			.Priority(0)
//		
//			.AddTransition("KINETIC_COMBO_SLASH04 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_Combo_AnimSocket"); })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_SLASH04 to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool {return (m_bLeftClick || m_bJump || m_bDash || m_bWalk) && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.5f); })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_SLASH04 to KINETIC_COMBO_KINETIC04_CHARGE", "KINETIC_COMBO_KINETIC04_CHARGE")
//			.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.5f) && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
//			.Priority(0)
//
//#pragma endregion ?¬ëž˜??ì½¤ë³´ 4
//
//#pragma region ?¤ë„¤??ì½¤ë³´ 4
//
//		.AddState("KINETIC_COMBO_KINETIC04_CHARGE")
//		.OnStart([&]() 
//		{
//			KineticObject_Targeting();
//
//			if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//				Enemy_Targeting(true);
//
//			if (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject())
//			{
//				m_KineticObjectOrigionPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_WorldMatrix();
//				static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_Trigger(true);
//			}
//
//			if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//			{
//				LookTarget();
//			}
//
//			m_bKineticCombo = true;
//			m_fKineticCombo_Slash = 0.f;
//			m_fKineticCharge = 0.f;
//			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_sLcLeR_Start);
//
//			m_pKineticAnimModel->SetPlayAnimation("AS_no0000_266_AL_Pcon_cLeR_Lv4");
//			Kinetic_Combo_MoveToKineticPoint();
//
//			m_vKineticComboRefPoint = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -5.f);
//		})
//		.Tick([&](double fTimeDelta)
//		{
//			if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//			{
//				_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
//				_vector vDirEnemy = m_vKineticComboRefPoint - vMyPos;
//
//				_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));
//
//				//m_pTransformCom->Chase(m_vKineticComboRefPoint, fDistance * 0.01f, 0.f);
//
//				_vector vLerpPos = XMVectorLerp(vMyPos, m_vKineticComboRefPoint, min(m_fKineticCharge, 1.f));
//				m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vLerpPos);
//
//				Kinetic_Combo_AttachLerpObject();
//			}
//
//			m_fKineticCharge += (_float)fTimeDelta;
//		})
//
//			.AddTransition("KINETIC_COMBO_KINETIC04_CHARGE to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
//			.Priority(0)
//		
//			.AddTransition("KINETIC_COMBO_KINETIC04_CHARGE to KINETIC_COMBO_KINETIC04_CANCEL", "KINETIC_COMBO_KINETIC04_CANCEL")
//			.Predicator([&]()->_bool {return !m_bKineticRB; })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_KINETIC04_CHARGE to KINETIC_COMBO_KINETIC04_THROW", "KINETIC_COMBO_KINETIC04_THROW")
//			.Predicator([&]()->_bool {return (m_fKineticCharge > 1.f) && (nullptr != CPlayerInfoManager::GetInstance()->Get_KineticObject()); })
//			.Priority(1)
//
//		.AddState("KINETIC_COMBO_KINETIC04_CANCEL")
//		.OnStart([&]()
//		{
//			m_fKineticCharge = 0.f;
//			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_sLcLeR_End);
//		})
//		.Tick([&](double fTimeDelta)
//		{
//			if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
//			{
//				string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
//				_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
//				m_pTransformCom->LocalMove(vLocal);
//			}
//		})
//
//			.AddTransition("KINETIC_COMBO_KINETIC04_CANCEL to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_bWalk || m_bDash || m_bLeftClick || m_bKineticRB || m_bJump; })
//			.Priority(0)
//
//		.AddState("KINETIC_COMBO_KINETIC04_THROW")
//		.OnStart([&]()
//		{
//			Start_RimLight("Kinetic_Combo_04_RimLight");
//			m_fKineticCharge = 0.f;
//			m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Pcon_cLeR_Lv4);
//		})
//		.Tick([&](double fTimeDelta)
//		{
//			_float fRatio = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetPlayRatio();
//			Tick_RimLight(fRatio);
//
//			if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
//			{
//				string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
//				_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
//				m_pTransformCom->LocalMove(vLocal);
//			}
//
//			Kinetic_Combo_KineticAnimation();
//		})
//		.OnExit([&]()
//		{
//			End_RimLight();
//		})
//
//			.AddTransition("KINETIC_COMBO_KINETIC04_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_KINETIC04_THROW to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.8f) && (m_bWalk || m_bDash || m_bJump || m_bLeftClick || m_bKineticRB); })
//			.Priority(0)
//
//
//#pragma endregion ?¤ë„¤??ì½¤ë³´ 4
//
//#pragma region ê³µì¤‘ ì½¤ë³´ ?¤ë„¤??
//
//			.AddState("KINETIC_COMBO_AIR_CAP")
//			.OnStart([&]() 
//			{
//				m_bSeperateAnim = false;
//				m_bKineticMove = false;
//				m_bKineticCombo = true;
//
//				SetAbleState({ false, false, false, false, true, false, true, true, true, false });
//
//				m_pASM->SetCurState("JUMP_FALL");
//				m_pKineticStataMachine->SetState("NO_USE_KINETIC");
//
//				KineticObject_Targeting();
//
//				if (nullptr == CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//					Enemy_Targeting(true);
//
//				if (CPlayerInfoManager::GetInstance()->Get_KineticObject())
//				{
//					m_KineticObjectOrigionPos = CPlayerInfoManager::GetInstance()->Get_KineticObject()->GetTransform()->Get_WorldMatrix();
//					static_cast<CMapKinetic_Object*>(CPlayerInfoManager::GetInstance()->Get_KineticObject())->Set_Trigger(true);
//				}
//
//				if (nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//				{
//					LookTarget();
//				}
//
//				m_fKineticCombo_Slash = 0.f;
//				m_fKineticCharge = 0.f;
//				m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_AirCap);
//
//				m_pKineticAnimModel->SetPlayAnimation("AS_no0000_279_AL_AirPcon");
//				Kinetic_Combo_MoveToKineticPoint();
//
//				// ?„ì‹œë¡??¤ë¡œ ë¹ ì?ê²??˜ëŠ” ì½”ë“œ
//				m_vKineticComboRefPoint = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -5.f);
//			})
//			.Tick([&](double fTimeDelta)
//			{
//				if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//				{
//					_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
//					_vector vDirEnemy = m_vKineticComboRefPoint - vMyPos;
//
//					_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));
//
//					_vector vLerpPos = XMVectorLerp(vMyPos, m_vKineticComboRefPoint, min(m_fKineticCharge, 1.f));
//					m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vLerpPos);
//
//					Kinetic_Combo_AttachLerpObject();
//				}
//
//				if (m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket") != nullptr)
//					m_fKineticCharge = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetPlayRatio();
//				else
//					m_fKineticCharge = 0.f;
//			})
//
//				.AddTransition("KINETIC_COMBO_AIR_CAP to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//				.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
//				.Priority(0)
//
//				.AddTransition("KINETIC_COMBO_AIR_CAP to KINETIC_COMBO_AIR_PCON", "KINETIC_COMBO_AIR_PCON")
//				.Predicator([&]()->_bool { return m_pASM->isSocketAlmostFinish("Kinetic_Combo_AnimSocket"); })
//				.Priority(0)
//
//			.AddState("KINETIC_COMBO_AIR_PCON")
//			.OnStart([&]()
//			{
//				// ë¦¼ë¼?´íŠ¸ ì»¤ë¸Œ ?ì„±
//				Start_RimLight("Kinetic_Combo_01_RimLight");
//
//				m_fKineticCharge = 0.f;
//				m_fKineticCombo_Kinetic = 0.f;
//				m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_Pcon);
//
//				Kinetic_Combo_KineticAnimation();
//			})
//			.Tick([&](double fTimeDelta)
//			{
//				// ë¦¼ë¼?´íŠ¸ ì»¤ë¸Œ ?™ìž‘ (? ë‹ˆë©”ì´???¬ìƒ ?ë„??ë§žì¶°??
//				_float fRatio = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetPlayRatio();
//				Tick_RimLight(fRatio);
//
//
//				if (!m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"))
//				{
//					string szCurAnimName = m_pASM->GetSocketAnimation("Kinetic_Combo_AnimSocket")->GetName();
//					_vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix(), szCurAnimName);
//					m_pTransformCom->LocalMove(vLocal);
//				}
//
//				Kinetic_Combo_KineticAnimation();
//			})
//			.OnExit([&]()
//			{
//				m_pCurve = nullptr;
//
//				// ë¦¼ë¼?´íŠ¸ ì¢…ë£Œ
//				End_RimLight();
//
//				m_bActiveGravity = true;
//			})
//
//			.AddTransition("KINETIC_COMBO_AIR_PCON to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_AIR_PCON to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//			.Predicator([&]()->_bool { return m_pASM->isSocketAlmostFinish("Kinetic_Combo_AnimSocket"); })
//			.Priority(0)
//
//			.AddTransition("KINETIC_COMBO_AIR_PCON to KINETIC_COMBO_AIRSLASH", "KINETIC_COMBO_AIRSLASH")
//			.Predicator([&]()->_bool { return m_bLeftClick && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.5625f); })
//			.Priority(0)
//
//			.AddState("KINETIC_COMBO_AIRSLASH")
//			.OnStart([&]()
//			{
//				m_bActiveGravity = false;
//
//				if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//				{
//					m_pTransformCom->LookAt_NonY(CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
//				}
//
//				m_pASM->InputAnimSocket("Kinetic_Combo_AnimSocket", m_KineticCombo_AirSlash);
//
//			})
//			.Tick([&](double fTimeDelta)
//			{
//				KineticObject_Targeting();
//
//				if (CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
//				{
//					_float4 EnemyPos = CPlayerInfoManager::GetInstance()->Get_TargetedMonster()->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
//					_float4 vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
//
//					EnemyPos.y = vMyPos.y;
//
//					_vector vDirEnemy = XMLoadFloat4(&EnemyPos) - m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
//
//					_float fDistance = XMVectorGetX(XMVector3Length(vDirEnemy));
//
//					if (2 == m_pASM->GetSocketSize("Kinetic_Combo_AnimSocket"))
//						m_pTransformCom->Chase(EnemyPos, fDistance * 0.02f, 2.f);
//				}
//			})
//			.OnExit([&]()
//			{
//				m_bActiveGravity = true;
//			})
//
//				.AddTransition("KINETIC_COMBO_AIRSLASH to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//				.Predicator([&]()->_bool { return m_pASM->isSocketEmpty("Kinetic_Combo_AnimSocket"); })
//				.Priority(0)
//
//				.AddTransition("KINETIC_COMBO_AIRSLASH to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//				.Predicator([&]()->_bool { return m_pASM->isSocketAlmostFinish("Kinetic_Combo_AnimSocket"); })
//				.Priority(0)
//
//				.AddTransition("KINETIC_COMBO_AIRSLASH to KINETIC_COMBO_NOUSE", "KINETIC_COMBO_NOUSE")
//				.Predicator([&]()->_bool { return m_bLeftClick && m_pASM->isSocketPassby("Kinetic_Combo_AnimSocket", 0.8182f); })
//				.Priority(0)
//
//
//#pragma endregion ê³µì¤‘ ì½¤ë³´ ?¤ë„¤??
//
//		.Build();
}

void CPlayerHotFixer::Player_Something_Update()
{
	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pPlayer->m_pSwordParticle_Fire))
	{
		m_pPlayer->m_pSwordParticle_Fire->SetDelete();
		m_pPlayer->m_pSwordParticle_Fire = nullptr;
	}

	m_pPlayer->m_pSwordParticle_Fire = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_FIRE_ATTACK, TEXT("Fire_Weapon_Particle"), LAYER_PLAYEREFFECT);
	_matrix matPivot = XMMatrixRotationY(XMConvertToRadians(180.f));
	m_pPlayer->m_pSwordParticle_Fire->Start_AttachPivot(m_pPlayer, matPivot, "RightWeapon", true, true);
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
