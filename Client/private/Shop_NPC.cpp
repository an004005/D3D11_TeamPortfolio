#include "stdafx.h"
#include "Shop_NPC.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Animation.h"
#include "Model.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "RigidBody.h"
#include "ControlledRigidBody.h"
#include "PhysX_Manager.h"
#include "Material.h"
#include "PlayerInfoManager.h"
#include "VFX_Manager.h"
#include "EffectSystem.h"
#include "Canvas_MainTalk.h"
#include "UI_Manager.h"
#include "Canvas_Shop.h"

CShop_NPC::CShop_NPC(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CScarletCharacter(pDevice, pContext)
{
}

CShop_NPC::CShop_NPC(const CShop_NPC& rhs)
	: CScarletCharacter(rhs)
{
}

HRESULT CShop_NPC::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShop_NPC::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Event()))
		return E_FAIL;

	if (FAILED(SetUp_Sound()))
		return E_FAIL;

	return S_OK;
}

void CShop_NPC::BeginTick()
{
    __super::BeginTick();    
    m_pModel->SetPlayAnimation("AS_np0100_002_AL_wait02");


}

void CShop_NPC::Tick(_double TimeDelta)
{
    m_fTimeDelta = TimeDelta;

    __super::Tick(TimeDelta);
    m_pModel->Tick(TimeDelta);
    Update_Animation(TimeDelta);

	if (DistanceCheck())
	{
		// UI 출력 (E 버튼 "상점" -> (필요 시)대사창 1회 출력 -> 상점 출력 (기능 이용)

		if (false == m_iEffectCheck)
		{
			m_iEffectCheck = true;

			m_pShopEffect = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"Z_Shop", PLAYERTEST_LAYER_FRONTUI);
			_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
			m_pShopEffect->Start_AttachOnlyPos(XMVectorSet(vPos.x, vPos.y + 2.3f, vPos.z, 1.0));
		}

		if (CGameInstance::GetInstance()->KeyDown(DIK_Z))
		{
			if (false == m_iTalkCheck && nullptr == m_pCanvas_MainTalk)
			{
				Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MainTalk.json");
				m_pCanvas_MainTalk = dynamic_cast<CCanvas_MainTalk*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Test", L"Canvas_MainTalk", &json));
				m_pCanvas_MainTalk->Add_Talk(0);
			}

			if (true == m_iTalkCheck)
			{
				m_iTalkCheck = false;
				m_iEffectCheck = false;
				dynamic_cast<CCanvas_Shop*>(CUI_Manager::GetInstance()->Find_WindowCanvas(L"CCanvas_Shop"))->Set_ShopUI();
			}
		}

		if (nullptr != m_pCanvas_MainTalk)
		{
			if (false == m_pCanvas_MainTalk->Get_End())
			{
				m_iTalkCheck = true;
				m_pCanvas_MainTalk->SetDelete();
				m_pCanvas_MainTalk = nullptr;
				m_pShopEffect->SetDelete();
				m_pShopEffect = nullptr;
				dynamic_cast<CCanvas_Shop*>(CUI_Manager::GetInstance()->Find_WindowCanvas(L"CCanvas_Shop"))->Set_ShopUI();
			}
		}
	}
	else
	{
		if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pShopEffect))
		{
			m_iEffectCheck = false;
			m_pShopEffect->SetDelete();
		}
	}
}

void CShop_NPC::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_bVisible && (nullptr != m_pRenderer))
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND_TOON, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
	}
}

void CShop_NPC::AfterPhysX()
{
	__super::AfterPhysX();
}

HRESULT CShop_NPC::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pModel->Render(m_pTransformCom);

	return S_OK;
}

HRESULT CShop_NPC::Render_ShadowDepth()
{
	m_pModel->Render_ShadowDepth(m_pTransformCom);
	return S_OK;
}

void CShop_NPC::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::CollapsingHeader("Anim Fast Modifier"))
	{
		static string szMonsterAnimName = "";
		if (ImGui::BeginListBox("Animation List"))
		{
			static char szSeachAnim[MAX_PATH] = "";
			ImGui::InputText("Anim Search", szSeachAnim, MAX_PATH);

			const string strSearch = szSeachAnim;
			const _bool bSearch = strSearch.empty() == false;

			for (auto& Pair : m_pModel->Get_AnimList())
			{
				if (bSearch)
				{
					if (Pair.first.find(strSearch) == string::npos)
						continue;
				}

				const bool bSelected = szMonsterAnimName == Pair.first;
				if (bSelected)
					ImGui::SetItemDefaultFocus();

				if (ImGui::Selectable(Pair.first.c_str(), bSelected))
					szMonsterAnimName = Pair.first;
			}
			ImGui::EndListBox();
		}

		if ("" != szMonsterAnimName)
		{
			CAnimation* pAnim = m_pModel->Get_AnimList()[szMonsterAnimName];
			if (pAnim != nullptr)
				pAnim->Imgui_RenderProperty();
		}

	}
}

HRESULT CShop_NPC::SetUp_Components(void* pArg)
{
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRenderer));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_NPC", L"Model", (CComponent**)&m_pModel));

	m_pModel->FindMaterial(L"Proto_MtrlAnim_Empty")->SetActive(false);
	/*if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		if (json.contains("Model"))
		{
			string ProtoModel = json["Model"];
			m_ModelName = CGameUtils::s2ws(ProtoModel);
			FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_ModelName.c_str(), m_ModelName.c_str(),
				(CComponent**)&m_pModel));
		}
	}*/

	return S_OK;
}

HRESULT CShop_NPC::SetUp_Event()
{
	return S_OK;
}

HRESULT CShop_NPC::SetUp_Sound()
{
	return S_OK;
}

void CShop_NPC::Update_Animation(_double TimeDelta)
{
    if (m_pModel->GetPlayAnimation()->IsFinished())
    {
        m_pModel->GetPlayAnimation()->Reset();
        m_fLerpTime = 0.f;
    }

    if (m_fLerpTime < m_fDuration)
    {
        m_pModel->GetPlayAnimation()->Update_Bones(TimeDelta, EAnimUpdateType::BLEND, m_fLerpTime / m_fDuration);
        m_fLerpTime += (_float)TimeDelta;
    }
    else
    {
        m_pModel->GetPlayAnimation()->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
    }

    m_pModel->Compute_CombindTransformationMatrix();

    _vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix());
    m_pTransformCom->LocalMove(vLocal);
}

_bool CShop_NPC::DistanceCheck()
{
	_matrix PlayerMatrix = CPlayerInfoManager::GetInstance()->Get_PlayerWorldMatrix();

	_vector vPlayerPos = PlayerMatrix.r[3];

	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	_vector vDir = vMyPos - vPlayerPos;

	_float fDistance = XMVectorGetX(XMVector3LengthEst(vDir));

	if (fDistance <= 4.f)
		return true;

	else return false;

}

CShop_NPC* CShop_NPC::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShop_NPC* pInstance = new CShop_NPC(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CShop_NPC");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CShop_NPC::Clone(void* pArg)
{
	CShop_NPC* pInstance = new CShop_NPC(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CShop_NPC");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CShop_NPC::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pModel);
}
