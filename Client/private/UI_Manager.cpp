#include "stdafx.h"
#include "..\public\UI_Manager.h"
#include "Canvas.h"

#include "Canvas_Acquisition.h"
#include "GameInstance.h"

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

CCanvas* CUI_Manager::Find_WindowCanvas(const wstring& pCanvasTag)
{
	auto	iter = find_if(m_mapWindowCanvass.begin(), m_mapWindowCanvass.end(), [&](pair<wstring, CCanvas*> pCantag) {
		return pCantag.first == pCanvasTag;
		});

	if (iter == m_mapWindowCanvass.end())
		return nullptr;

	return iter->second;
}

void CUI_Manager::Add_WindowCanvas(const wstring& pCanvasTag, CCanvas* pCanvas)
{
	m_mapWindowCanvass.emplace(pCanvasTag, pCanvas);
}

void CUI_Manager::Set_TempOff(const _bool bOff)
{
	for (auto& itr : m_mapMoveCanvass)
	{
		if (CGameInstance::GetInstance()->Check_ObjectAlive(itr.second))
			itr.second->TempOff(bOff);
	}

	for (auto& itr : m_mapCanvass)
	{
		if (CGameInstance::GetInstance()->Check_ObjectAlive(itr.second))
			itr.second->TempOff(bOff);
	}
}

void CUI_Manager::Clear()
{
	m_mapCanvass.clear();
	m_mapMoveCanvass.clear();
	m_mapWindowCanvass.clear();
}

void CUI_Manager::Free()
{
	Clear();
}
