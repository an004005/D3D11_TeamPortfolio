#include "stdafx.h"
#include "..\public\Boss1_AIController.h"
#include "Boss1.h"
#include "FSMComponent.h"


CBoss1_AIController::CBoss1_AIController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAIController(pDevice, pContext)
{
}

CBoss1_AIController::CBoss1_AIController(const CBoss1_AIController& rhs)
	: CAIController(rhs)
{
}

HRESULT CBoss1_AIController::Initialize(void* pArg)
{
	FAILED_CHECK(CAIController::Initialize(pArg));

	m_pFSM = CFSMComponentBuilder()
		.InitState("Start")
		.AddState("Start")
			.AddTransition("Start to Near", "Near")
				.Predicator([this]
				{
					return m_pCastedOwner->IsPlayingSocket() == false;
				})

		.AddState("Near")
			.Tick(this, &CBoss1_AIController::Tick_Near)
			.AddTransition("Near to Far", "Mid")
				.Predicator([this]
				{
					return m_fToTargetDistance > 5.f;
				})

		.AddState("Mid")
			.Tick(this, &CBoss1_AIController::Tick_Mid)
			.AddTransition("Mid to Near", "Near")
				.Predicator([this]
				{
					return m_fToTargetDistance <= 5.f;
				})
			.AddTransition("Mid to Far", "Far")
				.Predicator([this]
				{
					return m_fToTargetDistance > 15.f;
				})

		.AddState("Far")
			.Tick(this, &CBoss1_AIController::Tick_Far)
			.AddTransition("Far to Near", "Near")
				.Predicator([this]
				{
					return m_fToTargetDistance <= 15.f;
				})
		.Build();

	return S_OK;
}

void CBoss1_AIController::BeginTick()
{
	CAIController::BeginTick();
	m_pCastedOwner = dynamic_cast<CBoss1*>(TryGetOwner());
	Assert(m_pCastedOwner != nullptr);
}

void CBoss1_AIController::Tick(_double TimeDelta)
{
	CAIController::Tick(TimeDelta);

	if (m_pTarget == nullptr)
		return;

	m_fToTargetDistance = XMVectorGetX(XMVector3LengthEst(
		m_pTarget->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)
		- m_pCastedOwner->GetTransform()->Get_State(CTransform::STATE_TRANSLATION)));

	m_pFSM->Tick(TimeDelta);
}

void CBoss1_AIController::Tick_Near(_double TimeDelta)
{
	// 1. 스핀 공격, 플레이어 보고 때리기, 플레이어보고 뒤가기,

}

void CBoss1_AIController::Tick_Mid(_double TimeDelta)
{
	// 2. 게걸음, 물폭탄
}

void CBoss1_AIController::Tick_Far(_double TimeDelta)
{
	// 3. 게걸음, 물폭탄, 점프
}

void CBoss1_AIController::Free()
{
	CAIController::Free();
	Safe_Release(m_pFSM);
}

CComponent* CBoss1_AIController::Clone(void* pArg)
{
	CBoss1_AIController*		pInstance = new CBoss1_AIController(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBoss1_AIController");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBoss1_AIController* CBoss1_AIController::Create()
{
	CBoss1_AIController*		pInstance = new CBoss1_AIController(nullptr, nullptr);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBoss1_AIController");
		Safe_Release(pInstance);
	}
	return pInstance;
}
