#include "stdafx.h"
#include "..\public\UI_Manager.h"
#include "Canvas.h"

#include "Canvas_Acquisition.h"

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

void CUI_Manager::Set_TempOff(const _bool bOff)
{
	for (map<wstring, CCanvas*>::iterator iter = m_mapMoveCanvass.begin(); iter != m_mapMoveCanvass.end(); ++iter)
		iter->second->TempOff(bOff);

	for (map<wstring, CCanvas*>::iterator iter = m_mapCanvass.begin(); iter != m_mapCanvass.end(); ++iter)
		iter->second->TempOff(bOff);

	// 객체 내 에서 따로 UI를 보관하기 때문에
	dynamic_cast<CCanvas_Acquisition*>(Find_Canvas(L"Canvas_Acquisition"))->Set_Visible();
}

void CUI_Manager::Free()
{
	for (auto& Pair : m_mapMoveCanvass)
		Safe_Release(Pair.second);
	m_mapMoveCanvass.clear();

	for (auto& Pair : m_mapCanvass)
		Safe_Release(Pair.second);
	m_mapCanvass.clear();

}
