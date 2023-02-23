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

	//m_CanvasSize = CanvasRect{ 0.f, 0.f, (_float)g_iWinSizeX * 0.5f, (_float)g_iWinSizeY * 0.5f };
	
	m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet((_float)g_iWinSizeX * m_fX, (_float)g_iWinSizeY * m_fY, 0.f, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void CUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	//const _float2 PivotPair = Get_PivotXY(m_ePivot);

	//m_pTransformCom->Set_Scaled(_float3(m_fSizeX * m_vScale.x, m_fSizeY * m_vScale.y, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX + PivotPair.x, m_fY + PivotPair.y, 0.f, 1.f));
}

void CUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if(nullptr != m_pRendererCom)
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
	CShader::Imgui_RenderShaderParams(m_tParams);

	ImGui::InputFloat("Position X", &m_fX);
	ImGui::InputFloat("Position Y", &m_fY);
	ImGui::InputFloat("SizeX", &m_fSizeX);
	ImGui::InputFloat("SizeY", &m_fSizeY);

	// 전체적인 값을 변경할 때
	if (ImGui::Button("Set All"))
	{
		m_pTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
		_float fPositionX = (_float)g_iWinSizeX * m_fX;
		_float fPositionY = (_float)g_iWinSizeY * m_fY;
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(fPositionX, fPositionY, 0.f, 1.f));
	}

	ImGui::Separator();

	// 값을 곱해서 위치를 변경할 때
	static _float fValue_RePosition[2];
	ImGui::InputFloat2("Value Position", fValue_RePosition);

	if (ImGui::Button("Value Reposition"))
	{
		m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet((_float)g_iWinSizeX * fValue_RePosition[0], (_float)g_iWinSizeY * fValue_RePosition[1], 0.f, 1.f));
	}
	ImGui::SameLine();
	if (ImGui::Button("Set Value Reposition"))
	{
		m_fX = fValue_RePosition[0];
		m_fY = fValue_RePosition[1];
	}

	static _float fValue_ReSize[2];
	ImGui::InputFloat2("Value ReSize", fValue_ReSize);

	if (ImGui::Button("Value Resize"))
	{
		m_pTransformCom->Set_Scaled(_float3(m_fSizeX * fValue_ReSize[0], m_fSizeY * fValue_ReSize[1], 1.f));
	}
	ImGui::SameLine();
	if (ImGui::Button("Set Value Resize"))
	{
		m_fSizeX *= fValue_ReSize[0];
		m_fSizeY *= fValue_ReSize[1];
	}

	static _float fReSize[2];
	ImGui::DragFloat2("ReSize", fReSize);

	// 사이즈를 직접적으로 조정할 때
	static _bool	bReSize;
	if (ImGui::Button("ReSize"))
	{
		bReSize = !bReSize;
	}
	ImGui::SameLine();
	if (true == bReSize)
	{
		m_pTransformCom->Set_Scaled(_float3(fReSize[0], fReSize[1], 1.f));
	}
	if (ImGui::Button("Set Resize"))
	{
		m_fSizeX = fReSize[0];
		m_fSizeY = fReSize[1];
	}
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

_float2 CUI::Get_PivotXY(UI_PIVOT ePivot) const
{
	_float2 vPivotXY{ 0.f, 0.f };

	switch (ePivot)
	{
	case UI_PIVOT::CENTER:
		vPivotXY.x = m_CanvasSize.fCenterX;
		vPivotXY.y = m_CanvasSize.fCenterY;
		break;
	case UI_PIVOT::LEFT_TOP:
		vPivotXY.x = m_CanvasSize.fCenterX - m_CanvasSize.fHalfWidth;
		vPivotXY.y = m_CanvasSize.fCenterY + m_CanvasSize.fHalfHeight;
		break;
	case UI_PIVOT::TOP:
		vPivotXY.x = m_CanvasSize.fCenterX;
		vPivotXY.y = m_CanvasSize.fCenterY + m_CanvasSize.fHalfHeight;
		break;
	case UI_PIVOT::RIGHT_TOP:
		vPivotXY.x = m_CanvasSize.fCenterX + m_CanvasSize.fHalfWidth;
		vPivotXY.y = m_CanvasSize.fCenterY + m_CanvasSize.fHalfHeight;
		break;
	case UI_PIVOT::RIGHT:
		vPivotXY.x = m_CanvasSize.fCenterX + m_CanvasSize.fHalfWidth;
		vPivotXY.y = m_CanvasSize.fCenterY + m_CanvasSize.fHalfHeight;
		break;
	case UI_PIVOT::RIGHT_BOT:
		vPivotXY.x = m_CanvasSize.fCenterX + m_CanvasSize.fHalfWidth;
		vPivotXY.y = m_CanvasSize.fCenterY - m_CanvasSize.fHalfHeight;
		break;
	case UI_PIVOT::BOT:
		vPivotXY.x = m_CanvasSize.fCenterX;
		vPivotXY.y = m_CanvasSize.fCenterY - m_CanvasSize.fHalfHeight;
		break;
	case UI_PIVOT::LEFT_BOT:
		vPivotXY.x = m_CanvasSize.fCenterX - m_CanvasSize.fHalfWidth;
		vPivotXY.y = m_CanvasSize.fCenterY - m_CanvasSize.fHalfHeight;
		break;
	case UI_PIVOT::LEFT:
		vPivotXY.x = m_CanvasSize.fCenterX - m_CanvasSize.fHalfWidth;
		vPivotXY.y = m_CanvasSize.fCenterY;
		break;
	case UI_PIVOT::PIVOT_END:
		FALLTHROUGH
	default:
		NODEFAULT
			break;
	}

	return vPivotXY;
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
