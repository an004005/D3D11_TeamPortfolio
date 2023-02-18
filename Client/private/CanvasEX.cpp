#include "stdafx.h"
#include "..\public\CanvasEX.h"
#include <ImguiUtils.h>
#include "GameInstance.h"
#include "JsonLib.h"

CCanvasEX::CCanvasEX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvasEX::CCanvasEX(const CCanvasEX& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvasEX::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CCanvasEX::Initialize(void* pArg)
{
	m_fSizeX = 100.f;
	m_fSizeY = 100.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		m_bFullSize = json["FullSize"];
	}

	return S_OK;
}

void CCanvasEX::Tick(_double TimeDelta)
{
	if (m_bVisible == false)
		return;

	if (m_bFullSize)
	{
		m_fSizeX = (_float)g_iWinSizeX;
		m_fSizeY = (_float)g_iWinSizeY;
	}
	CCanvas::Tick(TimeDelta);
}

void CCanvasEX::Imgui_RenderProperty()
{
	static char szChildProtoName[MAX_PATH] = "ProtoUI_UIEX";
	ImGui::InputText("ChildProtoTag", szChildProtoName, MAX_PATH);
	static char szChildName[MAX_PATH]{};
	ImGui::InputText("ChildName", szChildName, MAX_PATH);
	ImGui::SameLine();

	if (ImGui::Button("Add Empty UI"))
	{
		wstring childTag = s2ws(szChildName);
		wstring childProtoTag = s2ws(szChildProtoName);
		if (Find_ChildUI(childTag.c_str()) == nullptr && childTag.empty() == false && childProtoTag.empty() == false)
			Add_ChildUI(LEVEL_NOW, childProtoTag.c_str(), childTag.c_str());
	}

	ImGui::Checkbox("FullSize", &m_bFullSize);
	
	CCanvas::Imgui_RenderProperty();
}

void CCanvasEX::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
	json["FullSize"] = m_bFullSize;
}

void CCanvasEX::Free()
{
	CCanvas::Free();
}
