#include "stdafx.h"
#include "..\public\Canvas_MainTalk.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "JsonStorage.h"
#include "PlayerInfoManager.h"

#include "Canvas_Quest.h"

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

	if(0.0 == m_dTalk_TimeAcc) CUI_Manager::GetInstance()->Set_TempOff(true);

	// 0 : �ð� �Ǹ� �ѱ��
	m_dTalk_TimeAcc += TimeDelta;
	if (m_dTalk_TimeAcc >= 3.0 && m_bNextTalk == false)
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
	pGameInstance->Render_Font(L"NeoPro", m_tTalkInfo.wsName.c_str(), vPosition + _float2(27.0f, -20.0f), 0.f, vFontSize, vColorB);
	pGameInstance->Render_Font(L"NeoPro", m_tTalkInfo.wsName.c_str(), vPosition + _float2(25.0f, -22.0f), 0.f, vFontSize, vColor);
	pGameInstance->Render_Font(L"NeoPro", m_tTalkInfo.wsTalk0.c_str(), vPosition + _float2(27.0f, 37.0f), 0.f, vFontSize, vColorB);
	pGameInstance->Render_Font(L"NeoPro", m_tTalkInfo.wsTalk0.c_str(), vPosition + _float2(25.0f, 35.0f), 0.f, vFontSize, vColor);
	pGameInstance->Render_Font(L"NeoPro", m_tTalkInfo.wsTalk1.c_str(), vPosition + _float2(27.0f, 77.0f), 0.f, vFontSize, vColorB);
	pGameInstance->Render_Font(L"NeoPro", m_tTalkInfo.wsTalk1.c_str(), vPosition + _float2(25.0f, 75.0f), 0.f, vFontSize, vColor);

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
		// ����Ʈ �ִ� ���
		if (-1 != m_iQuestNum)
		{
			if (11 == m_iQuestNum)
			{
				CPlayerInfoManager::GetInstance()->Set_SASMember(SASMEET::HANABI);
				CPlayerInfoManager::GetInstance()->Set_SASMember(SASMEET::TSUGUMI);
				CPlayerInfoManager::GetInstance()->Set_SASMember(SASMEET::GEMMA);
			}
			else if (28 == m_iQuestNum)
			{
				CPlayerInfoManager::GetInstance()->Set_SASMember(SASMEET::LUCA);
				CPlayerInfoManager::GetInstance()->Set_SASMember(SASMEET::SEEDEN);
				CPlayerInfoManager::GetInstance()->Set_SASMember(SASMEET::ARASHI);
				CPlayerInfoManager::GetInstance()->Set_SASMember(SASMEET::KYOTO);
			}
			else
			{
				Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Quest.json");
				CCanvas_Quest* pCanvas_Quest = dynamic_cast<CCanvas_Quest*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"Canvas_Quest", &json));
				assert(pCanvas_Quest != nullptr && "Failed to Clone : CCanvas_Quest");
				pCanvas_Quest->Add_Quest(m_iQuestNum);
			}

			m_iQuestNum = -1;
		}




		CUI_Manager::GetInstance()->Set_TempOff(false);
		Find_ChildUI(L"TalkName")->SetVisible(false);
		Find_ChildUI(L"Talk_BackGround")->SetVisible(false);

		m_dTalk_TimeAcc = 3.0;
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

void CCanvas_MainTalk::Add_Talk(const _int iIndex, const _int iQuest)
{
	m_iQuestNum = iQuest;

	MAINTALKINFO	tTalkInfo;

	switch (iIndex)
	{
		//******************************************************** ����
	case 0:
	{
		tTalkInfo.wsName = { L"������ ���丮" };
		tTalkInfo.wsTalk0 = { L"�� ��ȥ�� ��ذ� ���ο� ������ ��� �Ǹ��Ѵ�." };
		tTalkInfo.wsTalk1 = { L"����ؼ�, �� ����ϱ�......" };
	}
	break;

	//******************************************************** [0] 1 ~ 11
	case 1:
	{
		tTalkInfo.wsName = { L"�ϳ���" };
		tTalkInfo.wsTalk0 = { L"������! ù �ӹ��� �� �ϰ� �Ծ�?" };
		tTalkInfo.wsTalk1 = { L"���? ���? �����Ҿ�?" };
	}
	break;

	case 2:
	{
		tTalkInfo.wsName = { L"������" };
		tTalkInfo.wsTalk0 = { L"��! �ϳ��� �ʴ�  ������?" };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 3:
	{
		tTalkInfo.wsName = { L"�ϳ���" };
		tTalkInfo.wsTalk0 = { L"���� �� ���� �� �� �� ���� �� ����!" };
		tTalkInfo.wsTalk1 = { L"�׷���? �׸� ����?" };
	}
	break;

	case 4:
	{
		tTalkInfo.wsName = { L"�׸�" };
		tTalkInfo.wsTalk0 = { L"�ϳ��� ���߾� �׷����� �׻� �����ؾ���" };
		tTalkInfo.wsTalk1 = { L"�Ʊ� ������ �ƴϿ����� ��ĥ�� �߾�" };
	}
	break;

	case 5:
	{
		tTalkInfo.wsName = { L"�ϳ���" };
		tTalkInfo.wsTalk0 = { L"��! ���� �ҰԿ� �����մϴ�! " };
		tTalkInfo.wsTalk1 = { L"�׸��� �׻� ���� ������!" };
	}
	break;

	case 6:
	{
		tTalkInfo.wsName = { L"������" };
		tTalkInfo.wsTalk0 = { L"��..." };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 7:
	{
		tTalkInfo.wsName = { L"�ϳ���" };
		tTalkInfo.wsTalk0 = { L"��! ������ ���丮�� �̾߱� �����?" };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 8:
	{
		tTalkInfo.wsName = { L"������" };
		tTalkInfo.wsTalk0 = { L"���丮�� ��� �� ������ ���丮��?" };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 9:
	{
		tTalkInfo.wsName = { L"�ϳ���" };
		tTalkInfo.wsTalk0 = { L"��!! �¾�!! �Ʊ� �׸� ���谡 �˷��ּ̾�." };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 10:
	{
		tTalkInfo.wsName = { L"�׸�" };
		tTalkInfo.wsTalk0 = { L"�¾� ������ ������ ���丮 ��������" };
		tTalkInfo.wsTalk1 = { L"�츮�� ���̶� �ο� �� �ʿ��� �����۵��� ��ų� �ǸŸ� �Ͻ���." };
	}
	break;

	case 11:
	{
		tTalkInfo.wsName = { L"������" };
		tTalkInfo.wsTalk0 = { L"�� �׷����� ���� ���� ��������. �׸� ����." };
		tTalkInfo.wsTalk1 = { L"" };

		m_iQuestNum = 11;
	}
	break;

	//******************************************************** [0] 12 ~ 21 *
	case 12:
	{
		tTalkInfo.wsName = { L"�׸�" };
		tTalkInfo.wsTalk0 = { L"������ ����� ���� ���̳�? ������ �־�?" };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 13:
	{
		tTalkInfo.wsName = { L"������" };
		tTalkInfo.wsTalk0 = { L"�ϳ��� ����...?" };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 14:
	{
		tTalkInfo.wsName = { L"������" };
		tTalkInfo.wsTalk0 = { L"����.. ���ѹ��ȳ� �׷��� Ƽ�� �����?" };
		tTalkInfo.wsTalk1 = { L"�ϳ��� ������ �ٰ����µ� �����ָ� ������ ���� �� �𸣰ھ��." };
	}
	break;

	case 15:
	{
		tTalkInfo.wsName = { L"�׸�" };
		tTalkInfo.wsTalk0 = { L"�ϳ��� ���� ���� ������ ����̿�����." };
		tTalkInfo.wsTalk1 = { L"�׷���... �ϳ���� ������ �ʰ� �ذŶ�� ���� ������ �� ������?" };
	}
	break;

	case 16:
	{
		tTalkInfo.wsName = { L"������" };
		tTalkInfo.wsTalk0 = { L"������ 2��... �� Ư���� ���� �ִ�... " };
		tTalkInfo.wsTalk1 = { L"\"�����͸�\" ����..." };
	}
	break;

	case 17:
	{
		tTalkInfo.wsName = { L"�׸�" };
		tTalkInfo.wsTalk0 = { L"����. ���� �� �� ���� �� ���� �����ϴ� ��� ���� �����ָ�," };
		tTalkInfo.wsTalk1 = { L"����� �̷������ ������ �ִٰ� �ϴ���... �׷��� �Ƹ� ���ϱ� ���� ����ﲬ?" };
	}
	break;

	case 18:
	{
		tTalkInfo.wsName = { L"������" };
		tTalkInfo.wsTalk0 = { L"������? �ϳ��� ������ ���� ������ �� ������..." };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 19:
	{
		tTalkInfo.wsName = { L"������" };
		tTalkInfo.wsTalk0 = { L"�����ٰ� ������..." };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 20:
	{
		tTalkInfo.wsName = { L"�׸�" };
		tTalkInfo.wsTalk0 = { L"�翬���� ������ ����� ����� ����̴ϱ�." };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 21:
	{
		tTalkInfo.wsName = { L"������" };
		tTalkInfo.wsTalk0 = { L"�ٵ� ���� �����ؿ�!" };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	//******************************************************** [1] 22 ~ 26 * 

	case 22:
	{
		tTalkInfo.wsName = { L"�ϳ���" };
		tTalkInfo.wsTalk0 = { L"��ħ �� �����׿�!" };
		tTalkInfo.wsTalk1 = { L"������! ������! �ߴ������ ���� ������ �˷��ּ̾�." };
	}
	break;

	case 23:
	{
		tTalkInfo.wsName = { L"�׸�" };
		tTalkInfo.wsTalk0 = { L"�ߴ������ ������ �˷��̴ּٸ�..." };
		tTalkInfo.wsTalk1 = { L"�극�� ���� ���ϴ� �ű���?" };
	}
	break;

	case 24:
	{
		tTalkInfo.wsName = { L"�ϳ���" };
		tTalkInfo.wsTalk0 = { L"�¾ƿ�! �׸�����!" };
		tTalkInfo.wsTalk1 = { L"�극�� �ʿ����� �츮�� ������ �ִ� �� ���ɷ��� ���� �� ���� �� �ִ�!" };
	}
	break;

	case 25:
	{
		tTalkInfo.wsName = { L"�׸�" };
		tTalkInfo.wsTalk0 = { L"�¾� ������ �ʴɷ¿� �°� ���� �ٸ� �극�� ���� ������." };
		tTalkInfo.wsTalk1 = { L"�ڱ⿡�� �´� ������ �� Ȯ���ϰ� ��°� �߿�����." };
	}
	break;

	case 26:
	{
		tTalkInfo.wsName = { L"������" };
		tTalkInfo.wsTalk0 = { L"��⸸ �ص� ���� ��� �Ǵ°�." };
		tTalkInfo.wsTalk1 = { L"�� �극�� ���� Ȯ���� ����." };
	}
	break;

	//******************************************************** [2] 27 ~ 28
	case 27:
	{
		tTalkInfo.wsName = { L"������" };
		tTalkInfo.wsTalk0 = { L"�ٵ� ���� ��̳׿�." };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 28:
	{
		tTalkInfo.wsName = { L"������" };
		tTalkInfo.wsTalk0 = { L"???" };
		tTalkInfo.wsTalk1 = { L"" };

		m_iQuestNum = 28;
	}
	break;

	//******************************************************** ������ ��� 29 ~ 31
	case 29:
	{
		tTalkInfo.wsName = { L"�ߴ���" };
		tTalkInfo.wsTalk0 = { L"��... ����...." };
		tTalkInfo.wsTalk1 = { L"������... �츮 ������..." };
	}
	break;

	case 30:
	{
		tTalkInfo.wsName = { L"������" };
		tTalkInfo.wsTalk0 = { L"�ߴ���� ���� �˼��մϴ�." };
		tTalkInfo.wsTalk1 = { L"��θ� ���ؼ� �ð������� ���͵帱 ���� �������ϴ�." };
	}
	break;

	case 31:
	{
		tTalkInfo.wsName = { L"�ߴ���" };
		tTalkInfo.wsTalk0 = { L"�ٵ� ���� �̾��ϳ�... ��� ������ �̷��� �Ǿ���." };
		tTalkInfo.wsTalk1 = { L"�������� �̷��� �� ��Ź�ϳ�." };
	}
	break;

	case 32:
	{
		tTalkInfo.wsName = { L"�ߴ���" };
		tTalkInfo.wsTalk0 = { L"�׷���... ��¼�� �� ���� �̷��� �Ǿ������� �𸣰�����," };
		tTalkInfo.wsTalk1 = { L"����� ���� ��ų �״�." };
	}
	break;

	case 33:
	{
		tTalkInfo.wsName = { L"�ߴ���" };
		tTalkInfo.wsTalk0 = { L"�ٵ� �� ��� �������� ���� �������� ���� ã���ֱ⸦ ��Ź�Ѵ�." };
		tTalkInfo.wsTalk1 = { L"�ű��� �ذ��� ����� ���� �ž�." };
	}
	break;
	
	case 34:
	{
		tTalkInfo.wsName = { L"������" };
		tTalkInfo.wsTalk0 = { L"�˰ڽ��ϴ�." };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	//******************************************************** [4] 35 ~ 42
	case 35:
	{
		tTalkInfo.wsName = { L"�ߴ���" };
		tTalkInfo.wsTalk0 = { L"������. ���� �ʳ��ɷ��� �ʿ���. �ѽö� ���� �ð��� ������.. " };
		tTalkInfo.wsTalk1 = { L"�����̰� �� ����ö������ ���� ��.. ���ƾ߸� ��." };
	}
	break;
	
	case 36:
	{
		tTalkInfo.wsName = { L"�ߴ���" };
		tTalkInfo.wsTalk0 = { L"������... �����̸� ���ؼ�..." };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 37:
	{
		tTalkInfo.wsName = { L"������" };
		tTalkInfo.wsTalk0 = { L"�ߴ����, �����ּ���." };
		tTalkInfo.wsTalk1 = { L"�ð� ������ �ϰ� �Ǹ� �̷��� ��� ������ �𸣴°� �˰� ����ݾƿ�." };
	}
	break;

	// case 38:
	// {
	// 	tTalkInfo.wsName = { L"��ī" };
	// 	tTalkInfo.wsTalk0 = { L"�¾ƿ�, �ߴ����. �����̸� ���ƿ��� �� ����� ���� �ſ���." };
	// 	tTalkInfo.wsTalk1 = { L"���� ���͵帱�Կ�." };
	// }
	// break;
	
	case 39:
	{
		tTalkInfo.wsName = { L"�ߴ���" };
		tTalkInfo.wsTalk0 = { L"������ ��!!!" };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 40:
	{
		tTalkInfo.wsName = { L"�ߴ���" };
		tTalkInfo.wsTalk0 = { L"�װ� ���� ���� ������ �˱⳪��?" };
		tTalkInfo.wsTalk1 = { L"�ױ� �̷�? �����̰� ���� �̷��� �ƹ����� �������." };
	}
	break;

	case 41:
	{
		tTalkInfo.wsName = { L"������" };
		tTalkInfo.wsTalk0 = { L"...!!" };
		tTalkInfo.wsTalk1 = { L"�׷��ٸ� ���� �������� �����̴ϴ�." };
	}
	break;

	case 42:
	{
		tTalkInfo.wsName = { L"�ߴ���" };
		tTalkInfo.wsTalk0 = { L"...�׷���." };
		tTalkInfo.wsTalk1 = { L"����� �����ζ� �� ��ȹ�� ��⸻�� ���ְڴ�!" };
	}
	break;

	//******************************************************** 
	default:
	{
		tTalkInfo.wsName = { L"131" };
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