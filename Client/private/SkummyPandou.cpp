#include "stdafx.h"
#include "SkummyPandou.h"
#include "GameInstance.h"
#include "MathUtils.h"
#include "GameUtils.h"
#include "FSMComponent.h"
#include "AnimationInstance.h"
#include "Animation.h"
#include "Model.h"
#include "JsonStorage.h"

#include "SkPd_AnimInstance.h"
#include "FlowerLeg.h"
#include "Player.h"
#include "RigidBody.h"

CSkummyPandou::CSkummyPandou(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CSkummyPandou::CSkummyPandou(const CSkummyPandou & rhs)
	: CMonster(rhs)
{
}

HRESULT CSkummyPandou::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkummyPandou::Initialize(void * pArg)
{	
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	if (FAILED(Setup_AnimSocket()))
		return E_FAIL;
	
	//	m_fGravity = 평소엔 0으로 잡아주고, 피격시 중력값을 올려서 떨어트려준다.

	Json SkummyPandouTrigger = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/SkummyPandouTrigger.json");
	Json SkummyPandouSearch = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Monster/SkummyPandouSearch.json");

	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Trigger"),
		(CComponent**)&m_pTrigger, &SkummyPandouTrigger)))
		return E_FAIL;


	if (FAILED(Add_Component(LEVEL_NOW, TEXT("Prototype_Component_RigidBody"), TEXT("Search"),
		(CComponent**)&m_pSearch, &SkummyPandouSearch)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat3(&_float3(4.f, 0.5f, 10.f)));

	m_pTransformCom->SetSpeed(0.4f);

	m_strObjectTag = "Skummy_Pandou";

	//m_StrDamage.

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
					CGameInstance* pGameInstance = CGameInstance::GetInstance();
								
					m_vStorePos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					XMStoreFloat3(&m_fStorePos, m_vStorePos);

					if (!m_bInitialize)
					{
						if (m_fMyPos.x >= m_fStorePos.x && m_fMyPos.z >= m_fStorePos.z && m_fMyPos.x <= (m_fStorePos.x + 2.3f) && m_fMyPos.z <= (m_fStorePos.z + 2.3f) ||
							m_fMyPos.x <= m_fStorePos.x && m_fMyPos.z >= m_fStorePos.z && m_fMyPos.x >= (m_fStorePos.x - 2.3f) && m_fMyPos.z <= (m_fStorePos.z + 2.3f) ||
							m_fMyPos.x <= m_fStorePos.x && m_fMyPos.z <= m_fStorePos.z && m_fMyPos.x >= (m_fStorePos.x - 2.3f) && m_fMyPos.z >= (m_fStorePos.z - 2.3f) ||
							m_fMyPos.x >= m_fStorePos.x && m_fMyPos.z <= m_fStorePos.z && m_fMyPos.x <= (m_fStorePos.x + 2.3f) && m_fMyPos.z >= (m_fStorePos.z - 2.3f))
						{
							m_bRandomMove = false;
							m_bInitialize = true;
						}
						else
						{
							m_bIdle = false;
							m_bRandomMove = true;
						}									
					}

					// Trigger Tick으로 대체
					// 시야 범위 Trigger에 걸리기 전까진
					//if(!m_bSearchEye)
					//{		
					//	//m_bIdle = false;
					//	//m_bRandomMove = true;
					//}

					//else // 시야 범위 Trigger에 걸린
					//{
					//	m_bRandomMove = false;
					//	m_bInitialize = true;
					//}
					// ~Trigger Tick으로 대체

					else // 한 번 Player를 발견하면 여기로만 조건이 들어온다.
					{
						m_vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
						XMStoreFloat3(&m_fMyPos, m_vMyPos);

						if (m_fMyPos.x >= m_fStorePos.x && m_fMyPos.z >= m_fStorePos.z && m_fMyPos.x <= (m_fStorePos.x + 3.3f) && m_fMyPos.z <= (m_fStorePos.z + 3.3f) ||
							m_fMyPos.x <= m_fStorePos.x && m_fMyPos.z >= m_fStorePos.z && m_fMyPos.x >= (m_fStorePos.x - 3.3f) && m_fMyPos.z <= (m_fStorePos.z + 3.3f) ||
							m_fMyPos.x <= m_fStorePos.x && m_fMyPos.z <= m_fStorePos.z && m_fMyPos.x >= (m_fStorePos.x - 3.3f) && m_fMyPos.z >= (m_fStorePos.z - 3.3f) ||
							m_fMyPos.x >= m_fStorePos.x && m_fMyPos.z <= m_fStorePos.z && m_fMyPos.x <= (m_fStorePos.x + 3.3f) && m_fMyPos.z >= (m_fStorePos.z - 3.3f))
						{
							m_fTimeAcc += _float(TimeDelta * 1);

							if (m_fTimeAcc >= 5.f)
							{
								m_bIdle = false;

								if (m_iAfterIdlePt == 0)
									m_bMoveR = true;

								if (m_iAfterIdlePt == 1)
									m_bAttackStart = true;

								if (m_iAfterIdlePt == 2)
									m_bThreat = true;

								if (m_iAfterIdlePt == 3)
									m_bAttackStart = true;

								if (m_iAfterIdlePt == 4)
									m_bMoveL = true;

								if (m_iAfterIdlePt == 5)
									m_bMoveB = true;

								if (m_iAfterIdlePt == 6)
									m_bAttackStart = true;

								++m_iAfterIdlePt;

								if (m_iAfterIdlePt > 6)
									m_iAfterIdlePt = 0;
							}
						}
						else
						{
							m_fTimeAcc += _float(TimeDelta * 1);
							if (m_fTimeAcc >= 5.f)
							{
								m_bIdle = false;
								m_bMoveF = true;
							}
						}						
					}
				})

				.AddTransition("Idle to RandomMove", "RandomMove")
					.Predicator([this]
					{
						return !m_bIdle && m_bRandomMove;
					})

				.AddTransition("Idle to MoveF", "MoveF")
					.Predicator([this]
					{
						return !m_bIdle && m_bMoveF;
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

				.AddTransition("Idle to Attack_Start", "Attack_Start")
					.Predicator([this] 
					{
						return !m_bIdle && m_bAttackStart; 
					})

				.AddTransition("Idle to Threat", "Threat")
					.Predicator([this]
					{
						return !m_bIdle && m_bThreat;
					})

			.AddState("RandomMove")
				.OnStart([this]				
				{ 
					m_fTimeAcc = 0.f; 
					m_bCurrentRatioSave = false;
				})
				.Tick([this](_double TimeDelta)
				{				
					m_fTimeAcc += _float(TimeDelta * 1);

					m_pTransformCom->LocalMove(m_pModelCom->GetLocalMove(m_pTransformCom->Get_WorldMatrix()), 0.4f);

					if (m_fTimeAcc > 3.f)
					{
						m_bIdle = true;
						m_bRandomMove = false;
						m_bCurrentRatioSave = true;
					}
				})
				.AddTransition("RandomMove to Idle", "Idle")
					.Predicator([this]
					{
						return !m_bRandomMove && m_bIdle;
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
					
			.AddState("Attack_Start")
				.OnStart([this] 
				{
					_vector vAtkLook = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					/*_float3 fAtkLook;
					XMStoreFloat3(&fAtkLook, vAtkLook);
					fAtkLook.y += 0.5f;*/

					m_pTransformCom->LookAt(vAtkLook);
				})
				.Tick([this](_double TimeDelta)
				{
					auto pAnim = m_pModelCom->GetPlayAnimation();

					if (pAnim->GetPlayRatio() > 0.97)
					{
						m_bAttackStart = false;
						m_bAttacking = true;
					}
				})
				.AddTransition("Attack_Start to Attack_Ing", "Attack_Ing")
					.Predicator([this] 
					{
						return !m_bAttackStart && m_bAttacking;
					})

			.AddState("Attack_Ing")
				.OnStart([this]
				{
					m_fTimeAcc = 0.f;
				})
				.Tick([this](_double TimeDelta)
				{
					m_fTimeAcc += _float(TimeDelta * 1);

					m_pTransformCom->Go_Straight(0.3);

					if (m_fTimeAcc >= 2.f)
					{
						m_bAttacking = false;
						m_bAttackEnd = true;
					}					
				})

				.AddTransition("Attack_Ing to Attack_End", "Attack_End")
					.Predicator([this]
					{
						return !m_bAttacking && m_bAttackEnd;
					})

			.AddState("Attack_End")
				.Tick([this](_double TimeDelta)
				{
					auto pAnim = m_pModelCom->GetPlayAnimation();

					if (pAnim->GetPlayRatio() > 0.97)
					{
						m_bAttackEnd = false;
						m_bIdle = true;
					}
				})
				.AddTransition("Attack_End to Idle", "Idle")
					.Predicator([this]
					{
						return !m_bAttackEnd && m_bIdle;
					})

			.AddState("MoveF")				
				.Tick([this](_double TimeDelta)
				{
					m_vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					XMStoreFloat3(&m_fMyPos, m_vMyPos);

					m_vStorePos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					XMStoreFloat3(&m_fStorePos, m_vStorePos);

					m_pTransformCom->LookAt(m_vStorePos);
					m_pTransformCom->Chase(m_vStorePos, 0.13);

					if (m_fMyPos.x >= m_fStorePos.x && m_fMyPos.z >= m_fStorePos.z && m_fMyPos.x <= (m_fStorePos.x + 5.f) && m_fMyPos.z <= (m_fStorePos.z + 5.f) ||
						m_fMyPos.x <= m_fStorePos.x && m_fMyPos.z >= m_fStorePos.z && m_fMyPos.x >= (m_fStorePos.x - 5.f) && m_fMyPos.z <= (m_fStorePos.z + 5.f) ||
						m_fMyPos.x <= m_fStorePos.x && m_fMyPos.z <= m_fStorePos.z && m_fMyPos.x >= (m_fStorePos.x - 5.f) && m_fMyPos.z >= (m_fStorePos.z - 5.f) ||
						m_fMyPos.x >= m_fStorePos.x && m_fMyPos.z <= m_fStorePos.z && m_fMyPos.x <= (m_fStorePos.x + 5.f) && m_fMyPos.z >= (m_fStorePos.z - 5.f))
					{
						m_bMoveF = false;
						m_bAttackStart = true;
					}
				})
				.AddTransition("MoveF to Attack_Start", "Attack_Start")
					.Predicator([this]
					{
						return !m_bMoveF && m_bAttackStart;
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
					m_pTransformCom->Move(0.08, m_vDest);
					
					m_fTimeAcc += _float(TimeDelta * 1);

					if (m_fTimeAcc >= 3.f)
					{
						m_bMoveB = false;

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
							m_bAttackStart = true;

					}
				})
				.AddTransition("MoveB to Attack_Start", "Attack_Start")
					.Predicator([this]
					{
						return !m_bMoveB && m_bAttackStart;
					})

				.AddTransition("MoveB to Threat", "Threat")
					.Predicator([this]
					{
						return !m_bMoveB && m_bThreat;
					})
			
			.AddState("MoveL")
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
					m_pTransformCom->Move(0.008f, -vMyRight);

					if (pAnim->IsFinished() == true)
					{
						m_bMoveL = false;
						
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
							m_bAttackStart = true;
					}
				})
				.AddTransition("MoveL to Attack_Start", "Attack_Start")
					.Predicator([this]
				{
					return !m_bMoveL && m_bAttackStart;
				})

				.AddTransition("MoveL to Threat", "Threat")
					.Predicator([this]
				{
					return !m_bMoveL && m_bThreat;
				})


			.AddState("MoveR")
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
					m_pTransformCom->Move(0.008f, vMyRight);

					if (pAnim->IsFinished() == true)
					{
						m_bMoveR = false;

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
							m_bAttackStart = true;
					}
				})
					.AddTransition("MoveR to Attack_Start", "Attack_Start")
					.Predicator([this]
				{
					return !m_bMoveR && m_bAttackStart;
				})

					.AddTransition("MoveR to Threat", "Threat")
					.Predicator([this]
				{
					return !m_bMoveR && m_bThreat;
				})

						
			.Build();
	}

	// Tick에서 도는게 아닌
	// Player가 최초에 시야에 들어오는지를 체크하는 Trigger. 들어오면 : Player를 발견했다는 UI가 출력되며 이후 패턴들 실행.  들어오지 않았다면 : RandomMove
	//m_pTrigger->SetOnTriggerIn([this](CGameObject* pObj)
	//{
	//	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//	CFlowerLeg* pFlower = dynamic_cast<CFlowerLeg*>(pObj);
	//	if (pFlower != nullptr && pFlower->GetObjectTag() == "Flower_Leg")
	//	{
	//		m_pStorePt = dynamic_cast<CGameObject*>(pFlower);
	//	}

	//	if (m_pStorePt != nullptr)
	//		m_bSearchEye = true;
	//});

	//m_pSearch->SetOnTriggerIn([this](CGameObject* pObj)
	//{
	//	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//	CFlowerLeg* pFlower = dynamic_cast<CFlowerLeg*>(pObj);
	//	if (pFlower != nullptr && pFlower->GetObjectTag() == "Flower_Leg")
	//	{
	//		m_pStorePt = dynamic_cast<CGameObject*>(pFlower);
	//	}

	//	/*if (pFlower != nullptr)
	//		m_bArea = true;

	//	if (pFlower == nullptr)
	//		m_bArea = false;*/
	//});

	return S_OK;
}


void CSkummyPandou::BeginTick()
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

void CSkummyPandou::Tick(_double TimeDelta)
{
	CMonster::Tick(TimeDelta);

	m_pSearch->Update_Tick(m_pTransformCom);
	m_pTrigger->Update_Tick(m_pTransformCom);

	StateCheck(TimeDelta);

	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);
}

void CSkummyPandou::Late_Tick(_double TimeDelta)
{
	CMonster::Late_Tick(TimeDelta);

	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CSkummyPandou::Render()
{
	if (FAILED(CMonster::Render()))
		return E_FAIL;

	m_pModelCom->Render(m_pTransformCom);

	return S_OK;
}

void CSkummyPandou::Imgui_RenderProperty()
{
	CMonster::Imgui_RenderProperty();
	
	/*_bool bSearchEye = m_bSearchEye;
	_bool bArea = m_bArea;

	ImGui::Text("");*/

	m_pFSM->Imgui_RenderProperty();
}

void CSkummyPandou::AfterPhysX()
{
	__super::AfterPhysX();

	m_pTrigger->Update_AfterPhysX(m_pTransformCom);
	m_pSearch->Update_AfterPhysX(m_pTransformCom);
}

void CSkummyPandou::StateCheck(_double TimeDelta)
{
	m_fPlayRatio = 0.f;

	if (nullptr != m_pModelCom->GetPlayAnimation())
		m_fPlayRatio = m_pModelCom->GetPlayAnimation()->GetPlayRatio();
		
	// m_pFlowerLeg
}

_matrix CSkummyPandou::AttachCollider()
{
	_matrix SocketMatrix = m_pModelCom->GetBoneMatrix("Spine1") * m_pTransformCom->Get_WorldMatrix();

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	return SocketMatrix;
}

HRESULT CSkummyPandou::Setup_AnimSocket()
{
	CAnimation*	pAnimation = nullptr;
	// 맞는 방향에 따른 애니메이션 소켓화시켜줘야 함
	NULL_CHECK(pAnimation = m_pModelCom->Find_Animation("AS_em0700_411_AL_damage_m_F"));
	m_AirDmgSocket.push_back(pAnimation = m_pModelCom->Find_Animation("AS_em0700_411_AL_damage_m_F"));

	NULL_CHECK(pAnimation = m_pModelCom->Find_Animation("AS_em0700_424_AL_dead_down01"));
	m_DeadAnimSocket.push_back(pAnimation = m_pModelCom->Find_Animation("AS_em0700_424_AL_dead_down01"));

	return S_OK;
}

HRESULT CSkummyPandou::SetUp_Components(void * pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/*if (FAILED(__super::Add_Component(LEVEL_NOW, TEXT("MonsterSkummyPandou"), TEXT("Com_Model"),
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

	m_pASM = CSkPd_AnimInstance::Create(m_pModelCom, this);
	if (nullptr == m_pASM)
	{
		MSG_BOX("SkummyPandou's ASM Failed");
		return E_FAIL;
	}

	return S_OK;
}

CSkummyPandou * CSkummyPandou::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkummyPandou* pInstance = new CSkummyPandou(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSkummyPandou");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSkummyPandou::Clone(void * pArg)
{
	CSkummyPandou*		pInstance = new CSkummyPandou(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSkummyPandou");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkummyPandou::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pFSM);
	Safe_Release(m_pASM);
	Safe_Release(m_pTrigger);
	Safe_Release(m_pSearch);
}
