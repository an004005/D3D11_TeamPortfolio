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
	FAILED_CHECK(__super::Initialize(nullptr));

	m_eComponents.fill(false);

	m_pModelProtos = *static_cast<map<const _tchar*, CGameObject*>*>(pArg);
	
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
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	ImGui::Text("Select Components");
	ImGui::NewLine();

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
				wstring szProtoTag = proto.first;
				if (szProtoTag.find(strSearch) == wstring::npos)
					continue;
			}
			const bool bSelected = (0 == lstrcmp(proto.first, m_pModelProtoTag));

			if (bSelected)
				ImGui::SetItemDefaultFocus();

			//wchar -> char
			char pStr[64];
			wc2c(proto.first, pStr);

			if (ImGui::Selectable(pStr, bSelected))
			{
				m_pModelProtoTag = proto.first;
			}
		}

		ImGui::EndListBox();
	}

	ImGui::Checkbox("Psycokinesis", &m_eComponents[PSYCOKINESIS]);

	ImGui::NewLine();

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
	ImGui::NewLine();

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

				//wchar -> char
				char pStr[64];
				wc2c(dynamic_cast<CMapObject*>(obj)->Get_ModelTag(), pStr);
		
				_uint iCount = count_if(pGameObjects.begin(), iter, [&](CGameObject* pGameObject) {
					return dynamic_cast<CMapObject*>(pGameObject)->Get_ModelTag() == dynamic_cast<CMapObject*>(obj)->Get_ModelTag();
				});

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


	if (m_pGameObject)
	{
		ImGui::Separator();
		ImGui::Text("%s", typeid(*m_pGameObject).name());
		m_pGameObject->Imgui_RenderProperty();
		m_pGameObject->Imgui_RenderComponentProperties();
	}
	
}

void CScarletMap::LoadFromJson(const Json & json)
{
}

void CScarletMap::LoadMap(const _tchar * pMapPath, LEVEL eLevel)
{
	//각 레벨에 필요한 오브젝트들의 프로토타입 생성
	SetUp_Prototypes_MapObject(eLevel);
}

void CScarletMap::SaveMap(const _tchar * pMapPath)
{
}

void CScarletMap::SetUp_Prototypes_MapObject(LEVEL eLevel)
{
	if (eLevel == LEVEL_MAPTOOL) return;

	if (eLevel == LEVEL_GAMEPLAY)
	{

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
