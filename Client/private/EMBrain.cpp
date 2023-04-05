#include "stdafx.h"
#include "EMBrain.h"
#include "GameInstance.h"

CEMBrain::CEMBrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject(pDevice, pContext)
{
}
CEMBrain::CEMBrain(const CEMBrain& rhs)
    :CGameObject(rhs)
{
}

HRESULT CEMBrain::Initialize(void* pArg)
{
    FAILED_CHECK(__super::Initialize(pArg));

    FAILED_CHECK(SetUpComponents());
    return S_OK;
}

void CEMBrain::BeginTick()
{
    __super::BeginTick();
}

void CEMBrain::Tick(_double TimeDelta)
{
    __super::Tick(TimeDelta);
}

void CEMBrain::Late_Tick(_double TimeDelta)
{
    __super::Late_Tick(TimeDelta);
    FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this));
}

HRESULT CEMBrain::Render()
{
    FAILED_CHECK(__super::Render());

    m_pModelCom->Render(m_pTransformCom);

    return S_OK;

}

void CEMBrain::Imgui_RenderProperty()
{
}

HRESULT CEMBrain::Render_ShadowDepth()
{
    m_pModelCom->Render_ShadowDepth(m_pTransformCom);
    return S_OK;
}

HRESULT CEMBrain::SetUpComponents()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();

    FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
        (CComponent**)&m_pRendererCom));

    FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Prototype_Model_co2001", TEXT("ModelCom"),
        (CComponent**)&m_pModelCom));

    return S_OK;
}

CEMBrain* CEMBrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEMBrain* pInstance = new CEMBrain(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CEMBrain");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CEMBrain::Clone(void* pArg)
{
    CEMBrain* pInstance = new CEMBrain(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CEMBrain");
        Safe_Release(pInstance);
    }
    return pInstance;
}


void CEMBrain::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pRendererCom);
}
