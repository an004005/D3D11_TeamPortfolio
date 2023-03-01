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

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat3(&_float3(5.f, 0.f, 5.f)));

	m_pTransformCom->SetSpeed(1.f);

	m_strObjectTag = "Bron_Jon";

	{
		m_pFSM = CFSMComponentBuilder()
			.InitState("Idle")
			.AddState("Idle")
				.OnStart([this]
				{
					m_fTimeAcc = 0.f;
				})
				.Tick([this](_double TimeDelta)
				{
					m_fTimeAcc += _float(TimeDelta * 1);

					if (m_fTimeAcc >= 3.f)
					{
						m_bIdle = false;
						m_bBiteAtk = true;
					}
				})

				.AddTransition("Idle to BiteAtk", "BiteAtk")
					.Predicator([this]
					{
						return !m_bIdle && m_bBiteAtk;
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

						if (pGameInstance->SweepSphere(params))	// Á¶°Ç
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
}
