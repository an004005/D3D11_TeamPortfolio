#include "stdafx.h"
#include "..\public\ScarletMap.h"
#include "GameInstance.h"
#include "JsonStorage.h"

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

	m_pMapProtos = *static_cast<map<const _tchar*, CGameObject*>*>(pArg);

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
	ImGui::Text("Select Components");
	ImGui::NewLine();

	if (ImGui::BeginListBox("Component List"))
	{
		for (auto& proto : m_pMapProtos)
		{
			const bool bSelected = (0 == lstrcmp(proto.first, m_pMapProtoTag));

			if (bSelected)
				ImGui::SetItemDefaultFocus();

			//wchar -> char
			char pStr[64];
			wc2c(proto.first, pStr);

			if (ImGui::Selectable(pStr, bSelected))
			{
				m_pMapProtoTag = proto.first;
			}
		}

		ImGui::EndListBox();
	}

	ImGui::Checkbox("Psycokinesis", &m_eComponents[PSYCOKINESIS]);

	ImGui::NewLine();
	if (ImGui::Button("Create_MapNonAnim_Object"))
		Create_MapNonAnim_Object();


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

void CScarletMap::Create_MapNonAnim_Object()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	FAILED_CHECK(pGameInstance->Clone_GameObject(TEXT("Layer_MapNonAnimObject"), TEXT("Prototype_GameObject_MapNonAnim_Object")));
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
