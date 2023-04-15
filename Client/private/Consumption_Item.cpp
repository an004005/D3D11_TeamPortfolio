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
#include "GameManager.h"

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

    m_pModel = m_vecModelType.front();

   // 이 아이템이 무엇인가를 Item_Manager로부터 Random하게 받는다.
    if (!m_bItemModel)
    {
        for (auto& iter : CItem_Manager::GetInstance()->Get_ItmeInfo())
        {
            if (iter.second.eType == CItem_Manager::MAINITEM::BATTLE) // 소모품 이름
            {
                m_vecItemName.push_back(iter.first);
            }
            else if (iter.second.eType == CItem_Manager::MAINITEM::ETC) // 업적 이름
            {
                m_vecAchieveName.push_back(iter.first);
            }
        }
    }  
    else
    {
        int iABC = 12;
    }
    //m_pTransformCom->SetTransformDesc({ 18.f, 18.f }); 

    return S_OK;
}

void CConsumption_Item::BeginTick()
{
    __super::BeginTick();
    if (!m_bItemModel)
    {
        m_pModel->SetPlayAnimation("AS_co0700_001_wait");

        m_strName = m_vecItemName[m_eItemType];
        m_AchieveName = m_vecAchieveName[m_eAchieveType];
    } 
    else
    {

    }

    _int ia = 0;
}

void CConsumption_Item::Tick(_double TimeDelta)
{
    __super::Tick(TimeDelta);
    m_pModel->Tick(TimeDelta);

    if (!m_bItemModel)
    {
        m_pModel->Play_Animation(TimeDelta);

        if (!m_bOverlapCheck && m_pModel->Find_Animation("AS_co0700_001_wait")->IsFinished())
        {
            m_pModel->Find_Animation("AS_co0700_001_wait")->Reset();
        }

        //Update_Animation(TimeDelta);
        if (!m_bGetItem)
            PlayerOverlapCheck();
    }   
    else
    {
        if (!m_bGetItem)
            PlayerOverlapCheck();
    }
}

void CConsumption_Item::Late_Tick(_double TimeDelta)
{
    __super::Late_Tick(TimeDelta);

    if (!m_bItemModel)
    {
        if (m_bOverlapCheck && !m_bGetItem)  // Player가 Item을 먹었을 경우 Item_Manager에게 정보 전달
        {
            // 소모 아이템일 경우
            if (m_eTypeInfo == TYPE_USE)
            {
                CItem_Manager::GetInstance()->Set_ItemCount(m_strName, 1);
                // 획득한 아이템이 10개 이상 이라면 오른쪽에 획득 UI 를 띄우지 않는다.
                vector<pair<wstring, CItem_Manager::ITEMINFO>> vecItemInfo = CItem_Manager::GetInstance()->Get_ItmeInfo();
                auto iter = find_if(vecItemInfo.begin(), vecItemInfo.end(), [&](pair<wstring, CItem_Manager::ITEMINFO> pair) {
                    return pair.first == m_strName;
                    });

                if (iter != vecItemInfo.end())
                {
                    if (11 >= (*iter).second.iCount + 1)
                        CGameManager::GetInstance()->Set_AddlItem(m_strName);
                }
            }
            // 업적 아이템일 경우 
            else if (m_eTypeInfo == TYPE_ACHIEVE)
            {
                CItem_Manager::GetInstance()->Set_ItemCount(m_AchieveName, 1);

                vector<pair<wstring, CItem_Manager::ITEMINFO>> vecItemInfo = CItem_Manager::GetInstance()->Get_ItmeInfo();
                auto iter = find_if(vecItemInfo.begin(), vecItemInfo.end(), [&](pair<wstring, CItem_Manager::ITEMINFO> pair) {
                    return pair.first == m_AchieveName;
                    });

                if (iter != vecItemInfo.end())
                {
                    CGameManager::GetInstance()->Set_AddlItem(m_AchieveName);
                }
            }
            // ★ 애니메이션 교체(Get)
            m_pModel->SetPlayAnimation("AS_co0700_002_get");
            m_bGetItem = true;
        }

        // 아이템 먹었으면 애니메이션 실행 후 삭제
        if (m_pModel->GetPlayAnimation() == m_pModel->Find_Animation("AS_co0700_002_get") &&
            m_pModel->Find_Animation("AS_co0700_002_get")->IsFinished())
        {
            m_bDelete = true;
        }
    }
    else
    {
        if (m_bOverlapCheck && !m_bGetItem)
        {
            m_bDelete = true;
        }
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

    ImGui::Text("Item or Flower");
    
    if (ImGui::RadioButton("Item", m_eModelType == MODEL_ITEM))
    {
        m_bItemModel = false;
        m_eModelType = MODEL_ITEM;
    }    
    
    if (ImGui::RadioButton("Flower", m_eModelType == MODEL_FLOWER))
    {
        m_bItemModel = true;
        m_eModelType = MODEL_FLOWER;
    }
    
    ImGui::Text("Root Item Type");
    if (ImGui::RadioButton("Type Use", m_eTypeInfo == TYPE_USE))
    {
        m_eTypeInfo = TYPE_USE;
    }
    if (ImGui::RadioButton("Type Achieve", m_eTypeInfo == TYPE_ACHIEVE))
    {
        m_eTypeInfo = TYPE_ACHIEVE;
    }
    
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

    static array<const char*, AC_END> AchieveTypeNames{
        "Achiveve_KKB", "Achiveve_OSH", "Achiveve_JIB", "Achiveve_JJH", "Achiveve_PJW", "Achiveve_AJH"
    };
    if (ImGui::BeginCombo("Achieve Name Seletor", AchieveTypeNames[m_eAchieveType]))
    {
        for (int i = 0; i < AC_END; ++i)
        {
            const bool bSelectAC = false;
            if (ImGui::Selectable(AchieveTypeNames[i], bSelectAC))
                m_eAchieveType = static_cast<ACHIEVETYPE>(i);
        }
        ImGui::EndCombo();
    }
#ifdef _DEBUG
    if (ImGui::Button("Item Name Decision"))
    {
        if (MessageBox(NULL, L"Really this Setting Save?", L"System Message", MB_YESNO) == IDYES)
        {
            if (m_eTypeInfo == TYPE_USE)
            {
                for (int i = 0; i < ITEMTYPE_END; ++i)
                {
                    if (m_eItemType == i)
                    {
                        m_strName = m_vecItemName[i];
                    }
                }
            }
            else if (m_eTypeInfo == TYPE_ACHIEVE)
            {
                for (int i = 0; i < AC_END; ++i)
                {
                    if (m_eAchieveType == i)
                    {
                        m_AchieveName = m_vecAchieveName[i];
                    }
                }
            }            
        }       
    }
#endif
}

void CConsumption_Item::SaveToJson(Json& json)
{
    CGameObject::SaveToJson(json); 
    json["ModelTypeCheck"] = m_bItemModel;

    if (!m_bItemModel)
    {
        json["RootType"] = m_eTypeInfo;
        if (m_eTypeInfo == TYPE_USE)
        {
            for (int i = 0; i < ITEMTYPE_END; ++i)
            {
                if (m_eItemType == i)
                {
                    json["ItemType"] = m_eItemType;
                }
            }
        }
        else if (m_eTypeInfo == TYPE_ACHIEVE)
        {
            for (int i = 0; i < AC_END; ++i)
            {
                if (m_eAchieveType == i)
                {
                    json["AchieveType"] = m_eAchieveType;
                }
            }
        }
    }

    json["ModelTypeEnum"] = m_eModelType;
}

void CConsumption_Item::LoadFromJson(const Json& json)
{
    CGameObject::LoadFromJson(json);  

    if (json.contains("ModelTypeCheck"))
        m_bItemModel = json["ModelTypeCheck"];
    else
        m_bItemModel = false;

    if (!m_bItemModel)
    {
        if (json.contains("RootType"))
            m_eTypeInfo = json["RootType"];

        if (m_eTypeInfo == TYPE_USE)
        {
            for (int i = 0; i < ITEMTYPE_END; ++i)
            {
                if (json["ItemType"] == i)
                {
                    m_eItemType = json["ItemType"];
                }
            }
        }

        else if (m_eTypeInfo == TYPE_ACHIEVE)
        {
            for (int i = 0; i < AC_END; ++i)
            {
                if (json["AchieveType"] == i)
                {
                    m_eAchieveType = json["AchieveType"];
                }
            }
        }
    }   

    if (json.contains("ModelTypeEnum"))
        m_eModelType = json["ModelTypeEnum"];
    else
        m_eModelType = MODEL_ITEM;
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

    CModel* pModel = nullptr;
 
    if (m_eModelType == MODEL_ITEM)
    {
        FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_ConsumptionItem", L"Model", (CComponent**)&pModel));
        m_vecModelType.push_back(pModel);
    }
   
    else if (m_eModelType == MODEL_FLOWER)
    {
        FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_Flower", L"Model", (CComponent**)&pModel));
        m_vecModelType.push_back(pModel);
    }  

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

    for (auto iter : m_vecModelType)
    {
        Safe_Release(iter);
    }
    m_vecModelType.clear();
    
    Safe_Release(m_pRenderer);
    //Safe_Release(m_pModel);   
}
