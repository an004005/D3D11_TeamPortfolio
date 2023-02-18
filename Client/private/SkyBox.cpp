#include "stdafx.h"
#include "..\public\SkyBox.h"
#include <VIBuffer_Sphere.h>
#include "GameInstance.h"
#include "Model.h"

CSkyBox::CSkyBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CSkyBox::CSkyBox(const CSkyBox& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkyBox::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CSkyBox::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	/* For.Com_Renderer */
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_VIBuffer_Sphere"), TEXT("Com_Sphere"),
		(CComponent**)&m_pBuffer));

	// FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("../Bin/Resources/Meshes/Valorant/Texture/Skybox_M0_VeniceSky_DF.dds"), TEXT("SkyTexture"),
		// (CComponent**)&m_pTexture));

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorSkyBox"), TEXT("SkyBox_Shader"),
		(CComponent**)&m_pShader));


	return S_OK;
}

void CSkyBox::Late_Tick(_double TimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CSkyBox::Render()
{
	// FAILED_CHECK(m_pShader->Set_Matrix("g_WorldMatrix", &_float4x4::Identity));
	FAILED_CHECK(m_pShader->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShader->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));
	FAILED_CHECK(m_pTexture->Bind_ShaderResource(m_pShader, "g_Diffuse"));

	FAILED_CHECK(m_pShader->Begin(0));
	FAILED_CHECK(m_pBuffer->Render());

	return S_OK;
}

CSkyBox* CSkyBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
 	CSkyBox*		pInstance = new CSkyBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSkyBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSkyBox::Clone(void* pArg)
{
	CSkyBox*		pInstance = new CSkyBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSkyBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSkyBox::Free()
{
	__super::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
}
