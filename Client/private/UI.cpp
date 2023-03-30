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

void CUI::TempOff(const _bool bOff)
{
	if (true == bOff)	// -> true 라면 그리지 않는다.
	{
		if (false == m_bVisible) // 이전에 그려지지 않았던 애들 건너뛴다.
			return;

		m_bTempOff = true;		// m_bTempOff 가 true 가 된 애들을
		m_bVisible = false;		// 그리지 않는다.
	}
	else				// -> false 라면 그려준다.
	{
		if (false == m_bTempOff)
			return;

		m_bTempOff = false;		// 다음을 위해 초기화를 하고,
		m_bVisible = true;		// 그려준다.
	}
}

_float2 CUI::GetScreenSpaceLeftTop()
{
	const _float fWindowHalfWidth = static_cast<_float>(WINCX) * 0.5f;
	const _float fWindowHalfHeight = static_cast<_float>(WINCY) * 0.5f;

	const _float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	const _float fScreenCenterX = vPos.x + fWindowHalfWidth;
	const _float fScreenCenterY = -vPos.y + fWindowHalfHeight;

	return _float2{ fScreenCenterX - m_fSizeX * 0.5f, fScreenCenterY - m_fSizeY * 0.5f };
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

	m_CanvasSize = CanvasRect{ 0.f, 0.f, (_float)g_iWinSizeX * 0.5f, (_float)g_iWinSizeY * 0.5f };

	return S_OK;
}

void CUI::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	const _float2 PivotPair = GetPivotXY(m_ePivot);

	if (0.1f > m_vScale.x || 0.1f > m_vScale.y)	
		return; // 텍스처의 스케일을 조정할 때 0.0f 보다 작아지면 그 다음부터 0 * m_vScale.y 이 되기 때문에 0.1f 보다 작은 경우에는 return 시킨다.

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX * m_vScale.x, m_fSizeY * m_vScale.y, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(m_fX + PivotPair.x, m_fY + PivotPair.y, 0.f, 1.f));
}

void CUI::Late_Tick(_double TimeDelta)
{
	if (true == m_bTempOff)
		return;

	__super::Late_Tick(TimeDelta);

	m_pShaderCom->Tick(TimeDelta);
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
	CShader::Imgui_RenderShaderParams(m_tParams);

	ImGui::Separator();
	static array<const char*, UI_PIVOT_CNT> arrPivotName {
		"CENTER", "LEFT TOP", "TOP", "RIGHT_TOP", "RIGHT", "RIGHT BOT", "BOT", "LEFT BOT", "LEFT"
	};

	ImGui::Checkbox("Visible", &m_bVisible);
	_int iPriority = m_iPriority;
	ImGui::InputInt("Priority", &iPriority);
	m_iPriority = _uint(iPriority);

	if (ImGui::BeginCombo("Pivot", arrPivotName[static_cast<_uint>(m_ePivot)]))
	{
		for (int i = 0; i < UI_PIVOT_CNT; ++i)
		{
			const bool bSelected = false;
			if (ImGui::Selectable(arrPivotName[i], bSelected))
				m_ePivot = static_cast<UI_PIVOT>(i);
		}

		ImGui::EndCombo();
	}

	ImGui::Separator();
	ImGui::DragFloat("pos X", &m_fX);
	ImGui::DragFloat("pos Y", &m_fY);
	ImGui::DragFloat("Size X", &m_fSizeX);
	ImGui::DragFloat("Size Y", &m_fSizeY);

	ImGui::Separator();
	ImGui::DragFloat("Scale X", &m_vScale.x, 0.01f, 0.1f, 1.0f);
	ImGui::DragFloat("Scale Y", &m_vScale.y, 0.01f, 0.1f, 1.0f);

	ImGui::Separator();

	if (ImGui::Button("UseRot"))
	{
		m_bUseRot = !m_bUseRot;
		SetUseRotation(m_bUseRot);
	}

	if (m_bUseRot)
	{
		ImGui::DragFloat("Rotation(degree)", &m_fRadianRotation);
		m_pTransformCom->Rotation({ 0.0f, 0.0f, 1.0f, 0.0f }, m_fRadianRotation);
	}
	ImGui::Separator();

#ifdef _DEBUG
	if (ImGui::Button("Recompile"))
	{
		m_pShaderCom->ReCompileShader();
	}
#endif
}

void CUI::SaveToJson(Json & json)
{
	__super::SaveToJson(json);
	CShader::SaveShaderParam(m_tParams, json);

	json["PositionX"] = m_fX;
	json["PositionY"] = m_fY;
	json["SizeX"] = m_fSizeX;
	json["SizeY"] = m_fSizeY;
	json["ScaleX"] = m_vScale.x;
	json["ScaleY"] = m_vScale.y;
	json["Priority"] = m_iPriority;
	json["Pivot"] = static_cast<_uint>(m_ePivot);
}

void CUI::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);
	CShader::LoadShaderParam(m_tParams, json);

	m_fX = json["PositionX"];
	m_fY = json["PositionY"];
	m_fSizeX = json["SizeX"];
	m_fSizeY = json["SizeY"];
	m_vScale.x = json["ScaleX"];
	m_vScale.y = json["ScaleY"];
	m_iPriority = json["Priority"];
	m_ePivot = static_cast<UI_PIVOT>(json["Pivot"]);
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
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_ORTHO))))
		return E_FAIL;

	return S_OK;
}

void CUI::SetUseRotation(_bool bUseRot)
{
	m_bUseRot = bUseRot;
}

_float2 CUI::GetPivotXY(UI_PIVOT ePivot) const
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
		vPivotXY.y = m_CanvasSize.fCenterY;
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
