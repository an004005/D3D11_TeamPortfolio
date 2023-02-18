#include "stdafx.h"
#include "..\public\UIEx.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "ImguiUtils.h"

CUIEx::CUIEx(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CUIEx::CUIEx(const CUIEx& rhs)
	: CUI(rhs)
{
}

HRESULT CUIEx::Initialize_Prototype()
{
	return CUI::Initialize_Prototype();
}

HRESULT CUIEx::Initialize(void* pArg)
{
	m_fSizeX = 100.f;
	m_fSizeY = 100.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTexUI"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer_Rect"),
		(CComponent**)&m_pBuffer)))
		return E_FAIL;

	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		CShader::LoadShaderParam(m_tParams, json);

		m_bUseShader = json["UseShader"];
		m_strText = s2ws(json["Text"]);
		m_strFontTag = s2ws(json["FontTag"]);
		m_vTextPos = json["TextPos"];
		m_vTextScale = json["TextScale"];
		m_vTextColor = json["TextColor"];
		if (json.contains("FitParent"))
			m_bFitParent = json["FitParent"];
	}

	return S_OK;
}

void CUIEx::Tick(_double TimeDelta)
{
	if (m_bVisible == false)
		return;

	if (m_bFitParent)
	{
		m_fSizeX = m_CanvasSize.fHalfWidth * 2.f;
		m_fSizeY = m_CanvasSize.fHalfHeight * 2.f;
		m_ePivot = UI_PIVOT::CENTER;
	}

	__super::Tick(TimeDelta);

	if (IsCursorOn())
	{
		if (m_OnHover)
			m_OnHover();

		if (m_OnClick && CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB))
			m_OnClick();

		if (m_OnClicking && CGameInstance::GetInstance()->KeyPressing(CInput_Device::DIM_LB))
			m_OnClicking();
	}	
	else
	{
		if (m_OffHover)
			m_OffHover();
	}
}

HRESULT CUIEx::Render()
{
	if (m_bUseShader)
	{
		if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_ORTHO))))
			return E_FAIL;

		m_pShaderCom->Begin_Params(m_tParams);
		m_pBuffer->Render();
	}

	if (m_strText.empty() == false && m_strFontTag.empty() == false)
	{
		CGameInstance::GetInstance()
			->Render_Font(
				m_strFontTag.c_str(),
				m_strText.c_str(), 
				GetScreenSpaceLeftTop() + m_vTextPos,
				0.f,
				m_vTextScale,
				m_vTextColor);
	}

	return S_OK;
}

void CUIEx::SaveToJson(Json& json)
{
	CUI::SaveToJson(json);

	CShader::SaveShaderParam(m_tParams, json);
	json["UseShader"] = m_bUseShader;
	json["Text"] = ws2s(m_strText);
	json["FontTag"] = ws2s(m_strFontTag);
	json["TextPos"] = m_vTextPos;
	json["TextScale"] = m_vTextScale;
	json["TextColor"] = m_vTextColor;
	json["FitParent"] = m_bFitParent;
}

void CUIEx::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

	ImGui::Checkbox("Use Shader", &m_bUseShader);
	ImGui::Checkbox("Fit to Parent", &m_bFitParent);
	if (ImGui::CollapsingHeader("Shader Params"))
	{
		CShader::Imgui_RenderShaderParams(m_tParams);
	}
	if (ImGui::CollapsingHeader("Fonts"))
	{
		static char szText[MAX_PATH]{};
		strcpy(szText, ws2s(m_strText).c_str());
		ImGui::InputText("Text", szText, MAX_PATH);
		m_strText = s2ws(szText);

		static char szFontTag[MAX_PATH]{};
		strcpy(szFontTag, ws2s(m_strFontTag).c_str());
		ImGui::InputText("FontTag", szFontTag, MAX_PATH);
		m_strFontTag = s2ws(szFontTag);

		CImguiUtils::InputFloatWheel("TextPos x", &m_vTextPos.x);
		CImguiUtils::InputFloatWheel("TextPos y", &m_vTextPos.y);

		ImGui::InputFloat2("TextScale", (float*)&m_vTextScale);
		ImGui::ColorEdit4("TextColor", (float*)&m_vTextColor);
	}
}

void CUIEx::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBuffer);

	for (auto e : m_tParams.Textures)
		Safe_Release(e.first);
}
