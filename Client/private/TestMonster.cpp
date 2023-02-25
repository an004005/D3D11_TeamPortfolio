#include "stdafx.h"
#include "TestMonster.h"
#include "GameInstance.h"
#include "MathUtils.h"
#include "GameUtils.h"
#include "FSMComponent.h"
#include"Animation.h"
#include"Model.h"

CTestMonster::CTestMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTestMonster::CTestMonster(const CTestMonster & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTestMonster::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CTestMonster::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat3(&_float3(0.f, 0.f, 0.f)));

	m_strObjectTag = "Test_Monster";

	{
		m_pFSM = CFSMComponentBuilder()
			.InitState("Idle")
			.AddState("Idle")
			.Tick([this](_double TimeDelta)
		{
			if (!m_bCondition)
			{
				m_pModelCom->SetPlayAnimation("AS_em0100_102_AL_wait02");
				m_bCondition = true;
			}

			if (m_bWtoI == false)
			{
//				m_fTimeAcc += _float(TimeDelta * 0.1);
				m_fTimeAcc += TimeDelta;

				if (m_fTimeAcc >= 3.f)
				{
					m_bIdle = false;
					m_bWalk = true;
					m_fTimeAcc = 0.f;

					m_bWtoD = true;
				}
			}

			else
			{			
//				m_fTimeAcc += _float(TimeDelta * 0.1);

				m_fTimeAcc += TimeDelta;

				if (m_fTimeAcc >= 3.f)
				{
					m_bDead = true;
				}				
			}
		})
			.AddTransition("Idle to Walk", "Walk")
			.Predicator([this]		// 시간 횟수 // 0,0 확인
		{
			return m_bWalk == true && m_bWtoI == false;
		})

			.AddTransition("Idle to Dead", "Dead")
			.Predicator([this]
		{
			return m_bDead == true && m_bWtoI == true;
		})


			.AddState("Walk")
			.OnStart([this]
		{
			m_pModelCom->SetPlayAnimation("AS_em0100_155_AL_walk");
		})
			.Tick([this](_double TimeDelta)
		{
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			XMStoreFloat3(&m_fMyState, vPos);
			_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

			_vector vDir = vPos - vLook;
			
//			m_fTimeAcc += _float(TimeDelta * 0.3);

			m_fTimeAcc += TimeDelta;

			if (m_fTimeAcc >= 1.f)
			{
				++m_iLRWalkCNT;
				m_fTimeAcc = 0.f;
				m_pTransformCom->LookAt(vDir);
			}
			m_pTransformCom->Go_Straight(_double(TimeDelta * 0.00000005));

			if (m_iLRWalkCNT >= 6)
			{
				if (!m_bRtoW)
				{
					m_iLRWalkCNT = 0;
					m_bAttack = true;
				}				

				else
				{
					m_iLRWalkCNT = 0;					
					m_bWtoI = true;
					m_bCondition = false;
					m_fTimeAcc = 0.f;
				}
			}
		})
			.AddTransition("Walk to Attack", "Attack")
			.Predicator([this]
		{
			return m_bAttack == true && m_bRtoW == false && m_bWtoI == false;
		})
			.AddTransition("Walk to Idle", "Idle")
			.Predicator([this]
		{
			return m_bWtoI == true;
		})

			.AddState("Attack")
			.OnStart([this]
		{
			
		})
			.Tick([this](_double TimeDelta)
		{
			if (m_bCondition)
			{
				m_pModelCom->SetPlayAnimation("AS_em0100_203_AL_atk_a3_stomp");
				m_bCondition = false;
			}						// GetPlayAnimation() : 현재 진행중인 애니메이션
			auto pAnim = m_pModelCom->GetPlayAnimation();
						// GetPlayRatio() : 현재 진행중인 애니메이션의 프레임을 실시간으로 가져옴
			if (pAnim->GetPlayRatio() >= 0.95)
				m_pModelCom->SetPlayAnimation("AS_em0100_203_AL_atk_a3_stomp_end");

		})
			.AddTransition("Attack to Rush", "Rush")
			.Predicator([this]
		{
			return (m_pModelCom->Find_Animation("AS_em0100_203_AL_atk_a3_stomp_end")->GetPlayRatio() > 0.95);
		})

			.AddState("Rush")
			.OnStart([this]
		{
			_float3 fFinishPos = { 10.f, 0.f, 10.f };
			_vector vFinishPos = XMLoadFloat3(&fFinishPos);

			m_pTransformCom->LookAt(-vFinishPos);

			if (!m_bCondition)
			{
				m_pModelCom->SetPlayAnimation("AS_em0100_256_AL_atk_a3_rush_start");
				m_bCondition = true;
			}
		})

			.Tick([this](_double TimeDelta)
		{			
			auto pAnim = m_pModelCom->GetPlayAnimation();

			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			XMStoreFloat3(&m_fMyState, vPos);

			if (!m_bRush)
			{
				if(m_iRushStart)
					m_pTransformCom->Go_Straight(_double(TimeDelta * 0.00000004));
				
				if (pAnim->GetPlayRatio() > 0.7)				
					m_iRushStart = 1;
				
				if (pAnim->GetPlayRatio() > 0.95 && m_iRushStart == 1)
				{
					m_pModelCom->SetPlayAnimation("AS_em0100_257_AL_atk_a3_rush_loop");
					m_iRushStart = 2;
				}
			}			

			if (m_fMyState.x > 50.f && m_fMyState.z > 50.f)
			{
				m_bRush = true;
				m_iRushStart = 0;

				if (m_bCondition)
				{
					m_pModelCom->SetPlayAnimation("AS_em0100_258_AL_atk_a3_rush_stop_return");
					m_bCondition = false;
				}

				if (m_pModelCom->GetPlayAnimation()->GetPlayRatio() >= 0.95)
				{
					m_bRtoW = true;
					m_iLRWalkCNT = 0;
					m_fTimeAcc = 0.f;
				}
			}
		})
			.AddTransition("Rush to Walk", "Walk")
			.Predicator([this]
		{
			return m_bRtoW == true;
		})

			.AddState("Dead")
			.OnStart([this]
		{
			m_pModelCom->SetPlayAnimation("AS_em0100_474_AL_dead_down02");
		})
			.Tick([this](_double TimeDelta)
		{
			auto pAnim = m_pModelCom->GetPlayAnimation();

			if (pAnim->GetPlayRatio() > 0.95)
				pAnim->SetStay(true);
		})

			.Build();
	}

	return S_OK;
}

void CTestMonster::BeginTick()
{
	__super::BeginTick();
}

void CTestMonster::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pFSM->Tick(TimeDelta);
	m_pModelCom->Play_Animation(TimeDelta);
}


void CTestMonster::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTestMonster::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pModelCom->Render(m_pTransformCom);

	return S_OK;
}

void CTestMonster::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
	m_pFSM->Imgui_RenderProperty();
}

HRESULT CTestMonster::SetUp_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_NOW, TEXT("TestMonsterGoat"), TEXT("Com_ModelCom"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;
	// Prototype_Component_Shader_VtxAnimModel
	if (FAILED(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTestMonster::SetUp_ShaderResources()
{
	return S_OK;
}

CTestMonster * CTestMonster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTestMonster* pInstance = new CTestMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTestMonster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTestMonster::Clone(void * pArg)
{
	CTestMonster*		pInstance = new CTestMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTestMonster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTestMonster::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pFSM);
}




