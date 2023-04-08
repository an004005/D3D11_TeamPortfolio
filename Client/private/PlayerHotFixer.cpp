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
				// 타겟 종류에 따라 애니메이션과 연출 캠을 재생시키도록 한다

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

				else if (pTarget->GetPrototypeTag() == L"Monster_em1100")
				{
					m_pPlayer->m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_pPlayer->m_BrainCrash_em1100);
					static_cast<CEM0800*>(CPlayerInfoManager::GetInstance()->Get_TargetedMonster())->PlayBC();

					_vector BC_Pos = m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + (XMVector3Normalize(m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 8.f);
					BC_Pos += (XMVector3Normalize(m_pPlayer->m_pTransformCom->Get_State(CTransform::STATE_RIGHT)) * 3.f);
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

				m_pPlayer->m_pASM->InputAnimSocket("BrainCrash_AnimSocket", m_pPlayer->m_BrainCrash_em1100);
			}
		})
		.Tick([&](double fTimeDelta)
		{
			string szCurAnimName = m_pPlayer->m_pASM->GetSocketAnimation("BrainCrash_AnimSocket")->GetName();
			_float4 vLocal = m_pPlayer->m_pModel->GetLocalMove(m_pPlayer->m_pTransformCom->Get_WorldMatrix(), szCurAnimName);

			// 해당 부분도 타겟 종류에 따라 로컬무브를 다르게 적용시킨다.
			CAnimation* pAnimation = m_pPlayer->m_pModel->Find_Animation("AS_BC_em0200m_ch0100");
			_float fRatio = pAnimation->GetPlayRatio();

			m_pPlayer->m_pTransformCom->LocalMove({ vLocal.x, vLocal.y, vLocal.z });

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

			// em0110 전용
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

			.AddTransition("BRAINFIELD to BRAINFIELD_FINISH_BF", "BRAINFIELD_FINISH_BF")
			.Predicator([&]()->_bool { return (m_pPlayer->m_bBrainField) && (m_pPlayer->m_pController->KeyDown(CController::B)); })
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

			for (auto& iter : m_pPlayer->m_vecWeapon)
				iter->SetVisible(false);

			// 이거 키는거
			m_pPlayer->m_pBrainField->OpenBrainField(); //(여기서 실행하면 됨)

			// m_pBrainField->CloseBrainField();  끄는거
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
		})
		.Tick([&](double fTimeDelta) {})
		.OnExit([&]() { m_pPlayer->m_bBrainField = true; })
			.AddTransition("BRAINFIELD_ACTIONCAM_02 to BRAINFIELD", "BRAINFIELD")
			.Predicator([&]()->_bool { return m_pPlayer->m_pModel->Find_Animation("AS_BrainFieldOpen_c02_ch0100")->IsFinished(); })
			.Priority(0)

		.AddState("BRAINFIELD_FINISH_BF")
		.OnStart([&]() 
		{
			list<CAnimation*> TestAnim;
			TestAnim.push_back(m_pPlayer->m_pModel->Find_Animation("AS_ch0100_BrainField_close_BF"));
			m_pPlayer->m_pASM->AttachAnimSocket("Common_AnimSocket", TestAnim);
		})
		.Tick([&](double fTimeDelta) {})
		.OnExit([&]() {m_pPlayer->m_bBrainField = false; })
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
		})
		.Tick([&](double fTimeDelta) {})
		.OnExit([&]() {})
			.AddTransition("BRAINFIELD_FINISH_NF to BRAINFIELD", "BRAINFIELD")
			.Predicator([&]()->_bool { return m_pPlayer->m_pModel->Find_Animation("AS_ch0100_BrainField_close_NF")->IsFinished(); })
			.Priority(0)

		.Build();

	Safe_Release(pOriginFSM);
}

void CPlayerHotFixer::Player_Something_Update()
{
	// 플레이어에 추가할 내용이나 변경할 내용 있으면 해당 부분에서 추가 컴파일 가능

	m_pPlayer->m_BrandCrash_em0200.clear();
	m_pPlayer->m_BrandCrash_em0200.push_back(m_pPlayer->m_pModel->Find_Animation("AS_BC_em0200m_ch0100"));

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
