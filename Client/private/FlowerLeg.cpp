#include "stdafx.h"
#include "FlowerLeg.h"
#include "GameInstance.h"
#include "MathUtils.h"
#include "GameUtils.h"
#include "FSMComponent.h"
#include "AnimationInstance.h"
#include"Animation.h"
#include"Model.h"
#include "JsonStorage.h"

#include "FL_AnimInstance.h"

CFlowerLeg::CFlowerLeg(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster(pDevice, pContext)
{
}

CFlowerLeg::CFlowerLeg(const CFlowerLeg & rhs)
	: CMonster(rhs)
{
}

HRESULT CFlowerLeg::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CFlowerLeg::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat3(&_float3(0.f, 0.f, 0.f)));

	m_pTransformCom->SetSpeed(1.f);	

	m_strObjectTag = "Flower_Leg";

	{
		m_pFSM = CFSMComponentBuilder()
			.InitState("Idle")
			.AddState("Idle")
				.Tick([this](_double TimeDelta)
				{
					m_bIdle = true;
					m_bWalk = false;
										
				})

				.AddTransition("Idle to Walk", "Walk")
					.Predicator([this]
					{
						return m_eDikKey == KEY_W || m_eDikKey == KEY_S || m_eDikKey == KEY_A 
							|| m_eDikKey == KEY_D &&  m_eDikKey != KEY_END;
					})
			
				/*.AddTransition("Idle to Attack", "Attack")
					.Predicator([this]
					{
						return m_eDimMouse == MS_LB || m_eDimMouse == MS_RB 
							&& m_eDimMouse != MS_END;
					})*/

				/*.AddTransition("Idle to Jump", "Jump")
					.Predicator([this] 
					{
						return m_bAir;
					})*/

			.AddState("Walk")
				.Tick([this](_double TimeDelta)
				{
					m_bWalk = true;
					m_bIdle = false;					 
				})

				.AddTransition("Walk to Idle", "Idle")
					.Predicator([this]
					{
						return m_eDikKey == KEY_END;
					})

				/*.AddTransition("Walk to Jump", "Jump")
					.Predicator([this]
					{
						return m_bAir;
					})*/

			/*.AddState("Jump")
				.Tick([this](_double TimeDelta)
					{
						if (m_bLanding)
						{
							auto pAnim = m_pModelCom->GetPlayAnimation();

							if (pAnim->GetPlayRatio() > 0.9)							
								m_bLanding = false;					
						}
					})
				.AddTransition("Jump to Idle", "Idle")
					.Predicator([this] 
					{	
						return !m_bLanding;
					})*/

			/*.AddState("Attack")
				.OnStart([this]
				{
					if (m_eDimMouse == MS_LB)
						m_bAttackL = true;

					if (m_eDimMouse == MS_RB)
						m_bAttackR = true;
				})
				.Tick([this](_double TimeDelta)
				{
					auto pAnim = m_pModelCom->GetPlayAnimation();

					if (pAnim->GetPlayRatio() >= 0.95)
					{
						m_bAttackL = false;
						m_bAttackR = false;
						m_eDimMouse = MS_END;
					}
				})
				.AddTransition("Attack to Idle", "Idle")
					.Predicator([this]
					{
						return m_eDimMouse == MS_END;
					})*/

			.Build();
	}

	return S_OK;
}

void CFlowerLeg::BeginTick()
{
	__super::BeginTick();
}

void CFlowerLeg::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
			
	Key_Input(TimeDelta);

	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);
}

void CFlowerLeg::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CFlowerLeg::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pModelCom->Render(m_pTransformCom);

	return S_OK;
}

void CFlowerLeg::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
	
	if (ImGui::CollapsingHeader("MonsterAnimStateTest", ImGuiTreeNodeFlags_DefaultOpen))
	{
		
		if(ImGui::Button("Damaged"))
		{
			if (m_iTestHp <= 0)
				m_iTestHp = 0;

			else
				m_iTestHp -= 50;			
		}

		ImGui::Text("%d", m_iTestHp);
	}

	if (ImGui::CollapsingHeader("JumpSpeed", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("m_fJumpSpeed : PreValue = 13.3f");
		ImGui::InputFloat("m_fJumpSpeed", &m_fJumpSpeed);
		ImGui::Text("m_fJumpTime : %.4f", &m_fJumpTime);
		ImGui::Text("m_fGravity : PreValue = 7.7f");
		ImGui::InputFloat("m_fGravity", &m_fGravity);
		ImGui::Text("m_fOverLap : PreValue = 0.28f");
		ImGui::InputFloat("m_fOverLap", &m_fOverLap);
		ImGui::Text("m_fWeight : PreValue = 0.24f");
		ImGui::InputFloat("m_fWeight", &m_fWeight);
	}

	m_pModelCom->Imgui_RenderProperty();
	m_pFSM->Imgui_RenderProperty();
}

void CFlowerLeg::Key_Input(_double TimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->Get_DIKeyState(DIK_UP) & 0x80)
	{		
		m_pTransformCom->Go_Straight(0.1f);
		m_eDikKey = KEY_W;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_DOWN) & 0x80)
	{		
		m_pTransformCom->Go_Backward(0.1f);
		m_eDikKey = KEY_S;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_LEFT) & 0x80)
	{		
		m_pTransformCom->Go_Left(0.1f);
		m_eDikKey = KEY_A;
	}
	else if (pGameInstance->Get_DIKeyState(DIK_RIGHT) & 0x80)
	{	
		m_pTransformCom->Go_Right(0.1f);
		m_eDikKey = KEY_D;
	}
	else
	{
//		if(!m_bAir && !m_bLanding)
			m_eDikKey = KEY_END;
	}
	
	if (pGameInstance->KeyDown(CInput_Device::DIM_LB))
	{		
		m_eDimMouse = MS_LB;		
	}

	if (pGameInstance->KeyDown(CInput_Device::DIM_RB))
	{
		m_eDimMouse = MS_RB;
	}

	if (pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
	{		
		m_bAir = true;
	}

	// Jump
	if(m_bAir)
		JumpToGround(TimeDelta);
	// ~Jump
}

void CFlowerLeg::JumpToGround(_double TimeDelta)
{
	m_fMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

     if (m_fMyPos.y >= 0.f)
	{													
		m_fMyPos.y = m_fJumpSpeed * m_fJumpTime - (7.7f * m_fJumpTime * m_fJumpTime) * 0.28f;
		m_fJumpTime += 0.24f; 
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat3(&m_fMyPos));		
	}
	
	if (m_fMyPos.y <= 0.f)
	{
		m_bLanding = true;
		m_bAir = false;
		m_fMyPos.y = 0.f;
		m_fJumpTime = 0.f;
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat3(&m_fMyPos));
				
	}
}

HRESULT CFlowerLeg::Setup_AnimSocket()
{	
	return S_OK;
}

HRESULT CFlowerLeg::SetUp_Components(void* pArg)
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/*if (FAILED(__super::Add_Component(LEVEL_NOW, TEXT("MonsterFlowerLeg"), TEXT("Com_ModelCom"),
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
	
	m_pASM = CFL_AnimInstance::Create(m_pModelCom, this);
	if (nullptr == m_pASM)
	{
		MSG_BOX("FlowerLeg's ASM Failed");
		return E_FAIL;
	}

	return S_OK;
}


CFlowerLeg * CFlowerLeg::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFlowerLeg* pInstance = new CFlowerLeg(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFlowerLeg");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFlowerLeg::Clone(void * pArg)
{
	CFlowerLeg*		pInstance = new CFlowerLeg(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFlowerLeg");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFlowerLeg::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pFSM);
	Safe_Release(m_pASM);
}
