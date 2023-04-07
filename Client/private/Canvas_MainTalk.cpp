#include "stdafx.h"
#include "..\public\Canvas_MainTalk.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CCanvas_MainTalk::CCanvas_MainTalk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_MainTalk::CCanvas_MainTalk(const CCanvas_MainTalk& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_MainTalk::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_MainTalk::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	m_dTalk_TimeAcc = 5.0;

	return S_OK;
}

void CCanvas_MainTalk::Tick(_double TimeDelta)
{
	if (m_bRunning == false) return;

	CCanvas::Tick(TimeDelta);
	CUI_Manager::GetInstance()->Set_TempOff(true);

	// 0 : �ð� �Ǹ� �ѱ��
	m_dTalk_TimeAcc += TimeDelta;
	if (m_dTalk_TimeAcc >= 5.0 && m_bNextTalk == false)
		Show_Talk();

	// 1 ; Ű �Է����� �ѱ��
	if (CGameInstance::GetInstance()->KeyDown(DIK_RETURN))
		Show_Talk();
}

HRESULT CCanvas_MainTalk::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_float2 vFontSize = { 0.5f, 0.5f };
	_float4 vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	_float4 vColorB = { 0.0f, 0.0f, 0.0f, 1.0f };
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_float2 vPosition = Find_ChildUI(L"TalkName")->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"NeoPro", m_tTalkInfo.wsName.c_str(), vPosition + _float2(42.0f, -12.0f), 0.f, vFontSize, vColorB);
	pGameInstance->Render_Font(L"NeoPro", m_tTalkInfo.wsName.c_str(), vPosition + _float2(40.0f, -15.0f), 0.f, vFontSize, vColor);
	pGameInstance->Render_Font(L"NeoPro", m_tTalkInfo.wsTalk0.c_str(), vPosition + _float2(42.0f, 37.0f), 0.f, vFontSize, vColorB);
	pGameInstance->Render_Font(L"NeoPro", m_tTalkInfo.wsTalk0.c_str(), vPosition + _float2(40.0f, 35.0f), 0.f, vFontSize, vColor);
	pGameInstance->Render_Font(L"NeoPro", m_tTalkInfo.wsTalk1.c_str(), vPosition + _float2(42.0f, 77.0f), 0.f, vFontSize, vColorB);
	pGameInstance->Render_Font(L"NeoPro", m_tTalkInfo.wsTalk1.c_str(), vPosition + _float2(40.0f, 75.0f), 0.f, vFontSize, vColor);
	//pGameInstance->Render_Font(L"NeoPro", m_tTalkInfo.wsName.c_str(), vPosition + _float2(82.0f, 12.0f), 0.f, vFontSize, vColorB);
	//pGameInstance->Render_Font(L"NeoPro", m_tTalkInfo.wsName.c_str(), vPosition + _float2(80.0f, 10.0f), 0.f, vFontSize, vColor);
	//pGameInstance->Render_Font(L"NeoPro", m_tTalkInfo.wsTalk0.c_str(), vPosition + _float2(82.0f, 47.0f), 0.f, vFontSize, vColorB);
	//pGameInstance->Render_Font(L"NeoPro", m_tTalkInfo.wsTalk0.c_str(), vPosition + _float2(80.0f, 45.0f), 0.f, vFontSize, vColor);
	//pGameInstance->Render_Font(L"NeoPro", m_tTalkInfo.wsTalk1.c_str(), vPosition + _float2(82.0f, 82.0f), 0.f, vFontSize, vColorB);
	//pGameInstance->Render_Font(L"NeoPro", m_tTalkInfo.wsTalk1.c_str(), vPosition + _float2(80.0f, 80.0f), 0.f, vFontSize, vColor);

	return S_OK;
}

void CCanvas_MainTalk::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

}

void CCanvas_MainTalk::Show_Talk()
{
	if (m_qCurrentTalk.empty())
	{
		CUI_Manager::GetInstance()->Set_TempOff(false);
		Find_ChildUI(L"TalkName")->SetVisible(false);
		Find_ChildUI(L"Talk_BackGround")->SetVisible(false);

		m_dTalk_TimeAcc = 5.0;
		m_bRunning = false;
		m_bVisible = false;
		return;
	}

	m_tTalkInfo = m_qCurrentTalk.front();
	m_qCurrentTalk.pop();

	m_bVisible = true;
	m_dTalk_TimeAcc = 0.0;
	Find_ChildUI(L"TalkName")->SetVisible(true);
	Find_ChildUI(L"Talk_BackGround")->SetVisible(true);
}

void CCanvas_MainTalk::Add_Talk(const size_t iIndex)
{
	MAINTALKINFO	tTalkInfo;

	switch (iIndex)
	{
	case 0:
	{
		tTalkInfo.wsName = { L"�ϳ���"};
		tTalkInfo.wsTalk0 = { L"�Ÿ��� ���� �ִٰ� �Ʒ��� �ִٰ�......" };
		tTalkInfo.wsTalk1 = { L"����� �� ���� ���̰���� VR ����" };
	}
	break;

	case 1:
	{
		tTalkInfo.wsName = { L"������"};
		tTalkInfo.wsTalk0 = { L"�������� �� �������� ����." };
		tTalkInfo.wsTalk1 = { L"���� ���տ� �ִ� �Ϳ� �����ϴ� �ž�.)" };
	}
	break;

	case 2:
	{
		tTalkInfo.wsName = { L"����" };
		tTalkInfo.wsTalk0 = { L"�׽�Ʈ ��." };
		tTalkInfo.wsTalk1 = { L"�Ӵ�." };
	}
	break;

	default:
	{
		tTalkInfo.wsName = { L"��ī"};
		tTalkInfo.wsTalk0 = { L"��� ���� ��ī���� \" | \' " };
		tTalkInfo.wsTalk1 = { L"......" };
	}
	break;
	}

	m_qCurrentTalk.push(tTalkInfo);
	m_bRunning = true;
	m_bVisible = true;
}

CCanvas_MainTalk* CCanvas_MainTalk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCanvas_MainTalk* pInstance = new CCanvas_MainTalk(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_MainTalk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas* CCanvas_MainTalk::Clone(void* pArg)
{
	CCanvas_MainTalk* pInstance = new CCanvas_MainTalk(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_MainTalk");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_MainTalk::Free()
{
	CCanvas::Free();

}