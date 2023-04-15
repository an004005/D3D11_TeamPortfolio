#include "stdafx.h"
#include "..\public\UI_Manager.h"

#include <GameUtils.h>

#include "Canvas.h"

#include "Canvas_Acquisition.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
{
	//m_SoundStore.CloneSound("UIClick");		// 메인에서 클릭할 때
	//m_SoundStore.CloneSound("UIClick2");	// 불가능한 클릭
	//m_SoundStore.CloneSound("UIClick2");	// 파티 클릭 할 때
	//m_SoundStore.CloneSound("UIClick2");	// 상태이상 소리 긴 것
	//m_SoundStore.CloneSound("UIClick2");	// 상태이상 소리 짧은 것
	//m_SoundStore.CloneSound("UIClick2");	// 아이템 넘길 때
	//m_SoundStore.CloneSound("UIClick2");	// 아이템 사용 불가
	//m_SoundStore.CloneSound("UIClick2");	// 아이템 사용
	//m_SoundStore.CloneSound("UIClick2");	// 메인 들어갈 때
	//m_SoundStore.CloneSound("UIClick2");	// 메인 나갈 때
	//m_SoundStore.CloneSound("UIClick2");	// 몬스터 경고 (몬스터 발견 했을 때?)
	//m_SoundStore.CloneSound("UIClick2");	// 안내창 뜰 때
	//m_SoundStore.CloneSound("UIClick2");	// 안내칭 끌 때
	//m_SoundStore.CloneSound("UIClick2");	// art
	//m_SoundStore.CloneSound("UIClick2");	// ctrl
	//m_SoundStore.CloneSound("UIClick2");	// x
	//m_SoundStore.CloneSound("UIClick2");	// 마우스 클릭?
	//m_SoundStore.CloneSound("UIClick2");	// 튜토리얼 켤 때
	//m_SoundStore.CloneSound("UIClick2");	// 튜토리얼 끌 때
	//m_SoundStore.CloneSound("UIClick2");	// Tip 뜰 때
	//m_SoundStore.CloneSound("UIClick2");	// 불가능한 클릭
	//m_SoundStore.CloneSound("UIClick2");	// 레벨 업 할 때
	//m_SoundStore.CloneSound("UIClick2");	// 장비 장착할 때
#ifndef _DEBUG
	CGameUtils::HideCursor();
#endif
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

void CUI_Manager::Tick(_double TimeDelta)
{
	
	if (CGameInstance::GetInstance()->KeyDown(DIK_T))// 마우스 강제 활성화 비활성화
	{
		if (m_bMouseActive)
		{
			m_bMouseActive = false;
			CGameUtils::HideCursor();
		}
		else
		{
			m_bMouseActive = true;
			CGameUtils::ShowCursor();
		}
	}
}

void CUI_Manager::PlaySound(const string& SoundName)
{
	m_SoundStore.PlaySound(SoundName);
}

void CUI_Manager::Free()
{
	Clear();
}
