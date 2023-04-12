#include "stdafx.h"
#include "Consumption_Item.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Animation.h"
#include "Model.h"
#include "ImguiUtils.h"
#include "JsonStorage.h"
#include "JsonLib.h"
#include "GameUtils.h"
#include "PhysX_Manager.h"
#include "Material.h"

#include "Item_Manager.h"

CConsumption_Item::CConsumption_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CConsumption_Item::CConsumption_Item(const CConsumption_Item& rhs)
    : CGameObject(rhs)
{
}

HRESULT CConsumption_Item::Initialize_Prototype()
{
    return CGameObject::Initialize_Prototype();
}

HRESULT CConsumption_Item::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(SetUp_Components(pArg)))
        return E_FAIL;

   // 이 아이템이 무엇인가를 Item_Manager로부터 Random하게 받는다.
    for (auto& iter : CItem_Manager::GetInstance()->Get_ItmeInfo())
    {
        if (iter.second.eType == CItem_Manager::MAINITEM::BATTLE)
        {
            m_vecItemName.push_back(iter.first);
        }
    }

    return S_OK;
}

void CConsumption_Item::BeginTick()
{
    __super::BeginTick();
    m_pModel->SetPlayAnimation("AS_co0700_001_wait");

    m_strName = m_vecItemName[m_eItemType];

    _int ia = 0;
}

void CConsumption_Item::Tick(_double TimeDelta)
{
    __super::Tick(TimeDelta);
    m_pModel->Tick(TimeDelta);
    m_pModel->Play_Animation(TimeDelta);

    if (!m_bOverlapCheck && m_pModel->Find_Animation("AS_co0700_001_wait")->IsFinished())
    {
        m_pModel->Find_Animation("AS_co0700_001_wait")->Reset();
    }

    //Update_Animation(TimeDelta);
    if(!m_bGetItem)
         PlayerOverlapCheck();
}

void CConsumption_Item::Late_Tick(_double TimeDelta)
{
    __super::Late_Tick(TimeDelta);

    if (m_bOverlapCheck && !m_bGetItem)
    {      
        // Item_Manager에게 정보 전달
        CItem_Manager::GetInstance()->Set_ItemCount(m_strName, 1);                
        m_pModel->SetPlayAnimation("AS_co0700_002_get");
        m_bGetItem = true;      
        // ★ 애니메이션 교체(Get) -> 애니메이션에 Add_EventCaller로 삭제 적용
    }

    if (m_pModel->GetPlayAnimation() == m_pModel->Find_Animation("AS_co0700_002_get") && 
        m_pModel->Find_Animation("AS_co0700_002_get")->IsFinished())
    {
        m_bDelete = true;
    }

    if (m_bVisible && (nullptr != m_pRenderer))
    {
        m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND_TOON, this);
        m_pRenderer->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
    }
}

void CConsumption_Item::AfterPhysX()
{
    __super::AfterPhysX();
}

HRESULT CConsumption_Item::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    m_pModel->Render(m_pTransformCom);

    return S_OK;
}

HRESULT CConsumption_Item::Render_ShadowDepth()
{
    m_pModel->Render_ShadowDepth(m_pTransformCom);
    return S_OK;
}

void CConsumption_Item::Imgui_RenderProperty()
{
    __super::Imgui_RenderProperty();

    static array<const char*, ITEMTYPE_END> ItemTypeNames{
        "Solo_Small", "Solo_Mid", "Solo_Big", "Party_Small", "Party_Mid", "Party_Big", "SAS_Full"
    };
    if (ImGui::BeginCombo("Item Name Seletor", ItemTypeNames[m_eItemType]))
    {
        for (int i = 0; i < ITEMTYPE_END; ++i)
        {
            const bool bSelected = false;
            if (ImGui::Selectable(ItemTypeNames[i], bSelected))
                m_eItemType = static_cast<ITEMTYPE>(i);
        }
        ImGui::EndCombo();
    }
#ifdef _DEBUG
    if (ImGui::Button("Item Name Decision"))
    {
        if (MessageBox(NULL, L"Really this Type Save?", L"System Message", MB_YESNO) == IDYES)
        {
            for (int i = 0; i < ITEMTYPE_END; ++i)
            {
                if (m_eItemType == i)
                {
                    m_strName = m_vecItemName[i];
                }
            }
        }       
    }
#endif
}

void CConsumption_Item::SaveToJson(Json& json)
{
    CGameObject::SaveToJson(json); 
    for (int i = 0; i < ITEMTYPE_END; ++i)
    {
        if (m_eItemType == i)
        {
            json["ItemType"] = m_eItemType;
        }
    }   
}

void CConsumption_Item::LoadFromJson(const Json& json)
{
    CGameObject::LoadFromJson(json);    
    for (int i = 0; i < ITEMTYPE_END; ++i)
    {
        if (json["ItemType"] == i)
        {
            m_eItemType = json["ItemType"];           
        }
    }
}

void CConsumption_Item::Update_Animation(_double TimeDelta)
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

void CConsumption_Item::PlayerOverlapCheck()
{
    physx::PxOverlapHit hitBuffer[5];
    physx::PxOverlapBuffer overlapOut(hitBuffer, 5);

    SphereOverlapParams tParams;
    tParams.overlapOut = &overlapOut;
    tParams.fRadius = 0.7f;
    tParams.vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
    tParams.fVisibleTime = 1.f;
    tParams.iTargetType = CTB_PLAYER;
    tParams.queryFlags = physx::PxQueryFlag::eDYNAMIC;

    if (CGameInstance::GetInstance()->OverlapSphere(tParams))
    {
        m_bOverlapCheck = true;
    }
    else return;
}

HRESULT CConsumption_Item::SetUp_Components(void* pArg)
{
    FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
        (CComponent**)&m_pRenderer));

    FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_ConsumptionItem", L"Model", (CComponent**)&m_pModel));

    return S_OK;
}

CConsumption_Item* CConsumption_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CConsumption_Item* pInstance = new CConsumption_Item(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CConsumption_Item");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

CGameObject* CConsumption_Item::Clone(void* pArg)
{
    CConsumption_Item* pInstance = new CConsumption_Item(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CConsumption_Item");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CConsumption_Item::Free()
{
    __super::Free();

    Safe_Release(m_pRenderer);
    Safe_Release(m_pModel);
}
