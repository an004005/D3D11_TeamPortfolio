#include "stdafx.h"
#include "..\public\Renderer.h"
#include "GameObject.h"
#include "Target_Manager.h"
#include "Light_Manager.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "PipeLine.h"
#include "HDR.h"
#include "GameInstance.h"
#include "PostProcess.h"
#include "Graphic_Device.h"
#include "RenderTarget.h"
#include "Light_Manager.h"
#include "PhysX_Manager.h"


CRenderer::CRenderer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);

#ifdef _DEBUG
	char* pValue2 = NULL;
	size_t len = NULL;
	_dupenv_s(&pValue2, &len, "SHADER");

	if (pValue2 != nullptr)
		m_bVisibleTargets = true;
	free(pValue2);
#endif
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pGameObject)
{
	if (nullptr == pGameObject || 
		eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Add_DebugRenderGroup(CComponent * pComponent)
{
	if (nullptr == pComponent)
		return E_FAIL;
#ifdef _DEBUG
	m_DebugObject.push_back(pComponent);

	Safe_AddRef(pComponent);
#endif
	
	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
	_float fGamma = CHDR::GetInstance()->GetGamma();
	m_pShader->Set_RawValue("g_Gamma", &fGamma, sizeof(_float));

	if (FAILED(Render_ShadowDepth()))
		return E_FAIL;
	if (FAILED(Render_NonAlphaBlend()))
		return E_FAIL;
	if (FAILED(Render_LightAcc()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_HDR"))))
		return E_FAIL;

	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(Render_Blend()))
		return E_FAIL;
	if (FAILED(Render_NonLight()))
		return E_FAIL;
	if (FAILED(Render_AlphaBlend()))
		return E_FAIL;
	if (FAILED(Render_Outline()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_HDR"))))
		return E_FAIL;

	Render_HDR();

	if (FAILED(Render_PostProcess()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG

	if (FAILED(Render_DebugObject()))
		return E_FAIL;

	if (nullptr != m_pTarget_Manager && m_bVisibleTargets)
	{
		m_pTarget_Manager->Render_Debug(TEXT("MRT_Deferred"));
		m_pTarget_Manager->Render_Debug(TEXT("MRT_LightAcc"));
		m_pTarget_Manager->Render_Debug(TEXT("MRT_LightDepth"));
		m_pTarget_Manager->Render_Debug(TEXT("MRT_ToonDeferred"));
		m_pTarget_Manager->Render_Debug(TEXT("MRT_HDR"));

	}
#endif

	return S_OK;
}

void CRenderer::Clear()
{
	for (auto& pComponent : m_DebugObject)
		Safe_Release(pComponent);
	m_DebugObject.clear();

	for (_uint i = 0; i < RENDER_END; ++i)
	{
		for (auto& pGameObject : m_RenderObjects[i])
			Safe_Release(pGameObject);

		m_RenderObjects[i].clear();
	}
}

HRESULT CRenderer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	D3D11_VIEWPORT			ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof ViewportDesc);

	_uint			iNumViewports = 1;

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	/* 렌터타겟들을 생성하낟. */
	
	/* For.Target_Diffuse */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.3f, 0.3f, 0.3f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Flag"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;
	/* For.Target_Normal */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, &_float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	/* For.Target_Depth */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.f, 1.f, 0.f, 0.f))))
		return E_FAIL;
	/* For.Target_RMA */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_RMA"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(1.f, 0.f, 1.f, 0.f))))
		return E_FAIL;

	/* For.Target_AMB */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_AMB"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* For.Target_CTL */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_CTL"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	FAILED_CHECK(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_OutlineFlag"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, &_float4(0.f, 0.f, 0.f, 0.f)));


	/* For.Target_Diffuse_Copy */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Diffuse_Copy"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;
	/* For.Target_Normal_Copy */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Normal_Copy"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, &_float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	/* For.Target_Depth_Copy */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Depth_Copy"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(0.f, 1.f, 0.f, 1.f))))
		return E_FAIL;	

	/* For.Target_Shade */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, &_float4(0.0f, 0.0f, 0.0f, 1.f))))
		return E_FAIL;

	/* For.Target_Specular */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_Specular"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, &_float4(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;

	/* For.Target_HDR */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_HDR"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_FLOAT, &_float4(0.8f, 0.8f, 0.8f, 0.f))))
		return E_FAIL;

	/* For.Target_LDR */
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_LDR1"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.5f, 0.5f, 0.5f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, TEXT("Target_LDR2"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, &_float4(0.5f, 0.5f, 0.5f, 1.f))))
		return E_FAIL;

	/* For.MRT_Deferred */ /* 디퍼드 렌더링(빛)을 수행하기위해 필요한 데이터들을 저장한 렌더타겟들. */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_RMA"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_OutlineFlag"))))
		return E_FAIL;

	/* for.MRT_ToonDeferred*/
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_ToonDeferred"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_ToonDeferred"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_ToonDeferred"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_ToonDeferred"), TEXT("Target_AMB"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_ToonDeferred"), TEXT("Target_CTL"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_ToonDeferred"), TEXT("Target_OutlineFlag"))))
		return E_FAIL;


	/* For.MRT_LightAcc */ /* 빛 연산의 결과를 저장할 렌더타겟들.  */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	m_pEnv = dynamic_cast<CTexture*>(CGameInstance::GetInstance()->Clone_Component(L"../Bin/Resources/Textures/DiffuseEnv.dds"));
	m_pEnv2 = dynamic_cast<CTexture*>(CGameInstance::GetInstance()->Clone_Component(L"../Bin/Resources/Textures/BlueSkyIradiance.dds"));


	// HDR 텍스쳐 렌더링용
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HDR"), TEXT("Target_HDR"))))
		return E_FAIL;

	// For Effect
	// if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Flag"))))
	// 	return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_HDR"), TEXT("Target_Flag"))))
		return E_FAIL;
	// ~For Effect


	// Ready_ShadowDepthResources(8192, 8192);
	Ready_ShadowDepthResources(128, 128);

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pShader_PostProcess = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_PostProcessVFX.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements);
	if (m_pShader_PostProcess == nullptr)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));


#ifdef _DEBUG
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Diffuse"), 100.0f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Normal"), 100.0f, 300.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Depth"), 100.0f, 500.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_ShadowDepth"), 100.0f, 700.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Shade"), 300.0f, 100.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_Specular"), 300.0f, 300.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_AMB"), 300.0f, 500.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_Debug(TEXT("Target_CTL"), 300.0f, 700.f, 200.f, 200.f)))
		return E_FAIL;

	FAILED_CHECK(m_pTarget_Manager->Ready_Debug(TEXT("Target_Flag"), 500.0f, 100.f, 200.f, 200.f));


	FAILED_CHECK(m_pTarget_Manager->Ready_Debug(TEXT("Target_OutlineFlag"), 500.0f, 300.f, 200.f, 200.f));


#endif


	return S_OK;
}

HRESULT CRenderer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_ShadowDepthResources(_uint iWidth, _uint iHeight)
{
	// For.Target_ShadowDepth
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(m_pDevice, m_pContext, L"Target_ShadowDepth", iWidth, iHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, &_float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	m_pTarget_Manager->GetTarget(L"Target_ShadowDepth")->SetResizable(false);

	// For.MRT_ShadowDepth(Shadow)
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightDepth", L"Target_ShadowDepth")))
		return E_FAIL;

	// shadow depth용 depthStencil view 생성
	{
		ID3D11Texture2D*		pDepthStencilTexture = nullptr;
	
		D3D11_TEXTURE2D_DESC	TextureDesc;
		ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		TextureDesc.Width = iWidth;
		TextureDesc.Height = iHeight;
		TextureDesc.MipLevels = 1;
		TextureDesc.ArraySize = 1;
		TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.SampleDesc.Count = 1;

		TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		TextureDesc.CPUAccessFlags = 0;
		TextureDesc.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
			return E_FAIL;

		if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pShadowDepthStencilView)))
			return E_FAIL;

		Safe_Release(pDepthStencilTexture);
	}

	return S_OK;
}

void CRenderer::Imgui_RenderOtherWindow()
{
#ifdef _DEBUG
	if (ImGui::Button("Recompile deferred"))
	{
		m_pShader->ReCompileShader();
	}
	if (ImGui::Button("Recompile postprocess"))
	{
		m_pShader_PostProcess->ReCompileShader();
	}
	ImGui::Checkbox("Visible Targets", &m_bVisibleTargets);
#endif
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_ShadowDepth()
{
	m_pContext->ClearDepthStencilView(m_pShadowDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	D3D11_VIEWPORT			ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof ViewportDesc);

	_uint			iNumViewports = 1;
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	D3D11_VIEWPORT			DepthViewportDesc = ViewportDesc;
	DepthViewportDesc.Width = (_float)8192;
	DepthViewportDesc.Height = (_float)8192;
	m_pContext->RSSetViewports(iNumViewports, &DepthViewportDesc);

	if (FAILED(m_pTarget_Manager->Begin_MRT_WithDepthStencil(m_pContext, TEXT("MRT_LightDepth"), m_pShadowDepthStencilView)))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_SHADOWDEPTH])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_ShadowDepth();
		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_SHADOWDEPTH].clear();


	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_LightDepth"))))
		return E_FAIL;

	m_pContext->RSSetViewports(iNumViewports, &ViewportDesc);

	return S_OK;
}

HRESULT CRenderer::Render_NonAlphaBlend()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	/* Diffuse + Normal */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_Deferred"))))
		return E_FAIL;

	{
		for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHABLEND])
		{
			if (nullptr != pGameObject)
				pGameObject->Render();

			Safe_Release(pGameObject);
		}

		m_RenderObjects[RENDER_NONALPHABLEND].clear();
	}

	{
		// Decal Rendering 하기위해서 기존 diffuse, normal, depth를 복사해 SRV로 사용한다.
		m_pTarget_Manager->CopyRenderTarget(m_pContext, L"Target_Diffuse_Copy", L"Target_Diffuse");
		m_pTarget_Manager->CopyRenderTarget(m_pContext, L"Target_Normal_Copy", L"Target_Normal");
		m_pTarget_Manager->CopyRenderTarget(m_pContext, L"Target_Depth_Copy", L"Target_Depth");

		for (auto& pGameObject : m_RenderObjects[RENDER_DECAL])
		{
			if (nullptr != pGameObject)
				pGameObject->Render();

			Safe_Release(pGameObject);
		}

		m_RenderObjects[RENDER_DECAL].clear();
	}

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_Deferred"))))
		return E_FAIL;

	// toon deferred
	m_pTarget_Manager->GetTarget(TEXT("Target_Diffuse"))->SetIgnoreClearOnce();
	m_pTarget_Manager->GetTarget(TEXT("Target_Normal"))->SetIgnoreClearOnce();
	m_pTarget_Manager->GetTarget(TEXT("Target_Depth"))->SetIgnoreClearOnce();
	m_pTarget_Manager->GetTarget(TEXT("Target_OutlineFlag"))->SetIgnoreClearOnce();

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_ToonDeferred"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHABLEND_TOON])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_NONALPHABLEND_TOON].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_ToonDeferred"))))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	/* Shade */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pContext, TEXT("MRT_LightAcc"))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ShaderResourceView("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse")))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ShaderResourceView("g_NormalTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Normal")))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ShaderResourceView("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth")))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ShaderResourceView("g_RMATexture", m_pTarget_Manager->Get_SRV(TEXT("Target_RMA")))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ShaderResourceView("g_AMBTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_AMB")))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ShaderResourceView("g_CTLTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_CTL")))))
		return E_FAIL;
	
	CPipeLine* pPipeLine = CPipeLine::GetInstance();

	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrixInv", &pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrixInv", &pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_RawValue("g_vCamPosition", &pPipeLine->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;


	/* 빛 갯수만큼 사각형 버퍼(셰이드타겟의 전체 픽셀을 갱신할 수 있는 사이즈로 그려지는 정점버퍼. )를 그린다. */
	m_pLight_Manager->Render_Light(m_pVIBuffer, m_pShader);
	
	if (FAILED(m_pTarget_Manager->End_MRT(m_pContext, TEXT("MRT_LightAcc"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	// HDR
	if (FAILED(m_pShader->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	// if (FAILED(m_pShader->Set_Matrix("g_LightViewMatrix", CLight_Manager::GetInstance()->GetDirectionalLightView())))
	// 	return E_FAIL;
	// if (FAILED(m_pShader->Set_Matrix("g_LightProjMatrix", CLight_Manager::GetInstance()->GetDirectionalLightProj())))
	// 	return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	if (FAILED(m_pShader->Set_Matrix("g_ViewMatrixInv", &pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_ShaderResourceView("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_ShadeTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Shade")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_SpecularTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth")))))
		return E_FAIL;
	// if (FAILED(m_pShader->Set_ShaderResourceView("g_ShadowDepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_ShadowDepth")))))
	// 	return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_NormalTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Normal")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_RMATexture", m_pTarget_Manager->Get_SRV(TEXT("Target_RMA")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_AMBTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_AMB")))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_ShaderResourceView("g_CTLTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_CTL")))))
		return E_FAIL;

	m_pEnv->Bind_ShaderResource(m_pShader, "g_IrradianceTexture");
	m_pEnv2->Bind_ShaderResource(m_pShader, "g_RadianceTexture");


	m_pShader->Begin(3);
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	// HDR
	for (auto& pGameObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_NONLIGHT].clear();



	return S_OK;
}

HRESULT CRenderer::Render_AlphaBlend()
{
	// HDR
	for (auto& pGameObject : m_RenderObjects[RENDER_ALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Compute_CamDistance();
	}


	m_RenderObjects[RENDER_ALPHABLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
	{
		return pSour->GetCamDistance() > pDest->GetCamDistance();
	});

	for (auto& pGameObject : m_RenderObjects[RENDER_ALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_ALPHABLEND].clear();



	return S_OK;
}


HRESULT CRenderer::Render_HDR()
{
	CRenderTarget* pLDR1 = m_pTarget_Manager->GetTarget(L"Target_LDR1");
	CRenderTarget* pLDR2 = m_pTarget_Manager->GetTarget(L"Target_LDR2");
	pLDR1->Clear();
	pLDR2->Clear();

	CHDR::GetInstance()->PostProcessing(m_pTarget_Manager->Get_SRV(L"Target_HDR"), pLDR1->Get_RTV());
	return S_OK;
}

HRESULT CRenderer::Render_PostProcess()
{
	if (FAILED(m_pShader_PostProcess->Set_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader_PostProcess->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	CRenderTarget* pLDRSour = m_pTarget_Manager->GetTarget(L"Target_LDR1");
	CRenderTarget* pLDRDest = m_pTarget_Manager->GetTarget(L"Target_LDR2");
	ID3D11RenderTargetView*	pBackBufferView = nullptr;
	ID3D11DepthStencilView*	pDepthStencilView = nullptr;
	m_pContext->OMGetRenderTargets(1, &pBackBufferView, &pDepthStencilView);
	m_pContext->GSSetShader(nullptr, nullptr, 0);


	if (m_RenderObjects[POSTPROCESS_VFX].empty() == false)
	{
		CPipeLine* pPipeLine = CPipeLine::GetInstance();

		if (FAILED(m_pShader_PostProcess->Set_Matrix("g_ProjMatrixInv", &pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_PROJ))))
			return E_FAIL;
		if (FAILED(m_pShader_PostProcess->Set_Matrix("g_ViewMatrixInv", &pPipeLine->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW))))
			return E_FAIL;
		if (FAILED(m_pShader_PostProcess->Set_RawValue("g_vCamPosition", &pPipeLine->Get_CamPosition(), sizeof(_float4))))
			return E_FAIL;
		
		if (FAILED(m_pShader_PostProcess->Set_ShaderResourceView("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse")))))
			return E_FAIL;
		if (FAILED(m_pShader_PostProcess->Set_ShaderResourceView("g_NormalTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Normal")))))
			return E_FAIL;
		if (FAILED(m_pShader_PostProcess->Set_ShaderResourceView("g_ShadeTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Shade")))))
			return E_FAIL;
		if (FAILED(m_pShader_PostProcess->Set_ShaderResourceView("g_SpecularTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")))))
			return E_FAIL;
		if (FAILED(m_pShader_PostProcess->Set_ShaderResourceView("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth")))))
			return E_FAIL;
		if (FAILED(m_pShader_PostProcess->Set_ShaderResourceView("g_DepthMaintainTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth_Maintain")))))
			return E_FAIL;

		// For Effect
		if (FAILED(m_pShader_PostProcess->Set_ShaderResourceView("g_FlagTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Flag")))))
			return E_FAIL;
		// ~For Effect

		m_RenderObjects[POSTPROCESS_VFX].sort([](CGameObject* pLeft, CGameObject* pRight)
		{
			const auto pLeftPostFX = dynamic_cast<CPostProcess*>(pLeft);
			const auto pRightPostFX = dynamic_cast<CPostProcess*>(pRight);
			Assert(pLeftPostFX != nullptr);
			Assert(pRightPostFX != nullptr);

			return pLeftPostFX->GetPriority() < pRightPostFX->GetPriority();
		});

		for (auto pGameObject : m_RenderObjects[POSTPROCESS_VFX])
		{
			if (const auto pPostFX = dynamic_cast<CPostProcess*>(pGameObject))
			{
				ID3D11ShaderResourceView* pLDRSour_SRV = pLDRSour->Get_SRV();
				ID3D11RenderTargetView* pLDRDest_RTV = pLDRDest->Get_RTV();

				if (FAILED(m_pShader_PostProcess->Set_ShaderResourceView("g_LDRTexture", pLDRSour_SRV)))
					return E_FAIL;
				m_pContext->OMSetRenderTargets( 1, &pLDRDest_RTV, pDepthStencilView);

				CRenderTarget* pLDRTmp = pLDRSour;
				pLDRSour = pLDRDest;
				pLDRDest = pLDRTmp;

				m_pShader_PostProcess->Begin_Params(pPostFX->GetParam());
				m_pVIBuffer->Render();
			}

			Safe_Release(pGameObject);
		}

		m_RenderObjects[POSTPROCESS_VFX].clear();
	}


	m_pContext->OMSetRenderTargets(1, &pBackBufferView, pDepthStencilView);
	Safe_Release(pBackBufferView);
	Safe_Release(pDepthStencilView);

	// LDR to Backbuffer
	if (FAILED(m_pShader_PostProcess->Set_ShaderResourceView("g_LDRTexture", pLDRSour->Get_SRV())))
		return E_FAIL;

	m_pShader_PostProcess->Begin(0);
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Distortion(const _tchar* pTargetTag)
{
	return S_OK;
}

HRESULT CRenderer::Render_Outline()
{
	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	
	_uint		iNumViewports = 1;
	
	m_pContext->RSGetViewports(&iNumViewports, &ViewPortDesc);
	
	m_pShader->Set_RawValue("g_iWinCX", &ViewPortDesc.Width, sizeof(_float));
	m_pShader->Set_RawValue("g_iWinCY", &ViewPortDesc.Height, sizeof(_float));
	
	FAILED_CHECK(m_pShader->Set_ShaderResourceView("g_OutlineFlagTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_OutlineFlag"))));
	
	m_pShader->Set_Matrix("g_WorldMatrix", &m_WorldMatrix);
	m_pShader->Set_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Set_Matrix("g_ProjMatrix", &m_ProjMatrix);
	
	m_pShader->Begin(4);
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_DebugObject()
{
	for (auto pGameObject : m_RenderObjects[RENDER_DEBUG])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObjects[RENDER_DEBUG].clear();

	for (auto& pComponent : m_DebugObject)
	{
#ifdef _DEBUG
		if (nullptr != pComponent)
			pComponent->Render();
#endif

		Safe_Release(pComponent);
	}
#ifdef _DEBUG
	CPhysX_Manager::GetInstance()->DebugRender(m_pDevice, m_pContext);
#endif

	m_DebugObject.clear();

	return S_OK;
	
}

CRenderer * CRenderer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRenderer*		pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CRenderer * CRenderer::Clone(void * pArg)
{
	AddRef();

	return this;
}
void CRenderer::Free()
{
	__super::Free();


	// for (auto& pComponent : m_DebugObject)
	// 	Safe_Release(pComponent);
	//
	// m_DebugObject.clear();
	//
	// for (_uint i = 0; i < RENDER_END; ++i)
	// {
	// 	for (auto& pGameObject : m_RenderObjects[i])
	// 		Safe_Release(pGameObject);
	//
	// 	m_RenderObjects[i].clear();
	// }

	Clear();


	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pShader_PostProcess);

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pShadowDepthStencilView);
	Safe_Release(m_pEnv);
	Safe_Release(m_pEnv2);
}
