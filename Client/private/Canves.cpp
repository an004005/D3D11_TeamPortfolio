#include "stdafx.h"
#include "..\public\Canves.h"
#include "GameInstance.h"

CCanves::CCanves(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CCanves::CCanves(const CCanves & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCanves::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CCanves::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;	

	m_fSizeX = (_float)g_iWinSizeX;
	m_fSizeY = (_float)g_iWinSizeY;

	m_fX = m_fSizeX * 0.5f;
	m_fY = m_fSizeY * 0.5f;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, 
		XMVectorSet(m_fX - (_float)g_iWinSizeX * 0.5f, -m_fY + (_float)g_iWinSizeY * 0.5f, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CCanves::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CCanves::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if(nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CCanves::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CCanves::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTexUI"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer_Rect"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_Logo"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanves::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

CCanves * CCanves::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CCanves*		pInstance = new CCanves(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanves");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCanves::Clone(void * pArg)
{
	CCanves*		pInstance = new CCanves(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanves");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanves::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
