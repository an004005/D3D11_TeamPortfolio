#include "stdafx.h"
#include "..\public\PostProcess.h"
#include "GameInstance.h"

CPostProcess::CPostProcess(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CPostProcess::CPostProcess(const CPostProcess& rhs)
	: CGameObject(rhs)
	, m_iPriority(rhs.m_iPriority)
{
}


HRESULT CPostProcess::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRenderer));


	return S_OK;
}

void CPostProcess::Late_Tick(_double TimeDelta)
{
	if (m_bVisible)
		m_pRenderer->Add_RenderGroup(CRenderer::POSTPROCESS_VFX, this);
}

void CPostProcess::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();
	CShader::Imgui_RenderShaderParams(m_tParam);
}

void CPostProcess::Free()
{
	CGameObject::Free();
	Safe_Release(m_pRenderer);
}
