#include "stdafx.h"
#include "BuddyLumi.h"
#include "GameInstance.h"
#include "MathUtils.h"
#include "GameUtils.h"
#include "FSMComponent.h"
#include "AnimationInstance.h"
#include "Animation.h"
#include "Model.h"
#include "JsonStorage.h"

#include "BdLm_AnimInstance.h"
#include "TimerHelper.h"
#include "FlowerLeg.h"
#include "Player.h"

#include "RigidBody.h"
#include "ScarletWeapon.h"
#include "PhysX_Manager.h"

// TODO : Turn, 작업해둔 소켓 Test (Player와의 피`타격)

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
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBuddyLumi::Initialize(void * pArg)
{
	Json BuddyLumiTrigger = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BuddyLumiTrigger.json");
	Json BuddyLumiWeapon = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BuddyLumiWeapon.json");

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	if (FAILED(Setup_AnimSocket()))
		return E_FAIL;

	if (FAILED(SetUp_Event()))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Weapon"),
		(CComponent**)&m_pWeaponCollider, &BuddyLumiWeapon)))
		return E_FAIL;	

	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Trigger"),
		(CComponent**)&m_pTrigger, &BuddyLumiTrigger)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat3(&_float3(15.f, 0.f, 15.f)));
	
	m_pTransformCom->SetSpeed(1.1f);
	
	m_strObjectTag = "Buddy_Lumi";

	{
		m_pFSM = CFSMComponentBuilder()
			.InitState("Idle")			
			.AddState("Idle")		
				.OnStart([this]
				{
					m_bInitialize = false;
				})
				.Tick([this](_double TimeDelta)
				{
					m_fTimeAcc += _float(TimeDelta * 1);

					m_vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					m_vStorePos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					_vector vDir = m_vStorePos - m_vMyPos;

					/*if (m_fMyPos.x >= m_fStorePos.x && m_fMyPos.z >= m_fStorePos.z && m_fMyPos.x <= (m_fStorePos.x + 15.f) && m_fMyPos.z <= (m_fStorePos.z + 15.f) ||
						m_fMyPos.x <= m_fStorePos.x && m_fMyPos.z >= m_fStorePos.z && m_fMyPos.x >= (m_fStorePos.x - 15.f) && m_fMyPos.z <= (m_fStorePos.z + 15.f) ||
						m_fMyPos.x <= m_fStorePos.x && m_fMyPos.z <= m_fStorePos.z && m_fMyPos.x >= (m_fStorePos.x - 15.f) && m_fMyPos.z >= (m_fStorePos.z - 15.f) ||
						m_fMyPos.x >= m_fStorePos.x && m_fMyPos.z <= m_fStorePos.z && m_fMyPos.x <= (m_fStorePos.x + 15.f) && m_fMyPos.z >= (m_fStorePos.z - 15.f))*/
					if(XMVectorGetX(XMVector3Length(vDir)) > 15.f)
					{
						if (m_fTimeAcc >= 5.f && !m_bInitialize)	// 처음 생성되고 1회
						{
							m_bIdle = false;
							m_bRun = true;
							m_fTimeAcc = 0.f;
							m_bInitialize = true;
						}
					}
					else
					{
						m_bIdle = true;
						m_bRun = false;
					}					
				})

				.AddTransition("Idle to Run", "Run")
					.Predicator([this]
					{
						return !m_bIdle && m_bRun;
					})

				/*.AddTransition("Idle to Attack", "Attack")
					.Predicator([this]()->_bool {return m_bAttack; })

				.AddTransition("Idle to DodgeB", "DodgeB")
					.Predicator([this]()->_bool {return m_bDodgeB; })*/

			.AddState("Run")
				.Tick([this](_double TimeDelta)
				{
					m_fMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					m_fStorePos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					_vector vTargetPos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					_float3 fTargetPos;
					XMStoreFloat3(&fTargetPos, vTargetPos);
					_vector vDir = m_vStorePos - m_vMyPos;

					m_pTransformCom->LookAt(vTargetPos);
					m_pTransformCom->Chase(vTargetPos, 0.06f);

					/*_vector vDest = { 0.f, 0.f, 0.f, 1.f };

					m_pTransformCom->LookAt(vDest);
					m_pTransformCom->Chase(vDest, 0.03f);*/

					/*vector<_uint> vecRandomPattern;

					_uint iAttack = 1;
					vecRandomPattern.push_back(iAttack);

					_uint iDodgeB = 2;
					vecRandomPattern.push_back(iDodgeB);

					_uint iDodgeL = 3;
					vecRandomPattern.push_back(iDodgeL);

					_uint iDodgeR = 4;
					vecRandomPattern.push_back(iDodgeR);

					random_shuffle(vecRandomPattern.begin(), vecRandomPattern.end());*/
					// TODO : 4방면에 대응하는 조건 찾기

					//if (m_fMyPos.x <= 1.f && m_fMyPos.z <= 1.f)

					if (XMVectorGetX(XMVector3Length(vDir)) < 2.f)
					{
						m_bRun = false;

						m_fStorePos = m_fMyPos;

						if (m_iAfterRunPt == 0)
							m_bAttack = true;

						if (m_iAfterRunPt == 1)
							m_bDodgeB = true;

						if (m_iAfterRunPt == 2)
							m_bDodgeL = true;

						if (m_iAfterRunPt == 3)
							m_bAttack = true;

						if (m_iAfterRunPt == 4)
							m_bDodgeR = true;

						++m_iAfterRunPt;

						if (m_iAfterRunPt > 4)
							m_iAfterRunPt = 0;

						/*_uint iShuffleResult = vecRandomPattern.front();

						if (iShuffleResult == 1)
						m_bAttack = true;

						if (iShuffleResult == 2)
						m_bDodgeB = true;

						if (iShuffleResult == 3)
						m_bDodgeL = true;

						if (iShuffleResult == 4)
						m_bDodgeR = true;*/
					}										
				})

				.AddTransition("Run to Attack", "Attack")
					.Predicator([this]
					{
						return !m_bRun && m_bAttack;
					})

				.AddTransition("Run to DodgeB", "DodgeB")
					.Predicator([this]
					{
						return !m_bRun && m_bDodgeB;
					})

				.AddTransition("Run to DodgeL", "DodgeL")
					.Predicator([this]
					{
						return !m_bRun && m_bDodgeL;
					})

				.AddTransition("Run to DodgeR", "DodgeR")
					.Predicator([this]
					{
						return !m_bRun && m_bDodgeR;
					})

				.AddTransition("Run to Dead", "Dead")
					.Predicator([this]()->_bool {return m_bDead; })

			.AddState("Attack")
				.Tick([this](_double TimeDelta)
				{
					auto pAnim = m_pModelCom->GetPlayAnimation();

					if (pAnim == m_pModelCom->Find_Animation("AS_em0400_200_AL_atk_a1") && pAnim->IsFinished() == true)
					{
						m_bAttack = false;

						if (m_iAfterAtkPt == 0)
						{
							m_bThreat = true;							
						}
						
						if (m_iAfterAtkPt == 1)
						{
							m_bWalk = true;							
						}						

						++m_iAfterAtkPt;

						if (m_iAfterAtkPt > 1)
							m_iAfterAtkPt = 0;
					}
				})

				.AddTransition("Attack to Walk", "Walk")
					.Predicator([this]()->_bool { return !m_bAttack && m_bWalk; })

				.AddTransition("Attack to Threat", "Threat")
					.Predicator([this]()->_bool {return !m_bAttack && m_bThreat; })

				.AddTransition("Attack to Dead", "Dead")
					.Predicator([this]()->_bool {return m_bDead; })

			.AddState("Threat")
				.Tick([this](_double TimeDelta)
				{
					_vector vTargetPos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);

					m_pTransformCom->LookAt(vTargetPos);

					auto pAnim = m_pModelCom->GetPlayAnimation();

					if (pAnim == m_pModelCom->Find_Animation("AS_em0400_160_AL_threat") && pAnim->IsFinished() == true)
					{
						m_bThreat = false;
						m_bWalk = true;
//						m_bDead = true;
					}
				})

				.AddTransition("Threat to Walk", "Walk")
					.Predicator([this]()->_bool {return !m_bThreat && m_bWalk; })

				.AddTransition("Threat to Dead", "Dead")
					.Predicator([this]()->_bool {return m_bDead; })

			.AddState("Walk")
				.Tick([this](_double TimeDelta)
				{
					m_fTimeAcc += _float(TimeDelta * 1);

					//m_fMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

					//_vector vDest;
					//// 추후엔 플레이어가 회피해서 멀어진 경우 다시 그곳을 추적해서 Run하거나 걷는 식으로 바꿀 것.
					//if(m_iWalkPosition == 0)
					//	vDest = { (m_fStorePos.x + 3.f), 0.f, (m_fStorePos.z + 3.f), 1.f };

					//if (m_iWalkPosition == 1)
					//	vDest = { (m_fStorePos.x - 3.f), 0.f, (m_fStorePos.z + 3.f), 1.f };

					//if (m_iWalkPosition == 2)
					//	vDest = { (m_fStorePos.x + 3.f), 0.f, (m_fStorePos.z - 3.f), 1.f };

					//if (m_iWalkPosition == 3)
					//	vDest = { (m_fStorePos.x - 3.f), 0.f, (m_fStorePos.z - 3.f), 1.f };

					//_float3 fDest;
					//XMStoreFloat3(&fDest, vDest);

					//m_pTransformCom->LookAt(vDest);
					//m_pTransformCom->Chase(vDest, 0.018f);
					//// TODO : 4방면에 대응하는 조건 찾기
					//					
					//if (m_iWalkPosition == 0 && m_fMyPos.x >= (fDest.x - 0.5f) && m_fMyPos.z >= (fDest.z - 0.5f) ||
					//	m_iWalkPosition == 1 && m_fMyPos.x >= (fDest.x + 0.5f) && m_fMyPos.z <= (fDest.z - 0.5f) ||
					//	m_iWalkPosition == 2 && m_fMyPos.x <= (fDest.x - 0.5f) && m_fMyPos.z >= (fDest.z + 0.5f) ||
					//	m_iWalkPosition == 3 && m_fMyPos.x <= (fDest.x - 0.5f) && m_fMyPos.z <= (fDest.z - 0.5f))
					if(m_fTimeAcc >= 1.5f)
					{
						m_bWalk = false;
						
						m_bRun = true;		
						m_fTimeAcc = 0.f;
//						++m_iWalkPosition;	// 4가 되면 아래 조건에 걸려서 초기화

//						if (m_iWalkPosition > 3)
//							m_iWalkPosition = 0;
					}
					
				})

				.AddTransition("Walk to Run", "Run")
					.Predicator([this]()->_bool {return !m_bWalk && m_bRun; })

				.AddTransition("Walk to Dead", "Dead")
					.Predicator([this]()->_bool {return m_bDead; })

			.AddState("DodgeB")
					.OnStart([this] 
					{
						m_vStorePos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
						m_vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					})
				.Tick([this](_double TimeDelta) 
				{
					auto pAnim = m_pModelCom->GetPlayAnimation();

					// 뒤로 이동 
					_vector vDest = XMVectorSetW(m_vMyPos - m_vStorePos, 0.f);
					vDest = XMVector3Normalize(XMVectorSetY(vDest, 0.f));
					
					m_pTransformCom->Move(0.02, vDest);

					if (pAnim->IsFinished() == true)
					{
						m_bDodgeB = false;
						m_bWalk = true;
					}
				})
				.AddTransition("DodgeB to Walk", "Walk")
					.Predicator([this]()->_bool {return !m_bDodgeB && m_bWalk; })

				.AddTransition("DodgeB to Dead", "Dead")
					.Predicator([this]()->_bool {return m_bDead; })

			.AddState("DodgeL")
				.OnStart([this] 
				{
					m_vStorePos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				})

				.Tick([this](_double TimeDelta)
				{
					auto pAnim = m_pModelCom->GetPlayAnimation();
					
					_vector vMyRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
					// 중점 Axis 기준 왼쪽으로 Turn
					m_pTransformCom->LookAt(m_vStorePos);
					m_pTransformCom->Move(0.02, -vMyRight);

					if (pAnim->IsFinished() == true)
					{
						m_bDodgeL = false;
						m_bWalk = true;
					}
				})
				.AddTransition("DodgeL to Walk", "Walk")
					.Predicator([this]()->_bool {return !m_bDodgeL && m_bWalk; })

				.AddTransition("DodgeL to Dead", "Dead")
					.Predicator([this]()->_bool {return m_bDead; })

			.AddState("DodgeR")
				.OnStart([this]
				{
					m_vStorePos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
				})
				.Tick([this](_double TimeDelta)
				{
					auto pAnim = m_pModelCom->GetPlayAnimation();
					
					_vector vMyRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
					// 중점 Axis 기준 오른으로 Turn
					m_pTransformCom->LookAt(m_vStorePos);
					m_pTransformCom->Move(0.02, vMyRight);

					if (pAnim->IsFinished() == true)
					{
						m_bDodgeR = false;
						m_bWalk = true;
					}
				})
				.AddTransition("DodgeR to Walk", "Walk")
					.Predicator([this]()->_bool {return !m_bDodgeR && m_bWalk; })

				.AddTransition("DodgeR to Dead", "Dead")
					.Predicator([this]()->_bool {return m_bDead; })
			
			.AddState("Dead")			
				.OnStart([this] 
				{
					m_pASM->InputAnimSocket("BuddyLumi_DeadAnim", m_DeadAnimSocket);
				})
				.Tick([this](_double TimeDelta) 
				{

				})

			.Build();
	}

	//m_pWeaponCollider->SetOnTriggerIn([this](CGameObject* pObj)
	//{
	//	// 이벤트 콜러를 이용해서 공격 모션일때만 조건 들어오게 하기 
	//	if (m_bAtkCall)
	//	{
	//		CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//		CPlayer* pPlayer = dynamic_cast<CPlayer*>(pObj);

	//		DAMAGE_PARAM damageParam;

	//		_float3 fWeaponHitPosition = { m_pWeaponCollider->GetPxWorldMatrix().m[3][0], m_pWeaponCollider->GetPxWorldMatrix().m[3][1], m_pWeaponCollider->GetPxWorldMatrix().m[3][2] };

	//		damageParam.eAttackSAS = ESASType::SAS_END;
	//		damageParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	//		damageParam.vHitPosition = fWeaponHitPosition;
	//		damageParam.pCauser = this;
	//		damageParam.iDamage = 1;

	//		pPlayer->TakeDamage(damageParam);
	//	}
	//});

	//m_pWeaponCollider->SetOnTriggerOut([this](CGameObject* pObj)
	//{


	//});


	return S_OK;
}

void CBuddyLumi::BeginTick()
{
	__super::BeginTick();
	
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, L"Layer_Player")->GetGameObjects())
	{
		if (iter->GetPrototypeTag() == TEXT("Player"))
		{
			m_pPlayer = iter;
		}
	}
}

void CBuddyLumi::Tick(_double TimeDelta)
{	
	CMonster::Tick(TimeDelta);

	Collision();

	m_pTrigger->Update_Tick(m_pTransformCom);

	m_pFSM->Tick(TimeDelta);
	m_pSocketFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);	

	/*m_pTransformCom->MoveVelocity(1.f, m_tmp);
	m_tmp = _float3::Zero;*/ // 예제 코드
}

void CBuddyLumi::Late_Tick(_double TimeDelta)
{	
	CMonster::Late_Tick(TimeDelta);
		
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CBuddyLumi::Render()
{
	if (FAILED(CMonster::Render()))
		return E_FAIL;

	m_pModelCom->Render(m_pTransformCom);

	return S_OK;
}

void CBuddyLumi::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	/*
	if (ImGui::CollapsingHeader("BuddyLumiAnimSocket", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("GroundDmg"))
		{
			m_bDamage = true;
			m_pASM->InputAnimSocket("BuddyLumi_GroundDmgAnim", m_GroundDmgSocket);
		}

		if (ImGui::Button("AirDmg"))
		{
			m_bAir = true;
			m_pASM->InputAnimSocket("BuddyLumi_AirDmgAnim", m_AirDmgSocket);
		}

		if (ImGui::Button("Dead"))
		{
			m_bDead = true;
			m_pASM->InputAnimSocket("BuddyLumi_DeadAnim", m_DeadAnimSocket);
		}
	}
	*/

	m_pFSM->Imgui_RenderProperty();
	m_pSocketFSM->Imgui_RenderProperty();
}

void CBuddyLumi::TakeDamage(DAMAGE_PARAM tDamageParams)
{
	/*
	// 예제 코드
	_vector tmp = _float4{ tDamageParams.vHitFrom.x, tDamageParams.vHitFrom.y , tDamageParams.vHitFrom.z, 1.f };
	_float4 vBackDir = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) - tmp;
	vBackDir.Normalize();

	m_tmp = _float3{ vBackDir.x, vBackDir.y ,vBackDir.z } *2.f;

	*/

	EBaseAxis eHitFrom = CClientUtils::GetDamageFromAxis(m_pTransformCom, tDamageParams.vHitFrom);
	// ↑ 공격이 들어올 방향 
	m_eAtkType = tDamageParams.eAttackType;

	m_eHitDir = eHitFrom;

	m_bStruck = true;
}

void CBuddyLumi::AfterPhysX()
{
	__super::AfterPhysX();
	// 무기 콜라이더의 Update_Tick(매트릭스) 
	m_pWeaponCollider->Update_Tick(AttachCollider());

	m_pWeaponCollider->Update_AfterPhysX(m_pTransformCom);

	m_pTrigger->Update_AfterPhysX(m_pTransformCom);
}

void CBuddyLumi::Collision()
{
	DAMAGE_PARAM damageParam;

	_float3 fWeaponHitPosition = { m_pWeaponCollider->GetPxWorldMatrix().m[3][0], m_pWeaponCollider->GetPxWorldMatrix().m[3][1], m_pWeaponCollider->GetPxWorldMatrix().m[3][2] };

	damageParam.eAttackSAS = ESASType::SAS_END;
	damageParam.vHitFrom = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	damageParam.vHitPosition = fWeaponHitPosition;
	damageParam.pCauser = this;
	damageParam.iDamage = 1;

	Collision_Check_Capsule(m_pWeaponCollider, damageParam);
}

_matrix CBuddyLumi::AttachCollider()
{
	_matrix	SocketMatrix = m_pModelCom->GetBoneMatrix("RightWeapon") * m_pTransformCom->Get_WorldMatrix();

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
	
	return SocketMatrix;
}

HRESULT CBuddyLumi::Setup_AnimSocket()
{
	CAnimation*	pAnimation = nullptr;
			
	NULL_CHECK(pAnimation = m_pModelCom->Find_Animation("AS_em0400_411_AL_damage_m_F"));		
	m_GroundDmgSocket.push_back(pAnimation = m_pModelCom->Find_Animation("AS_em0400_411_AL_damage_m_F"));
	
	NULL_CHECK(pAnimation = m_pModelCom->Find_Animation("AS_em0400_406_AL_damage_air2"));
	m_AirDmgSocket.push_back(pAnimation = m_pModelCom->Find_Animation("AS_em0400_406_AL_damage_air2"));
		
	NULL_CHECK(pAnimation = m_pModelCom->Find_Animation("AS_em0400_424_AL_dead_down"));
	m_DeadAnimSocket.push_back(pAnimation = m_pModelCom->Find_Animation("AS_em0400_424_AL_dead_down"));

	return S_OK;
}

HRESULT CBuddyLumi::Setup_WeakAnimState()
{	
	CAnimation*	pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModelCom->Find_Animation("AS_em0400_401_AL_damage_l_F"));
	m_HitLightFoward.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModelCom->Find_Animation("AS_em0400_402_AL_damage_l_B"));
	m_HitLightBack.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModelCom->Find_Animation("AS_em0400_411_AL_damage_m_F"));
	m_HitMiddleFoward.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModelCom->Find_Animation("AS_em0400_412_AL_damage_m_B"));
	m_HitMiddleBack.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModelCom->Find_Animation("AS_em0400_413_AL_damage_m_L"));
	m_HitMiddleLeft.push_back(pAnimation);

	NULL_CHECK(pAnimation = m_pModelCom->Find_Animation("AS_em0400_414_AL_damage_m_R"));
	m_HitMiddleRight.push_back(pAnimation);

	// m_bDamage : 피격 소켓이 돌기 위한 조건으로, 기본적으로 m_pFSM(소켓 아님)
	// 이 돌때는 항시 false를 던져주다가 m_bStruck이 true일 때 조건에 따라 
	// 지상 피격(m_bAir가 false), 공중 피격(m_bAir가 true)에 따라 어떤 소켓일지가
	// 갈리게 된다.
	{
		m_pSocketFSM = CFSMComponentBuilder()
			.InitState("No_Hit")
			.AddState("No_Hit")
				.Tick([this](_double TimeDelta) { m_bDamage = false; })

				.AddTransition("No_Hit to Ground_Hit", "Ground_Hit")
					.Predicator([this] {return m_bStruck && !m_bAir; })
					.Priority(0)

				.AddTransition("No_Hit to Air_Hit", "Air_Hit")
					.Predicator([this] {return m_bStruck && m_bAir; })
					.Priority(0)

#pragma region Ground_Hit

			.AddState("Ground_Hit")
				.OnStart([this]
				{				
					if (m_eAtkType == EAttackType::ATK_LIGHT)	// 기본 공격(평타)
					{
						if (m_eHitDir == EBaseAxis::NORTH)	// NORTH : 전방
						{
							m_pASM->InputAnimSocket("BuddyLumi_GroundDmgAnim", m_HitLightFoward);
							m_Haxistype = HAS_FL;
						}
						else if (m_eHitDir == EBaseAxis::SOUTH)	// SOUTH : 후방
						{
							m_pASM->InputAnimSocket("BuddyLumi_GroundDmgAnim", m_HitLightBack);
							m_Haxistype = HAS_BL;
						}
					}

					else if (m_eAtkType == EAttackType::ATK_MIDDLE)
					{
						if (m_eHitDir == EBaseAxis::NORTH) // NORTH : 전방
						{
							m_pASM->InputAnimSocket("BuddyLumi_GroundDmgAnim", m_HitMiddleFoward);
							m_Haxistype = HAS_FM;
						}
						else if (m_eHitDir == EBaseAxis::SOUTH)	// SOUTH : 후방
						{
							m_pASM->InputAnimSocket("BuddyLumi_GroundDmgAnim", m_HitMiddleBack);
							m_Haxistype = HAS_BM;
						}
						else if (m_eHitDir == EBaseAxis::WEST)	// WEST : 좌측
						{
							m_pASM->InputAnimSocket("BuddyLumi_GroundDmgAnim", m_HitMiddleLeft);
							m_Haxistype = HAS_LM;
						}
						else if (m_eHitDir == EBaseAxis::EAST)	// EAST : 우측
						{
							m_pASM->InputAnimSocket("BuddyLumi_GroundDmgAnim", m_HitMiddleRight);
							m_Haxistype = HAS_RM;
						}
					}
				})

				.Tick([this](_double TimeDelta)
				{
					m_bDamage = true;

					if (m_pASM->isSocketPassby("BuddyLumi_GroundDmgAnim") > 0.92)
					{
						m_bStruck = false;
					}
				})

				.AddTransition("Ground_Hit to Ground_Hit", "Ground_Hit")
					.Predicator([this] {return m_bStruck && !m_bAir && m_pASM->isSocketPassby("BuddyLumi_GroundDmgAnim") <= 0.92; })
					.Priority(0)

				.AddTransition("Ground_Hit to No_Hit", "No_Hit")
					.Predicator([this] {return !m_bStruck && !m_bAir && m_pASM->isSocketAlmostFinish("BuddyLumi_GroundDmgAnim"); })
					.Priority(0)

				.AddTransition("Ground_Hit to Air_Hit", "Air_Hit")
					.Predicator([this] {return m_bStruck && m_bAir; })
					.Priority(0)

#pragma endregion Ground_Hit
			
#pragma region Air_Hit

#pragma endregion Air_Hit


			.Build();
	}



	return S_OK;
}

HRESULT CBuddyLumi::SetUp_Components(void* pArg)
{	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/*if (FAILED(__super::Add_Component(LEVEL_NOW, TEXT("MonsterBuddyLumi"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;*/

	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		if (json.contains("Model"))
		{
			string ProtoModel = json["Model"];
			m_ModelName = CGameUtils::s2ws(ProtoModel);
			FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_ModelName.c_str(), m_ModelName.c_str(),
				(CComponent**)&m_pModelCom));
		}
	}
	
	m_pASM = CBdLm_AnimInstance::Create(m_pModelCom, this);
	if (nullptr == m_pASM)
	{
		MSG_BOX("BuddyLumi's ASM Failed");
		return E_FAIL;
	}

	FAILED_CHECK(Setup_WeakAnimState());

	return S_OK;
}

HRESULT CBuddyLumi::SetUp_Event()
{
	m_pModelCom->Add_EventCaller("Swing_Start", [&] {Event_AtkCall(true); });
	m_pModelCom->Add_EventCaller("Swing_End", [&] {Event_AtkCall(false); });

	return S_OK;
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
	__super::Free();
	
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pFSM);
	Safe_Release(m_pSocketFSM);
	Safe_Release(m_pASM);
	Safe_Release(m_pTrigger);
	Safe_Release(m_pWeaponCollider);
}
