#include "stdafx.h"
#include "..\public\ScarletMap.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "MapObject.h"

CScarletMap::CScarletMap(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CScarletMap::CScarletMap(const CScarletMap & rhs)
	:CGameObject(rhs)
{
}

HRESULT CScarletMap::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CScarletMap::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));
	
	FAILED_CHECK(SetUp_Components());

	return S_OK;
}

void CScarletMap::BeginTick()
{
}

void CScarletMap::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CScarletMap::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CScarletMap::Render()
{
	FAILED_CHECK(__super::Render());

	return S_OK;
}

void CScarletMap::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	ImGui::Text("Select Components");
	ImGui::Separator();

	static char szSearchProto[MAX_PATH] = "";
	ImGui::InputText("ProtoType Search", szSearchProto, MAX_PATH);

	const wstring strSearch = s2ws(szSearchProto);
	const _bool bSearch = strSearch.empty() == false;

	if (ImGui::BeginListBox("ProtoType List"))
	{
		for (auto& proto : m_pModelProtos)
		{
			if (bSearch)
			{
				if (proto.find(strSearch) == wstring::npos)
					continue;
			}

			const bool bSelected = (proto == m_pModelProtoTag);

			if (bSelected)
				ImGui::SetItemDefaultFocus();

			
			char pStr[64];
			wc2c(proto.c_str(), pStr);

			if (ImGui::Selectable(pStr, bSelected))
			{
				m_pModelProtoTag = proto;
			}
		}

		ImGui::EndListBox();
	}

	ImGui::Separator();

	if (ImGui::Button("Create_MapNonAnim_Object"))
	{	
		Json json;
		json["ModelTag"] = ws2s(m_pModelProtoTag);
		FAILED_CHECK(pGameInstance->Clone_GameObject(TEXT("Layer_MapNonAnimObject"), TEXT("Prototype_GameObject_MapNonAnim_Object"), &json));
	}

	if (ImGui::Button("Delete_MapNonAnim_Object"))
	{
		if (m_pGameObject)
		{
			m_pGameObject->SetDelete();
			m_pGameObject = nullptr;
		}
	}

	ImGui::NewLine();
	ImGui::Separator();

	CLayer* pLayer = pGameInstance->GetLayer(LEVEL_NOW, TEXT("Layer_MapNonAnimObject"));
	
	if (pLayer != nullptr)
	{
		auto pGameObjects = pLayer->GetGameObjects();

		static char szSearchObject[MAX_PATH] = "";
		ImGui::InputText("NonAnimObject Search", szSearchObject, MAX_PATH);

		const wstring strSearch = s2ws(szSearchObject);
		const _bool bSearch = strSearch.empty() == false;

		if (ImGui::BeginListBox("NonAnimObject List"))
		{
			std::list<CGameObject*>::iterator iter = pGameObjects.begin();

			for (auto& obj : pGameObjects)
			{
				if (bSearch)
				{
					wstring szProtoTag = dynamic_cast<CMapObject*>(obj)->Get_ModelTag();
					if (szProtoTag.find(strSearch) == wstring::npos)
						continue;
				}

				const bool bSelected = (obj == m_pGameObject);

				if (bSelected)
					ImGui::SetItemDefaultFocus();

				string str = ws2s(dynamic_cast<CMapObject*>(obj)->Get_ModelTag());
		
				_uint iCount = count_if(pGameObjects.begin(), iter, [&](CGameObject* pGameObject) {
					return dynamic_cast<CMapObject*>(pGameObject)->Get_ModelTag() == dynamic_cast<CMapObject*>(obj)->Get_ModelTag();
				});

				char pStr[MAX_PATH]{};
				sprintf_s(pStr, sizeof(pStr), "%s %d", str.c_str(), iCount);
				
				if (ImGui::Selectable(pStr, bSelected))
				{
					m_pGameObject = obj;
				}

				++iter;
			}
			ImGui::EndListBox();
		}
	}

	if (m_pGameObject)
	{
		ImGui::Separator();
		ImGui::Text("%s", typeid(*m_pGameObject).name());
		m_pGameObject->Imgui_RenderProperty();
		m_pGameObject->Imgui_RenderComponentProperties();
	}
}

void CScarletMap::SaveToJson(OUT Json & json)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CLayer* pLayer = pGameInstance->GetLayer(LEVEL_NOW, TEXT("Layer_MapNonAnimObject"));

	if (pLayer == nullptr) return;

	__super::SaveToJson(json);

	auto pGameObjects = pLayer->GetGameObjects();

	for (auto& obj : pGameObjects)
	{
		json["ModelTags"].push_back(ws2s(dynamic_cast<CMapObject*>(obj)->Get_ModelTag()));
	}
}

void CScarletMap::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	for (auto prototag : json["ProtoTags"])
	{
		wstring tag = s2ws(prototag);
		m_pModelProtos.emplace_back(tag);
	}

	if (json.contains("ModelTags"))
	{
		for (auto Modeltag : json["ModelTags"])
		{
			Json json;
			json["ModelTag"] = Modeltag;
			FAILED_CHECK(pGameInstance->Clone_GameObject(TEXT("Layer_MapNonAnimObject"), TEXT("Prototype_GameObject_MapNonAnim_Object"), &json));
		}
	}
	
}

HRESULT CScarletMap::SetUp_Components()
{
	return S_OK;
}


CScarletMap * CScarletMap::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CScarletMap*		pInstance = new CScarletMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CScarletMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CScarletMap::Clone(void* pArg)
{
	CScarletMap*		pInstance = new CScarletMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CScarletMap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CScarletMap::Free()
{
	__super::Free();
}
