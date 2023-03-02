#include "stdafx.h"
#include "SkummyPool.h"
#include "GameInstance.h"
#include "MathUtils.h"
#include "GameUtils.h"
#include "FSMComponent.h"
#include "AnimationInstance.h"
#include "Animation.h"
#include "Model.h"
#include "JsonStorage.h"

#include "SkmP_AnimInstance.h"
#include "FlowerLeg.h"
#include "Player.h"
#include "SkMpBullet.h"
#include "RigidBody.h"


CSkummyPool::CSkummyPool(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CSkummyPool::CSkummyPool(const CSkummyPool & rhs)
	: CMonster(rhs)
{
}

HRESULT CSkummyPool::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkummyPool::Initialize(void * pArg)
{
	Json SkummyPoolTrigger = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/Test.json");
	pArg = &SkummyPoolTrigger;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	if (FAILED(Setup_AnimSocket()))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Trigger"), 
		(CComponent**)&m_pTrigger, &SkummyPoolTrigger)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat3(&_float3(0.f, 0.f, 13.f)));

	m_pTransformCom->SetSpeed(1.f);

	m_strObjectTag = "Skummy_Pool";

	//m_StrDamage.

	{
		m_pFSM = CFSMComponentBuilder()
			.InitState("Idle")
			.AddState("Idle")
				.OnStart([this] 
				{ 
					m_bInitialize = false; 
					m_fTimeAcc = 0.f;
				})
				.Tick([this](_double TimeDelta)
				{
					// Player의 Position 계속 받아옴
					_float3 fTargetPos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					// 내 Position 갱신
					m_fMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

					m_fTimeAcc += _float(TimeDelta);

					if (m_fTimeAcc >= 3.f && !m_bInitialize)
					{
						m_bIdle = false;
						m_bInitialize = true;
					}

					if (m_bInitialize)
					{		// 기준점 밖일 때
						if (m_fMyPos.x >= (fTargetPos.x + 5.f) || m_fMyPos.z >= (fTargetPos.z + 5.f) ||
							m_fMyPos.x <= (fTargetPos.x - 5.f) || m_fMyPos.z <= (fTargetPos.z - 5.f))
						{
							vector<_uint> vecRandomPattern;

							_uint iAttack = 1;
							vecRandomPattern.push_back(iAttack);

							_uint iThreat = 2;
							vecRandomPattern.push_back(iThreat);

							random_shuffle(vecRandomPattern.begin(), vecRandomPattern.end());

							_uint iShuffleResult = vecRandomPattern.front();

							if (iShuffleResult == 1)
								m_bThreat = true;

							if (iShuffleResult == 2)
								m_bAttack = true;
						}

						else
						{							
							vector<_uint> vecRandomPattern;

							_uint iMoveB = 1;
							vecRandomPattern.push_back(iMoveB);

							_uint iMoveL = 2;
							vecRandomPattern.push_back(iMoveL);

							_uint iMoveR = 3;
							vecRandomPattern.push_back(iMoveR);

							_uint iAttack = 4;
							vecRandomPattern.push_back(iAttack);

							_uint iThreat = 5;
							vecRandomPattern.push_back(iThreat);

							/*_uint iMoveB2 = 6;
							vecRandomPattern.push_back(iMoveB2);

							_uint iMoveL2 = 7;
							vecRandomPattern.push_back(iMoveL2);

							_uint iMoveR2 = 8;
							vecRandomPattern.push_back(iMoveR2);*/
							
							random_shuffle(vecRandomPattern.begin(), vecRandomPattern.end());

							_uint iShuffleResult = vecRandomPattern.front();

							if (iShuffleResult == 1)
								m_bMoveB = true;

							if (iShuffleResult == 2)
								m_bMoveL = true;

							if (iShuffleResult == 3)
								m_bMoveR = true;

							if (iShuffleResult == 4)
								m_bAttack = true;

							if (iShuffleResult == 5)
								m_bThreat = true;

							/*if(iShuffleResult == 6)
								m_bMoveB = true;
							
							if(iShuffleResult == 7)
								m_bMoveL = true;

							if(iShuffleResult == 8)
								m_bMoveR = true;*/
						}
					}
				})

				.AddTransition("Idle to Attack", "Attack")
					.Predicator([this] 
					{
						return !m_bIdle && m_bAttack;
					})

				.AddTransition("Idle to Threat", "Threat")
					.Predicator([this]
					{
						return !m_bIdle && m_bThreat;
					})

				.AddTransition("Idle to MoveB", "MoveB")
					.Predicator([this]
					{
						return !m_bIdle && m_bMoveB;
					})

				.AddTransition("Idle to MoveL", "MoveL")
					.Predicator([this]
					{
						return !m_bIdle && m_bMoveL;
					})

				.AddTransition("Idle to MoveR", "MoveR")
					.Predicator([this]
					{
						return !m_bIdle && m_bMoveR;
					})

			.AddState("Attack")
				.OnStart([this]
				{
					m_bCreateBullet = false;					
				})
				.Tick([this](_double TimeDelta)
				{
					_vector vTargetPos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					m_pTransformCom->LookAt(vTargetPos);

					auto pAnim = m_pModelCom->GetPlayAnimation();

					if (pAnim->GetPlayRatio() > 0.66 && !m_bCreateBullet)
					{
						CGameInstance* pGameInstance = CGameInstance::GetInstance();
						// 투사체 생성						
						auto pObj = pGameInstance->Clone_GameObject_Get(TEXT("Layer_Bullet"), TEXT("SkMpBullet"));
						if (CSkMpBullet* pBullet = dynamic_cast<CSkMpBullet*>(pObj))
						{
							pBullet->Set_Owner(this);

							_matrix BoneMtx = m_pModelCom->GetBoneMatrix("Alga_F_03") * m_pTransformCom->Get_WorldMatrix();
							_float4x4 fBoneMtx;
							XMStoreFloat4x4(&fBoneMtx, BoneMtx);

							_vector vPrePos = { fBoneMtx.m[3][0], fBoneMtx.m[3][1], fBoneMtx.m[3][2], fBoneMtx.m[3][3] };
							
							_vector vDest = XMVectorSetW(vTargetPos - vPrePos, 0.f);
							vDest = XMVector3Normalize(XMVectorSetY(vDest, 0.f));

							pBullet->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, vPrePos);
							pBullet->Set_ShootDir(vDest);
							
							pBullet->GetTransform()->LookAt(vTargetPos);
							
						}

						m_bCreateBullet = true;
					}

					if (pAnim->GetPlayRatio() > 0.95)
					{
						m_bAttack = false;
						m_bIdle = true;
					}					
				})

				.AddTransition("Attack to Idle", "Idle")
					.Predicator([this]
					{
						return !m_bAttack && m_bIdle;
					})

			.AddState("Threat")
				.Tick([this](_double TimeDelta)
				{
					auto pAnim = m_pModelCom->GetPlayAnimation();

					if (pAnim->GetPlayRatio() > 0.95)
					{
						m_bThreat = false;

						vector<_uint> vecRandomPattern;

						_uint iIdle = 1;
						vecRandomPattern.push_back(iIdle);

						_uint iAttack = 2;
						vecRandomPattern.push_back(iAttack);						

						random_shuffle(vecRandomPattern.begin(), vecRandomPattern.end());

						_uint iShuffleResult = vecRandomPattern.front();

						if (iShuffleResult == 1)
							m_bIdle = true;

						if (iShuffleResult == 2)
							m_bAttack = true;						
					}
				})
				.AddTransition("Threat to Attack", "Attack")
					.Predicator([this]
					{
						return !m_bThreat && m_bAttack;
					})

				.AddTransition("Threat to Idle", "Idle")
					.Predicator([this]
					{
						return !m_bThreat && m_bIdle;
					})

			.AddState("MoveB")
				.OnStart([this]
				{
					m_fMovingTime = 0.f;
					m_vStorePos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					m_vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);						
				})
				.Tick([this](_double TimeDelta)
				{
					_vector vTargetPos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					m_pTransformCom->LookAt(vTargetPos);

					_vector vDest = m_vMyPos - m_vStorePos;					
					m_pTransformCom->Move(0.06f, vDest);

					m_fMovingTime += _float(TimeDelta);

					if(m_fMovingTime >= 6.f)
					{
						// 움직임 통제
						m_bMoveB = false;
						m_bAttack = true;
					}
				})

				.AddTransition("MoveB to Attack", "Attack")
						.Predicator([this] 
						{
							return !m_bMoveB && m_bAttack;
						})

			.AddState("MoveL")
				.OnStart([this]
				{
					m_fMovingTime = 0.f;
					m_vStorePos = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
				})
					.Tick([this](_double TimeDelta)
				{
					_vector vTargetPos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					m_pTransformCom->LookAt(vTargetPos);

					_vector vDest = (m_vStorePos * -1);
					m_pTransformCom->Move(0.06f, vDest);

					m_fMovingTime += _float(TimeDelta);

					if (m_fMovingTime >= 6.f)
					{
						// 움직임 통제
						m_bMoveL = false;
						m_bAttack = true;
					}
				})

				.AddTransition("MoveL to Attack", "Attack")
				.Predicator([this]
				{
					return !m_bMoveL && m_bAttack;
				})


			.AddState("MoveR")
				.OnStart([this]
				{
					m_fMovingTime = 0.f;
					m_vStorePos = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
				})
					.Tick([this](_double TimeDelta)
				{
					_vector vTargetPos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					m_pTransformCom->LookAt(vTargetPos);

					_vector vDest = m_vStorePos;
					m_pTransformCom->Move(0.06f, vDest);

					m_fMovingTime += _float(TimeDelta);

					if (m_fMovingTime >= 6.f)
					{
						// 움직임 통제
						m_bMoveR = false;
						m_bAttack = true;
					}				
				})
					.AddTransition("MoveR to Attack", "Attack")
					.Predicator([this]
				{
					return !m_bMoveR && m_bAttack;
				})


			.Build();
	}	
	// Tick에서 도는게 아닌
	m_pTrigger->SetOnTriggerIn([this](CGameObject* pObj)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();

		CFlowerLeg* pFlower = dynamic_cast<CFlowerLeg*>(pObj);

		/*if (pFlower == nullptr)
			MSG_BOX("null");

		else
			MSG_BOX("!null");*/
	});

	return S_OK;
}

void CSkummyPool::BeginTick()
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

void CSkummyPool::Tick(_double TimeDelta)
{
	CMonster::Tick(TimeDelta);

	m_pTrigger->Update_Tick(m_pTransformCom);

	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);
}

void CSkummyPool::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CSkummyPool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pModelCom->Render(m_pTransformCom);

	return S_OK;
}

void CSkummyPool::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
	m_pFSM->Imgui_RenderProperty();
}

void CSkummyPool::AfterPhysX()
{
	__super::AfterPhysX();
	m_pTrigger->Update_AfterPhysX(m_pTransformCom);
}

HRESULT CSkummyPool::Setup_AnimSocket()
{
	CAnimation*	pAnimation = nullptr;

	NULL_CHECK(pAnimation = m_pModelCom->Find_Animation("AS_em0600_411_AL_damage_m_F"));
	m_GroundDmgSocket.push_back(pAnimation = m_pModelCom->Find_Animation("AS_em0600_411_AL_damage_m_F"));

	NULL_CHECK(pAnimation = m_pModelCom->Find_Animation("AS_em0600_424_AL_dead_down"));
	m_DeadAnimSocket.push_back(pAnimation = m_pModelCom->Find_Animation("AS_em0600_424_AL_dead_down"));

	return S_OK;
}

HRESULT CSkummyPool::SetUp_Components(void * pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_NOW, TEXT("MonsterSkummyPool"), TEXT("Com_Model"), 
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	/*if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		if (json.contains("Model"))
		{
			string ProtoModel = json["Model"];
			m_ModelName = CGameUtils::s2ws(ProtoModel);
			FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_ModelName.c_str(), m_ModelName.c_str(),
				(CComponent**)&m_pModelCom));
		}
	}*/	

	m_pASM = CSkmP_AnimInstance::Create(m_pModelCom, this);
	if (nullptr == m_pASM)
	{
		MSG_BOX("SkummyPool's ASM Failed");
		return E_FAIL;
	}

	return S_OK;
}

CSkummyPool * CSkummyPool::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkummyPool* pInstance = new CSkummyPool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSkummyPool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSkummyPool::Clone(void * pArg)
{
	CSkummyPool*		pInstance = new CSkummyPool(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSkummyPool");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkummyPool::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pFSM);
	Safe_Release(m_pASM);
	Safe_Release(m_pTrigger);
}
