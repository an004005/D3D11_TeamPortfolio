#include "stdafx.h"
#include "..\public\UI_Manager.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
{
}

CCanvas * CUI_Manager::Find_MoveCanvas(const wstring& pCanvasTag)
{
	auto	iter = find_if(m_mapMoveCanvass.begin(), m_mapMoveCanvass.end(), [&](pair<wstring, CCanvas*> pCantag) {
		return pCantag.first == pCanvasTag;
	});

	if (iter == m_mapMoveCanvass.end())
		return nullptr;

	return iter->second;
}

void CUI_Manager::Add_MoveCanvas(const wstring& pCanvasTag, CCanvas * pCanvas)
{
	m_mapMoveCanvass.emplace(pCanvasTag, pCanvas);
}

CCanvas * CUI_Manager::Find_Canvas(const wstring & pCanvasTag)
{
	auto	iter = find_if(m_mapCanvass.begin(), m_mapCanvass.end(), [&](pair<wstring, CCanvas*> pCantag) {
		return pCantag.first == pCanvasTag;
	});

	if (iter == m_mapCanvass.end())
		return nullptr;

	return iter->second;
}

void CUI_Manager::Add_Canvas(const wstring & pCanvasTag, CCanvas * pCanvas)
{
	m_mapCanvass.emplace(pCanvasTag, pCanvas);
}

void CUI_Manager::Free()
{
	for (auto& Pair : m_mapMoveCanvass)
		Safe_Release(Pair.second);
	m_mapMoveCanvass.clear();
}
