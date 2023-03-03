#include "stdafx.h"
#include "..\public\Boss1_AIController.h"


CBoss1_AIController::CBoss1_AIController(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAIController(pDevice, pContext)
{
}

CBoss1_AIController::CBoss1_AIController(const CBoss1_AIController& rhs)
	: CAIController(rhs)
{
}

void CBoss1_AIController::Tick(_double TimeDelta)
{
	CAIController::Tick(TimeDelta);
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
