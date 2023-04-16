#include "stdafx.h"
#include "..\public\GameObject.h"
#include "GameInstance.h"
#include "GameUtils.h"
#include "ImguiUtils.h"
#include "JsonLib.h"
#include "Object_Manager.h"

const _tchar* CGameObject::m_pTransformComTag = TEXT("Com_Transform");

CGameObject::CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_bCloned(false)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	CObject_Manager::Add_Object(this);
}

CGameObject::CGameObject(const CGameObject & rhs) 
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_bCloned(true)
	, m_strPrototypeTag(rhs.m_strPrototypeTag)
	, m_strObjectTag(rhs.m_strObjectTag)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	CObject_Manager::Add_Object(this);
}

CGameObject::~CGameObject()
{
	CObject_Manager::Delete_Object(this);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void * pArg)
{
	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		LoadFromJson(json);
	}

	if (FAILED(Add_Component(LEVEL_STATIC, CGameInstance::m_pPrototypeTransformTag, m_pTransformComTag, (CComponent**)&m_pTransformCom, pArg)))
		return E_FAIL;

	return S_OK;
}

void CGameObject::BeginTick()
{
	for (const auto& Com : m_Components)
		Com.second->BeginTick();
}

void CGameObject::Tick(_double TimeDelta)
{
}

void CGameObject::Late_Tick(_double TimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

void CGameObject::SaveToJson(Json& json)
{
	json["Prototype_GameObject"] = CGameUtils::ws2s(m_strPrototypeTag);
	json["ObjectTag"] = m_strObjectTag;

	for (const auto& Com : m_Components)
		Com.second->SaveToJson(json);
}

void CGameObject::LoadFromJson(const Json& json)
{
	if (json.contains("ObjectTag"))
		m_strObjectTag = json["ObjectTag"];
}

void CGameObject::Imgui_RenderComponentProperties()
{
	for (const auto& com : m_Components)
	{
		char szName[256];
		CGameUtils::wc2c(com.first.c_str(), szName);

        if (ImGui::CollapsingHeader(szName))
			com.second->Imgui_RenderProperty();
	}
}

void CGameObject::Imgui_RenderProperty()
{
	char szObjectTag[MAX_PATH]{};
	strcpy_s(szObjectTag, MAX_PATH, m_strObjectTag.c_str());
	ImGui::InputText("ObjectTag", szObjectTag, MAX_PATH);
	if (strcmp(szObjectTag, m_strObjectTag.c_str()) != 0)
		m_strObjectTag = szObjectTag;

	if (ImGui::Button("Visibie"))
		m_bVisible = !m_bVisible;

	if (ImGui::Button("Delete"))
		m_bDelete = true;

	CImguiUtils::FileDialog_FileSelector("Save Object to", ".json", "../Bin/Resources/Objects/",
	[this](const string& filePath)
	{
		Json json;
		SaveToJson(json);
		std::ofstream file(filePath);
		file << json;
	});
}

void CGameObject::Compute_CamDistance()
{
	_vector		vWorldPos = m_pTransformCom->Get_WorldMatrix().r[3];
	_vector		vCamPos = XMLoadFloat4(&CPipeLine::GetInstance()->Get_CamPosition());
	m_fCamDistance = XMVectorGetX(XMVector3Length(vWorldPos - vCamPos));
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pComponentTag, CComponent** ppOut, void * pArg)
{
	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CComponent*	pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(pComponentTag, pComponent);

	Safe_AddRef(pComponent);

	pComponent->SetOwner(this);

	*ppOut = pComponent;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CGameObject::Delete_Component(const _tchar* pComponentTag)
{
	CComponent* pComponent = Find_Component(pComponentTag);
	if (pComponent)
	{
		m_Components.erase(pComponentTag);
		Safe_Release(pComponent);
		return S_OK;
	}

	return E_FAIL;
}

CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto	iter = m_Components.find(pComponentTag);

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free()
{
	Safe_Release(m_pTransformCom);


	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	m_Components.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
