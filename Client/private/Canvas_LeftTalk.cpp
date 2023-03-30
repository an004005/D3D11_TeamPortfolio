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
	// x : [0] ������ [1] ������ [2] ����
	// y : [0] ������ [1] ī��� [2] �ϳ��� [3] �׸� [4] ������ [5] ��ī [6] ī�ɷ� [7] �õ� [8] ��ī [9] �ƶ�� [10] ��Į�� ��Ʈ���� [13] �ߴ��� [18] ������
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
		tTalkInfo.wsTalk0 = { L"(�̰� ���� ����...... ������� �� �̻�," };
		tTalkInfo.wsTalk1 = { L"������ ���� ���ۿ� ����......" };
	}
	break;

	case 1:
	{
		tTalkInfo.fFace = { 1.0f, 0.0f };
		tTalkInfo.wsTalk0 = { L"�������� �� �������� ����." };
		tTalkInfo.wsTalk1 = { L"���� ���տ� �ִ� �Ϳ� �����ϴ� �ž�.)" };
	}
	break;

	case 3:
	{
		tTalkInfo.fFace = { 1.0f, 0.0f };
		tTalkInfo.wsTalk0 = { L"�׽�Ʈ ��." };
		tTalkInfo.wsTalk1 = { L"�Ӵ�." };
	}
	break;

	default:
	{
		tTalkInfo.fFace = { 0.0f, 0.0f };
		tTalkInfo.wsTalk0 = { L"��� ���� ��ī���� 131��" };
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