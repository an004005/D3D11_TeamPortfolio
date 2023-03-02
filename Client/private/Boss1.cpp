#include "stdafx.h"
#include "..\public\Boss1.h"
#include "Model.h"
#include "JsonStorage.h"
#include "Boss1_AnimationInstance.h"
#include "FSMComponent.h"
#include "GameInstance.h"
#include "Controller.h"
#include "TestCamera.h"

CBoss1::CBoss1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster(pDevice, pContext)
{
}

CBoss1::CBoss1(const CBoss1& rhs)
	: CMonster(rhs)
{
}

HRESULT CBoss1::Initialize_Prototype()
{
	return CMonster::Initialize_Prototype();
}

HRESULT CBoss1::Initialize(void* pArg)
{
	FAILED_CHECK(CMonster::Initialize(pArg));

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"MonsterBoss1", L"Model", (CComponent**)&m_pModelCom));
	m_pModelCom->ActivateLocalRotation(true);

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Component_LocalController"), TEXT("Com_Controller"), (CComponent**)&m_pController));
	// FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Component_TestCamera"), TEXT("Com_Controller"), (CComponent**)&m_pController));

	m_pCam = dynamic_cast<CTestCamera*>(CGameInstance::GetInstance()->Add_Camera("TestBossCam", LEVEL_NOW, L"Camera", L"Prototype_TestCamera"));
	Safe_AddRef(m_pCam);

	m_pTransformCom->SetRotPerSec(XMConvertToRadians(100.f));

	m_pASM = CBoss1_AnimationInstance::Create(m_pModelCom, this);
	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
		.Build();

	return S_OK;
}

void CBoss1::BeginTick()
{
	CMonster::BeginTick();
	m_pModelCom->SetPlayAnimation("AS_em0300_101_AL_wait01");
	m_pCam->SetMainCamera();
}

void CBoss1::Tick(_double TimeDelta)
{
	CMonster::Tick(TimeDelta);

	if (m_pCam->IsMainCamera())
		m_pController->Tick(TimeDelta);
	else
		m_pController->Invalidate();

	m_pCam->SetMouseAxis(m_pController->GetMouseAxis());


	m_vMoveAxis = m_pController->GetMoveAxis();

	m_pFSM->Tick(TimeDelta);
	m_pASM->Tick(TimeDelta);

	const _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	m_pTransformCom->LocalMove(m_pModelCom->GetLocalMove(WorldMatrix));


	_vector vCamLook = XMVectorSetY(m_pCam->GetTransform()->Get_State(CTransform::STATE_LOOK), 0.f) ;
	_vector vLookAt = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + XMVector3Normalize(vCamLook) * 5.f;
	m_fTurnRamain = m_pTransformCom->LookAt_SmoothYaw(vLookAt, TimeDelta);
}

void CBoss1::Late_Tick(_double TimeDelta)
{
	CMonster::Late_Tick(TimeDelta);
	if (m_bVisible)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	m_pCam->AttachTo(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
}

HRESULT CBoss1::Render()
{
	m_pModelCom->Render(m_pTransformCom);
	return S_OK;
}

void CBoss1::Imgui_RenderProperty()
{
	CMonster::Imgui_RenderProperty();
	// m_pFSM->Imgui_RenderProperty();
	m_pASM->Imgui_RenderState();
}

CBoss1* CBoss1::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss1* pInstance = new CBoss1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBoss1");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBoss1::Clone(void* pArg)
{
	CBoss1*		pInstance = new CBoss1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss1");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoss1::Free()
{
	CMonster::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pFSM);
	Safe_Release(m_pASM);
	Safe_Release(m_pCam);
	Safe_Release(m_pController);
}
