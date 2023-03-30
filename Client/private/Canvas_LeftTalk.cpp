#include "stdafx.h"
#include "..\public\Canvas_LeftTalk.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "Talk_FaceUI.h"
#include "Talk_BackGroundUI.h"

CCanvas_LeftTalk::CCanvas_LeftTalk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_LeftTalk::CCanvas_LeftTalk(const CCanvas_LeftTalk& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_LeftTalk::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_LeftTalk::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_Canvas(L"CCanvas_LeftTalk", this);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	return S_OK;
}

void CCanvas_LeftTalk::Tick(_double TimeDelta)
{
	if (m_bRunning == false) return;

	CCanvas::Tick(TimeDelta);

	m_dTalk_TimeAcc += TimeDelta;

	if (m_dTalk_TimeAcc >= 3.0)
		Show_Talk();
}

void CCanvas_LeftTalk::Show_Talk()
{
	// x : [0] 가만히 [1] 공격중 [2] 웃음
	// y : [0] 유이토 [1] 카사네 [2] 하나비 [3] 겜마 [4] 츠구미 [5] 루카 [6] 카케로 [7] 시덴 [8] 쿄카 [9] 아라시 [10] 스칼렛 스트링스 [13] 중대장 [18] 츠구미
	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	if (m_qCurrentTalk.empty())
	{
		m_dTalk_TimeAcc = 3.0;
		m_bRunning = false;
		return;
	}

	TALKINFO	tTalkInfo = m_qCurrentTalk.front();
	m_qCurrentTalk.pop();

	dynamic_cast<CTalk_FaceUI*>(Find_ChildUI(L"Talk_Face"))->Set_Face(tTalkInfo.fFace, tTalkInfo.wsTalk0, tTalkInfo.wsTalk1);
	dynamic_cast<CTalk_BackGroundUI*>(Find_ChildUI(L"Talk_BackGround"))->Set_TalkUse();
	Find_ChildUI(L"Talk_Icon")->SetVisible(true);

	m_dTalk_TimeAcc = 0.0;
	m_bRunning = true;
}

void CCanvas_LeftTalk::Add_Talk(const size_t iIndex)
{
	TALKINFO	tTalkInfo;

	switch (iIndex)
	{
	case 0:
	{
		tTalkInfo.fFace = { 1.0f, 0.0f };
		tTalkInfo.wsTalk0 = { L"(이게 최종 시험...... 여기까지 온 이상," };
		tTalkInfo.wsTalk1 = { L"각오를 다질 수밖에 없어......" };
	}
	break;

	case 1:
	{
		tTalkInfo.fFace = { 1.0f, 0.0f };
		tTalkInfo.wsTalk0 = { L"쓸데없는 건 생각하지 말자." };
		tTalkInfo.wsTalk1 = { L"그저 눈앞에 있는 것에 집중하는 거야.)" };
	}
	break;

	case 3:
	{
		tTalkInfo.fFace = { 1.0f, 0.0f };
		tTalkInfo.wsTalk0 = { L"테스트 용." };
		tTalkInfo.wsTalk1 = { L"임다." };
	}
	break;

	default:
	{
		tTalkInfo.fFace = { 0.0f, 0.0f };
		tTalkInfo.wsTalk0 = { L"쥬신 게임 아카데미 131기" };
		tTalkInfo.wsTalk1 = { L"......" };
	}
	break;
	}


	m_qCurrentTalk.push(tTalkInfo);
	m_bRunning = true;
}

CCanvas_LeftTalk* CCanvas_LeftTalk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCanvas_LeftTalk* pInstance = new CCanvas_LeftTalk(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_LeftTalk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas* CCanvas_LeftTalk::Clone(void* pArg)
{
	CCanvas_LeftTalk* pInstance = new CCanvas_LeftTalk(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_LeftTalk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_LeftTalk::Free()
{
	CCanvas::Free();

}