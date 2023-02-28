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
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	if (FAILED(Setup_AnimSocket()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat3(&_float3(15.f, 0.f, 15.f)));
	
	m_pTransformCom->SetSpeed(1.f);
	
	m_strObjectTag = "Buddy_Lumi";

	{
		m_pFSM = CFSMComponentBuilder()
			.InitState("Idle")
			.AddState("Idle")		
				.Tick([this](_double TimeDelta)
				{
					m_fTimeAcc += _float(TimeDelta * 1);
					
					if (m_fTimeAcc >= 7.4f && !m_bInitialize)	// 처음 생성되고 1회
					{
						m_bRun = true;
						m_fTimeAcc = 0.f;
						m_bInitialize = true;
					}
				})

				.AddTransition("Idle to Run", "Run")
					.Predicator([this]
					{
						return m_bRun;
					})

				/*.AddTransition("Idle to Attack", "Attack")
					.Predicator([this]()->_bool {return m_bAttack; })

				.AddTransition("Idle to DodgeB", "DodgeB")
					.Predicator([this]()->_bool {return m_bDodgeB; })*/

			.AddState("Run")
				.Tick([this](_double TimeDelta)
				{
					m_fMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
					
					_vector vTargetPos = m_pFlowerLeg->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
					_float3 fTargetPos;
					XMStoreFloat3(&fTargetPos, vTargetPos);

					m_pTransformCom->LookAt(vTargetPos);
					m_pTransformCom->Chase(vTargetPos, 0.03f);

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
										
					if (m_fMyPos.x >= fTargetPos.x && m_fMyPos.z >= fTargetPos.z && m_fMyPos.x <= (fTargetPos.x + 1.3f) && m_fMyPos.z <= (fTargetPos.z + 1.3f) ||
						m_fMyPos.x <= fTargetPos.x && m_fMyPos.z >= fTargetPos.z && m_fMyPos.x >= (fTargetPos.x - 1.3f) && m_fMyPos.z <= (fTargetPos.z + 1.3f) ||
						m_fMyPos.x <= fTargetPos.x && m_fMyPos.z <= fTargetPos.z && m_fMyPos.x >= (fTargetPos.x - 1.3f) && m_fMyPos.z >= (fTargetPos.z - 1.3f) ||
						m_fMyPos.x >= fTargetPos.x && m_fMyPos.z <= fTargetPos.z && m_fMyPos.x <= (fTargetPos.x + 1.3f) && m_fMyPos.z >= (fTargetPos.z - 1.3f))
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
					_vector vTargetPos = m_pFlowerLeg->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);

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
						//m_vStorePos =
					})
				.Tick([this](_double TimeDelta) 
				{
					auto pAnim = m_pModelCom->GetPlayAnimation();

					// 뒤로 이동 
					

					if (pAnim == m_pModelCom->Find_Animation("AS_em0400_135_AL_dodge_B") && pAnim->IsFinished() == true)
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
				.Tick([this](_double TimeDelta)
				{
					auto pAnim = m_pModelCom->GetPlayAnimation();

					// 중점 Axis 기준 왼쪽으로 Turn

					if (pAnim == m_pModelCom->Find_Animation("AS_em0400_140_AL_dodge_L") && pAnim->IsFinished() == true)
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
				.Tick([this](_double TimeDelta)
				{
					auto pAnim = m_pModelCom->GetPlayAnimation();

					// 중점 Axis 기준 오른쪽으로 Turn

					if (pAnim == m_pModelCom->Find_Animation("AS_em0400_145_AL_dodge_R") && pAnim->IsFinished() == true)
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

	return S_OK;
}

void CBuddyLumi::BeginTick()
{
	__super::BeginTick();
	
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, L"Layer_Monster")->GetGameObjects())
	{
		if (iter->GetPrototypeTag() == TEXT("FlowerLeg"))
		{
			int iA = 0;

			m_pFlowerLeg = iter;			
		}
	}
}

void CBuddyLumi::Tick(_double TimeDelta)
{	
	__super::Tick(TimeDelta);

	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);	
}

void CBuddyLumi::Late_Tick(_double TimeDelta)
{	
	__super::Late_Tick(TimeDelta);
		
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CBuddyLumi::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pModelCom->Render(m_pTransformCom);

	return S_OK;
}

void CBuddyLumi::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

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

	m_pModelCom->Imgui_RenderProperty();
	m_pFSM->Imgui_RenderProperty();
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

HRESULT CBuddyLumi::SetUp_Components(void* pArg)
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

	m_pASM = CBdLm_AnimInstance::Create(m_pModelCom, this);
	if (nullptr == m_pASM)
	{
		MSG_BOX("BuddyLumi's ASM Failed");
		return E_FAIL;
	}

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
	Safe_Release(m_pASM);
}
