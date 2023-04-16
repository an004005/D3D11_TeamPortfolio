#include "stdafx.h"
#include "..\public\UI_Manager.h"
#include <GameUtils.h>
#include "Canvas.h"

#include "Canvas_Acquisition.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
{
    m_SoundStore.CloneSound("MainClick");      // 메인에서 클릭할 때 +(BrainMap 클릭)
    m_SoundStore.CloneSound("UnableClick");   // 불가능한 클릭 +
    m_SoundStore.CloneSound("PartyClick");   // 파티 클릭 할 때 +
    m_SoundStore.CloneSound("DangerLong");   // 상태이상 소리 긴 것
    m_SoundStore.CloneSound("DangerShort");   // 상태이상 소리 짧은 것 + 
    m_SoundStore.CloneSound("ItemSwitch");   // 아이템 넘길 때 + ★
    m_SoundStore.CloneSound("ItemUnable");   // 아이템 사용 불가 +
    m_SoundStore.CloneSound("ItemUse");   // 아이템 사용 + ★
    m_SoundStore.CloneSound("MainIn");   // 메인 들어갈 때 + + + ★
    m_SoundStore.CloneSound("MainOut");   // 메인 나갈 때 + + + ★
    m_SoundStore.CloneSound("MonsterAlert");   // 몬스터 경고 (몬스터 발견 했을 때?)
    m_SoundStore.CloneSound("PopupClose");   // 안내창 뜰 때
    m_SoundStore.CloneSound("PopupOn");   // 안내칭 끌 때
    m_SoundStore.CloneSound("Alt");   // art + ★
    m_SoundStore.CloneSound("Ctrl");   // ctrl + ★
    m_SoundStore.CloneSound("X");   // x + ★
    m_SoundStore.CloneSound("Mouseover");   // 마우스 클릭? +
    m_SoundStore.CloneSound("TABLarge");   // 튜토리얼 켤 때 + ★
    m_SoundStore.CloneSound("TABSmall");   // 튜토리얼 끌 때 + ★
    m_SoundStore.CloneSound("TIP");   // Tip 뜰 때 +z ★
    m_SoundStore.CloneSound("Unable");   // 불가능한 클릭
    m_SoundStore.CloneSound("Upgrade");   // 레벨 업 할 때 + ★
    m_SoundStore.CloneSound("Weapon");   // 장비 장착할 때 + ★

#ifndef _DEBUG
    CGameUtils::HideCursor();
#endif
    //   CUI_Manager::GetInstance()->PlaySound("xxX");
}

CCanvas* CUI_Manager::Find_MoveCanvas(const wstring& pCanvasTag)
{
    auto   iter = m_mapMoveCanvass.find(pCanvasTag);

    if (iter == m_mapMoveCanvass.end())
        return nullptr;

    return iter->second;
}

void CUI_Manager::Add_MoveCanvas(const wstring& pCanvasTag, CCanvas* pCanvas)
{
    Assert(m_mapMoveCanvass.find(pCanvasTag) == m_mapMoveCanvass.end());
    m_mapMoveCanvass.emplace(pCanvasTag, pCanvas);
}

CCanvas* CUI_Manager::Find_Canvas(const wstring& pCanvasTag)
{
    auto   iter = m_mapCanvass.find(pCanvasTag);

    if (iter == m_mapCanvass.end())
        return nullptr;

    return iter->second;
}

void CUI_Manager::Add_Canvas(const wstring& pCanvasTag, CCanvas* pCanvas)
{
    Assert(m_mapCanvass.find(pCanvasTag) == m_mapCanvass.end());
    m_mapCanvass.emplace(pCanvasTag, pCanvas);
}

CCanvas* CUI_Manager::Find_WindowCanvas(const wstring& pCanvasTag)
{
    auto   iter = m_mapWindowCanvass.find(pCanvasTag);

    if (iter == m_mapWindowCanvass.end())
        return nullptr;

    return iter->second;
}

void CUI_Manager::Add_WindowCanvas(const wstring& pCanvasTag, CCanvas* pCanvas)
{
    Assert(m_mapWindowCanvass.find(pCanvasTag) == m_mapWindowCanvass.end());
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
			m_bMouseActive = false; // 플레이어 세계
			CGameUtils::HideCursor();
		}
		else
		{
			m_bMouseActive = true; // UI 세계
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