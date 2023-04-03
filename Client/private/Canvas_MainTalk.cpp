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

	CUI_Manager::GetInstance()->Add_Canvas(L"CCanvas_MainTalk", this);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	return S_OK;
}

void CCanvas_MainTalk::Tick(_double TimeDelta)
{
	if (m_bRunning == false) return;

	CCanvas::Tick(TimeDelta);

	// 0 : �ð� �Ǹ� �ѱ��
	m_dTalk_TimeAcc += TimeDelta;
	if (m_dTalk_TimeAcc >= 3.0 && m_bNextTalk == false)
		Show_Talk();

	// 1 ; Ű �Է����� �ѱ��
	if (CGameInstance::GetInstance()->KeyDown(DIK_RETURN))
	{
		m_bNextTalk = true;
		Show_Talk();
	}
}

HRESULT CCanvas_MainTalk::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_float2 vFontSize = { 0.45f, 0.45f };
	_float4 vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//_tchar szText[MAX_PATH] = TEXT("");
	//wsprintf(szText, TEXT("/  %d"), 1);
	TALKINFO	tTalkInfo = m_qCurrentTalk.front();
	m_qCurrentTalk.pop();
	//_float2 vPosition = Find_ChildUI(L"Shop_ItemInfo")->GetScreenSpaceLeftTop();
	//pGameInstance->Render_Font(L"Pretendard32", tTalkInfo.wsName.c_str(), vPosition + _float2(560.0f, 55.0f), 0.f, vFontSize, vColor);
	//pGameInstance->Render_Font(L"Pretendard32", tTalkInfo.wsTalk0.c_str(), vPosition + _float2(560.0f, 55.0f), 0.f, vFontSize, vColor);
	//pGameInstance->Render_Font(L"Pretendard32", tTalkInfo.wsTalk1.c_str(), vPosition + _float2(560.0f, 55.0f), 0.f, vFontSize, vColor);

	return S_OK;
}

void CCanvas_MainTalk::Show_Talk()
{
	if (m_qCurrentTalk.empty())
	{
		m_dTalk_TimeAcc = 3.0;
		m_bRunning = false;
		m_bNextTalk = false;
		return;
	}

	m_dTalk_TimeAcc = 0.0;
	m_bNextTalk = false;
	m_bVisible = true;
}

void CCanvas_MainTalk::Add_Talk(const size_t iIndex)
{
	TALKINFO	tTalkInfo;

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

	case 3:
	{
		tTalkInfo.wsName = { L"����" };
		tTalkInfo.wsTalk0 = { L"�׽�Ʈ ��." };
		tTalkInfo.wsTalk1 = { L"�Ӵ�." };
	}
	break;

	default:
	{
		tTalkInfo.wsName = { L"��ī"};
		tTalkInfo.wsTalk0 = { L"��� ���� ��ī���� 131��" };
		tTalkInfo.wsTalk1 = { L"......" };
	}
	break;
	}

	m_qCurrentTalk.push(tTalkInfo);
	m_bRunning = true;
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