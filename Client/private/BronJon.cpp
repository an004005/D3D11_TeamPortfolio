#include "stdafx.h"
#include "BronJon.h"
#include "GameInstance.h"
#include "MathUtils.h"
#include "GameUtils.h"
#include "FSMComponent.h"
#include "AnimationInstance.h"
#include "Animation.h"
#include "Model.h"
#include "JsonStorage.h"
#include "Bone.h"

#include "BrJ_AnimInstance.h"
#include "FlowerLeg.h"
#include "Player.h"
#include "RigidBody.h"
#include "PhysX_Manager.h"

CBronJon::CBronJon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CBronJon::CBronJon(const CBronJon & rhs)
	: CMonster(rhs)
{
}

HRESULT CBronJon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBronJon::Initialize(void * pArg)
{
	Json BronJonTrigger = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BronJonTrigger.json");
	Json BronJonJaw = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/BronJonJaw.json");

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	if (FAILED(Setup_AnimSocket()))
		return E_FAIL;


	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Jaw"),
		(CComponent**)&m_pJawRBody, &BronJonJaw)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Trigger"),
		(CComponent**)&m_pTrigger, &BronJonTrigger)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat3(&_float3(0.f, 0.f, 30.f)));

	m_pTransformCom->SetSpeed(1.f);

	m_strObjectTag = "Bron_Jon";

	{
		m_pFSM = CFSMComponentBuilder()
			.InitState("Idle")
			.AddState("Idle")
				.OnStart([this]
				{
					m_vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					XMStoreFloat3(&m_fMyPos, m_vMyPos);
					m_fTimeAcc = 0.f;
				})
				.Tick([this](_double TimeDelta)
				{
					m_vStorePos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					XMStoreFloat3(&m_fStorePos, m_vStorePos);

					m_fTimeAcc += _float(TimeDelta * 1);

					if (m_fMyPos.x >= m_fStorePos.x && m_fMyPos.z >= m_fStorePos.z && m_fMyPos.x <= (m_fStorePos.x + 20.f) && m_fMyPos.z <= (m_fStorePos.z + 20.f) ||
						m_fMyPos.x <= m_fStorePos.x && m_fMyPos.z >= m_fStorePos.z && m_fMyPos.x >= (m_fStorePos.x - 20.f) && m_fMyPos.z <= (m_fStorePos.z + 20.f) ||
						m_fMyPos.x <= m_fStorePos.x && m_fMyPos.z <= m_fStorePos.z && m_fMyPos.x >= (m_fStorePos.x - 20.f) && m_fMyPos.z >= (m_fStorePos.z - 20.f) ||
						m_fMyPos.x >= m_fStorePos.x && m_fMyPos.z <= m_fStorePos.z && m_fMyPos.x <= (m_fStorePos.x + 20.f) && m_fMyPos.z >= (m_fStorePos.z - 20.f))
					{
						if (m_fTimeAcc >= 3.f)
						{
							m_bIdle = false;

							if (m_fMyPos.x >= m_fStorePos.x && m_fMyPos.z >= m_fStorePos.z && m_fMyPos.x <= (m_fStorePos.x + 6.f) && m_fMyPos.z <= (m_fStorePos.z + 6.f) ||
								m_fMyPos.x <= m_fStorePos.x && m_fMyPos.z >= m_fStorePos.z && m_fMyPos.x >= (m_fStorePos.x - 6.f) && m_fMyPos.z <= (m_fStorePos.z + 6.f) ||
								m_fMyPos.x <= m_fStorePos.x && m_fMyPos.z <= m_fStorePos.z && m_fMyPos.x >= (m_fStorePos.x - 6.f) && m_fMyPos.z >= (m_fStorePos.z - 6.f) ||
								m_fMyPos.x >= m_fStorePos.x && m_fMyPos.z <= m_fStorePos.z && m_fMyPos.x <= (m_fStorePos.x + 6.f) && m_fMyPos.z >= (m_fStorePos.z - 6.f))
							{
								m_bNear = true;
								m_bFar = false;

								if (m_iNearPattern == 0)
									m_bBiteAtk = true;

								if (m_iNearPattern == 1)
									m_bMoveR_Start = true;

								if (m_iNearPattern == 2)
									m_bMoveB = true;

								if (m_iNearPattern == 3)
									m_bLaserAtkStart = true;

								if (m_iNearPattern == 4)
									m_bMoveL_Start = true;

								if (m_iNearPattern == 5)
									m_bBiteAtk = true;

								if (m_iNearPattern == 6)
									m_bLaserAtkStart = true;

								++m_iNearPattern;

								if (m_iNearPattern > 6)
									m_iNearPattern = 0;
							}

							else
							{
								m_bNear = false;
								m_bFar = true;

								if (m_iFarPattern == 0)
									m_bMoveF = true;

								if (m_iFarPattern == 1)
									m_bLaserAtkStart = true;

								++m_iFarPattern;

								if (m_iFarPattern > 1)
									m_iFarPattern = 0;
							}
						}
					}
					else
						m_bIdle = true;
				})

				.AddTransition("Idle to BiteAtk", "BiteAtk")
					.Predicator([this]
					{
						return m_bNear && !m_bIdle && m_bBiteAtk;
					})

				.AddTransition("Idle to LaserAtkStart", "LaserAtkStart")
					.Predicator([this]
					{
						return !m_bIdle && m_bLaserAtkStart;
					})

				.AddTransition("Idle to Threat", "Threat")
					.Predicator([this]
					{
						return m_bNear && !m_bIdle && m_bThreat;
					})
				
				.AddTransition("Idle to MoveF", "MoveF")
					.Predicator([this]
					{
						return m_bFar && !m_bIdle && m_bMoveF;
					})

				.AddTransition("Idle to MoveB", "MoveB")
					.Predicator([this]
					{
						return m_bNear && !m_bIdle && m_bMoveB;
					})

				.AddTransition("Idle to MoveL_Start", "MoveL_Start")
					.Predicator([this]
					{
						return m_bNear && !m_bIdle && m_bMoveL_Start;
					})

				.AddTransition("Idle to MoveR_Start", "MoveR_Start")
					.Predicator([this]
					{
						return m_bNear && !m_bIdle && m_bMoveR_Start;
					})

			.AddState("BiteAtk")
				.OnStart([this]
				{
					m_bCreateBite = false;
				})
				.Tick([this](_double TimeDelta)
				{
					CGameInstance* pGameInstance = CGameInstance::GetInstance();
							
					auto pAnim = m_pModelCom->GetPlayAnimation();

					if (pAnim->GetPlayRatio() > 0.3 && !m_bCreateBite)
					{
						m_vStorePos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
						m_pTransformCom->LookAt(m_vStorePos);

						_matrix matJaw = AttachCollider();
						_vector vJawPos = matJaw.r[3];
						_float4 fJawPos;
						XMStoreFloat4(&fJawPos, vJawPos);

						fJawPos.z += 2.f;

						physx::PxSweepHit hitBuff[5];
						physx::PxSweepBuffer overlapOut(hitBuff, 5);

						SphereSweepParams params;
						params.fDistance = 1.f;
						params.fRadius = 1.5f;
						params.iTargetType = CTB_PLAYER;
						params.sweepOut = &overlapOut;
						params.vPos = fJawPos;//vJawPos;
						params.vUnitDir = _float3{ 0.f, 0.f, 1.f };
						params.fVisibleTime = 0.1f; // 보여지는 시간

						if (pGameInstance->SweepSphere(params))	// 조건
						{
							for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
							{
								if (overlapOut.getAnyHit(i).actor)
									int iA = 0;//MSG_BOX("Touch");
								CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(overlapOut.getAnyHit(i).actor);
							}
						}
						m_bCreateBite = true;
					}				

					if (pAnim->GetPlayRatio() > 0.98)
					{
						m_bIdle = true;
						m_bBiteAtk = false;
					}
				})

				.AddTransition("BiteAtk to Idle", "Idle")
					.Predicator([this]
					{
						return !m_bBiteAtk && m_bIdle;
					})

			.AddState("Threat")
				.Tick([this](_double TimeDelta)
				{
					m_vStorePos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					m_pTransformCom->LookAt(m_vStorePos);

					auto pAnim = m_pModelCom->GetPlayAnimation();

					if (pAnim->GetPlayRatio() > 0.95)
					{
						m_bThreat = false;
						m_bIdle = true;
					}
				})
				.AddTransition("Threat to Idle", "Idle")
					.Predicator([this]
					{
						return !m_bThreat && m_bIdle;
					})

			.AddState("MoveF")
				.OnStart([this]
				{
					m_fTimeAcc = 0.f;
				})
				.Tick([this](_double TimeDelta)
				{
					m_vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					XMStoreFloat3(&m_fMyPos, m_vMyPos);

					m_vStorePos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					XMStoreFloat3(&m_fStorePos, m_vStorePos);

					m_pTransformCom->LookAt(m_vStorePos);
					m_pTransformCom->Chase(m_vStorePos, 0.05);

					m_fTimeAcc += _float(TimeDelta * 1);					

					if (m_fTimeAcc >= 2)
					{
						m_bMoveF = false;

						if (m_fMyPos.x >= m_fStorePos.x && m_fMyPos.z >= m_fStorePos.z && m_fMyPos.x <= (m_fStorePos.x + 6.f) && m_fMyPos.z <= (m_fStorePos.z + 6.f) ||
							m_fMyPos.x <= m_fStorePos.x && m_fMyPos.z >= m_fStorePos.z && m_fMyPos.x >= (m_fStorePos.x - 6.f) && m_fMyPos.z <= (m_fStorePos.z + 6.f) ||
							m_fMyPos.x <= m_fStorePos.x && m_fMyPos.z <= m_fStorePos.z && m_fMyPos.x >= (m_fStorePos.x - 6.f) && m_fMyPos.z >= (m_fStorePos.z - 6.f) ||
							m_fMyPos.x >= m_fStorePos.x && m_fMyPos.z <= m_fStorePos.z && m_fMyPos.x <= (m_fStorePos.x + 6.f) && m_fMyPos.z >= (m_fStorePos.z - 6.f))
						{							
							m_bIdle = true;
						}

						else						
							m_bLaserAtkStart = true;						
					}					
				})

				.AddTransition("MoveF to Idle", "Idle")
					.Predicator([this]
					{
						return !m_bMoveF && m_bIdle;
					})

				.AddTransition("MoveF to LaserAtkStart", "LaserAtkStart")
					.Predicator([this] 
					{ 
						return !m_bMoveF && m_bLaserAtkStart; 
					})

			.AddState("MoveB")
				.OnStart([this]
				{
					m_vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					m_vStorePos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					m_vDest = m_vMyPos - m_vStorePos;

					m_fTimeAcc = 0.f;
				})
				.Tick([this](_double TimeDelta)
				{
					m_pTransformCom->Move(0.06, m_vDest);

					m_fTimeAcc += _float(TimeDelta * 1);

					if (m_fTimeAcc >= 2.f)
					{
						m_bMoveB = false;

						vector<_uint> vecRandomPattern;

						_uint iLaserAttack = 1;
						vecRandomPattern.push_back(iLaserAttack);

						_uint iIdle = 2;
						vecRandomPattern.push_back(iIdle);

						_uint iLeftMove = 3;
						vecRandomPattern.push_back(iLeftMove);

						_uint iRightMove = 4;
						vecRandomPattern.push_back(iRightMove);

						random_shuffle(vecRandomPattern.begin(), vecRandomPattern.end());

						_uint iShuffleResult = vecRandomPattern.front();

						if (iShuffleResult == 1)
							m_bLaserAtkStart = true;

						if (iShuffleResult == 2)
							m_bIdle = true;

						if (iShuffleResult == 3)
							m_bMoveL_Start = true;

						if (iShuffleResult == 4)
							m_bMoveR_Start = true;
					}
				})

				.AddTransition("MoveB to LaserAtkStart", "LaserAtkStart")
					.Predicator([this]
					{
						return !m_bMoveB && m_bLaserAtkStart;
					})

				.AddTransition("MoveB to Idle", "Idle")
					.Predicator([this]
					{
						return !m_bMoveB && m_bIdle;
					})

				.AddTransition("MoveB to MoveL_Start", "MoveL_Start")
					.Predicator([this]
					{
						return !m_bMoveB && m_bMoveL_Start;
					})

				.AddTransition("MoveB to MoveR_Start", "MoveR_Start")
					.Predicator([this]
					{
						return !m_bMoveB && m_bMoveR_Start;
					})

			.AddState("MoveL_Start")
				.OnStart([this]
				{
					m_fTimeAcc = 0.f;
					m_vMyPos = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
				})
				.Tick([this](_double TimeDelta)
				{
					_vector vTargetPos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					m_pTransformCom->LookAt(vTargetPos);

					_vector vDest = (m_vMyPos * -1);
					m_pTransformCom->Move(0.06, vDest);

					auto pAnim = m_pModelCom->GetPlayAnimation();

					if (pAnim->GetPlayRatio() > 0.95)
					{
						m_bMoveL_Start = false;
						m_bMoveL_End = true;
					}
				})
				.AddTransition("MoveL_Start to MoveL_End", "MoveL_End")
					.Predicator([this]
					{
						return !m_bMoveL_Start && m_bMoveL_End;
					})

			.AddState("MoveL_End")
				.OnStart([this]
				{
					m_fTimeAcc = 0.f;
					m_vMyPos = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
				})
				.Tick([this](_double TimeDelta) 
				{
					_vector vTargetPos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					m_pTransformCom->LookAt(vTargetPos);

					_vector vDest = (m_vMyPos * -1);
					m_pTransformCom->Move(0.06, vDest);

					m_fTimeAcc += _float(TimeDelta * 1);

					if (m_fTimeAcc >= 2.4f)
					{
						m_bMoveL_End = false;
						m_bIdle = true;
					}
				})
				.AddTransition("MoveL_End to Idle", "Idle")
					.Predicator([this]
					{
						return !m_bMoveL_End && m_bIdle;
					})

			.AddState("MoveR_Start")
				.OnStart([this]
				{
					m_fTimeAcc = 0.f;
					m_vMyPos = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
				})
				.Tick([this](_double TimeDelta)
				{
					_vector vTargetPos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					m_pTransformCom->LookAt(vTargetPos);

					_vector vDest = (m_vMyPos * 1);
					m_pTransformCom->Move(0.06, vDest);

					auto pAnim = m_pModelCom->GetPlayAnimation();

					if (pAnim->GetPlayRatio() > 0.95)
					{
						m_bMoveR_Start = false;
						m_bMoveR_End = true;
					}
				})
				.AddTransition("MoveR_Start to MoveR_End", "MoveR_End")
					.Predicator([this]
				{
					return !m_bMoveR_Start && m_bMoveR_End;
				})

			.AddState("MoveR_End")
				.OnStart([this]
				{
					m_fTimeAcc = 0.f;
					m_vMyPos = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
				})
				.Tick([this](_double TimeDelta)
				{
					_vector vTargetPos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					m_pTransformCom->LookAt(vTargetPos);

					_vector vDest = (m_vMyPos * 1);
					m_pTransformCom->Move(0.06, vDest);

					m_fTimeAcc += _float(TimeDelta * 1);

					if (m_fTimeAcc >= 2.4f)
					{
						m_bMoveR_End = false;
						m_bIdle = true;
					}
				})
				.AddTransition("MoveR_End to Idle", "Idle")
					.Predicator([this]
					{
						return !m_bMoveR_End && m_bIdle;
					})

			.AddState("LaserAtkStart")
				.OnStart([this]
				{
					m_bCreateLaser = false;
				})
				.Tick([this](_double TimeDelta)
				{
					auto pAnim = m_pModelCom->GetPlayAnimation();

					if (pAnim->GetPlayRatio() <= 0.35)
					{
						m_vStorePos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					}

					m_pTransformCom->LookAt(m_vStorePos);

					if (pAnim->GetPlayRatio() > 0.97)
					{
						m_bLaserAtkStart = false;
						m_bLaserAtkIng = true;
					}
				})

				.AddTransition("LaserAtkStart to LaserAtkIng", "LaserAtkIng")
					.Predicator([this]
					{
						return !m_bLaserAtkStart && m_bLaserAtkIng;
					})

			.AddState("LaserAtkIng")
				.OnStart([this]
				{
					m_fTimeAcc = 0.f;

					m_vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

					_vector vDir = m_vStorePos - m_vMyPos;
					
					if (!m_bCreateLaser)
					{
						CGameInstance* pGameInstance = CGameInstance::GetInstance();

						_matrix matWeapon = WeaponRigidBodyMatrix();
						_vector vWeaponPos = matWeapon.r[3];
						_float4 fWeaponPos;
						XMStoreFloat4(&fWeaponPos, vWeaponPos);
						// 임시 코드						

						physx::PxSweepHit hitBuff[5];
						physx::PxSweepBuffer overlapOut(hitBuff, 5);

						SphereSweepParams params;
						params.fDistance = 50.f;
						params.fRadius = 2.f;
						params.iTargetType = CTB_PLAYER;
						params.sweepOut = &overlapOut;
						params.vPos = fWeaponPos;
						params.vUnitDir = vDir;//_float3{ 0.f, 0.f, 1.f };
						params.fVisibleTime = 0.1f; // 보여지는 시간
						if (pGameInstance->SweepSphere(params))	// 조건
						{
							for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
							{								
								if (overlapOut.getAnyHit(i).actor)
									int iA = 0;//MSG_BOX("Touch");
								CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(overlapOut.getAnyHit(i).actor);
							}
						}					
					}					
				})

				.Tick([this](_double TimeDelta)
				{
					m_fTimeAcc += _float(TimeDelta * 1);

					if (m_fTimeAcc >= 2.f)
					{
						m_bLaserAtkIng = false;
						m_bLaserAtkEnd = true;
					}

					else if(m_fTimeAcc >= 1.6f)
					{
						// Rigid Body Create
						m_bCreateLaser = true;
					}
				})
				
				.AddTransition("LaserAtkIng to LaserAtkEnd", "LaserAtkEnd")
					.Predicator([this]
					{
						return !m_bLaserAtkIng && m_bLaserAtkEnd;
					})

			.AddState("LaserAtkEnd")
				.Tick([this](_double TimeDelta)
				{
					auto pAnim = m_pModelCom->GetPlayAnimation();

					if (pAnim->GetPlayRatio() > 0.97)
					{
						m_bLaserAtkEnd = false;
						m_bIdle = true;
					}
				})
				.AddTransition("LaserAtkEnd to Idle", "Idle")
					.Predicator([this]
					{
						return !m_bLaserAtkEnd && m_bIdle;
					})				

			.Build();
	}

	return S_OK;
}

void CBronJon::BeginTick()
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

void CBronJon::Tick(_double TimeDelta)
{
	CMonster::Tick(TimeDelta);
	
	m_pTrigger->Update_Tick(m_pTransformCom);
	
	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);
}

void CBronJon::Late_Tick(_double TimeDelta)
{
	CMonster::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CBronJon::Render()
{
	if (FAILED(CMonster::Render()))
		return E_FAIL;

	m_pModelCom->Render(m_pTransformCom);

	return S_OK;
}

void CBronJon::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
	m_pFSM->Imgui_RenderProperty();
}

void CBronJon::AfterPhysX()
{
	__super::AfterPhysX();

	m_pJawRBody->Update_Tick(AttachCollider());

	m_pJawRBody->Update_AfterPhysX(m_pTransformCom);

	m_pTrigger->Update_AfterPhysX(m_pTransformCom);
}

_matrix CBronJon::AttachCollider()
{
	_matrix	SocketMatrix = m_pModelCom->GetBoneMatrix("Jaw") * m_pTransformCom->Get_WorldMatrix();

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	return SocketMatrix;
}

_matrix CBronJon::WeaponRigidBodyMatrix()
{
	_matrix	WeaponMatrix = m_pModelCom->GetBoneMatrix("Weapon") * m_pTransformCom->Get_WorldMatrix();

	WeaponMatrix.r[0] = XMVector3Normalize(WeaponMatrix.r[0]);
	WeaponMatrix.r[1] = XMVector3Normalize(WeaponMatrix.r[1]);
	WeaponMatrix.r[2] = XMVector3Normalize(WeaponMatrix.r[2]);

	return WeaponMatrix;
}

HRESULT CBronJon::Setup_AnimSocket()
{
	CAnimation*	pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModelCom->Find_Animation("AS_em0800_411_AL_damage_m_F"));
	m_GroundDmgSocket.push_back(pAnimation = m_pModelCom->Find_Animation("AS_em0800_411_AL_damage_m_F"));

	NULL_CHECK(pAnimation = m_pModelCom->Find_Animation("AS_em0800_424_AL_dead_down"));
	m_DeadAnimSocket.push_back(pAnimation = m_pModelCom->Find_Animation("AS_em0800_424_AL_dead_down"));

	return S_OK;
}

HRESULT CBronJon::SetUp_Components(void * pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

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

	m_pASM = CBrJ_AnimInstance::Create(m_pModelCom, this);
	if (nullptr == m_pASM)
	{
		MSG_BOX("BronJon's ASM Failed");
		return E_FAIL;
	}

	return S_OK;
}

CBronJon * CBronJon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBronJon* pInstance = new CBronJon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBronJon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBronJon::Clone(void * pArg)
{
	CBronJon*		pInstance = new CBronJon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBronJon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBronJon::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pFSM);
	Safe_Release(m_pASM);
	Safe_Release(m_pTrigger);
	Safe_Release(m_pJawRBody);
}
