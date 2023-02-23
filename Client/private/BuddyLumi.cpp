#include "stdafx.h"
#include "BuddyLumi.h"
#include "GameInstance.h"
#include "MathUtils.h"
#include "GameUtils.h"
#include "FSMComponent.h"
#include "AnimationInstance.h"
#include"Animation.h"
#include"Model.h"
#include "JsonStorage.h"

#include "BdLm_AnimInstance.h"
#include "TimerHelper.h"
#include "FlowerLeg.h"

CBuddyLumi::CBuddyLumi(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CBuddyLumi::CBuddyLumi(const CBuddyLumi & rhs)
	: CGameObject(rhs)
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
					m_fTimeAcc += _float(TimeDelta * 2);
					
					if (m_fTimeAcc >= 6.4f && !m_bInitialize)	// 처음 생성되고 1회
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
					_vector vDest = { 0.f, 0.f, 0.f, 1.f };

					m_pTransformCom->LookAt(vDest);
					m_pTransformCom->Chase(vDest, 0.07f);

					vector<_uint> vecRandomPattern;

					_uint iAttack = 1;
					vecRandomPattern.push_back(iAttack);
					
					_uint iDodgeB = 2;
					vecRandomPattern.push_back(iDodgeB);

					_uint iDodgeL = 3;
					vecRandomPattern.push_back(iDodgeL);

					_uint iDodgeR = 4;
					vecRandomPattern.push_back(iDodgeR);

					random_shuffle(vecRandomPattern.begin(), vecRandomPattern.end());

					if (m_fMyPos.x <= 2.f && m_fMyPos.z <= 2.f)
					{
						m_bRun = false;

						_uint iShuffleResult = vecRandomPattern.front();

						if (iShuffleResult == 1)
							m_bAttack = true;

						if (iShuffleResult == 2)
							m_bDodgeB = true;

						if (iShuffleResult == 3)
							m_bDodgeL = true;

						if (iShuffleResult == 4)
							m_bDodgeR = true;
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
					}
				})

				.AddTransition("Attack to Walk", "Walk")
					.Predicator([this]()->_bool { return !m_bAttack && m_bWalk; })

				.AddTransition("Attack to Threat", "Threat")
					.Predicator([this]()->_bool {return !m_bAttack && m_bThreat; })

			.AddState("Threat")
				.Tick([this](_double TimeDelta)
				{
					auto pAnim = m_pModelCom->GetPlayAnimation();

					if (pAnim == m_pModelCom->Find_Animation("AS_em0400_160_AL_threat") && pAnim->IsFinished() == true)
					{
						m_bThreat = false;
						m_iAfterAtkPt = 1;
					}
				})

				.AddTransition("Threat to Run", "Run")// 여기 조건
					.Predicator([this]()->_bool {return 0; })

			.AddState("Walk")
				.Tick([this](_double TimeDelta)
				{
					_vector vDest = { 4.f, 0.f, 4.f, 1.f };

					m_pTransformCom->LookAt(vDest);
					m_pTransformCom->Chase(vDest, 0.01f);

					if (m_fMyPos.x >= 4.f && m_fMyPos.z >= 4.f)
					{
						m_bWalk = false;
						m_iAfterAtkPt = 0;
					}
				})

			.Build();
	}

	return S_OK;
}

void CBuddyLumi::BeginTick()
{
	__super::BeginTick();
}

void CBuddyLumi::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_fMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	
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
			m_pASM->InputAnimSocket("BuddyLumi_GroundDmgAnim", m_GroundDmgSocket);
		}

		if (ImGui::Button("AirDmg"))
		{
			m_pASM->InputAnimSocket("BuddyLumi_AirDmgAnim", m_AirDmgSocket);
		}

		if (ImGui::Button("Dead"))
		{
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
