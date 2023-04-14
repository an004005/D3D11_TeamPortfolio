#include "stdafx.h"
#include "EMBrain.h"
#include "GameInstance.h"
#include "VFX_Manager.h"

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

    //bc할때 생성하면, FSM에서 생성을 안하고 넘어가때가 있어서
    //여기서 어떤 함수든 호출이 되면 그때부터 랜더되게 함
    m_bVisible = false;
    return S_OK;
}

void CEMBrain::Late_Tick(_double TimeDelta)
{
    __super::Late_Tick(TimeDelta);

    if(m_bVisible)
        FAILED_CHECK(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this));
}

HRESULT CEMBrain::Render()
{
    FAILED_CHECK(__super::Render());
    m_pModelCom->Render(m_pTransformCom);

    return S_OK;

}

HRESULT CEMBrain::Render_ShadowDepth()
{
    m_pModelCom->Render_ShadowDepth(m_pTransformCom);
    return S_OK;
}

void CEMBrain::InitBC()
{
    m_bVisible = true;
    m_pDefaultParticle = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"em0110_Bug_Particle");
    assert(m_pDefaultParticle != nullptr);
    Safe_AddRef(m_pDefaultParticle);

    m_pDefaultParticle->Start_NoAttach(this, true);
}

void CEMBrain::BeginBC()
{
    //particle
    m_bVisible = true;

    m_pStartParticle = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"Monster_Brain_Crush_Enemy_Light_Particle");
    assert(m_pStartParticle != nullptr);
    Safe_AddRef(m_pStartParticle);

    m_pStartParticle->Start_NoAttach(this, true);

    //effects
    m_DistortionEffect = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"Brain_Crush_Distortion");
    assert(m_DistortionEffect != nullptr);
    Safe_AddRef(m_DistortionEffect);
    m_DistortionEffect->Start_NoAttach(this, true);

    //m_pLoopEffect = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"Brain_Crush_Destroy_Light_Loop");
    //assert(m_pLoopEffect != nullptr);
    //Safe_AddRef(m_pLoopEffect);
    //m_pLoopEffect->Start_NoAttach(this, true);
}

void CEMBrain::EndBC()
{
    m_bVisible = true;

    //Delete LoofEffect 
    if (m_pLoopEffect != nullptr)
    {
        m_pLoopEffect->SetDelete();
        Safe_Release(m_pLoopEffect);
        m_pLoopEffect = nullptr;
    }

    //Create Particle
    m_pDestroyParticle = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_MONSTER, L"Brain_Crush_Destroy_Light_Particle");
    assert(m_pDestroyParticle != nullptr);
    Safe_AddRef(m_pDestroyParticle);
    m_pDestroyParticle->Start_NoAttach(this, false);

   //Create Effect
    m_pDestroyEffect = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_MONSTER, L"Brain_Crush_Destroy_Light");
    assert(m_pDestroyEffect != nullptr);
    Safe_AddRef(m_pDestroyEffect);
    m_pDestroyEffect->Start_NoAttach(this, false);
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

    if (m_bCloned)
    {
        if (m_pDefaultParticle != nullptr)
        {
            m_pDefaultParticle->SetDelete();
            Safe_Release(m_pDefaultParticle);
            m_pDefaultParticle = nullptr;
        }

        if (m_DistortionEffect != nullptr)
        {
            m_DistortionEffect->SetDelete();
            Safe_Release(m_DistortionEffect);
            m_DistortionEffect = nullptr;
        }

        if (m_pStartParticle != nullptr)
        {
            m_pStartParticle->SetDelete();
            Safe_Release(m_pStartParticle);
            m_pStartParticle = nullptr;
        }

        if (m_pLoopEffect != nullptr)
        {
            m_pLoopEffect->SetDelete();
            Safe_Release(m_pLoopEffect);
            m_pLoopEffect = nullptr;
        }

        if (m_pDestroyParticle != nullptr)
        {
            m_pDestroyParticle->SetDelete();
            Safe_Release(m_pDestroyParticle);
            m_pDestroyParticle = nullptr;
        }

        if (m_pDestroyEffect != nullptr)
        {
            m_pDestroyEffect->SetDelete();
            Safe_Release(m_pDestroyEffect);
            m_pDestroyEffect = nullptr;
        }
    }
}
