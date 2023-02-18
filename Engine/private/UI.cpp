#include "stdafx.h"
#include "..\public\UI.h"
#include "GameInstance.h"
#include "GameUtils.h"
#include "ImguiUtils.h"
#include "JsonLib.h"


CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CUI::CUI(const CUI& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUI::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());
	return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));
	m_CanvasSize = CanvasRect{0.f, 0.f, static_cast<_float>(WINCX) * 0.5f, static_cast<_float>(WINCY) * 0.5f};

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		json["X"].get_to(m_fX);
		json["Y"].get_to(m_fY);
		json["SizeX"].get_to(m_fSizeX);
		json["SizeY"].get_to(m_fSizeY);
		json["UseRot"].get_to(m_bUseRot);
		// json["Revolve"].get_to(m_fRadianRevolve);
		json["Rotation"].get_to(m_fRadianRotation);
		m_iPriority = json["Priority"];
		m_ePivot = static_cast<UI_PIVOT>(json["Pivot"]);
		SetUseRotation(m_bUseRot);
	}


	return S_OK;
}

void CUI::Tick(_double TimeDelta)
{
	const _float2 PivotPair = GetPivotXY(m_ePivot);

	m_pTransformCom->Set_Scaled(_float3(m_fSizeX * m_vScale.x, m_fSizeY * m_vScale.y, 1.f));

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, 
		XMVectorSet(m_fX + PivotPair.x, m_fY + PivotPair.y, 0.f, 1.f));

	if (m_bUseRot)
	{
		// 자전 셋팅
		static const _vector vZAxis = XMLoadFloat4(&_float4::UnitZ);
		m_pTransformCom->Rotation(vZAxis, m_fRadianRotation);

		// 공전 셋팅
		// m_pTransformCom->Set_ParentWorldMatrix(_float4x4::CreateFromAxisAngle(_float3::UnitZ, m_fRadianRevolve));
	}
}

void CUI::Late_Tick(_double TimeDelta)
{
	if (m_bVisible)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

void CUI::SaveToJson(Json& json)
{
	json["Prototype_GameObject"] = CGameUtils::ws2s(m_strPrototypeTag);
	json["X"] = m_fX;
	json["Y"] = m_fY;
	json["SizeX"] = m_fSizeX;
	json["SizeY"] = m_fSizeY;
	json["UseRot"] = m_bUseRot;
	// json["Revolve"] = m_fRadianRevolve;
	json["Rotation"] = m_fRadianRotation;
	json["Pivot"] = static_cast<_uint>(m_ePivot);
	json["Priority"] = m_iPriority;
}

void CUI::Imgui_RenderComponentProperties()
{
	for (const auto& com : m_Components)
	{
		// UI의 Transform을 기즈모로 조종할 수 없기 때문에 Imgui로도 출력하지 않는다.
		if (dynamic_cast<CTransform*>(com.second))
			continue;

		char szName[256];
		CGameUtils::wc2c(com.first.c_str(), szName);

        if (ImGui::CollapsingHeader(szName, ImGuiTreeNodeFlags_DefaultOpen))
			com.second->Imgui_RenderProperty();
	}
}

void CUI::Imgui_RenderProperty()
{
	static array<const char*, UI_PIVOT_CNT> arrPivotName {
		"CENTER", "LEFT TOP", "TOP", "RIGHT_TOP", "RIGHT", "RIGHT BOT", "BOT", "LEFT BOT", "LEFT"
	};

	ImGui::Checkbox("Visible", &m_bVisible);
	_int iPriority = m_iPriority;
	ImGui::InputInt("Priority", &iPriority);
	m_iPriority = iPriority;

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

	CImguiUtils::InputFloatWheel("pos X", &m_fX);
	CImguiUtils::InputFloatWheel("pos Y", &m_fY);
	CImguiUtils::InputFloatWheel("Size X", &m_fSizeX);
	CImguiUtils::InputFloatWheel("Size Y", &m_fSizeY);

	if (ImGui::Button("UseRot"))
	{
		m_bUseRot = !m_bUseRot;
		SetUseRotation(m_bUseRot);
	}

	if (m_bUseRot)
	{
		// _float fDegreeRevolve = XMConvertToDegrees(m_fRadianRevolve);
		// CImguiUtils::InputFloatWheel("Revolve(degree)", &fDegreeRevolve);
		// m_fRadianRevolve = XMConvertToRadians(fDegreeRevolve);

		_float fDegreeRotation = XMConvertToDegrees(m_fRadianRotation);
		CImguiUtils::InputFloatWheel("Rotation(degree)", &fDegreeRotation);
		m_fRadianRotation = XMConvertToRadians(fDegreeRotation);
	}
}

void CUI::SetUseRotation(_bool bUseRot)
{
	m_bUseRot = bUseRot;
	// transform의 parent를 공전 행렬로 사용하기 위함
	// m_pTransformCom->SetChild(bUseRot);
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

	const RECT rt {
		static_cast<_long>(fScreenCenterX - m_fSizeX * 0.5f),
		static_cast<_long>(fScreenCenterY - m_fSizeY * 0.5f),
		static_cast<_long>(fScreenCenterX + m_fSizeX * 0.5f),
		static_cast<_long>(fScreenCenterY + m_fSizeY * 0.5f)
	};

	return PtInRect(&rt, ptClient) == TRUE;
}

_bool CUI::IsCursorOn()
{
	return IsCursorOn(CGameUtils::GetClientCursor());
}

_float2 CUI::GetScreenSpaceLeftTop()
{
	const _float fWindowHalfWidth = static_cast<_float>(WINCX) * 0.5f;
	const _float fWindowHalfHeight = static_cast<_float>(WINCY) * 0.5f;

	const _float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);
	const _float fScreenCenterX = vPos.x + fWindowHalfWidth;
	const _float fScreenCenterY = -vPos.y + fWindowHalfHeight;

	return _float2{fScreenCenterX - m_fSizeX * 0.5f, fScreenCenterY - m_fSizeY * 0.5f};
}

void CUI::RenderFont(const _tchar* pFontTag, const _tchar* pText, _float2 vScale, _fvector vColor,
	_uint iFontSize)
{
	const _int iLen = lstrlen(pText);
	_float fTextWidth = (_float)iLen * iFontSize;
	_float fTexHeight = (_float)iFontSize;

	_float2 vPos = GetScreenSpaceLeftTop();
	vPos.x += (m_fSizeX - fTextWidth) * 0.5f;
	vPos.y += (m_fSizeY - fTexHeight) * 0.5f;

	CGameInstance::GetInstance()->Render_Font(pFontTag, pText, vPos, 0.f, vScale, vColor);
}

_float2 CUI::GetPivotXY(UI_PIVOT ePivot) const
{
	_float2 vPivotXY{0.f, 0.f};

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
	Safe_Release(m_pRendererCom);
}
