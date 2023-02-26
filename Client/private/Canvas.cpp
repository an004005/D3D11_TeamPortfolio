#include "stdafx.h"
#include "..\public\Canvas.h"
#include "GameInstance.h"
#include "JsonLib.h"
#include "JsonStorage.h"

CCanvas::CCanvas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CCanvas::CCanvas(const CCanvas& rhs)
	: CUI(rhs)
{
}

HRESULT CCanvas::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CCanvas::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fSizeX = _float(g_iWinSizeX);
	m_fSizeY = _float(g_iWinSizeY);

	m_bVisible = false;

	return S_OK;
}

void CCanvas::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	for (auto iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end();)
	{
		const auto ChildPair = *iter;

		if (ChildPair.second->IsDeleted())
		{
			ChildPair.second->Release();
			iter = m_mapChildUIs.erase(iter);
		}
		else
		{
			ChildPair.second->Tick(TimeDelta);
			++iter;
		}
	}
}

void CCanvas::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	vector<CUI*> tmpUI;
	tmpUI.reserve(m_mapChildUIs.size());

	for (const auto& Pair : m_mapChildUIs)
		tmpUI.push_back(Pair.second);

	std::sort(tmpUI.begin(), tmpUI.end(), [](const CUI* left, const CUI* right)
	{
		return left->Get_Priority() < right->Get_Priority();
	});

	for (auto pUI : tmpUI)
		pUI->Late_Tick(TimeDelta);
}

void CCanvas::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	ImGui::Separator();
	static char szChildProtoName[MAX_PATH]{};
	ImGui::InputText("ChildProtoTag", szChildProtoName, MAX_PATH);
	static char szChildName[MAX_PATH]{};
	ImGui::InputText("ChildName", szChildName, MAX_PATH);

	if (ImGui::Button("Add Empty UI"))	// 캔버스 안 에서 유아이를 생성한다.
	{
		wstring childTag = s2ws(szChildName);
		wstring childPrototypeTag = s2ws(szChildProtoName);

		if (Find_ChildUI(childTag.c_str()) == nullptr && childTag.empty() == false && childPrototypeTag.empty() == false)
			Add_ChildUI(LEVEL_NOW, childPrototypeTag.c_str(), childTag.c_str());
	}

	if (ImGui::BeginListBox("UI List"))	// 캔버스 안 에서 생성한 유아이를 확인한다.
	{
	
		for (auto& UI : m_mapChildUIs)
		{
			const bool bSelected = (UI.second == m_pUI);

			if (bSelected)
				ImGui::SetItemDefaultFocus();

			char pStr[64];
			wc2c(UI.first.c_str(), pStr);

			if (ImGui::Selectable(pStr, bSelected))
			{
				m_pUI = UI.second;
			}
		}

		ImGui::EndListBox();
	}

	ImGui::BeginChild("ChildUIProperty", { 500.f, 500.f }, true);	// 유아이의 정보를 확인 및 수정 가능하다.
	if (m_pUI)
	{
		ImGui::Separator();
		ImGui::Text("%s", typeid(*m_pUI).name());
		m_pUI->Imgui_RenderProperty();
	}
	ImGui::EndChild();
}

void CCanvas::SaveToJson(Json & json)
{
	__super::SaveToJson(json);
	json["ChildrenUI"] = Json::array();

	for (auto pChild : m_mapChildUIs)
	{
		Json jsonChild;
		jsonChild["ChildTag"] = ws2s(pChild.first);
		pChild.second->SaveToJson(jsonChild);
		json["ChildrenUI"].push_back(jsonChild);
	}
}

void CCanvas::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	for (auto& childJson : json["ChildrenUI"])
	{
		string protoTag = childJson["Prototype_GameObject"];
		string childTag = childJson["ChildTag"];
		auto pUI = Add_ChildUI(LEVEL_NOW, s2ws(protoTag).c_str(),s2ws(childTag).c_str(), (void*)&childJson);
		Assert(pUI != nullptr);
	}
}

CUI * CCanvas::Find_ChildUI(const _tchar * pChildTag)
{
	auto	iter = m_mapChildUIs.find(pChildTag);
	if (iter == m_mapChildUIs.end())
		return nullptr;

	return iter->second;
}

CUI * CCanvas::Add_ChildUI(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pChildTag, void * pArg)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CGameObject * pGameObject = pGameInstance->Find_Prototype(LEVEL_NOW, pPrototypeTag);
	if (nullptr == pGameObject)
	{
		MSG_BOX("Not Found");
		return nullptr;
	}

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

CCanvas * CCanvas::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas*		pInstance = new CCanvas(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDefaultUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CCanvas::Clone(void * pArg)
{
	CCanvas*		pInstance = new CCanvas(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDefaultUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas::Free()
{
	__super::Free();

	for (auto& Pair : m_mapChildUIs)
		Safe_Release(Pair.second);
	m_mapChildUIs.clear();
}
