#include "stdafx.h"
#include "..\public\LambdaRenderObject.h"


CLambdaRenderObject::CLambdaRenderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CLambdaRenderObject::CLambdaRenderObject(const CLambdaRenderObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CLambdaRenderObject::Render()
{
	if (m_RenderFunction)
		m_RenderFunction();
	return S_OK;
}

void CLambdaRenderObject::SetRenderFunction(const std::function<void()>& RenderFunction)
{
	m_RenderFunction = RenderFunction;
}

CLambdaRenderObject* CLambdaRenderObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
 	CLambdaRenderObject*		pInstance = new CLambdaRenderObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLambdaRenderObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CLambdaRenderObject::Clone(void* pArg)
{
	CLambdaRenderObject*		pInstance = new CLambdaRenderObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLambdaRenderObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLambdaRenderObject::Free()
{
	CGameObject::Free();
}
