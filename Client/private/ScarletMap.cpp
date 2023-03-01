#include "stdafx.h"
#include "..\public\ScarletMap.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "MapObject.h"
#include "GameUtils.h"
#include "MapInstance_Object.h"

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
				if (proto.first.find(strSearch) == wstring::npos)
					continue;
			}

			const bool bSelected = (proto.first == m_pModelProtoInfo.first);

			if (bSelected)
				ImGui::SetItemDefaultFocus();

			char pStr[MAX_PATH];
			strcpy(pStr, CGameUtils::GetFileName(ws2s(proto.first)).c_str());

			if (ImGui::Selectable(pStr, bSelected))
			{
				m_pModelProtoInfo = proto;

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
		if (m_pModelProtoInfo.second == PROTOINFO::NON_INSTANCE)
		{
			Json json;
			json["ModelTag"] = ws2s(m_pModelProtoInfo.first);

			InitPosition.x += Interval.x;
			InitPosition.y += Interval.y;
			InitPosition.z += Interval.z;

			_float4 InitPos = DoInput == false ? SetUp_InitPosition() : InitPosition;
			json["InitPos"] = InitPos;

			CMapObject* pMapObject = nullptr;
			pMapObject = dynamic_cast<CMapObject*>(pGameInstance->Clone_GameObject_Get(TEXT("Layer_MapNonAnimObject"), TEXT("Prototype_GameObject_MapNonAnim_Object"), &json));
			assert(pMapObject != nullptr);

			m_pMapObjects.emplace_back(pMapObject);
			m_pGameObject = pMapObject;
		}
		else
		{
			MSG_BOX("Wrong ModelMatch");
		}
	}

	//같은 모델이 있는지 확인 후, 있으면 추가, 없으면 생성 후 추가
	if (ImGui::Button("Create_MapInstance_Object"))
	{
		if (m_pModelProtoInfo.second == PROTOINFO::INSTANCE)
		{
			CGameObject* pGameObject = pGameInstance->Find_ObjectByPredicator(LEVEL_NOW, [this](CGameObject* pGameObject) {
				return dynamic_cast<CMapInstance_Object*>(pGameObject)->Get_ModelTag() == m_pModelProtoInfo.first;
			}, TEXT("Layer_MapInstanceObject"));

			if (pGameObject == nullptr) // ModelTag를 못찾으면 생성
			{
				Json json;
				json["ModelTag"] = ws2s(m_pModelProtoInfo.first);

				pGameObject = pGameInstance->Clone_GameObject_Get(TEXT("Layer_MapInstanceObject"), TEXT("Prototype_GameObject_MapInstance_Object"), &json);
				assert(pGameObject != nullptr);
				m_pMapObjects.emplace_back(dynamic_cast<CMapObject*>(pGameObject));
			}

			CModel_Instancing* pModel = dynamic_cast<CMapInstance_Object*>(pGameObject)->Get_Model_Instancing();
			assert(pModel != nullptr);

			InitPosition.x += Interval.x;
			InitPosition.y += Interval.y;
			InitPosition.z += Interval.z;

			_matrix		WorldMatrix = XMMatrixTranslation(InitPosition.x, InitPosition.y, InitPosition.z);
			pModel->Add_Instance(WorldMatrix);
			pModel->Map_Meshs();
			dynamic_cast<CMapInstance_Object*>(pGameObject)->Set_Focus();
		}
		else
		{
			MSG_BOX("Wrong ModelMatch");
		}
	}

	ImGui::Separator();

	static char szSearchObject[MAX_PATH] = "";
	ImGui::InputText("MapObject Search", szSearchObject, MAX_PATH);

	const wstring strObjSearch = s2ws(szSearchObject);
	const _bool bObjSearch = strObjSearch.empty() == false;

	if (ImGui::BeginListBox("MapObject List"))
	{
		for (size_t i = 0; i < m_pMapObjects.size(); ++i)
		{
			if (bObjSearch)
			{
				wstring szProtoTag = m_pMapObjects[i]->Get_ModelTag();
				if (szProtoTag.find(strObjSearch) == wstring::npos)
					continue;
			}

			const bool bSelected = (m_pMapObjects[i] == m_pGameObject);

			if (bSelected)
				ImGui::SetItemDefaultFocus();

			string str = ws2s(m_pMapObjects[i]->Get_ModelTag());

			char pStr[MAX_PATH]{};
			strcpy(pStr, CGameUtils::GetFileName(str).c_str());
			sprintf_s(pStr, sizeof(pStr), "%s %zd", pStr, i);

			if (ImGui::Selectable(pStr, bSelected))
			{
				m_pGameObject = m_pMapObjects[i];
			}

		}

		ImGui::EndListBox();
	}

	ImGui::Separator();


	if (ImGui::Button("Delete_MapNonAnim_Object"))
	{
		if (m_pModelProtoInfo.second == PROTOINFO::NON_INSTANCE)
		{
			if (m_pGameObject)
			{
				m_pGameObject->SetDelete();	
				m_pMapObjects.erase(remove(m_pMapObjects.begin(), m_pMapObjects.end(), m_pGameObject), m_pMapObjects.end());
				m_pGameObject = nullptr;
			}
		}
		else
		{
			MSG_BOX("Wrong ModelMatch");
		}
	}

	ImGui::Separator();

	if (ImGui::Button("Clear Map"))
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

	__super::SaveToJson(json);

	json["MapObjects"] = Json::array();

	for (auto& obj : m_pMapObjects)
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

	if (json.contains("Model_ProtoTypes"))
	{
		for (auto proto : json["Model_ProtoTypes"])
		{
			pair<string, PROTOINFO> Model_Info = proto;
			m_pModelProtos.emplace_back(s2ws(Model_Info.first), Model_Info.second);
		}
	}

	if (json.contains("MapObjects"))
	{
		for (auto mapObj : json["MapObjects"])
		{
			CreateMapObjectFromLoad(mapObj);
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

void CScarletMap::CreateMapObjectFromLoad(Json & json)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CMapObject* pMapObject = nullptr;

	string Prototag = json["Prototype_GameObject"];

	if (0 == strcmp(Prototag.c_str(), "Prototype_GameObject_MapNonAnim_Object"))
	{
		pMapObject = dynamic_cast<CMapObject*>(pGameInstance->Clone_GameObject_Get(TEXT("Layer_MapNonAnimObject"), TEXT("Prototype_GameObject_MapNonAnim_Object"), &json));
	} 

	else if (0 == strcmp(Prototag.c_str(), "Prototype_GameObject_MapInstance_Object"))
	{	
		pMapObject = dynamic_cast<CMapObject*>(pGameInstance->Clone_GameObject_Get(TEXT("Layer_MapInstanceObject"), TEXT("Prototype_GameObject_MapInstance_Object"), &json));
	}

	assert(pMapObject != nullptr);
	m_pMapObjects.emplace_back(pMapObject);
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
