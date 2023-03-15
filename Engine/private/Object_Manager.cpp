#include "stdafx.h"
#include "..\public\Object_Manager.h"
#include "Layer.h"
#include "GameObject.h"
#include "GameUtils.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CObject_Manager)

unordered_set<CGameObject*> CObject_Manager::s_AliveObjects{};


CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Reserve_Manager(_uint iNumLevels)
{
	if (m_iNumLevels != 0)
		return E_FAIL;

	m_vecPrototypes.resize(iNumLevels);
	m_vecBeginTicks.resize(iNumLevels);
	m_vecLayers.resize(iNumLevels);
	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObject_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	for (auto& Pair : m_vecLayers[iLevelIndex])
	{
		delete[] Pair.first; // map툴로 layer를 생성할 수 있기 때문에 동적할당한 캐릭터를 사용
		Safe_Release(Pair.second);
	}

	m_vecLayers[iLevelIndex].clear();

	for (auto& Pair : m_vecPrototypes[iLevelIndex])
	{
		delete[] Pair.first;
		Safe_Release(Pair.second);
	}

	m_vecPrototypes[iLevelIndex].clear();

	m_vecBeginTicks[iLevelIndex].clear();

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr != Find_Prototype(iLevelIndex, pPrototypeTag))
		return E_FAIL;

	_tchar* pPrototypeTagCopy = new _tchar[lstrlen(pPrototypeTag) + 1];
	lstrcpy(pPrototypeTagCopy, pPrototypeTag);

	pPrototype->SetPrototypeTag(pPrototypeTagCopy);
	m_vecPrototypes[iLevelIndex].emplace(pPrototypeTagCopy, pPrototype);
	
	return S_OK;
}

HRESULT CObject_Manager::Clone_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
{
	CGameObject*		pPrototype = Find_Prototype(iLevelIndex, pPrototypeTag);
	if (nullptr == pPrototype)
	{
		// clone할 때 못 찾으면 static도 찾아본다.
		pPrototype = Find_Prototype(LEVEL_STATIC, pPrototypeTag);
		if (nullptr == pPrototype)
			return E_FAIL;
	}

	CGameObject*		pCloned = pPrototype->Clone(pArg);

	CLayer*		pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		if (FAILED(pLayer->Add_GameObject(pCloned)))
			return E_FAIL;

		_tchar* pLayerTagCopy = new _tchar[lstrlen(pLayerTag) + 1];
		lstrcpy(pLayerTagCopy, pLayerTag);

		m_vecLayers[iLevelIndex].emplace(pLayerTagCopy, pLayer);
	}
	else
		pLayer->Add_GameObject(pCloned);

	m_vecBeginTicks[iLevelIndex].push_back(pCloned);

	return S_OK;
}

CGameObject* CObject_Manager::Clone_GameObject_Get(_uint iLevelIndex, const _tchar* pLayerTag,
	const _tchar* pPrototypeTag, void* pArg)
{
	CGameObject*		pPrototype = Find_Prototype(iLevelIndex, pPrototypeTag);
	if (nullptr == pPrototype)
	{
		// clone할 때 못 찾으면 static도 찾아본다.
		pPrototype = Find_Prototype(LEVEL_STATIC, pPrototypeTag);
		if (nullptr == pPrototype)
			return nullptr;
	}

	CGameObject*		pCloned = pPrototype->Clone(pArg);

	CLayer*		pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		Assert(pLayer != nullptr);

		FAILED_CHECK(pLayer->Add_GameObject(pCloned));

		_tchar* pLayerTagCopy = new _tchar[lstrlen(pLayerTag) + 1];
		lstrcpy(pLayerTagCopy, pLayerTag);

		m_vecLayers[iLevelIndex].emplace(pLayerTagCopy, pLayer);
	}
	else
		pLayer->Add_GameObject(pCloned);

	m_vecBeginTicks[iLevelIndex].push_back(pCloned);

	return pCloned;
}

CGameObject* CObject_Manager::Clone_GameObject_NoLayer(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg)
{
	CGameObject*		pPrototype = Find_Prototype(iLevelIndex, pPrototypeTag);
	if (nullptr == pPrototype)
	{
		// clone할 때 못 찾으면 static도 찾아본다.
		pPrototype = Find_Prototype(LEVEL_STATIC, pPrototypeTag);
		if (nullptr == pPrototype)
			return nullptr;
	}

	auto pCloned = pPrototype->Clone(pArg);
	m_vecBeginTicks[iLevelIndex].push_back(pCloned);

	return pCloned;
}

wcmap<class CLayer*>& CObject_Manager::GetLayers(_uint iLevelIndex)
{
	Assert(iLevelIndex < m_iNumLevels);
	return m_vecLayers[iLevelIndex];
}

CLayer* CObject_Manager::GetLayer(_uint iLevelIndex, const _tchar* pLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	return Find_Layer(iLevelIndex, pLayerTag);
}

CGameObject* CObject_Manager::Find_ObjectByPredicator(_uint iLevelIndex, std::function<_bool(CGameObject*)> Pred, const _tchar* pLayerTag)
{
	if (iLevelIndex >= m_iNumLevels || Pred == nullptr)
		return nullptr;

	if (pLayerTag == nullptr)
	{
		for (const auto& layerPair : m_vecLayers[iLevelIndex])
		{
			for (const auto& obj : layerPair.second->GetGameObjects())
			{
				if (Pred(obj))
				{
					return obj;
				}
			}
		}
	}

	else
	{
		CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);

		if (pLayer == nullptr)
			return nullptr;

		for (const auto& obj : pLayer->GetGameObjects())
		{
			if (Pred(obj))
			{
				return obj;
			}
		}
	}

	return nullptr;
}

list<CGameObject*> CObject_Manager::Find_AllObjectByPredicator(_uint iLevelIndex, std::function<_bool(CGameObject*)> Pred)
{
	list<CGameObject*> outs;

	if (iLevelIndex >= m_iNumLevels || Pred == nullptr)
		return outs;

	for (const auto& layerPair : m_vecLayers[iLevelIndex])
	{
		for (const auto& obj : layerPair.second->GetGameObjects())
		{
			if (Pred(obj))
			{
				outs.push_back(obj);
			}
		}
	}

	return outs;
}


void CObject_Manager::Tick(_double TimeDelta)
{
	if (!m_vecBeginTicks[m_iUpdatedLevel].empty())
	{
		for (const auto pGameObject : m_vecBeginTicks[m_iUpdatedLevel])
			pGameObject->BeginTick();
		m_vecBeginTicks[m_iUpdatedLevel].clear();
	}

	for (auto& Pair : m_vecLayers[m_iUpdatedLevel])
	{
		if(nullptr != Pair.second)
			Pair.second->Tick(TimeDelta);			
	}
}

void CObject_Manager::Late_Tick(_double TimeDelta)
{
	for (auto& Pair : m_vecLayers[m_iUpdatedLevel])
	{
		if (nullptr != Pair.second)
			Pair.second->Late_Tick(TimeDelta);
	}
}

void CObject_Manager::AfterPhysX()
{
	for (auto& Pair : m_vecLayers[m_iUpdatedLevel])
	{
		if (nullptr != Pair.second)
			Pair.second->AfterPhysX();
	}
}

void CObject_Manager::Imgui_ProtoViewer(const _tchar*& szSelectedProto)
{
	if (ImGui::TreeNode("ProtoViewer"))
	{
		if (ImGui::TreeNode("Current Level"))
		{
			if (ImGui::BeginListBox("##"))
			{
				for (auto& ProtoPair : m_vecPrototypes[LEVEL_NOW])
				{
					const bool bSelected = (szSelectedProto != nullptr) && (0 == lstrcmp(ProtoPair.first, szSelectedProto));
					if (bSelected)
						ImGui::SetItemDefaultFocus();

					char szViewName[512], szProtoName[256];
					CGameUtils::wc2c(ProtoPair.first, szProtoName);
					sprintf_s(szViewName, "%s [%s]", szProtoName, typeid(*ProtoPair.second).name());
					if (ImGui::Selectable(szViewName, bSelected))
						szSelectedProto = ProtoPair.first;
				}
				
				ImGui::EndListBox();
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Static Level"))
		{
			if (ImGui::BeginListBox("##"))
			{
				for (auto& ProtoPair : m_vecPrototypes[LEVEL_STATIC])
				{
					const bool bSelected = (szSelectedProto != nullptr) && (0 == lstrcmp(ProtoPair.first, szSelectedProto));
					if (bSelected)
						ImGui::SetItemDefaultFocus();

					char szViewName[512], szProtoName[256];
					CGameUtils::wc2c(ProtoPair.first, szProtoName);
					sprintf_s(szViewName, "%s [%s]", szProtoName, typeid(*ProtoPair.second).name());
					if (ImGui::Selectable(szViewName, bSelected))
						szSelectedProto = ProtoPair.first;
				}

				ImGui::EndListBox();
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

void CObject_Manager::Imgui_ObjectViewer(_uint iLevel, CGameObject*& pSelectedObject)
{
	if (m_iNumLevels <= iLevel)
		return;

	const LAYERS& targetLevel = m_vecLayers[iLevel];

	if (ImGui::TreeNode("ObjectViewer"))
	{
		for (auto& Pair : targetLevel) // for layer loop
		{
			char szLayerTag[256];
			CGameUtils::wc2c(Pair.first, szLayerTag);
			if (ImGui::TreeNode(szLayerTag))  // for object loop listbox
			{
				if (ImGui::BeginListBox("##"))
				{
					for (auto& obj : Pair.second->GetGameObjects())
					{
						const bool bSelected = pSelectedObject == obj;
						if (bSelected) 
						{
							ImGui::SetItemDefaultFocus();
						}

						char szAddressName[256];
						sprintf_s(szAddressName, "%s[%p]", typeid(*obj).name(), obj);
						if (ImGui::Selectable(szAddressName, bSelected))
						{
							pSelectedObject = obj;
						}
					}
					ImGui::EndListBox();
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	// if (bFound == false)
		// pSelectedObject = nullptr;
}

void CObject_Manager::Imgui_LayerCombo(_uint iLevel, const _tchar*& pLayerName)
{
	if (ImGui::BeginCombo("Layer Names", "##"))
	{
		for (const auto& LayerPair : m_vecLayers[iLevel])
		{
			char szLayerBuff[256];
			CGameUtils::wc2c(LayerPair.first, szLayerBuff);
			const bool bSelected = false;
			if (ImGui::Selectable(szLayerBuff, bSelected))
				pLayerName = LayerPair.first;
		}

		ImGui::EndCombo();
	}
}

void CObject_Manager::Add_Object(CGameObject* pObject)
{
	Assert(pObject != nullptr);
	s_AliveObjects.insert(pObject);
}

void CObject_Manager::Delete_Object(CGameObject* pObject)
{
	Assert(pObject != nullptr);
	s_AliveObjects.erase(pObject);
}

_bool CObject_Manager::Check_ObjectAlive(CGameObject* pObject)
{
	return pObject != nullptr && s_AliveObjects.find(pObject) != s_AliveObjects.end();
}

CGameObject * CObject_Manager::Find_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag)
{
	auto	iter = m_vecPrototypes[iLevelIndex].find(pPrototypeTag);
	if (iter == m_vecPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Add_InLayer(const _tchar * pLayerTag, CGameObject * pGameObject)
{
	auto iter = Find_Layer(LEVEL_NOW, pLayerTag);

	if (iter == nullptr)
	{
		CLayer* pLayer = nullptr;
		pLayer = CLayer::Create();
		Assert(pLayer != nullptr);

		FAILED_CHECK(pLayer->Add_GameObject(pGameObject));

		_tchar* pLayerTagCopy = new _tchar[lstrlen(pLayerTag) + 1];
		lstrcpy(pLayerTagCopy, pLayerTag);

		m_vecLayers[LEVEL_NOW].emplace(pLayerTagCopy, pLayer);
	}
	else
	{
		FAILED_CHECK(iter->Add_GameObject(pGameObject));
	}
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	auto	iter = m_vecLayers[iLevelIndex].find(pLayerTag);
	if (iter == m_vecLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
		Clear(i);

}

