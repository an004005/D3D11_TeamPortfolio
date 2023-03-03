#include "stdafx.h"
#include "..\public\UI_Manager.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
{
}

CCanvas * CUI_Manager::Find_CCanvas(const wstring& pCanvasTag)
{
	auto	iter = find_if(m_mapCanvass.begin(), m_mapCanvass.end(), [&](pair<wstring, CCanvas*> pCantag) {
		return pCantag.first == pCanvasTag;
	});

	if (iter == m_mapCanvass.end())
		return nullptr;

	return iter->second;
}

void CUI_Manager::Add_CCanvas(const wstring& pCanvasTag, CCanvas * pCanvas)
{
	m_mapCanvass.emplace(pCanvasTag, pCanvas);
}

void CUI_Manager::Free()
{
	for (auto& Pair : m_mapCanvass)
		Safe_Release(Pair.second);
	m_mapCanvass.clear();
}
