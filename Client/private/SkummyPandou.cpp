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

CSkummyPandou::CSkummyPandou(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CSkummyPandou::CSkummyPandou(const CSkummyPandou & rhs)
	: CGameObject(rhs)
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

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat3(&_float3(-7.f, 0.5f, 7.f)));

	m_pTransformCom->SetSpeed(1.f);

	m_strObjectTag = "Skummy_Pandou";

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
									
				})




			.Build();
	}

	return S_OK;
}


void CSkummyPandou::BeginTick()
{
	__super::BeginTick();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	for (auto& iter : pGameInstance->GetLayer(LEVEL_NOW, L"Layer_Monster")->GetGameObjects())
	{
		if (iter->GetPrototypeTag() == TEXT("FlowerLeg"))
		{
			m_pFlowerLeg = iter;
		}
	}
}

void CSkummyPandou::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	StateCheck(TimeDelta);

	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);
}

void CSkummyPandou::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CSkummyPandou::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pModelCom->Render(m_pTransformCom);

	return S_OK;
}

void CSkummyPandou::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
	
	m_pModelCom->Imgui_RenderProperty();
	m_pFSM->Imgui_RenderProperty();
}

void CSkummyPandou::StateCheck(_double TimeDelta)
{
	m_fPlayRatio = 0.f;

	if (nullptr != m_pModelCom->GetPlayAnimation())
		m_fPlayRatio = m_pModelCom->GetPlayAnimation()->GetPlayRatio();

	
	// m_pFlowerLeg
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
}
