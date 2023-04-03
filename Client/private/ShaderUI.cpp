#include "stdafx.h"
#include "..\public\ShaderUI.h"
#include "GameInstance.h"

CShaderUI::CShaderUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CShaderUI::CShaderUI(const CShaderUI& rhs)
	: CUI(rhs)
{
}

HRESULT CShaderUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShaderUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CShaderUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

CShaderUI * CShaderUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CShaderUI*		pInstance = new CShaderUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CShaderUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CShaderUI::Clone(void * pArg)
{
	CShaderUI*		pInstance = new CShaderUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CShaderUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CShaderUI::Free()
{
	__super::Free();

}
