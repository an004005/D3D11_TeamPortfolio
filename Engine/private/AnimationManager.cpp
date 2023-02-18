#include "stdafx.h"
#include "..\public\AnimationManager.h"
#include "Animation.h"
#include "GameUtils.h"

IMPLEMENT_SINGLETON(CAnimationManager)

HRESULT CAnimationManager::LoadAnimation(const char* pAnimPath)
{
	CAnimation* pAnim = CAnimation::Create(pAnimPath);
	if (pAnim)
	{
		m_mapAnims.emplace(pAnim->GetName(), pAnim);
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CAnimationManager::LoadAnimations(const char* pAnimDirPath)
{
	CGameUtils::ListFilesRecursive(pAnimDirPath, [this](const string& fileName)
	{
		static const vector<string> Exts{".anim"};
		if (CGameUtils::CheckExt(fileName.c_str(), Exts))
		{
			IM_LOG("Load Animation : %s", fileName.c_str());
			LoadAnimation(fileName.c_str());
		}
	});

	return S_OK;
}

CAnimation* CAnimationManager::GetAnimation(const string& strAnimName)
{
	const auto itr = m_mapAnims.find(strAnimName);
	if (itr != m_mapAnims.end())
		return itr->second;

	return nullptr;
}

CAnimation* CAnimationManager::GetAnimation(const char* pAnimName)
{
	const auto itr = m_mapAnims.find(pAnimName);
	if (itr != m_mapAnims.end())
		return itr->second;

	return nullptr;
}

void CAnimationManager::Free()
{
	for (auto& Pair : m_mapAnims)
	{
		Safe_Release(Pair.second);
	}
	m_mapAnims.clear();
}
