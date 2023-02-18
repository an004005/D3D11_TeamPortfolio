#include "stdafx.h"
#include "..\public\Canvas.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "JsonLib.h"
#include "ImguiUtils.h"

CCanvas::CCanvas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CCanvas::CCanvas(const CCanvas& rhs)
	: CUI(rhs)
{
}

HRESULT CCanvas::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		for (auto& childJson : json["ChildrenUI"])
		{
			string protoTag = childJson["Prototype_GameObject"];
			string childTag = childJson["ChildTag"];
			auto pUI = Add_ChildUI(LEVEL_NOW, 
				CGameUtils::s2ws(protoTag).c_str(),
				CGameUtils::s2ws(childTag).c_str(),
				&childJson);
			Assert(pUI != nullptr);
		}
	}

	return S_OK;
}

void CCanvas::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	const _float2 PivotPair = GetPivotXY(m_ePivot);


	// 자식들의 캔버스 최신화
	const CanvasRect ThisCanvasSize{
		m_fX + PivotPair.x,
		m_fY + PivotPair.y,
		m_fSizeX * 0.5f,
		m_fSizeY * 0.5f
	};
	
	for (auto& Pair : m_mapChildUIs)
		Pair.second->SetCanvasSize(ThisCanvasSize);

	for (auto itr = m_mapChildUIs.begin(); itr != m_mapChildUIs.end();)
	{
		const auto ChildPair = *itr;

		if (ChildPair.second->IsDeleted())
		{
			ChildPair.second->Release();
			itr = m_mapChildUIs.erase(itr);
		}
		else
		{
			ChildPair.second->Tick(TimeDelta);
			++itr;
		}
	}
}

void CCanvas::Late_Tick(_double TimeDelta)
{
	if (m_bVisible == false)
		return;

	__super::Late_Tick(TimeDelta);

	// for (const auto& Pair : m_mapChildUIs)
	// 	Pair.second->Late_Tick(TimeDelta);

	vector<CUI*> tmpUI;
	tmpUI.reserve(m_mapChildUIs.size());
	
	for (const auto& Pair : m_mapChildUIs)
		tmpUI.push_back(Pair.second);
	
	std::sort(tmpUI.begin(), tmpUI.end(), [](const CUI* left, const CUI* right)
	{
		return left->GetPriority() < right->GetPriority();
	});
	
	for (auto pUI : tmpUI)
		pUI->Late_Tick(TimeDelta);
}

void CCanvas::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
	ImGui::Separator();

	CImguiUtils::FileDialog_FileSelector("Save this Canvas", ".json", "../Bin/Resources/Textures/UI/UI_PositionData/", [this](const string& filePath)
	{
		Json json;
		SaveToJson(json);
		std::ofstream file(filePath);
		file << json;
	});

	ImGui::Separator();

	POINT pt;
	::GetCursorPos(&pt);
	::ScreenToClient(CGameInstance::GetInstance()->GetHWND(), &pt);
	ImGui::Text("cursor : %d, %d", pt.x, pt.y);

	ImGui::Separator();

    if (ImGui::CollapsingHeader("Child UIs"))
    {
		if (ImGui::BeginListBox("##"))
	    {
			for (auto& ChildPair : m_mapChildUIs)
			{
				const bool bSelected = (m_pSelectedChild != nullptr) && (lstrcmp(m_pSelectedChild, ChildPair.first.c_str()) == 0);
				if (bSelected)
					ImGui::SetItemDefaultFocus();

				char szProtoName[256];
				CGameUtils::wc2c(ChildPair.first.c_str(), szProtoName);

				if (ImGui::Selectable(szProtoName, bSelected))
					m_pSelectedChild = ChildPair.first.c_str();
			}
			ImGui::EndListBox();
	    }

		ImGui::Separator();

		ImGui::BeginChild("ChildUIProperty", {400.f, 400.f}, true);
		if (m_pSelectedChild)
		{
			if (CUI* pUI = Find_ChildUI(m_pSelectedChild))
			{
				pUI->Imgui_RenderProperty();
				pUI->Imgui_RenderComponentProperties();
			}
		}
		ImGui::EndChild();
    }

	ImGui::Separator();
}

void CCanvas::SaveToJson(Json& json)
{
	__super::SaveToJson(json);
	json["ChildrenUI"] = Json::array();

	for (auto pChild : m_mapChildUIs)
	{
		Json jsonChild;
		jsonChild["ChildTag"] = CGameUtils::ws2s(pChild.first);
		pChild.second->SaveToJson(jsonChild);
		json["ChildrenUI"].push_back(jsonChild);
	}
}

CUI* CCanvas::Find_ChildUI(const _tchar* pChildTag)
{
	auto	iter = m_mapChildUIs.find(pChildTag);
	if (iter == m_mapChildUIs.end())
		return nullptr;

	return iter->second;
}

CUI* CCanvas::Add_ChildUI(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pChildTag, void* pArg)
{
	if (Find_ChildUI(pChildTag))
	{
		IM_WARN("Child Tag is Duplicated");
		return nullptr;
	}

	CUI* pChildUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(iLevelIndex, pPrototypeTag, pArg));
	Assert(pChildUI != nullptr);

	m_mapChildUIs.emplace(pChildTag, pChildUI);

	return pChildUI;
}

void CCanvas::Free()
{
	__super::Free();

	for (auto& Pair : m_mapChildUIs)
		Safe_Release(Pair.second);
	m_mapChildUIs.clear();
}