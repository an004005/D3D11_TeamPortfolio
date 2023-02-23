#include "stdafx.h"
#include "..\public\UI.h"
#include "GameInstance.h"
#include "JsonLib.h"

CUI::CUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI::CUI(const CUI & rhs)
	: CGameObject(rhs)
{
}

_bool CUI::IsCursorOn(POINT ptClient)
{
	// PtInRect()
	const _float fWindowHalfWidth = static_cast<_float>(WINCX) * 0.5f;
	const _float fWindowHalfHeight = static_cast<_float>(WINCY) * 0.5f;

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));

	const _float fScreenCenterX = vPos.x + fWindowHalfWidth;
	const _float fScreenCenterY = -vPos.y + fWindowHalfHeight;

	const RECT rt{
		static_cast<_long>(fScreenCenterX - m_fSizeX * 0.5f),
		static_cast<_long>(fScreenCenterY - m_fSizeY * 0.5f),
		static_cast<_long>(fScreenCenterX + m_fSizeX * 0.5f),
		static_cast<_long>(fScreenCenterY + m_fSizeY * 0.5f)
	};

	return PtInRect(&rt, ptClient) == TRUE;
}

HRESULT CUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CUI::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;	
	
	m_fX = 0.5f;
	m_fY = 0.5f;
	m_fSizeX = 100.0f;
	m_fSizeY = 100.0f;

	m_ImguiInfo.fPosition.x = (_float)g_iWinSizeX * m_fX;
	m_ImguiInfo.fPosition.y = (_float)g_iWinSizeY * m_fY;

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet((_float)g_iWinSizeX * m_fX, (_float)g_iWinSizeY * m_fY, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet((_float)g_iWinSizeX * m_fX, (_float)g_iWinSizeY * m_fY, 0.f, 1.f));
}

void CUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_bVisible)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin_Params(m_tParams);
	m_pVIBufferCom->Render();

	return S_OK;
}

void CUI::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
	CUI::Imgui_UIInfo();
}

void CUI::SaveToJson(Json & json)
{
	__super::SaveToJson(json);
	CShader::SaveShaderParam(m_tParams, json);

	json["PositionX"] = m_fX;
	json["PositionY"] = m_fY;
	json["SizeX"] = m_fSizeX;
	json["SizeY"] = m_fSizeY;
}

void CUI::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);
	CShader::LoadShaderParam(m_tParams, json);

	m_fX = json["PositionX"];
	m_fY = json["PositionY"];
	m_fSizeX = json["SizeX"];
	m_fSizeY = json["SizeY"];
}

HRESULT CUI::SetUp_Components()
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

	return S_OK;
}

HRESULT CUI::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

void CUI::Imgui_UIInfo()
{
	if (m_ImguiInfo.bModify)
	{
		m_fX = m_ImguiInfo.fPosition.x;
		m_fY = m_ImguiInfo.fPosition.y;
		m_fSizeX = m_ImguiInfo.fSize.x;
		m_fSizeY = m_ImguiInfo.fSize.y;
	}

	ImGui::DragFloat("Position X", &m_fX);
	ImGui::DragFloat("Position Y", &m_fY);
	ImGui::DragFloat("SizeX", &m_fSizeX);
	ImGui::DragFloat("SizeY", &m_fSizeY);

	ImGui::Separator();
	ImGui::Checkbox("UI Move", &m_ImguiInfo.bModify);
	ImGui::SameLine();
	if (ImGui::Button("Reset Position"))
	{
		m_fX = m_ImguiInfo.fPosition.x;
		m_fY = m_ImguiInfo.fPosition.y;
	}
	ImGui::SameLine();
	if (ImGui::Button("Set Size"))
	{
		m_fSizeX *= m_ImguiInfo.fSize.x;
		m_fSizeY *= m_ImguiInfo.fSize.y;
	}

	ImGui::DragFloat("Reposition X", &m_ImguiInfo.fPosition.x);
	ImGui::DragFloat("Reposition Y", &m_ImguiInfo.fPosition.y);
	ImGui::DragFloat("ReSize X", &m_ImguiInfo.fSize.x);
	ImGui::DragFloat("ReSize Y", &m_ImguiInfo.fSize.y);
}

void CUI::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	for (auto pTexture : m_tParams.Textures)
	{
		Safe_Release(pTexture.first);
	}
}
