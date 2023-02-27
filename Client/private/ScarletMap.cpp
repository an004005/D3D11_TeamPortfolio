#include "stdafx.h"
#include "..\public\ScarletMap.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "MapObject.h"
#include "GameUtils.h"

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
	if (LEVEL_NOW != LEVEL_MAPTOOL) return;

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

			char pStr[MAX_PATH];
			strcpy(pStr, CGameUtils::GetFileName(ws2s(proto)).c_str());

			if (ImGui::Selectable(pStr, bSelected))
			{
				m_pModelProtoTag = proto;
			}
		}

		ImGui::EndListBox();
	}

	ImGui::Separator();

	static _bool DoInput = false;
	static _float4 InitPosition = { 0.f, 0.f, 0.f, 1.f };
	static _float3 Interval = { 0.f, 0.f, 0.f };
	ImGui::Checkbox("InputPosition", &DoInput);

	if (DoInput)
	{
		ImGui::InputFloat3("SetInitPosition", (float*)&InitPosition);
		ImGui::InputFloat3("SetIntermal", (float*)&Interval);
	}
	
	if (ImGui::Button("Create_MapNonAnim_Object"))
	{	
		Json json;
		json["ModelTag"] = ws2s(m_pModelProtoTag);
	
		InitPosition.x += Interval.x;
		InitPosition.y += Interval.y;
		InitPosition.z += Interval.z;

		_float4 InitPos = DoInput == false ? SetUp_InitPosition() : InitPosition;
		json["InitPos"] = InitPos;
		FAILED_CHECK(pGameInstance->Clone_GameObject(TEXT("Layer_MapNonAnimObject"), TEXT("Prototype_GameObject_MapNonAnim_Object"), &json));
	}

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
				strcpy(pStr, CGameUtils::GetFileName(str).c_str());
				sprintf_s(pStr, sizeof(pStr), "%s %d", pStr, iCount);
				
				if (ImGui::Selectable(pStr, bSelected))
				{
					m_pGameObject = obj;
				}
				++iter;
			}
			ImGui::EndListBox();
		}
	}

	ImGui::Separator();


	if (ImGui::Button("Delete_MapNonAnim_Object"))
	{
		if (m_pGameObject)
		{
			m_pGameObject->SetDelete();
			m_pGameObject = nullptr;
		}
	}

	ImGui::Separator();

	if(ImGui::Button("Clear Map"))
		ClearMap();

	ImGui::BeginChild("Selected Object", { 500.f, 200.f });

	if (m_pGameObject)
	{
		ImGui::Separator();
		ImGui::Text("%s", typeid(*m_pGameObject).name());
		m_pGameObject->Imgui_RenderProperty();
		m_pGameObject->Imgui_RenderComponentProperties();
	}

	ImGui::EndChild();
}

void CScarletMap::SaveToJson(OUT Json & json)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CLayer* pLayer = pGameInstance->GetLayer(LEVEL_NOW, TEXT("Layer_MapNonAnimObject"));

	if (pLayer == nullptr) return;

	__super::SaveToJson(json);

	auto pGameObjects = pLayer->GetGameObjects();

	json["MapObjects"] = Json::array();

	for (auto& obj : pGameObjects)
	{
		Json jsonMapObject;
		obj->SaveToJson(jsonMapObject);
		json["MapObjects"].push_back(jsonMapObject);
	}
}

void CScarletMap::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (json.contains("ProtoTags"))
	{
		for (auto prototag : json["ProtoTags"])
		{
			wstring tag = s2ws(prototag);
			m_pModelProtos.emplace_back(tag);
		}
	}

	if (json.contains("MapObjects"))
	{
		for (auto mapObj : json["MapObjects"])
		{
			FAILED_CHECK(pGameInstance->Clone_GameObject(TEXT("Layer_MapNonAnimObject"), TEXT("Prototype_GameObject_MapNonAnim_Object"), &mapObj));
		}
	}
	
}

HRESULT CScarletMap::SetUp_Components()
{
	return S_OK;
}

_float4 CScarletMap::SetUp_InitPosition()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	_matrix CamWorldMatrix = pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);

	_vector vCamLook = CamWorldMatrix.r[2];
	_vector vCamPos = CamWorldMatrix.r[3];

	_float4 vInitPos;
	XMStoreFloat4(&vInitPos, XMVectorSetW(vCamPos + XMVector3Normalize(vCamLook) * 20.f, 1.f));

	return vInitPos;
}

void CScarletMap::ClearMap()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	auto pLayer = pGameInstance->GetLayer(LEVEL_NOW, TEXT("Layer_MapNonAnimObject"));

	auto& MapObjects = pLayer->GetGameObjects();

	for (auto& obj : MapObjects)
		obj->SetDelete();
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
