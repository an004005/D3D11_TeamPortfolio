#include "stdafx.h"
#include "..\public\Loading.h"
#include "GameInstance.h"
#include "PartUI.h"

CLoading::CLoading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CLoading::CLoading(const CLoading& rhs)
	: CCanvas(rhs)
{
}

HRESULT CLoading::Initialize_Prototype()
{
	CCanvas::Initialize_Prototype();

	auto pGameInstance = CGameInstance::GetInstance();

	pGameInstance->Add_Prototype(L"ProtoUI_LogoLoadingAnim", CPartUI::Create(m_pDevice, m_pContext, 
	L"../Bin/Resources/Textures/UI/TX_SharedUtilities_UI_LoadingThrobberTri.png"));
	return S_OK;
}

HRESULT CLoading::Initialize(void* pArg)
{
	m_fSizeX = 100.f;
	m_fSizeY = 100.f;
	CCanvas::Initialize(pArg);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTexUI"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer_Rect"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"../Bin/Resources/Textures/UI/Loading_Ascent.png", TEXT("TextureCom"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	Add_ChildUI(LEVEL_STATIC, L"ProtoUI_LogoLoadingAnim", L"LoadingAnim");
	m_pLoadingAnim = dynamic_cast<CPartUI*>(Find_ChildUI(L"LoadingAnim"));


	return S_OK;
}

void CLoading::Tick(_double TimeDelta)
{
	m_CanvasSize = CanvasRect{0.f, 0.f, static_cast<_float>(g_iWinSizeX) * 0.5f, static_cast<_float>(g_iWinSizeY) * 0.5f};
	m_fSizeX = (_float)g_iWinSizeX;
	m_fSizeY = (_float)g_iWinSizeY;
	m_fTime += TimeDelta;
	if (m_fTime >= 0.05f * 8 * 9)
		m_fTime = 0.f;
	CCanvas::Tick(TimeDelta);
}

HRESULT CLoading::Render()
{
	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_ORTHO)));
	m_pShaderCom->Set_Param_Texture(0, m_pTextureCom);
	m_pShaderCom->Begin(0);
	m_pVIBufferCom->Render();

	m_pLoadingAnim->Render_SetShader([&](CShader* pShader, CTexture* pTex)
	{
		const _float fFrameTime = 0.05f;
		const int iWidhtCnt = 8;
		const int iHeightCnt = 9;

		pShader->Set_Param_Texture(0, pTex);
		pShader->Set_Param_float(0, &m_fTime);
		pShader->Set_Param_float(1, &fFrameTime);
		pShader->Set_Param_Int(0, &iWidhtCnt);
		pShader->Set_Param_Int(1, &iHeightCnt);
		pShader->Begin(9);
	});


	return S_OK;
}

void CLoading::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();
}

void CLoading::Free()
{
	CCanvas::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
