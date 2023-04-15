#include "stdafx.h"
#include "..\public\Canvas_Quest.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "UI_Manager.h"
#include "GameManager.h"

#include "DefaultUI.h"
#include "ShaderUI.h"
#include "GameManager_Tutorial.h"

#include "Level_Loading_Simple.h"
#include "Level_ConstructionSite3F.h"

CCanvas_Quest::CCanvas_Quest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_Quest::CCanvas_Quest(const CCanvas_Quest& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_Quest::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_Quest::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	m_fBackGround_StartPos = Find_ChildUI(L"BackGround")->Get_Position();
	Find_ChildUI(L"BackGround")->Set_Position(_float2(-500.0f, m_fBackGround_StartPos.y));

	m_fQuestMove = true;
	m_bVisible = true;

	return S_OK;
}

void CCanvas_Quest::Tick(_double TimeDelta)
{
	if (CGameInstance::GetInstance()->KeyDown(DIK_E))
	{
		m_fQuestMove = !m_fQuestMove;
		m_bVisible = true;
	}

	Success_Tick();
	if (!m_bVisible) return;

	CCanvas::Tick(TimeDelta);

	Move_Tick(TimeDelta);
}

HRESULT CCanvas_Quest::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_float2 vFontSize = { 0.5f, 0.5f };
	_float4 vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_float2 vPosition = Find_ChildUI(L"BackGround")->GetScreenSpaceLeftTop(); // ChakeBase
	pGameInstance->Render_Font(L"Pretendard32", m_wsQuest.c_str(), vPosition + _float2(430.0f, 23.0f), 0.f, vFontSize, vColor);

	return S_OK;
}

void CCanvas_Quest::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	//if (ImGui::Button("Set_Quest"))
	//{
	//	Set_Quest(0);
	//}
	//ImGui::SameLine();
	//if (ImGui::Button("Set_Quest"))
	//{
	//	Set_SuccessQuest();
	//}
}

void CCanvas_Quest::Add_Quest(const _int iIndex, const _bool bRender)
{
	if (false == bRender) m_fQuestMove = false;
	else CUI_Manager::GetInstance()->PlaySound("MonsterAlert");
	m_iQuestIndex = iIndex;

	QUESTINFO tQuestInfo;

	switch (iIndex)
	{
	case 0 :
	{
		m_wsQuest = L"���ο� ��Ÿ�� ��� ���̸� óġ�ϼ���.";
	}
	break;

	case 1:
	{
		m_wsQuest = L"����� �Բ� �����忡 ��Ÿ�� ���̸� óġ�ϼ���.";
	}
	break;

	case 2:
	{
		m_wsQuest = L"�ϳ��� ���� �����͸��� ã�� �����ϱ�.";
	}
	break;

	case 3:
	{
		m_wsQuest = L"�������� �������� ���� ã�� �ߴ��忡�� ����.";
	}
	break;

	case 4:
	{
		m_wsQuest = L"����� �Բ� �����忡 ��Ÿ�� ���̸� óġ�ϼ���.";
	}
	break;

	default:
	{
		m_wsQuest = L"��� ���� ��ī���� �����ϱ�.";
	}
	break;
	}
}

void CCanvas_Quest::Move_Tick(const _double& TimeDelta)
{
	_float2 vPosition = Find_ChildUI(L"BackGround")->Get_Position();
	Find_ChildUI(L"ChakeBase")->Set_Position(_float2(vPosition.x - 100.0f, vPosition.y));
	Find_ChildUI(L"Chake")->Set_Position(_float2(vPosition.x + 130.0f, vPosition.y));

	_float fSpeed = 500.0f;
	if (true == m_fQuestMove)
	{
		if (m_fBackGround_StartPos.x > vPosition.x)
		{
			vPosition.x += _float(TimeDelta) * fSpeed;
			Find_ChildUI(L"BackGround")->Set_Position(_float2(vPosition.x, m_fBackGround_StartPos.y));
		}
		else
		{
			Find_ChildUI(L"BackGround")->Set_Position(m_fBackGround_StartPos);

			m_dMove_TimeAcc += TimeDelta;
			if (m_fSuccessQuest)
			{
				Success(TimeDelta);

				if (3.5 < m_dMove_TimeAcc)
				{
					m_dMove_TimeAcc = 0.0;
					m_fQuestMove = false;
				}
			}
			else
			{
				if (3.0 < m_dMove_TimeAcc)
				{
					m_dMove_TimeAcc = 0.0;
					m_fQuestMove = false;
				}
			}
		}
	}
	else
	{
		if (-500.0f <= vPosition.x)
		{
			vPosition.x -= _float(TimeDelta) * fSpeed;
			Find_ChildUI(L"BackGround")->Set_Position(_float2(vPosition.x, m_fBackGround_StartPos.y));
		}
		else
		{
			m_bVisible = false;
			Find_ChildUI(L"BackGround")->Set_Position(_float2(-500.0f, m_fBackGround_StartPos.y));

			if (m_fSuccessQuest)
				CGameObject::SetDelete();
		}
	}
}

void CCanvas_Quest::Success(const _double& TimeDelta)
{
	_float fRatio = dynamic_cast<CShaderUI*>(Find_ChildUI(L"Chake"))->Get_Floats0();
	fRatio += _float(TimeDelta) * 0.5f;
	if (1.0f < fRatio)	// UITODO : ���߿� �۾��� ���� ���̸� �����ϰ� ���� ��� 1.0f �� ������ �޾Ƽ� �����ϸ� �ȴ�.
		fRatio = 1.0f;
	dynamic_cast<CShaderUI*>(Find_ChildUI(L"Chake"))->Set_Floats0(fRatio);
}

void CCanvas_Quest::Success_Tick()
{
	// ����Ʈ ���� ����.
	if (0 == m_iQuestIndex)
	{
		if (10 == dynamic_cast<CGameManager_Tutorial*>(CGameManager_Tutorial::GetInstance())->Get_MonstaerDeadCount())
		{
			Set_SuccessQuest();
			dynamic_cast<CGameManager_Tutorial*>(CGameManager_Tutorial::GetInstance())->Set_MonsterDeadCount();

			CGameManager::GetInstance()->Set_LeftTalk(13);
			CGameManager::GetInstance()->Set_LeftTalk(14);
			CGameManager::GetInstance()->Set_SuccessQuest(500);
		}
	}
	else if (1 == m_iQuestIndex)	// 4�� ��Ʈ
	{
		// ���⼭�� �߰��� �÷��̾� �̵����� �Ѵ�.
		if (3.0 < m_d3FMapMove_TimaAcc)
		{
			CGameInstance::GetInstance()->Open_Loading(
				LEVEL_CONSTRUCTIONSITE_3F,
				CLevel_Loading_Simple::Create<CLevel_ConstructionSite3F>(m_pDevice, m_pContext));
		}
		else
			m_d3FMapMove_TimaAcc += TIME_DELTA;
	}
	else if (2 == m_iQuestIndex)
	{
		if (CGameManager::GetInstance()->Get_Flower())
		{
			m_iQuestIndex = -1;
			Set_SuccessQuest();

			CGameManager::GetInstance()->Set_LeftTalk(26);
			CGameManager::GetInstance()->Set_LeftTalk(27);
			CGameManager::GetInstance()->Set_LeftTalk(28);
			CGameManager::GetInstance()->Set_LeftTalk(29);
			CGameManager::GetInstance()->Set_SuccessQuest(2000);
		}
	}
	else if (3 == m_iQuestIndex)
	{
		CGameObject::SetDelete();
	}
	else if (4 == m_iQuestIndex)
	{
		// ����Ʈ ���� ���� : �⸧ ������ ������ �ȴ�.
		if (LEVEL_NOW == LEVEL_CONSTRUCTIONSITE_3F)
		{
			if (CGameManager::GetInstance()->Get_EM0320Dead())
			{
				m_iQuestIndex = -1;
				Set_SuccessQuest();
				CGameManager::GetInstance()->Set_SuccessQuest(1000);
			}
		}
	}
}

CCanvas_Quest* CCanvas_Quest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCanvas_Quest* pInstance = new CCanvas_Quest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_Quest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas* CCanvas_Quest::Clone(void* pArg)
{
	CCanvas_Quest* pInstance = new CCanvas_Quest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_Quest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_Quest::Free()
{
	CCanvas::Free();

}

//L
//������� ���� �������� ����� �Ǿ���, ù��° ����Ʈ�� ���η� ���ϰ� �˴ϴ�.
//ù ��° ����Ʈ�� ���� ���� ���̸� ��� óġ�ؾ� �մϴ�.

//M
//����Ʈ�� �Ϸ��ϰ� ������ ���� ���ƴٴϴٰ� ���� 3���� �����ϴ�. (���� �޴����� ������ ������ Ȯ���� �� �ֽ��ϴ�.)
//�̶� ������� ������ ���� �����ϰ� ��Ʋ �������� ���, �Ǹ� �� �� ��° ����Ʈ�� �޽��ϴ�.

//L
//�� ��° ����Ʈ�� ������ 3�� ���� ������ ���� �������� ���� ������ �Բ� �⵿�Ͽ� ���̸� ó�϶�� �̼��� �޽��ϴ�.

//L
//���̸� ��� óġ�ϰ� ���� ������ ���ư��µ� �� �� �ϳ���� �ߴ����� ȣ��� ������ ģ����� ������ ���Ѵ�.

//M
//������ ģ����� ������ ���ư��ϴ�.������ ���ư��� ���� ������� �ϳ����� ���� ������ ���� � ���� ���ٱ� ����ϴٰ�
//�����̰� ������ 2������ Ư���ϰ� ã�� ����� �� "�����͸�"�� ã�ڰ� �մϴ�.�� ���� �����ϸ� ����� �̷�����ٴ� ������ �ִٰ� �˷��ݴϴ�.
//+
//�׷��� �ϳ��� �� ��ħ ���ƿ��� �ϳ��� �극�θʿ� ���� ������ ��� �Դٸ鼭 �˷��ݴϴ�.

//L
//�׸��� ���� �Ʊ� �޾Ƶ� �̼��� �ִٸ鼭 �� ���� ������ 2������ ���Ѵ�.

//M
//����Ʈ�� ��ġ�� �ٽ� ������ ���ư��� ������ ������� �����鼭 � ���̰� �־��ٸ鼭 ��� ���ٸ� ���ϴ�. (���� ������ ����ȭ �Ǿ� �־��ٸ� ����ȭ �� ������ ����ؼ� �̾߱� �Ѵ�.)
//
//M
//�׷��ٰ� ���ڱ� ��� ������� �޷����ϴ�.�޷��� ������ �ߴ����� �־���, �ߴ��忡�� �� ��° ����Ʈ�� �ް� �˴ϴ�.
//�׹� ° ����Ʈ�� ���� ����ö�� �ִ� �ߴ����� �� "������"�� ���ؾ� �ϴ� �̼��̶�� �մϴ�.

//M
//������ ���͸� ��� óġ�ϰ� ���� ���� óġ�Ϸ��� ���� �ߴ����� ��Ÿ���鼭 �ڽ��� �� �̶�� ������ ���߶�� �մϴ�.
//�׷��� �ߴ����� ������ ����Ʈ�� �����̸� ���ϱ� ���� �� ����� �������� ���� �������� ���� ã�� �޶�� �մϴ�.
//�׷��� ������ �� ������ ����ϰ� �־ ���� ���� �� �����ϴ�.

//L
//�̶� �Ϸ�ī�� ������ �޾Ƽ� �ߴ����� �ð� ������ �Ϸ��� �Ѵٴ� ���� �˰� �˴ϴ�.�ð� ������ �Ͼ�� ��� ������ �ùε��� �����ϴٴ� ���� �˰�, ��� ����鿡�� �̸� ���� ���� ���ʹ޶�� �մϴ�.
//�׷��� ������ ���ư��� ���ƴٴϴٰ� Ʈ���Ÿ� ���, ������� ������鼭 �ߴ����� ��ȯ���� ���������� �̵��ϰ� �˴ϴ�.

//M
//�װ����� �ߴ����� �ڽ��� �� �ϳ����� ���ؼ� �ð� ������ �ϱ� ���� ������� ��Ź�� �߰�,
//�׷��⿡�� �ٸ� �ùε鿡�� ���ذ� �� �� �ֱ� ������ ������� �ݴ��մϴ�.
//�׷��� �ߴ����� ȥ�ڼ� �ϰڴٸ鼭 ���س� ���� ����� �մϴ�.
//�׷��� �ߴ����� ���� ���ؼ� �ߴ���� ����� �ο��� ���� �ǰ� ����� ����� �¸��� ������ �ȴ�.
