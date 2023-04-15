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
		m_wsQuest = L"도로에 나타난 모든 괴이를 처치하세요.";
	}
	break;

	case 1:
	{
		m_wsQuest = L"동료와 함께 공사장에 나타난 괴이를 처치하세요.";
	}
	break;

	case 2:
	{
		m_wsQuest = L"하나비를 위한 히마와리를 찾아 전달하기.";
	}
	break;

	case 3:
	{
		m_wsQuest = L"병원에서 나오미의 약을 찾아 중대장에게 가기.";
	}
	break;

	case 4:
	{
		m_wsQuest = L"동료와 함께 공사장에 나타난 괴이를 처치하세요.";
	}
	break;

	default:
	{
		m_wsQuest = L"쥬신 게임 아카데미 졸업하기.";
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
	if (1.0f < fRatio)	// UITODO : 나중에 글씨에 따라서 길이를 조절하고 싶은 경우 1.0f 를 변수로 받아서 수정하면 된다.
		fRatio = 1.0f;
	dynamic_cast<CShaderUI*>(Find_ChildUI(L"Chake"))->Set_Floats0(fRatio);
}

void CCanvas_Quest::Success_Tick()
{
	// 퀘스트 성공 조건.
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
	else if (1 == m_iQuestIndex)	// 4랑 세트
	{
		// 여기서는 추가로 플레이어 이동까지 한다.
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
		// 퀘스트 성공 조건 : 기름 보스를 잡으면 된다.
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
//유이토는 이제 공식적인 대원이 되었고, 첫번째 퀘스트로 도로로 향하게 됩니다.
//첫 번째 퀘스트는 도로 맵의 괴이를 모두 처치해야 합니다.

//M
//퀘스트를 완료하고 마을로 가서 돌아다니다가 동료 3명을 만납니다. (메인 메뉴에서 동료의 정보를 확인할 수 있습니다.)
//이때 동료들이 상점에 대해 설명하고 배틀 아이템을 사고, 판매 후 두 번째 퀘스트를 받습니다.

//L
//두 번째 퀘스트는 공사장 3층 에서 강해진 괴물 출현으로 인해 동료들과 함께 출동하여 괴이를 처하라고 미션을 받습니다.

//L
//괴이를 모두 처치하고 이후 마을로 돌아가는데 이 때 하나비는 중대장의 호출로 나머지 친구들과 마을로 향한다.

//M
//나머지 친구들과 마을로 돌아갑니다.마을에 돌아가고 남은 대원끼리 하나비의 생일 선물을 위해 어떤 것을 해줄까 고민하다가
//츠구미가 공사장 2층에서 특별하고 찾기 어려운 꽃 "히마와리"를 찾자고 합니다.이 꽃을 전달하면 사랑이 이루어진다는 전설이 있다고 알려줍니다.
//+
//그렇게 하나비가 때 마침 돌아오고 하나비가 브레인맵에 대해 설명을 듣고 왔다면서 알려줍니다.

//L
//그리고 나서 아까 받아둔 미션이 있다면서 다 같이 공사장 2층으로 향한다.

//M
//퀘스트를 마치고 다시 마을로 돌아가서 나머지 동료들을 만나면서 어떤 괴이가 있었다면서 잠시 수다를 떱니다. (만약 마을이 초토화 되어 있었다면 초토화 된 이유를 상상해서 이야기 한다.)
//
//M
//그러다가 갑자기 모든 대원들이 달려갑니다.달려간 곳에는 중대장이 있었고, 중대장에게 네 번째 퀘스트를 받게 됩니다.
//네번 째 퀘스트는 지금 지하철에 있는 중대장의 딸 "나오미"를 구해야 하는 미션이라고 합니다.

//M
//하지만 몬스터를 모두 처치하고 보스 까지 처치하려는 순간 중대장이 나타나면서 자신의 딸 이라고 공격을 멈추라고 합니다.
//그렇게 중대장은 마지막 퀘스트로 나오미를 구하기 위해 구 토벌군 병원으로 가서 나오미의 약을 찾아 달라고 합니다.
//그러나 병원은 물 보스가 장악하고 있어서 약을 구할 수 없습니다.

//L
//이때 하루카의 연락을 받아서 중대장이 시간 역행을 하려고 한다는 것을 알게 됩니다.시간 역행이 일어나면 모든 대원들과 시민들이 위험하다는 것을 알고, 모든 대원들에게 이를 막기 위해 도와달라고 합니다.
//그렇게 마을로 돌아가서 돌아다니다가 트리거를 밟고, 대원들이 사라지면서 중대장의 소환으로 보스맵으로 이동하게 됩니다.

//M
//그곳에서 중대장이 자신의 딸 하나만을 위해서 시간 역행을 하기 위해 대원에게 부탁을 했고,
//그러기에는 다른 시민들에게 피해가 갈 수 있기 때문에 대원들은 반대합니다.
//그러나 중대장은 혼자서 하겠다면서 방해나 되지 말라고 합니다.
//그렇게 중대장을 막기 위해서 중대장과 대원의 싸움이 시작 되고 결론은 대원의 승리로 끝나게 된다.
