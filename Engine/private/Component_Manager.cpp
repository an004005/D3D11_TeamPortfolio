#include "stdafx.h"
#include "..\public\Component_Manager.h"
#include "Component.h"
#include "GameInstance.h"
#include "GameUtils.h"
#include "Graphic_Device.h"

IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Reserve_Manager(_uint iNumLevels)
{
	if (m_iNumLevels != 0)
		return E_FAIL;

	m_pPrototypes.resize(iNumLevels);

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CComponent_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	for (auto& protoPair : m_pPrototypes[iLevelIndex])
	{
		delete[] protoPair.first;
		Safe_Release(protoPair.second);
	}
	m_pPrototypes[iLevelIndex].clear();

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if ( iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	if (nullptr != Find_Prototype(iLevelIndex, pPrototypeTag))
		return E_FAIL;

	_tchar* pPrototypeTagCopy = new _tchar[lstrlen(pPrototypeTag) + 1];
	lstrcpy(pPrototypeTagCopy, pPrototypeTag);

	pPrototype->SetPrototypeTag(pPrototypeTagCopy);
	m_pPrototypes[iLevelIndex].emplace(pPrototypeTagCopy, pPrototype);

	return S_OK;
}

CComponent * CComponent_Manager::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	CComponent*		pPrototype = Find_Prototype(iLevelIndex, pPrototypeTag);
	if (nullptr == pPrototype)
	{
		// 클론할 때 못 찾으면 핸들링시도
		pPrototype = Handle_FindPrototypeFailed(iLevelIndex, pPrototypeTag);
		if (pPrototype == nullptr)
			return nullptr;
	}

	CComponent*		pComponent = pPrototype->Clone(pArg);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;	
}

wcmap<CComponent*>* CComponent_Manager::GetProtoTypes(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	return &m_pPrototypes[iLevelIndex];
}

CComponent * CComponent_Manager::Find_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = m_pPrototypes[iLevelIndex].find(pPrototypeTag);
	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

CComponent* CComponent_Manager::Handle_FindPrototypeFailed(_uint iLevelIndex, const _tchar* pPrototypeTag)
{
	// 1. 찾은 프로토타입이 스태틱인지 확인
	{
		const _uint iStaticLevel = LEVEL_STATIC;
		auto	iter = find_if(m_pPrototypes[iStaticLevel].begin(), m_pPrototypes[iStaticLevel].end(), CTag_Finder(pPrototypeTag));
		if (iter != m_pPrototypes[iStaticLevel].end())
			return iter->second;	
	}

	// 2. 찾는 프로토타입이 텍스쳐경로인지 확인
	if (CGameUtils::IsTexture(pPrototypeTag))
	{
		_uint iTexCnt = 0;
		_tchar	szTexturePath[MAX_PATH] = TEXT("");
		wsprintf(szTexturePath, pPrototypeTag, iTexCnt);
		if (!lstrcmp(szTexturePath, pPrototypeTag)) // 1장
			iTexCnt = 1;
		else
		{
			while (true)
			{
				wsprintf(szTexturePath, pPrototypeTag, iTexCnt);
				if (CGameUtils::FileExist(szTexturePath))
					++iTexCnt;
				else break;
			}
		}

		FAILED_CHECK(Add_Prototype(iLevelIndex, pPrototypeTag, 
			CTexture::Create(
				CGraphic_Device::GetInstance()->GetDevice(), 
				CGraphic_Device::GetInstance()->GetContext(),
				pPrototypeTag, 
				iTexCnt)))

		if (CComponent* pCom = Find_Prototype(iLevelIndex, pPrototypeTag))
			return pCom;
	}

	// 3. 찾는 프로토타입이 모델경로인지 확인
	static const vector<wstring> Exts{ L".static_model", L".anim_model" };
	if (CGameUtils::CheckExt(pPrototypeTag, Exts))
	{
		char szModelPath[MAX_PATH];
		CGameUtils::wc2c(pPrototypeTag, szModelPath);
	
		FAILED_CHECK(Add_Prototype(iLevelIndex, pPrototypeTag,
			CModel::Create(
				CGraphic_Device::GetInstance()->GetDevice(),
				CGraphic_Device::GetInstance()->GetContext(),
				szModelPath
			)));
	
		if (CComponent* pCom = Find_Prototype(iLevelIndex, pPrototypeTag))
			return pCom;
	}

	return nullptr;
}

void CComponent_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
		Clear(i);
}
