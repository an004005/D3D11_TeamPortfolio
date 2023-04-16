#include "stdafx.h"
#include "..\public\UI_Manager.h"
#include <GameUtils.h>
#include "Canvas.h"

#include "Canvas_Acquisition.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
{
    m_SoundStore.CloneSound("MainClick");      // ���ο��� Ŭ���� �� +(BrainMap Ŭ��)
    m_SoundStore.CloneSound("UnableClick");   // �Ұ����� Ŭ�� +
    m_SoundStore.CloneSound("PartyClick");   // ��Ƽ Ŭ�� �� �� +
    m_SoundStore.CloneSound("DangerLong");   // �����̻� �Ҹ� �� ��
    m_SoundStore.CloneSound("DangerShort");   // �����̻� �Ҹ� ª�� �� + 
    m_SoundStore.CloneSound("ItemSwitch");   // ������ �ѱ� �� + ��
    m_SoundStore.CloneSound("ItemUnable");   // ������ ��� �Ұ� +
    m_SoundStore.CloneSound("ItemUse");   // ������ ��� + ��
    m_SoundStore.CloneSound("MainIn");   // ���� �� �� + + + ��
    m_SoundStore.CloneSound("MainOut");   // ���� ���� �� + + + ��
    m_SoundStore.CloneSound("MonsterAlert");   // ���� ��� (���� �߰� ���� ��?)
    m_SoundStore.CloneSound("PopupClose");   // �ȳ�â �� ��
    m_SoundStore.CloneSound("PopupOn");   // �ȳ�Ī �� ��
    m_SoundStore.CloneSound("Alt");   // art + ��
    m_SoundStore.CloneSound("Ctrl");   // ctrl + ��
    m_SoundStore.CloneSound("X");   // x + ��
    m_SoundStore.CloneSound("Mouseover");   // ���콺 Ŭ��? +
    m_SoundStore.CloneSound("TABLarge");   // Ʃ�丮�� �� �� + ��
    m_SoundStore.CloneSound("TABSmall");   // Ʃ�丮�� �� �� + ��
    m_SoundStore.CloneSound("TIP");   // Tip �� �� +z ��
    m_SoundStore.CloneSound("Unable");   // �Ұ����� Ŭ��
    m_SoundStore.CloneSound("Upgrade");   // ���� �� �� �� + ��
    m_SoundStore.CloneSound("Weapon");   // ��� ������ �� + ��

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
	
	if (CGameInstance::GetInstance()->KeyDown(DIK_T))// ���콺 ���� Ȱ��ȭ ��Ȱ��ȭ
	{
		if (m_bMouseActive)
		{
			m_bMouseActive = false; // �÷��̾� ����
			CGameUtils::HideCursor();
		}
		else
		{
			m_bMouseActive = true; // UI ����
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