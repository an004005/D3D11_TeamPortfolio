#include "stdafx.h"
#include "..\public\Player.h"
#include "GameInstance.h"
#include "Shader.h"
#include "AnimationInstance.h"
#include "Animation.h"
#include "FSMComponent.h"
#include "BaseAnimInstance.h"
#include "Model.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "Controller.h"
#include "ScarletWeapon.h"
#include "Camera.h"
#include "TrailSystem.h"
#include "Weapon_wp0190.h"
#include "RigidBody.h"
#include "EffectSystem.h"
#include "ControlledRigidBody.h"
#include "MapKinetic_Object.h"
#include "PhysX_Manager.h"
#include "Monster.h"
#include <random>
#include "TrailSystem.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CScarletCharacter(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CScarletCharacter(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	if (FAILED(Setup_AnimSocket()))
		return E_FAIL;

	if (FAILED(Setup_Parts()))
		return E_FAIL;

//	if (FAILED(SetUp_Event()))	-> 이건 진짜 천천히 보자...
//		return E_FAIL;

	if (FAILED(SetUp_HitStateMachine()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 2.f, 0.f, 0.f));

	m_pCollider->SetPosition(XMVectorSet(0.f, 2.f, 0.f, 0.f));//SetPxWorldMatrix(m_pTransformCom->Get_WorldMatrix());

	m_pTransformCom->SetTransformDesc({ 1.f, XMConvertToRadians(720.f) });

	m_pPlayerCam = m_pGameInstance->Add_Camera("PlayerCamera", LEVEL_NOW, L"Layer_Camera", L"Prototype_GameObject_Camera_Player");
	Safe_AddRef(m_pPlayerCam);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Json ScifiEffect = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Scifi/Scifi_DefaultAttack_1.json");
	m_pEffect = pGameInstance->Clone_GameObject_Get(L"Layer_PostVFX", L"ProtoVFX_EffectSystem", &ScifiEffect);

	
		/*Json ScifiEffect = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Scifi/Scifi_DefaultAttack_1.json");
	pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoVFX_EffectSystem", &ScifiEffect);*/
	

	//m_pPlayerCam = dynamic_cast<CCamera*>(m_pGameInstance->Clone_GameObject_Get(L"Layer_Camera", TEXT("Prototype_GameObject_Camera_Player")));
	//Assert(m_pPlayerCam != nullptr);
	//Safe_AddRef(m_pPlayerCam);

	return S_OK;
}

void CPlayer::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_pPlayerCam->IsMainCamera())
		m_pController->Tick(TimeDelta);
	else
		m_pController->Invalidate();

	Search_Usable_KineticObject();

	MoveStateCheck(TimeDelta);
	BehaviorCheck(TimeDelta);

	m_pHitStateMachine->Tick(TimeDelta);

	if(!m_bHit)
		m_pKineticStataMachine->Tick(TimeDelta);

	SeperateCheck();

	m_pASM->Tick(TimeDelta);

	if (m_bCanMove)
	{
		_float fSpeedControl = 0.f;

		if (m_bWalk)				fSpeedControl = 0.1f;

		if (m_bCanRun)				fSpeedControl *= 2.f;

		m_pTransformCom->Move(fSpeedControl, m_vMoveDir);
	}

	for (auto& iter : m_vecWeapon)
	{
		iter->Tick(TimeDelta);
		{	// 충돌 판정
			DAMAGE_PARAM tParam;
			ZeroMemory(&tParam, sizeof(DAMAGE_PARAM));
			tParam.iDamage = 1;
			Collision_Check(static_cast<CScarletWeapon*>(iter)->Get_Trigger(), tParam);
		}
	}

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->KeyDown(DIK_L))
	{
		//m_pASM->InputAnimSocket("Upper_Saperate_Animation", m_TransNeutralSocket);
		Jump();
		m_pASM->InputAnimSocket("AnimSocket_Test", m_Fall);
	}

	isPlayerAttack();


	/*if (CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB))
	{
		physx::PxSweepHit hitBuffer[4];
		physx::PxSweepBuffer overlapOut(hitBuffer, 4);
		CapsuleSweepParams params2;
		params2.sweepOut = &overlapOut;
		params2.fRadius = 0.5f;
		params2.fHalfHeight = 2.f;
		params2.vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		params2.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
		params2.vUnitDir = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		params2.fDistance = 3.f;
		params2.iTargetType = CTB_MONSTER;

		if (CGameInstance::GetInstance()->SweepCapsule(params2))
		{
			for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
			{
				auto pHit = overlapOut.getAnyHit(i);
				CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(pHit.actor);
				if (auto pMonster = dynamic_cast<CMonster*>(pCollidedObject))
				{
					DAMAGE_PARAM tParam;
					tParam.iDamage = 1;
					tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					pMonster->TakeDamage(tParam);
				}
			}
		}
	}*/

	//	//physx::PxRaycastHit hitBuffer[1];
	//	//physx::PxRaycastBuffer rayOut(hitBuffer, 1);

	//	//RayCastParams param;
	//	//param.rayOut = &rayOut;
	//	//param.vOrigin = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//	//param.vDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	//	//param.fDistance = 5.f;
	//	//param.iTargetType = CTB_MONSTER;
	//	//param.bSingle = true;
	//	//if (CGameInstance::GetInstance()->RayCast(param))
	//	//{
	//	//	for (int i = 0; i < rayOut.getNbAnyHits(); ++i)
	//	//	{
	//	//		auto pHit = rayOut.getAnyHit(i);
	//	//		CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(pHit.actor);
	//	//		if (auto pMonster = dynamic_cast<CMonster*>(pCollidedObject))
	//	//		{
	//	//			DAMAGE_PARAM tParam;
	//	//			tParam.iDamage = 1;
	//	//			tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//	//			pMonster->TakeDamage(tParam);
	//	//		}

	//	//	}
	//	//}
	//}
}

void CPlayer::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	for (auto& iter : m_vecWeapon)
		iter->Late_Tick(TimeDelta);

	if (m_bVisible && (nullptr != m_pRenderer))
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND_TOON, this);
}

void CPlayer::AfterPhysX()
{
	__super::AfterPhysX();

	for (auto& iter : m_vecWeapon)
	{
		static_cast<CScarletWeapon*>(iter)->Setup_BoneMatrix(m_pModel, m_pTransformCom->Get_WorldMatrix());

	}

	Attack_Effect("Eff01", 1.f);
}

HRESULT CPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pModel->Render(m_pTransformCom);

	return S_OK;
}

void CPlayer::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
	ImGui::SliderFloat("PlayerTurnSpeed", &m_fTurnSpeed, 0.f, 1000.f);
	if (ImGui::Button("TurnAccess"))
	{
		m_pTransformCom->SetTransformDesc({ 1.f, XMConvertToRadians(m_fTurnSpeed) });
	}

	ImGui::SliderFloat("JumpPower", &m_fJumpPower, 0.f, 100.f);

	if (ImGui::CollapsingHeader("Weapons"))
	{
		ImGui::Indent(20.f);
		if (m_vecWeapon.empty() == false)
		{
			m_vecWeapon.front()->Imgui_RenderProperty();
			m_vecWeapon.front()->Imgui_RenderComponentProperties();
		}
		ImGui::Unindent(20.f);
	}

	m_pKineticStataMachine->Imgui_RenderProperty();

	if (ImGui::CollapsingHeader("Weapon"))
	{
		m_vecWeapon.front()->Imgui_RenderComponentProperties();
	}
}

HRESULT CPlayer::SetUp_Components(void * pArg)
{
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRenderer));

	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		if (json.contains("Model"))
		{
			string ProtoModel = json["Model"];
			m_ModelName = CGameUtils::s2ws(ProtoModel);
			FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_ModelName.c_str(), m_ModelName.c_str(),
				(CComponent**)&m_pModel));
		}
	}

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Component_LocalController"), TEXT("Com_Controller"),
		(CComponent**)&m_pController));
	
	NULL_CHECK(m_pASM = CBaseAnimInstance::Create(m_pModel, this));
	FAILED_CHECK(Setup_KineticStateMachine());

	return S_OK;
}

HRESULT CPlayer::SetUp_Event()
{
	m_pModel->Add_EventCaller("Turn_Enable", [&]() {Event_SetCanTurn(true); });
	m_pModel->Add_EventCaller("Turn_Disable", [&]() {Event_SetCanTurn(false); });

	m_pModel->Add_EventCaller("Move_Enable", [&]() {Event_SetCanMove(true); });
	m_pModel->Add_EventCaller("Move_Disable", [&]() {Event_SetCanMove(false); });

	m_pModel->Add_EventCaller("Run_Enable", [&]() {Event_SetCanRun(true); });
	m_pModel->Add_EventCaller("Run_Disable", [&]() {Event_SetCanRun(false); });

	m_pModel->Add_EventCaller("AtkTurn_Enable", [&]() {Event_SetCanTurn_Attack(true); });
	m_pModel->Add_EventCaller("AtkTurn_Disable", [&]() {Event_SetCanTurn_Attack(false); });

	m_pModel->Add_EventCaller("LocalRevise_Enable", [&]() {Event_SetLocalRevise(true); });
	m_pModel->Add_EventCaller("LocalRevise_Disable", [&]() {Event_SetLocalRevise(false); });

	m_pModel->Add_EventCaller("OnAir_Enable", [&]() {Event_SetOnAir(true); });
	m_pModel->Add_EventCaller("OnAir_Disable", [&]() {Event_SetOnAir(false); });

	m_pModel->Add_EventCaller("OnAir_Enable", [&]() {Event_SetOnAir(true); });
	m_pModel->Add_EventCaller("OnAir_Disable", [&]() {Event_SetOnAir(false); });

	m_pModel->Add_EventCaller("Gravity_Enable", [&]() {Event_SetGravity(true); });
	m_pModel->Add_EventCaller("Gravity_Disable", [&]() {Event_SetGravity(false); });

	m_pModel->Add_EventCaller("MoveLimitReset", [&]() {Event_MoveLimitReset(); });
	m_pModel->Add_EventCaller("Event_AttackLimitReset", [&]() {Event_AttackLimitReset(); });
	m_pModel->Add_EventCaller("Event_ChargeReset", [&]() {Event_ChargeReset(); });
	
	return S_OK;
}

HRESULT CPlayer::Setup_KineticStateMachine()
{
	// 염력에 사용할 애니메이션 추가

	CAnimation*	pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_301_AL_capture_wait_start"));
	m_Kinetic_RB_Start.push_back(m_pModel->Find_Animation("AS_ch0100_301_AL_capture_wait_start"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_301_AL_capture_wait_loop"));
	m_Kinetic_RB_Loop.push_back(m_pModel->Find_Animation("AS_ch0100_301_AL_capture_wait_loop"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_301_AL_capture_wait_cancel"));
	m_Kinetic_RB_Cancel.push_back(m_pModel->Find_Animation("AS_ch0100_301_AL_capture_wait_cancel"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_303_AL_throw2_wait_start"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_303_AL_throw2_wait_loop"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_303_AL_throw2_wait_cancel"));
	m_Kinetic_RB_Throw01_Start.push_back(m_pModel->Find_Animation("AS_ch0100_303_AL_throw2_wait_start"));
	m_Kinetic_RB_Throw01_Loop.push_back(m_pModel->Find_Animation("AS_ch0100_303_AL_throw2_wait_loop"));
	m_Kinetic_RB_Throw01_Cancel.push_back(m_pModel->Find_Animation("AS_ch0100_303_AL_throw2_wait_cancel"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_302_AL_throw1_wait_start"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_302_AL_throw1_wait_loop"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_302_AL_throw1_wait_cancel"));
	m_Kinetic_RB_Throw02_Start.push_back(m_pModel->Find_Animation("AS_ch0100_302_AL_throw1_wait_start"));
	m_Kinetic_RB_Throw02_Loop.push_back(m_pModel->Find_Animation("AS_ch0100_302_AL_throw1_wait_loop"));
	m_Kinetic_RB_Throw02_Cancel.push_back(m_pModel->Find_Animation("AS_ch0100_302_AL_throw1_wait_cancel"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_311_AL_AirCap_start"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_311_AL_AirCap_loop"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_311_AL_AirCap_cancel"));
	m_Kinetic_RB_Air_Start.push_back(m_pModel->Find_Animation("AS_ch0100_311_AL_AirCap_start"));
	m_Kinetic_RB_Air_Loop.push_back(m_pModel->Find_Animation("AS_ch0100_311_AL_AirCap_loop"));
	m_Kinetic_RB_Air_Cancel.push_back(m_pModel->Find_Animation("AS_ch0100_311_AL_AirCap_cancel"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_312_AL_AirCap_throw1_start"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_312_AL_AirCap_throw1_loop"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_312_AL_AirCap_throw1_cancel"));
	m_Kinetic_RB_Air_Throw01_Start.push_back(m_pModel->Find_Animation("AS_ch0100_312_AL_AirCap_throw1_start"));
	m_Kinetic_RB_Air_Throw01_Loop.push_back(m_pModel->Find_Animation("AS_ch0100_312_AL_AirCap_throw1_loop"));
	m_Kinetic_RB_Air_Throw01_Cancel.push_back(m_pModel->Find_Animation("AS_ch0100_312_AL_AirCap_throw1_cancel"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_313_AL_AirCap_throw2_start"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_313_AL_AirCap_throw2_loop"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_313_AL_AirCap_throw2_cancel"));
	m_Kinetic_RB_Air_Throw02_Start.push_back(m_pModel->Find_Animation("AS_ch0100_313_AL_AirCap_throw2_start"));
	m_Kinetic_RB_Air_Throw02_Loop.push_back(m_pModel->Find_Animation("AS_ch0100_313_AL_AirCap_throw2_loop"));
	m_Kinetic_RB_Air_Throw02_Cancel.push_back(m_pModel->Find_Animation("AS_ch0100_313_AL_AirCap_throw2_cancel"));

	// ~염력에 사용할 애니메이션 추가

	m_pKineticStataMachine = CFSMComponentBuilder().InitState("NO_USE_KINETIC")

		.AddState("NO_USE_KINETIC")
		.Tick([&](double g_fTimeDelta) { m_bKineticMove = false; })
			.AddTransition("NO_USE_KINETIC to KINETIC_RB_START", "KINETIC_RB_START")
			.Predicator([&]()->_bool{return m_bKineticRB && !m_bAir && (nullptr != m_pKineticObject);})
			.Priority(0)

			.AddTransition("NO_USE_KINETIC to KINETIC_RB_AIR_START", "KINETIC_RB_AIR_START")
			.Predicator([&]()->_bool {return m_bKineticRB && m_bAir && (nullptr != m_pKineticObject); })
			.Priority(0)

#pragma region KineticRB

		.AddState("KINETIC_RB_START")
			.OnStart([&]() { m_pASM->InputAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Start); })
			.Tick([&](double g_fTimeDelta) 
			{ 
				m_bKineticMove = true;

				if (nullptr != m_pKineticObject)
				{
					random_device rng;
					uniform_real_distribution<_double> dist0(-1.0, 1.0);
					uniform_real_distribution<_double> dist1(-1.0, 1.0);
					uniform_real_distribution<_double> dist2(-1.0, 1.0);

					_float3 vForce = _float3(0.f, 120.f, 0.f);
					_float3 vTorque = _float3(dist0(rng), dist1(rng), dist2(rng));
					static_cast<CMapKinetic_Object*>(m_pKineticObject)->Add_Physical(vForce, vTorque);
				}
			})
			.AddTransition("KINETIC_RB_START to KINETIC_RB_LOOP", "KINETIC_RB_LOOP")
			.Predicator([&]()->_bool{return m_bKineticRB && m_pASM->isSocketAlmostFinish("Kinetic_AnimSocket");})
			.Priority(0)

			.AddTransition("KINETIC_RB_START to KINETIC_RB_CANCEL", "KINETIC_RB_CANCEL")
			.Predicator([&]()->_bool{return !m_bKineticRB;})
			.Priority(0)

		.AddState("KINETIC_RB_LOOP")
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Loop); })
			.Tick([&](double g_fTimeDelta) 
			{
				m_bKineticMove = true; 

				if (nullptr != m_pKineticObject)
				{
					random_device rng;
					uniform_real_distribution<_double> dist0(-1.0, 1.0);
					uniform_real_distribution<_double> dist1(-1.0, 1.0);
					uniform_real_distribution<_double> dist2(-1.0, 1.0);

					_float3 vForce = _float3(0.f, 120.f, 0.f);
					_float3 vTorque = _float3(dist0(rng), dist1(rng), dist2(rng));
					static_cast<CMapKinetic_Object*>(m_pKineticObject)->Add_Physical(vForce, vTorque);
				}
			})

			.AddTransition("KINETIC_RB_START to KINETIC_RB_THROW_01_START", "KINETIC_RB_THROW_01_START")
			.Predicator([&]()->_bool{return m_bKineticRB && m_pASM->isSocketAlmostFinish("Kinetic_AnimSocket");})
			.Priority(0)

			.AddTransition("KINETIC_RB_START to KINETIC_RB_CANCEL", "KINETIC_RB_CANCEL")
			.Predicator([&]()->_bool{return !m_bKineticRB;})
			.Priority(0)

		.AddState("KINETIC_RB_CANCEL")
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Cancel); })
			.Tick([&](double g_fTimeDelta) {m_bKineticMove = true; })

			.AddTransition("KINETIC_RB_CANCEL to NO_USE_KINETIC", "NO_USE_KINETIC")
			.Predicator([&]()->_bool{return !m_bKineticRB && m_pASM->isSocketEmpty("Kinetic_AnimSocket");})
			.Priority(0)

			.AddTransition("KINETIC_RB_CANCEL to KINETIC_RB_START", "KINETIC_RB_START")
			.Predicator([&]()->_bool{return m_bKineticRB && (m_pASM->GetSocketAnimation("Kinetic_AnimSocket")->GetPlayRatio() >= 0.2f);})
			.Priority(0)
			
#pragma endregion KineticRB

#pragma region KineticRB_Throw

		// 일반 던지기
		.AddState("KINETIC_RB_THROW_01_START")	// 루프 / 앤드
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Throw01_Start); })
			.Tick([&](double g_fTimeDelta) 
			{
				m_bKineticMove = true;

				if (nullptr != m_pKineticObject)
				{
					_float3 vForce = { XMVectorGetX(m_vCamLook), XMVectorGetY(m_vCamLook), XMVectorGetZ(m_vCamLook) };
					vForce *= 1000.f;
					static_cast<CMapKinetic_Object*>(m_pKineticObject)->Add_Physical(vForce);
				}
			})

			.AddTransition("KINETIC_RB_THROW_01_START to KINETIC_RB_THROW_01_END", "KINETIC_RB_THROW_01_END")
			.Predicator([&]()->_bool {return !m_bKineticRB && m_pASM->isSocketAlmostFinish("Kinetic_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_RB_THROW_01_START to KINETIC_RB_THROW_01_LOOP", "KINETIC_RB_THROW_01_LOOP")
			.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketAlmostFinish("Kinetic_AnimSocket") && m_pASM->CheckSocketAnim("Kinetic_AnimSocket", "AS_ch0100_303_AL_throw2_wait_start"); })
			.Priority(0)

		.AddState("KINETIC_RB_THROW_01_LOOP")	// 앤드 / 스타트2
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Throw01_Loop); })
			.Tick([&](double g_fTimeDelta) {m_bKineticMove = true; }) // 루프 반복되도록 끝나가면 더 넣어주기

			.AddTransition("KINETIC_RB_THROW_01_LOOP to KINETIC_RB_THROW_01_END", "KINETIC_RB_THROW_01_END")
			.Predicator([&]()->_bool {return !m_bKineticRB; })
			.Priority(0)

			.AddTransition("KINETIC_RB_THROW_01_LOOP to KINETIC_RB_THROW_02_START", "KINETIC_RB_THROW_02_START")
			.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketAlmostFinish("Kinetic_AnimSocket") && m_pASM->CheckSocketAnim("Kinetic_AnimSocket", "AS_ch0100_303_AL_throw2_wait_loop"); })
			.Priority(0)

		.AddState("KINETIC_RB_THROW_01_END")	// 종료 / 처음시작
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Throw01_Cancel); })
			.Tick([&](double g_fTimeDelta) {m_bKineticMove = true; })

			.AddTransition("KINETIC_RB_THROW_01_END to NO_USE_KINETIC", "NO_USE_KINETIC")
			.Predicator([&]()->_bool {return !m_bKineticRB && m_pASM->isSocketEmpty("Kinetic_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_RB_THROW_01_END to KINETIC_RB_START", "KINETIC_RB_START")
			.Predicator([&]()->_bool {return m_bKineticRB && (m_pASM->GetSocketAnimation("Kinetic_AnimSocket")->GetPlayRatio() >= 0.2f); })
			.Priority(0)

		// 일반 던지기 연속
		.AddState("KINETIC_RB_THROW_02_START")
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Throw02_Start); })
			.Tick([&](double g_fTimeDelta) {m_bKineticMove = true; })

			.AddTransition("KINETIC_RB_THROW to KINETIC_RB_THROW_02_END", "KINETIC_RB_THROW_02_END")
			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_AnimSocket"); })
			.Priority(0)

		.AddState("KINETIC_RB_THROW_02_LOOP")
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Throw02_Loop); })
			.Tick([&](double g_fTimeDelta) {m_bKineticMove = true; })

			.AddTransition("KINETIC_RB_THROW to KINETIC_RB_THROW_02_END", "KINETIC_RB_THROW_02_END")
			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_AnimSocket"); })
			.Priority(0)

		.AddState("KINETIC_RB_THROW_02_END")
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_AnimSocket", m_Kinetic_RB_Throw02_Cancel); })
			.Tick([&](double g_fTimeDelta) {m_bKineticMove = true; })

			.AddTransition("KINETIC_RB_THROW to NO_USE_KINETIC", "NO_USE_KINETIC")
			.Predicator([&]()->_bool {return m_pASM->isSocketEmpty("Kinetic_AnimSocket"); })
			.Priority(0)

#pragma endregion KineticRB_Throw

#pragma region KineticRB_Air

		.AddState("KINETIC_RB_AIR_START")
		.OnStart([&]() { m_pASM->InputAnimSocket("Kinetic_Air_AnimSocket", m_Kinetic_RB_Air_Start); })
		.Tick([&](double g_fTimeDelta) { m_bKineticMove = false; m_bCanMove = false; m_bActiveGravity = false; })

			.AddTransition("KINETIC_RB_AIR_START to KINETIC_RB_AIR_LOOP", "KINETIC_RB_AIR_LOOP")
			.Predicator([&]()->_bool{return m_bKineticRB && m_pASM->isSocketAlmostFinish("Kinetic_Air_AnimSocket");})
			.Priority(0)

			.AddTransition("KINETIC_RB_AIR_START to KINETIC_RB_AIR_CANCEL", "KINETIC_RB_AIR_CANCEL")
			.Predicator([&]()->_bool{return !m_bKineticRB;})
			.Priority(0)

		.AddState("KINETIC_RB_AIR_LOOP")
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_Air_AnimSocket", m_Kinetic_RB_Air_Loop); })
			.Tick([&](double g_fTimeDelta) { m_bKineticMove = false; m_bCanMove = false; m_bActiveGravity = false; })

			.AddTransition("KINETIC_RB_AIR_LOOP to KINETIC_RB_AIR_THROW_01_START", "KINETIC_RB_AIR_THROW_01_START")
			.Predicator([&]()->_bool{return m_bKineticRB && m_pASM->isSocketAlmostFinish("Kinetic_Air_AnimSocket");})
			.Priority(0)

			.AddTransition("KINETIC_RB_AIR_LOOP to KINETIC_RB_AIR_CANCEL", "KINETIC_RB_AIR_CANCEL")
			.Predicator([&]()->_bool{return !m_bKineticRB;})
			.Priority(0)

		.AddState("KINETIC_RB_AIR_CANCEL")
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_Air_AnimSocket", m_Kinetic_RB_Air_Cancel); })
			.Tick([&](double g_fTimeDelta) { m_bKineticMove = false; m_bCanMove = false; m_bActiveGravity = false; })
			.OnExit([&]() {m_bCanMove = true; m_bActiveGravity = true; })

			.AddTransition("KINETIC_RB_AIR_CANCEL to NO_USE_KINETIC", "NO_USE_KINETIC")
			.Predicator([&]()->_bool{return !m_bKineticRB && m_pASM->isSocketAlmostFinish("Kinetic_Air_AnimSocket");})	// 취소시간만 조절해주면 될듯
			.Priority(0)

			.AddTransition("KINETIC_RB_AIR_CANCEL to KINETIC_RB_AIR_START", "KINETIC_RB_AIR_START")
			.Predicator([&]()->_bool{return m_bKineticRB && (m_pASM->GetSocketAnimation("Kinetic_Air_AnimSocket")->GetPlayRatio() >= 0.2f);})
			.Priority(0)

#pragma endregion KineticRB_Air

#pragma region KineticRB_Air_Throw

		.AddState("KINETIC_RB_AIR_THROW_01_START")
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_Air_AnimSocket", m_Kinetic_RB_Air_Throw01_Start); })
			.Tick([&](double g_fTimeDelta) {m_bKineticMove = false; m_bCanMove = false; m_bActiveGravity = false; })

			.AddTransition("KINETIC_RB_AIR_THROW_01_START to KINETIC_RB_AIR_THROW_01_END", "KINETIC_RB_AIR_THROW_01_END")
			.Predicator([&]()->_bool {return !m_bKineticRB && m_pASM->isSocketAlmostFinish("Kinetic_Air_AnimSocket"); })
			.Priority(0)

			.AddTransition("KINETIC_RB_AIR_THROW_01_START to KINETIC_RB_AIR_THROW_01_LOOP", "KINETIC_RB_AIR_THROW_01_LOOP")
			.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketAlmostFinish("Kinetic_Air_AnimSocket") && m_pASM->CheckSocketAnim("Kinetic_Air_AnimSocket", "AS_ch0100_312_AL_AirCap_throw1_start"); })
			.Priority(0)

		.AddState("KINETIC_RB_AIR_THROW_01_LOOP")
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_Air_AnimSocket", m_Kinetic_RB_Air_Throw01_Loop); })
			.Tick([&](double g_fTimeDelta) {m_bKineticMove = false; m_bCanMove = false; m_bActiveGravity = false; }) // 루프 반복되도록 끝나가면 더 넣어주기

			.AddTransition("KINETIC_RB_AIR_THROW_01_LOOP to KINETIC_RB_AIR_THROW_01_END", "KINETIC_RB_AIR_THROW_01_END")
			.Predicator([&]()->_bool {return !m_bKineticRB; })
			.Priority(0)

			.AddTransition("KINETIC_RB_AIR_THROW_01_LOOP to KINETIC_RB_AIR_THROW_02_START", "KINETIC_RB_AIR_THROW_02_START")
			.Predicator([&]()->_bool {return m_bKineticRB && m_pASM->isSocketAlmostFinish("Kinetic_Air_AnimSocket") && m_pASM->CheckSocketAnim("Kinetic_Air_AnimSocket", "AS_ch0100_312_AL_AirCap_throw1_loop"); })
			.Priority(0)

		.AddState("KINETIC_RB_AIR_THROW_01_END")
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_Air_AnimSocket", m_Kinetic_RB_Air_Throw01_Cancel); })
			.Tick([&](double g_fTimeDelta) {m_bKineticMove = false; m_bCanMove = true; m_bActiveGravity = true; })

			.AddTransition("KINETIC_RB_AIR_THROW_01_END to NO_USE_KINETIC", "NO_USE_KINETIC")
			.Predicator([&]()->_bool {return !m_bKineticRB && m_pASM->isSocketPassby("Kinetic_Air_AnimSocket", 0.7f); })
			.Priority(0)

			.AddTransition("KINETIC_RB_AIR_THROW_01_END to KINETIC_RB_AIR_START", "KINETIC_RB_AIR_START")
			.Predicator([&]()->_bool {return m_bKineticRB && (m_pASM->GetSocketAnimation("Kinetic_Air_AnimSocket")->GetPlayRatio() >= 0.2f); })
			.Priority(0)

		.AddState("KINETIC_RB_AIR_THROW_02_START")
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_Air_AnimSocket", m_Kinetic_RB_Air_Throw02_Start); })
			.Tick([&](double g_fTimeDelta) {m_bKineticMove = false; m_bCanMove = false; m_bActiveGravity = false; })

			.AddTransition("KINETIC_RB_AIR_THROW_02_START to KINETIC_RB_AIR_THROW_02_END", "KINETIC_RB_AIR_THROW_02_END")
			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_Air_AnimSocket"); })
			.Priority(0)

		.AddState("KINETIC_RB_AIR_THROW_02_LOOP")
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_Air_AnimSocket", m_Kinetic_RB_Air_Throw02_Loop); })
			.Tick([&](double g_fTimeDelta) {m_bKineticMove = false; m_bCanMove = false; m_bActiveGravity = false; })

			.AddTransition("KINETIC_RB_AIR_THROW_02_LOOP to KINETIC_RB_AIR_THROW_02_END", "KINETIC_RB_AIR_THROW_02_END")
			.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Kinetic_Air_AnimSocket"); })
			.Priority(0)

		.AddState("KINETIC_RB_AIR_THROW_02_END")
			.OnStart([&]() { m_pASM->AttachAnimSocket("Kinetic_Air_AnimSocket", m_Kinetic_RB_Air_Throw02_Cancel); })
			.Tick([&](double g_fTimeDelta) {m_bKineticMove = false; m_bCanMove = true; m_bActiveGravity = true; })

			.AddTransition("KINETIC_RB_AIR_THROW_02_END to NO_USE_KINETIC", "NO_USE_KINETIC")
			.Predicator([&]()->_bool {return m_pASM->isSocketPassby("Kinetic_Air_AnimSocket", 0.7f) || (!m_bAir); })
			.Priority(0)

#pragma endregion KineticRB_Air_Throw

		.Build();

	return S_OK;
}

HRESULT CPlayer::SetUp_HitStateMachine()
{
	CAnimation*	pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_401_AL_damage_l_FL"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_401_AL_damage_l_F"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_401_AL_damage_l_FR"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_402_AL_damage_l_B"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_411_AL_damage_h_F"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_412_AL_damage_h_B"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_413_AL_damage_h_L"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_414_AL_damage_h_R"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_433_AL_blow_landing_F"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_434_AL_down_F"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_435_AL_wakeup_F"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_438_AL_breakfall_F"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_455_AL_rise_start"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_457_AL_fall_start"));

	m_Hit_FL_Level01.push_back(m_pModel->Find_Animation("AS_ch0100_401_AL_damage_l_FL"));
	m_Hit_F_Level01.push_back(m_pModel->Find_Animation("AS_ch0100_401_AL_damage_l_F"));
	m_Hit_FR_Level01.push_back(m_pModel->Find_Animation("AS_ch0100_401_AL_damage_l_FR"));
	m_Hit_B_Level01.push_back(m_pModel->Find_Animation("AS_ch0100_402_AL_damage_l_B"));

	m_Hit_F_Level02.push_back(m_pModel->Find_Animation("AS_ch0100_411_AL_damage_h_F"));
	m_Hit_B_Level02.push_back(m_pModel->Find_Animation("AS_ch0100_412_AL_damage_h_B"));
	m_Hit_L_Level02.push_back(m_pModel->Find_Animation("AS_ch0100_413_AL_damage_h_L"));
	m_Hit_R_Level02.push_back(m_pModel->Find_Animation("AS_ch0100_414_AL_damage_h_R"));

	m_Knuckback.push_back(m_pModel->Find_Animation("AS_ch0100_455_AL_rise_start"));

	m_Fall.push_back(m_pModel->Find_Animation("AS_ch0100_457_AL_fall_start"));

	m_FallDown_Back.push_back(m_pModel->Find_Animation("AS_ch0100_433_AL_blow_landing_F"));
	m_FallDown_Back.push_back(m_pModel->Find_Animation("AS_ch0100_434_AL_down_F"));
	m_FallDown_Back.push_back(m_pModel->Find_Animation("AS_ch0100_435_AL_wakeup_F"));

	m_BreakFall_Front.push_back(m_pModel->Find_Animation("AS_ch0100_438_AL_breakfall_F"));

	m_pHitStateMachine = CFSMComponentBuilder().InitState("NON_HIT")
		.AddState("NON_HIT")
		.OnStart([&]() {m_bHit = false; })
		.OnExit([&]() {m_bHit = true; })

		.AddTransition("NON_HIT to AIRBORNE", "AIRBORNE")
		.Predicator([&]()->_bool {return m_bTestKey && m_bAir; })
		.Priority(0)

		.AddTransition("NON_HIT to KNUCKBACK", "KNUCKBACK")
		.Predicator([&]()->_bool {return m_bTestKey && !m_bAir; })
		.Priority(0)

		.AddState("KNUCKBACK")
		.OnStart([&]() { m_pASM->InputAnimSocket("Hit_AnimSocket", m_Knuckback);})
		.Tick([&](double g_fTimeDelta) 
		{ 
			m_bWalk = false;
			_vector vOpTest = m_pModel->GetOptionalMoveVector(m_pTransformCom->Get_WorldMatrix(), "AS_ch0100_455_AL_rise_start");
			m_pTransformCom->LocalMove(vOpTest);
		})
		.AddTransition("KNUCKBACK to FALLDOWN", "FALLDOWN")
		.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Hit_AnimSocket");; })
		.Priority(0)

		.AddState("AIRBORNE")
		.OnStart([&]() { m_pASM->InputAnimSocket("Hit_AnimSocket", m_Knuckback); Jump(); })
		.Tick([&](double g_fTimeDelta) 
		{ 
			m_bWalk = false;
			m_bAir = true;
		})
		.AddTransition("AIRBORNE to FALL", "FALL")
		.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Hit_AnimSocket"); })
		.Priority(1)

		.AddTransition("AIRBORNE to FALLDOWN", "FALLDOWN")
		.Predicator([&]()->_bool {return m_bOnFloor && m_pASM->isSocketAlmostFinish("Hit_AnimSocket");; })
		.Priority(0)

		.AddState("FALL")
		.OnStart([&]() { m_pASM->AttachAnimSocket("Hit_AnimSocket", m_Fall); })
		.Tick([&](double g_fTimeDelta) 
		{
			m_bWalk = false;
			m_bAir = true;

			if (m_pASM->isSocketAlmostFinish("Hit_AnimSocket"))
			{
				m_pASM->AttachAnimSocket("Hit_AnimSocket", m_Fall);
			}
		})
		.AddTransition("FALL to FALLDOWN", "FALLDOWN")
		.Predicator([&]()->_bool {return m_bOnFloor; })
		.Priority(0)

		.AddState("FALLDOWN")
		.OnStart([&]() { m_pASM->InputAnimSocket("Hit_AnimSocket", m_FallDown_Back); })
		.Tick([&](double g_fTimeDelta) 
		{
			m_bWalk = false;
			m_bAir = false;
		})
		.AddTransition("FALLDOWN to NON_HIT", "NON_HIT")
		.Predicator([&]()->_bool {return m_pASM->isSocketAlmostFinish("Hit_AnimSocket"); })
		.Priority(0)


		.Build();

	return S_OK;
}

HRESULT CPlayer::Setup_AnimSocket()
{
	CAnimation*	pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_011_AL_walk_start_F"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_016_AL_walk"));
	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_018_AL_walk_end"));

	m_TestAnimSocket.push_back(pAnimation = m_pModel->Find_Animation("AS_ch0100_011_AL_walk_start_F"));
	m_TestAnimSocket.push_back(pAnimation = m_pModel->Find_Animation("AS_ch0100_016_AL_walk"));
	m_TestAnimSocket.push_back(pAnimation = m_pModel->Find_Animation("AS_ch0100_018_AL_walk_end"));

	NULL_CHECK(pAnimation = m_pModel->Find_Animation("AS_ch0100_104_Up_trans_neutral"));
	m_TransNeutralSocket.push_back(m_pModel->Find_Animation("AS_ch0100_104_Up_trans_neutral"));

	return S_OK;
}

_bool CPlayer::isPlayerAttack(void)
{
	string szCurAnim = "";

	if (nullptr != m_pModel->GetPlayAnimation())
		szCurAnim = m_pModel->GetPlayAnimation()->GetName();

	string szKeyString = "atk";

	if (szCurAnim.find(szKeyString) != string::npos)
	{
		//IM_LOG(szCurAnim.c_str());
		return true;
	}

	return false;
}

_bool CPlayer::BeforeCharge(_float fBeforeCharge)
{
	if (fBeforeCharge > m_fBefCharge)
	{
		m_fBefCharge += g_fTimeDelta;
		return false;
	}
	else
	{
		m_fBefCharge = 0.f;
		return true;
	}
}

_bool CPlayer::Charge(_uint iNum, _float fCharge)
{
	if (fCharge > m_fCharge[iNum])
	{
		m_fCharge[iNum] += g_fTimeDelta;
		return false;
	}
	else
	{
		//IM_LOG(to_string(m_fCharge[iNum]).c_str());
		return true;
	}
}

CPlayer & CPlayer::SetAbleState(REMOTE tagRemote)
{
	m_bCanTurn = tagRemote.CanTurn;
	m_bCanMove = tagRemote.CanMove;
	m_bCanRun = tagRemote.CanRun;
	m_bCanTurn_Attack = tagRemote.AttackTurn;

	m_bAir = tagRemote.OnAir;
	m_bActiveGravity = tagRemote.Gravity;

	if (tagRemote.MoveLimitReset) { MoveLimitReset(); }
	if (tagRemote.AttackLimitReset) { AttackLimitReset(); }
	if (tagRemote.ChargeReset) { Reset_Charge(); }

	m_bLocalRevise = tagRemote.LocalRevise;

	return *this;
}

void CPlayer::Reset_Charge()
{
	m_fBefCharge = 0.f;
	for (auto& iter : m_fCharge)
		iter = 0.f;
}

_bool CPlayer::UseAttackCnt(_uint eType)
{
	switch (eType)
	{
	case LIMIT_NONCHARGE_FLOOR:
		if (0 < m_AttackLimit.m_iNonChargeAttack_Floor) { m_AttackLimit.m_iNonChargeAttack_Floor -= 1; return true; }
		break;
	case LIMIT_NONCHARGE_AIR:
		if (0 < m_AttackLimit.m_iNonChargeAttack_Air) { m_AttackLimit.m_iNonChargeAttack_Air -= 1; return true; }
		break;
	case LIMIT_AIRATK01:
		if (0 < m_AttackLimit.m_iAttack_Air01) { m_AttackLimit.m_iAttack_Air01 -= 1; return true; }
		break;
	case LIMIT_AIRATK02:
		if (0 < m_AttackLimit.m_iAttack_Air02) { m_AttackLimit.m_iAttack_Air02 -= 1; return true; }
		break;
	case LIMIT_AIRDODGEATK:
		if (0 < m_AttackLimit.m_iAttack_AirDodge) { m_AttackLimit.m_iAttack_AirDodge -= 1; return true; }
		break;
	default:
		break;
	}
	return false;
}

_bool CPlayer::UseMoveCnt(_uint eType)
{
	switch (eType)
	{
	case LIMIT_DOUBLEJUMP:
		if (0 < m_MoveLimit.m_iDoubleJump) { m_MoveLimit.m_iDoubleJump -= 1; return true; }
		break;
	case LIMIT_AIRDODGE:
		if (0 < m_MoveLimit.m_iAirDodge) { m_MoveLimit.m_iAirDodge -= 1; return true; }
		break;
	default:
		break;
	}
	return false;
}

void CPlayer::AttackLimitReset(void)
{
	m_AttackLimit.m_iNonChargeAttack_Floor = m_AttackLimit.MAX_iNonChargeAttack_Floor;
	m_AttackLimit.m_iNonChargeAttack_Air = m_AttackLimit.MAX_iNonChargeAttack_Air;
	m_AttackLimit.m_iAttack_Air01 = m_AttackLimit.MAX_iAttack_Air01;
	m_AttackLimit.m_iAttack_Air02 = m_AttackLimit.MAX_iAttack_Air02;
	m_AttackLimit.m_iAttack_AirDodge = m_AttackLimit.MAX_iAttack_AirDodge;
}

void CPlayer::MoveLimitReset(void)
{
	m_MoveLimit.m_iDoubleJump = m_MoveLimit.MAX_iDoubleJump;
	m_MoveLimit.m_iAirDodge = m_MoveLimit.MAX_iAirDodge;
}

void CPlayer::Jump()
{
	m_fYSpeed = m_fJumpPower;
}

void CPlayer::SmoothTurn_Attack(_double TimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Vector4 vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPlayerPos.w = 1.f;

	Vector4 vCamLook = pGameInstance->Get_CamLook();
	vCamLook.y = 0.f;
	vCamLook.w = 0.f;
	vCamLook.Normalize();

	Vector4 vTarget = vPlayerPos + vCamLook;

	m_pTransformCom->LookAt_Smooth(vTarget, TimeDelta * 0.5f);
}

void CPlayer::BehaviorCheck(_double TimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	m_fPlayRatio = 0.f;
	m_bLeftClick = false;
	
	if (nullptr != m_pModel->GetPlayAnimation())
		m_fPlayRatio = m_pModel->GetPlayAnimation()->GetPlayRatio();
	m_bLeftClick = m_pController->KeyDown(CController::MOUSE_LB);
	m_bDash = m_pController->KeyDown(CController::SHIFT);

	m_bNonCharge = m_pController->KeyUp(CController::C);
	m_bCharge = m_pController->KeyPress(CController::C);

	if (!m_bCharge) m_fBefCharge = 0.f;

	m_bJump = m_pController->KeyDown(CController::SPACE);

	if (m_bCanTurn_Attack)
	{
		SmoothTurn_Attack(TimeDelta);
	}

	m_bKineticRB = m_pController->KeyPress(CController::MOUSE_RB);
	m_bKineticG = m_pController->KeyPress(CController::G);

	m_bTestKey = m_pController->KeyDown(CController::E);

	m_vCamLook = pGameInstance->Get_CamLook();

	//_bool bCurIdle = false;
	//if(m_pModel->GetPlayAnimation() != nullptr)
	//	bCurIdle = m_pModel->GetPlayAnimation()->GetName() != "AS_ch0100_002_AL_wait02";
}

void CPlayer::MoveStateCheck(_double TimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	m_bWalk = false;
	m_eMoveDir = DIR_END;

	m_vMoveDir = m_pController->GetMoveAxis();

	Vector4 vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	vPlayerPos.w = 1.f;

	m_vLastDir = vPlayerPos - m_vBefPos;
	m_vLastDir.w = 0.f;
	m_vBefPos = vPlayerPos;

	if (0.f != XMVectorGetX(XMVector3Length(m_vMoveDir)))
	{
		m_bWalk = true;

		Vector3 vPlayerLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		vPlayerLook.y = 0.f;
		vPlayerLook.Normalize();

		Vector3 vPlayerRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
		vPlayerRight.y = 0.f;
		vPlayerRight.Normalize();

		Matrix matCam = pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);
		_vector vScale, vRotate, vTrans;
		XMMatrixDecompose(&vScale, &vRotate, &vTrans, matCam);
		Matrix matCamRot = XMMatrixRotationQuaternion(vRotate);
		m_vMoveDir = XMVector3TransformNormal(m_vMoveDir, matCamRot);
		m_vMoveDir.y = 0.f;
		m_vMoveDir.Normalize();

		_float fAxis_Look = vPlayerLook.Dot(m_vMoveDir);
		_float fAxis_Right = vPlayerRight.Dot(m_vMoveDir);

		if (fabs(fAxis_Look) > fabs(fAxis_Right))
		{
			if (0.f < fAxis_Look) { m_eMoveDir = DIR_F; }
			else { m_eMoveDir = DIR_B; }
		}
		else
		{
			if (0.f < fAxis_Right) { m_eMoveDir = DIR_R; }
			else { m_eMoveDir = DIR_L; }
		}

		if (m_bCanTurn)
			m_pTransformCom->LookAt_Smooth(vPlayerPos + m_vMoveDir, TimeDelta);

		if (m_bAir)	// 공중 상태에선 회전이 먹지 않음
			m_vMoveDir = vPlayerLook;

		if (m_bLocalRevise)
		{
			Vector4 vCamLook = pGameInstance->Get_CamLook();
			m_pTransformCom->SetAxis(CTransform::STATE_LOOK, vCamLook);
		}

	}
}

void CPlayer::SeperateCheck()
{
	m_bSeperateAnim = true;

	if (m_pModel->GetPlayAnimation() != nullptr)
	{
		m_bSeperateAnim = (m_pModel->GetPlayAnimation()->GetName() != "AS_ch0100_002_AL_wait02") &&
			(m_bKineticMove || (!m_pASM->isSocketEmpty("Upper_Saperate_Animation")));
	}
}

void CPlayer::LookAtDir(Vector3 Vector)
{
	m_pTransformCom->LookAt(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + XMVectorSet(Vector.x, 0.f, Vector.z, 0.f));
}

HRESULT CPlayer::Setup_Parts()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json Weapon = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/wp0190.json");
	Weapon["Model"] = "../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model";

	CGameObject*	pGameObject = nullptr;

	WEAPON_DESC		Desc;
	ZeroMemory(&Desc, sizeof(WEAPON_DESC));
	Desc.m_PivotMatrix = m_pModel->GetPivotMatrix();
	Desc.m_pSocket = m_pModel->Get_BonePtr("RightWeapon");
	Desc.m_pTransform = m_pTransformCom;
	Desc.m_pJson = &Weapon;

	//pGameInstance->Clone_GameObject(TEXT("Layer_Player"), TEXT("PlayerWeapon"), &Desc);

	pGameObject = pGameInstance->Clone_GameObject_NoLayer(LEVEL_NOW, TEXT("PlayerWeapon"), &Desc);
	m_vecWeapon.push_back(pGameObject);

	return S_OK;
}

void CPlayer::Attack_Effect(const string& szBoneName, _float fSize)
{
	_matrix	SocketMatrix = m_pModel->GetPivotMatrix() * m_pModel->GetBoneMatrix(szBoneName) * m_pTransformCom->Get_WorldMatrix();

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]) *	fSize;
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]) * fSize;
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]) * fSize;

	static_cast<CEffectSystem*>(m_pEffect)->GetTransform()->Set_WorldMatrix(SocketMatrix);
}

void CPlayer::Search_Usable_KineticObject()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();



	if (pGameInstance->GetLayer(LEVEL_NOW, L"Layer_Kinetic") == nullptr
		|| pGameInstance->GetLayer(LEVEL_NOW, L"Layer_Kinetic")->GetGameObjects().empty())
	{
		m_pKineticObject = nullptr;
	}
	else
	{
		for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, L"Layer_Kinetic")->GetGameObjects())
		{
			static _float fDistance = 10.f;

			_vector vTargetPos = iter->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
			_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

			_float fCurDistance = XMVectorGetX(XMVector3Length(vTargetPos - vMyPos));

			if (fDistance > fCurDistance)
			{
				m_pKineticObject = iter;
			}
		}
	}
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto& iter : m_vecWeapon)
		Safe_Release(iter);
	m_vecWeapon.clear();

	Safe_Release(m_pKineticStataMachine);
	Safe_Release(m_pHitStateMachine);
	Safe_Release(m_pTrail);
	Safe_Release(m_pASM);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pModel);
	Safe_Release(m_pController);
	Safe_Release(m_pPlayerCam);
}
