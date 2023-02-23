#include "stdafx.h"
#include "..\public\Canvas.h"
#include "GameInstance.h"
#include "JsonLib.h"
#include "JsonStorage.h"

#include "DefaultUI.h"

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
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

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

	for (const auto& Pair : m_mapChildUIs)
		Pair.second->Late_Tick(TimeDelta);
}

void CCanvas::Imgui_RenderProperty()
{
	static char szChildProtoName[MAX_PATH]{};/* = "Default_UI"*/
	ImGui::InputText("ChildProtoTag", szChildProtoName, MAX_PATH);
	static char szChildName[MAX_PATH]{};
	ImGui::InputText("ChildName", szChildName, MAX_PATH);
	ImGui::SameLine();

	if (ImGui::Button("Add Empty UI"))
	{
		wstring childTag = s2ws(szChildName);
		wstring childPrototypeTag = s2ws(szChildProtoName);

		if (Find_ChildUI(childTag.c_str()) == nullptr && childTag.empty() == false && childPrototypeTag.empty() == false)
			Add_ChildUI(LEVEL_NOW, childPrototypeTag.c_str(), childTag.c_str());
	}

	for (const auto& Pair : m_mapChildUIs)
		Pair.second->Imgui_RenderProperty();

	/*
	1. 이 캔버스에 넣고 싶은 UI(캔버스 포함)의 프로트타입 태크를 입력받는다.
	2. 입력받은 이름을 클론하고 바로 리스트에 넣는다.
	3. 끝
	*/


	//CGameObject::SetDelete();
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
	if (Find_ChildUI(pChildTag))
	{
		IM_WARN("Child Tag is Duplicated");
		return nullptr;
	}

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/UI/UI_Dafault.json");
	CUI* pChildUI = dynamic_cast<CUI*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(iLevelIndex, pPrototypeTag, (void*)&json));
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
