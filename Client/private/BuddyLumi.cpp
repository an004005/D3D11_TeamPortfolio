#include "stdafx.h"
#include "BuddyLumi.h"
#include <random>

#include "GameInstance.h"
#include "MathUtils.h"
#include "GameUtils.h"
#include "FSMComponent.h"
#include "AnimationInstance.h"
#include "Animation.h"
#include "Model.h"
#include "JsonStorage.h"
#include "PhysX_Manager.h"

#include "BdLm_Controller.h"
#include "BdLm_AnimInstance.h"
#include "RigidBody.h"
#include "Player.h"

#include "ScarletWeapon.h"
#include "VFX_Manager.h"

#include "MonsterHpUI.h"
CBuddyLumi::CBuddyLumi(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CBuddyLumi::CBuddyLumi(const CBuddyLumi & rhs)
	: CMonster(rhs)
{
}

HRESULT CBuddyLumi::Initialize_Prototype()
{
	return CMonster::Initialize_Prototype();
}

HRESULT CBuddyLumi::Initialize(void * pArg)
{
	Json BuddyLumiTrigger = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BuddyLumi/BuddyLumiTrigger.json");
	MoveTransformJson(BuddyLumiTrigger, pArg);
	pArg = &BuddyLumiTrigger;

	FAILED_CHECK(CMonster::Initialize(pArg));

	Json BuddyLumiWeapon = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BuddyLumi/BuddyLumiWeapon.json");

	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Weapon"),
		(CComponent**)&m_pWeaponCollider, &BuddyLumiWeapon)))
		return E_FAIL;

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"MonsterBuddyLumi", L"Model", (CComponent**)&m_pModelCom));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Proto_BdLm_Controller"), TEXT("Com_Controller"), (CComponent**)&m_pController));

	// Event Caller
	m_pModelCom->Add_EventCaller("SwingEff_Start", [this]
	{
		if (!m_bDead)
		{
//			CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0400_Attack")->Start_Attach(this, m_strBoneName, false);

			m_pSwingEffect = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"em0400_Attack");
			m_pSwingEffect->Start_Attach(this, m_strBoneName, true);
			Safe_AddRef(m_pSwingEffect);
		}
	});

	m_pModelCom->Add_EventCaller("Swing_Start", [this] 
	{ 
		m_bAtkSwitch = true; 
		m_bHitMove = true;
	});
	m_pModelCom->Add_EventCaller("Swing_End", [this]
	{
		m_bHitMove = false;
		m_bAtkSwitch = false; 
	});

	m_pModelCom->Add_EventCaller("SwingEff_End", [this]
	{
		if (m_bCloned == true)
		{
			if (m_pSwingEffect != nullptr)
			{
				m_pSwingEffect->SetDelete();
				Safe_Release(m_pSwingEffect);
				m_pSwingEffect = nullptr;
			}
		}
	});

	m_pModelCom->Add_EventCaller("Upper", [this] 
	{
		m_fGravity = 20.f;
		m_fYSpeed = 10.f;  
	});
	m_pModelCom->Add_EventCaller("Successive", [this] 
	{ 
		m_fGravity = 3.f;
		m_fYSpeed = 1.5f;
	});
	m_pModelCom->Add_EventCaller("AirDamageReset", [this]
	{
		m_fGravity = 20.f;
		m_fYSpeed = 0.f;
	});
	m_pModelCom->Add_EventCaller("Damage_End", [this] { m_bHitMove = false; });
	// ~Event Caller
	m_iMaxHP = 1100;
	m_iHP = m_iMaxHP; // ★
	m_pTransformCom->SetRotPerSec(XMConvertToRadians(220.f));
	m_strBoneName = "RightShoulder";
	m_vFinDir = { 0.f, 0.f, 0.f, 0.f };

	m_pASM = CBdLm_AnimInstance::Create(m_pModelCom, this);

	// 소켓 애니메이션 추가

	m_pAtk_Swing = m_pModelCom->Find_Animation("AS_em0400_200_AL_atk_a1");

	m_pThreat = m_pModelCom->Find_Animation("AS_em0400_160_AL_threat");

	m_pDodgeL = m_pModelCom->Find_Animation("AS_em0400_140_AL_dodge_L");
	m_pDodgeR = m_pModelCom->Find_Animation("AS_em0400_145_AL_dodge_R");
	m_pDodgeB = m_pModelCom->Find_Animation("AS_em0400_135_AL_dodge_B");

	m_pDamage_L_F = m_pModelCom->Find_Animation("AS_em0400_401_AL_damage_l_F");
	m_pDamage_L_B = m_pModelCom->Find_Animation("AS_em0400_402_AL_damage_l_B");

	m_pDamage_M_F = m_pModelCom->Find_Animation("AS_em0400_411_AL_damage_m_F");
	m_pDamage_M_B = m_pModelCom->Find_Animation("AS_em0400_412_AL_damage_m_B");
	m_pDamage_M_L = m_pModelCom->Find_Animation("AS_em0400_413_AL_damage_m_L");
	m_pDamage_M_R = m_pModelCom->Find_Animation("AS_em0400_414_AL_damage_m_R");

	m_pBlowStart = m_pModelCom->Find_Animation("AS_em0400_432_AL_blow_start_F");
	m_pBlowLand = m_pModelCom->Find_Animation("AS_em0400_433_AL_blow_landing_F");
	m_pGetUp = m_pModelCom->Find_Animation("AS_em0400_427_AL_getup_01");
	m_pRiseStart = m_pModelCom->Find_Animation("AS_em0400_455_AL_rise_start");

	m_pDeadAnim = m_pModelCom->Find_Animation("AS_em0400_424_AL_dead_down");

	// ~소켓 애니메이션 추가

	return S_OK;
}

void CBuddyLumi::BeginTick()
{
	__super::BeginTick();
}

void CBuddyLumi::Tick(_double TimeDelta)
{
	/*if (!m_bActive)
		return;*/
	CMonster::Tick(TimeDelta);

	auto pPlayer = CGameInstance::GetInstance()->Find_ObjectByPredicator(LEVEL_NOW, [this](CGameObject* pObj)
	{
		return dynamic_cast<CPlayer*>(pObj) != nullptr;
	});
	m_pTarget = dynamic_cast<CScarletCharacter*>(pPlayer);

	// Controller
	m_pController->SetTarget(m_pTarget);

	if (m_bDead == false)
		m_pController->Tick(TimeDelta);

	m_bRun = m_pController->IsRun();
	_bool bOnfloor = IsOnFloor();

	if (!m_bDead && m_pController->KeyDown(CController::NUM_1))
	{
		m_bHitMove = false;
		m_pASM->AttachAnimSocket("Buddy", { m_pDodgeL });
		m_bAtkSwitch = false;
		m_bOneHit = false;
	}

	if (!m_bDead && m_pController->KeyDown(CController::NUM_2))
	{
		m_bHitMove = false;
		m_pASM->AttachAnimSocket("Buddy", { m_pDodgeR });
		m_bAtkSwitch = false;
		m_bOneHit = false;
	}

	if (!m_bDead && m_pController->KeyDown(CController::NUM_3))
	{
		m_bHitMove = false;
		m_pASM->AttachAnimSocket("Buddy", { m_pDodgeB });
		m_bAtkSwitch = false;
		m_bOneHit = false;
	}

	if (!m_bDead && m_pController->KeyDown(CController::MOUSE_LB))
	{		
		m_pASM->AttachAnimSocket("Buddy", { m_pAtk_Swing });
	}

	if (!m_bDead && m_pController->KeyDown(CController::MOUSE_RB))
	{
		m_bHitMove = false;
		m_pASM->AttachAnimSocket("Buddy", { m_pThreat });
		m_bAtkSwitch = false;
		m_bOneHit = false;
	}

	if (!m_bDead && !m_bAirStruck && m_bStruck && !m_bAirMaintain || m_pController->KeyDown(CController::Q))
	{
		m_bStruck = false;
		m_pController->ClearCommands();

		if (m_eAtkType == EAttackType::ATK_LIGHT)
		{
			if (m_eHitDir == EBaseAxis::NORTH)
			{
				m_pASM->InputAnimSocket("Buddy", { m_pDamage_L_F });
				m_bHitMove = true;
			}
			else
			{
				m_pASM->InputAnimSocket("Buddy", { m_pDamage_L_B });
				m_bHitMove = true;
			}
		}

		if (m_eAtkType == EAttackType::ATK_MIDDLE || m_eAtkType == EAttackType::ATK_HEAVY)
		{
			if (m_eHitDir == EBaseAxis::NORTH)
				m_pASM->InputAnimSocket("Buddy", { m_pDamage_M_F });

			else if (m_eHitDir == EBaseAxis::SOUTH)
				m_pASM->InputAnimSocket("Buddy", { m_pDamage_M_B });

			else if (m_eHitDir == EBaseAxis::WEST)
				m_pASM->InputAnimSocket("Buddy", { m_pDamage_M_L });

			else if (m_eHitDir == EBaseAxis::EAST)
				m_pASM->InputAnimSocket("Buddy", { m_pDamage_M_R });
		}
	}

	if (!m_bDead && !m_bStruck && m_bAirStruck && !m_bAirMaintain || m_pController->KeyDown(CController::X))
	{
		m_bHitMove = false;
		m_bAirStruck = false;
		m_pController->ClearCommands();

		m_pASM->AttachAnimSocket("Buddy", { m_pBlowStart });
		m_bAirMaintain = true;
	}

	if (m_bAirMaintain && !m_bDead && (m_bStruck || m_bAirStruck))
	{
		m_bAirStruck = false;
		m_bStruck = false;
		m_pASM->InputAnimSocket("Buddy", { m_pRiseStart });
	}		

	if (!m_bDead && m_bAirMaintain)
	{
		if (m_pASM->isSocketPassby("Buddy", 0.5f))
		{
			if (bOnfloor)
			{
				m_pASM->InputAnimSocket("Buddy", { m_pBlowLand, m_pGetUp });		
				m_bAirMaintain = false;
			}
		}
	}

	m_fTurnRemain = m_pController->GetTurnRemain();
	m_vMoveAxis = m_pController->GetMoveAxis();

	
	m_pASM->Tick(TimeDelta);

	_float fMoveSpeed = 0.f;

	if (m_bRun)
		fMoveSpeed = 3.f;

	else
		fMoveSpeed = 1.8f;

	m_vMoveAxis.Normalize();

	if (!m_bHitMove)
	{
		const _float fYaw = m_pTransformCom->GetYaw_Radian();
		_float3 vVelocity;
		XMStoreFloat3(&vVelocity, fMoveSpeed * XMVector3TransformNormal(XMVector3Normalize(m_vMoveAxis), XMMatrixRotationY(fYaw)));
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
		m_bOneTick = false;

		m_vPreDir = { 0.f, 0.f, 0.f, 0.f };
		m_vCurDir = { 0.f, 0.f, 0.f, 0.f };
		m_vFinDir = { 0.f, 0.f, 0.f, 0.f };
	}
	else
		HitDir(TimeDelta);
}

void CBuddyLumi::Late_Tick(_double TimeDelta)
{
	/*if (!m_bActive)
		return;*/
	__super::Late_Tick(TimeDelta);

	if (m_bAtkSwitch)
		Swing_SweepCapsule(m_bOneHit);
			
	if (nullptr != m_pRendererCom && m_bVisible)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CBuddyLumi::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CBuddyLumi::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	static char bonename[MAX_PATH]{};

	ImGui::InputText("BoneName", bonename, MAX_PATH);
	if (ImGui::Button("MonsterChangeBoneName"))
	{
		m_strBoneName = bonename;
	}

	// HP Bar Check	
//	ImGui::Text("HP : %d", m_iHP);

	m_pASM->Imgui_RenderState();
}

void CBuddyLumi::SetUp_UI()
{
	//HP UI
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CMonsterHpUI* pUI_HP = nullptr;
	pUI_HP = dynamic_cast<CMonsterHpUI*>(pGameInstance->Clone_GameObject_Get(TEXT("Layer_UI"), TEXT("Prototype_GameObject_MonsterHP")));

	assert(pUI_HP != nullptr);
	pUI_HP->Set_Owner(this);

	_float4x4 UI_PivotMatrix = Matrix(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.41f, 0.0f, 1.0f
	);

	pUI_HP->SetPivotMatrix(UI_PivotMatrix);

	//FindEye
	UI_PivotMatrix = Matrix(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-0.324f, 1.014f, 0.0f, 1.0f
	);

	m_UI_PivotMatrixes[FINDEYES] = UI_PivotMatrix;
}

void CBuddyLumi::TakeDamage(DAMAGE_PARAM tDamageParams)
{
	if (m_bDead)
		return;
	/*
	// 예제 코드
	_vector tmp = _float4{ tDamageParams.vHitFrom.x, tDamageParams.vHitFrom.y , tDamageParams.vHitFrom.z, 1.f };
	_float4 vBackDir = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) - tmp;
	vBackDir.Normalize();

	m_tmp = _float3{ vBackDir.x, vBackDir.y ,vBackDir.z } *2.f;

	*/
	EBaseAxis eHitFrom = CClientUtils::GetDamageFromAxis(m_pTransformCom, tDamageParams.vHitFrom);
	// ↑ 공격이 들어올 방향 
	m_eHitDir = eHitFrom;
	
	m_eAtkType = tDamageParams.eAttackType;
	m_iHP -= tDamageParams.iDamage;

	if (m_eAtkType == EAttackType::ATK_TO_AIR)	
		m_bAirStruck = true;	
	
	if (m_eAtkType != EAttackType::ATK_TO_AIR)	
		m_bStruck = true;

	if (m_eAtkType == EAttackType::ATK_HEAVY || m_bAirStruck)
	{
		if (m_bCloned == true)
		{
			if (m_pSwingEffect != nullptr)
			{
				m_pSwingEffect->SetDelete();
				Safe_Release(m_pSwingEffect);
				m_pSwingEffect = nullptr;
			}

		}
	}
	
	if (m_iHP <= 0 && !m_bDead)
	{
		m_pController->ClearCommands();
		m_DeathTimeline.PlayFromStart();
		m_pASM->InputAnimSocket("Buddy", { m_pDeadAnim });
		m_bDead = true;
	}

	__super::TakeDamage(tDamageParams);
}

void CBuddyLumi::AfterPhysX()
{
	/*if (!m_bActive)
		return;*/
	__super::AfterPhysX();
	m_pWeaponCollider->Update_Tick(AttachCollider(m_pWeaponCollider));
	m_pWeaponCollider->Update_AfterPhysX(m_pTransformCom);
}

void CBuddyLumi::Swing_SweepCapsule(_bool bCol)
{
	if (bCol)
	{
		m_CollisionList.clear();
		return;
	}

	Matrix mWeaponMatrix = m_pWeaponCollider->GetPxWorldMatrix();
	_float4 vTailPos = _float4(mWeaponMatrix.Translation().x, mWeaponMatrix.Translation().y, mWeaponMatrix.Translation().z, 1.f);
	_float3 vLook = _float3(mWeaponMatrix.Up().x, mWeaponMatrix.Up().y, mWeaponMatrix.Up().z);

	physx::PxSweepHit hitBuffer[5];
	physx::PxSweepBuffer overlapOut(hitBuffer, 5);

	PxCapsuleSweepParams param;
	param.sweepOut = &overlapOut;
	param.CapsuleGeo = m_pWeaponCollider->Get_CapsuleGeometry();
	param.pxTransform = m_pWeaponCollider->Get_PxTransform();

	_float4	vWeaponDir = vTailPos - m_BeforePos;

	param.vUnitDir = _float3(vWeaponDir.x, vWeaponDir.y, vWeaponDir.z);
	param.fDistance = param.vUnitDir.Length();
	param.iTargetType = CTB_PLAYER;
	param.fVisibleTime = 0.f;

	if (CGameInstance::GetInstance()->PxSweepCapsule(param))
	{
		for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
		{
			auto pHit = overlapOut.getAnyHit(i);
			CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(pHit.actor);
			if (auto pTarget = dynamic_cast<CScarletCharacter*>(pCollidedObject))
			{
				_bool bDamagedTarget = true;
				for (auto& iter : m_CollisionList)
				{
					if (iter == pTarget)
					{
						bDamagedTarget = false;
						break;
					}
				}
				if (bDamagedTarget)
				{
					DAMAGE_PARAM tParam;

					tParam.pCauser = this;
					tParam.vHitNormal = _float3(pHit.normal.x, pHit.normal.y, pHit.normal.z);
					tParam.vHitPosition = _float3(pHit.position.x, pHit.position.y, pHit.position.z);
					tParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					tParam.iDamage = (rand() % 70) + 40;

					pTarget->TakeDamage(tParam);

					m_CollisionList.push_back(pTarget);
					m_bOneHit = true;
				}
			}
		}
	}

	m_BeforePos = vTailPos;

}

_matrix CBuddyLumi::AttachCollider(CRigidBody* pRigidBody)
{
	_matrix	SocketMatrix;

	if (pRigidBody == m_pWeaponCollider)
		SocketMatrix = m_pModelCom->GetBoneMatrix("RightWeapon") * m_pTransformCom->Get_WorldMatrix();

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	return SocketMatrix;
}

void CBuddyLumi::HitDir(_double TimeDelta)
{	
	// 몸의 중점을 잡는 뼈
	_matrix matRef = m_pModelCom->GetBoneMatrix("Reference") * m_pTransformCom->Get_WorldMatrix();
	_vector vRef = matRef.r[3];

	// 밀려나는 거리만큼의 뼈
	_matrix matWeak = m_pModelCom->GetBoneMatrix("Weak01") * m_pTransformCom->Get_WorldMatrix();
	_vector vWeak = matWeak.r[3];

	// 현재 위치
	_vector	vPosition = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	if (!m_bOneTick)
		m_vPreDir = vPosition;

	// 방향
	_vector vDest = vRef - vWeak;

	m_vCurDir = vDest;

	if (m_bOneTick)
		m_vFinDir = m_vCurDir - m_vPreDir;

	_float fRange = XMVectorGetX(m_vFinDir);

	m_pTransformCom->LocalMove(m_vFinDir);

	m_vPreDir = m_vCurDir;

	m_bOneTick = true;
}

//void CBuddyLumi::SetActive()
//{
//	CMonster::SetActive();
//}

_bool CBuddyLumi::IsPlayingSocket() const
{
	return m_pASM->isSocketEmpty("Buddy") == false;
}

CBuddyLumi * CBuddyLumi::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBuddyLumi* pInstance = new CBuddyLumi(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBuddyLumi");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBuddyLumi::Clone(void * pArg)
{
	CBuddyLumi*		pInstance = new CBuddyLumi(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBuddyLumi");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBuddyLumi::Free()
{
	CMonster::Free();

	if (m_bCloned == true)
	{
		if (m_pSwingEffect != nullptr)
		{
			m_pSwingEffect->SetDelete();
			Safe_Release(m_pSwingEffect);
		}
	}

	Safe_Release(m_pASM);
	Safe_Release(m_pController);
	Safe_Release(m_pWeaponCollider);
}
