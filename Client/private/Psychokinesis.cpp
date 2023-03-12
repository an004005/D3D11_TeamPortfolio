#include "stdafx.h"
#include "..\public\Psychokinesis.h"


CPsychokinesis::CPsychokinesis(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CPsychokinesis::CPsychokinesis(const CPsychokinesis & rhs)
	: CComponent(rhs)
{
}

HRESULT CPsychokinesis::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CPsychokinesis::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	return S_OK;
}


void CPsychokinesis::Imgui_RenderProperty()
{
}

CPsychokinesis * CPsychokinesis::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPsychokinesis*		pInstance = new CPsychokinesis(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPsychokinesis");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CPsychokinesis::Clone(void* pArg)
{
	CPsychokinesis*		pInstance = new CPsychokinesis(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPsychokinesis");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPsychokinesis::Free()
{
	__super::Free();
}
