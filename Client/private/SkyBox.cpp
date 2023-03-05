#include "stdafx.h"
#include "..\public\SkyBox.h"
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

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("../Bin/Resources/Model/StaticModel/Sky/SkySphere.static_model"), TEXT("Com_Model"), (CComponent**)&m_pModelCom));

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorSkyBox"), TEXT("SkyBox_Shader"),
		(CComponent**)&m_pShader));

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("../Bin/Resources/99.DefaultSky/T_Sky_01_A.png"), TEXT("Texture"),
		(CComponent**)&m_pTexture));
	

	return S_OK;
}

void CSkyBox::Late_Tick(_double TimeDelta)
{
	return;
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT CSkyBox::Render()
{

	FAILED_CHECK(m_pShader->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShader->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));
	FAILED_CHECK(m_pTexture->Bind_ShaderResource(m_pShader, "g_Diffuse"));

	FAILED_CHECK(m_pShader->Begin(0));
	m_pModelCom->Render(m_pTransformCom);


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
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
}
