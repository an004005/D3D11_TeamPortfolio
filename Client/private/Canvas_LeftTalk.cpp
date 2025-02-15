#include "stdafx.h"
#include "..\public\Canvas_LeftTalk.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "GameUtils.h"
#include "JsonStorage.h"
#include "MathUtils.h"

#include "Talk_FaceUI.h"
#include "Talk_BackGroundUI.h"
#include "Canvas_Quest.h"

#include "Level_Loading_Simple.h"
#include "Level_ConstructionSite2F.h"
#include "Level_Subway.h"

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

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	return S_OK;
}

void CCanvas_LeftTalk::Tick(_double TimeDelta)
{
	if (m_bRunning == false) return;

	CCanvas::Tick(TimeDelta);

	if (false == m_bSound)
	{
		m_bSound = true;
		CUI_Manager::GetInstance()->PlaySound("TIP");
	}

	m_dTalk_TimeAcc += TimeDelta;
	if (m_dTalk_TimeAcc >= 3.0)
		Show_Talk();
}

void CCanvas_LeftTalk::Show_Talk()
{
	// x : [0] 가만히 [1] 공격중 [2] 웃음
	// y : [0] 유이토 [1] 카사네 [2] 하나비 [3] 겜마 [4] 츠구미 [5] 루카 [6] 카케로 [7] 시덴 [8] 쿄카 [9] 아라시 [10] 스칼렛 스트링스 [13] 중대장 [17] 하루카 [18] 츠구미
	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	if (m_qCurrentTalk.empty())
	{
		m_bSound = false;

		// 퀘스트 있는 경우
		if (-1 != m_iQuestNum)
		{
			Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Quest.json");
			CCanvas_Quest* pCanvas_Quest = dynamic_cast<CCanvas_Quest*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"Canvas_Quest", &json));
			assert(pCanvas_Quest != nullptr && "Failed to Clone : CCanvas_Quest");
			pCanvas_Quest->Add_Quest(m_iQuestNum);

			m_iQuestNum = -1;
		}

		if (-1 != m_iMapMove)
		{
			if (12 == m_iMapMove)
			{
				CGameInstance::GetInstance()->Open_Loading(
					LEVEL_CONSTRUCTIONSITE_2F,
					CLevel_Loading_Simple::Create<CLevel_ConstructionSite2F>(m_pDevice, m_pContext));
			
				m_iMapMove = -1;
			}
			else if (25 == m_iMapMove)
			{
				CGameInstance::GetInstance()->Open_Loading(
					LEVEL_SUBWAY,
					CLevel_Loading_Simple::Create<CLevel_Subway>(m_pDevice, m_pContext));

				m_iMapMove = -1;
			}
		}

		// 재사용 하는 경우 초기화
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

void CCanvas_LeftTalk::Add_Talk(const _int iIndex, const _int iQuest)
{
	m_iQuestNum = iQuest;

	TALKINFO	tTalkInfo;
	tTalkInfo.fFace = { 0.0f, 0.0f };
	tTalkInfo.wsTalk0.clear();
	tTalkInfo.wsTalk1.clear();

	switch (iIndex)
	{
	//******************************* 0 ~ 3 * 퀘스트 0
	case 0:
	{
		tTalkInfo.fFace = { 2.0f, 17.0f };
		tTalkInfo.wsTalk0 = TEXT("유이토 안녕! 정식 대원이 된걸 축하해 ");
		tTalkInfo.wsTalk1 = TEXT("난 하루카 이고 내 초뇌능력은 정신감응이야 ");
	}
	break;

	case 1:
	{
		tTalkInfo.fFace = { 0.0f, 17.0f };
		tTalkInfo.wsTalk0 = L"너의 담당 오퍼레이터가 되었어.";
		tTalkInfo.wsTalk1 = L"앞으로 잘 부탁해 유이토!";
	}
	break;

	case 2:
	{
		tTalkInfo.fFace = { 0.0f, 17.0f };
		tTalkInfo.wsTalk0 = L"지금 이 도로에 괴이가 출현했어";
		tTalkInfo.wsTalk1 = L"유이토의 실력 기대해볼게!";
	}
	break;

	case 3:
	{
		tTalkInfo.fFace = { 1.0f, 0.0f };
		tTalkInfo.wsTalk0 = L"고마워 하루카!";
		tTalkInfo.wsTalk1 = L"나도 앞으로 잘 부탁해.";
	}
	break;

	//******************************* 4 ~ 6 * 퀘스트 1
	case 4:
	{
		tTalkInfo.fFace = { 1.0f, 17.0f };
		tTalkInfo.wsTalk0 = L"유이토 공사장 3층에";
		tTalkInfo.wsTalk1 = L"많은 괴이가 출현했대.";
	}
	break;

	case 5:
	{
		tTalkInfo.fFace = { 0.0f, 17.0f };
		tTalkInfo.wsTalk0 = L"마침 동료들과 잘 만났네!";
		tTalkInfo.wsTalk1 = L"다 같이 공사장으로 얼른 이동하자.";
	}
	break;

	case 6:
	{
		tTalkInfo.fFace = { 0.0f, 0.0f };
		tTalkInfo.wsTalk0 = L"고마워 히루카!";
		tTalkInfo.wsTalk1 = L"";
	}
	break;

	//******************************* 7 ~ 9 *
	case 7:
	{
		tTalkInfo.fFace = { 2.0f, 17.0f };
		tTalkInfo.wsTalk0 = L"다들 멋지게 성공했네! 고생했어";
		tTalkInfo.wsTalk1 = L"대로로 돌아가서 괴이 출현을 대비해 줘";
	}
	break;

	case 8:
	{
		tTalkInfo.fFace = { 2.0f, 17.0f };
		tTalkInfo.wsTalk0 = L"하나비는 중대장님께서 잠깐 보자고 하시네";
		tTalkInfo.wsTalk1 = L"잠시 다녀올래?";
	}
	break;

	case 9:
	{
		tTalkInfo.fFace = { 2.0f, 2.0f };
		tTalkInfo.wsTalk0 = L"응 알겠어 히루카!";
		tTalkInfo.wsTalk1 = L"";
	}
	break;

	//******************************* 10 ~ 12 브레인 맵을 찍고 나서 *
	case 10:
	{
		tTalkInfo.fFace = { 2.0f, 0.0f };
		tTalkInfo.wsTalk0 = L"하나비 덕에 더 많은 걸 알게 되었네";
		tTalkInfo.wsTalk1 = L"고마워 하나비";
	}
	break;

	case 11:
	{
		tTalkInfo.fFace = { 2.0f, 2.0f };
		tTalkInfo.wsTalk0 = L"헤헤... 항상 내가 더 고맙지!!";
		tTalkInfo.wsTalk1 = L"앗! 히루카 연락은 왔어?";
	}
	break;

	case 12:
	{
		tTalkInfo.fFace = { 1.0f, 4.0f };
		tTalkInfo.wsTalk0 = L"응 공사장 2층으로 가야해";
		tTalkInfo.wsTalk1 = L"얼른 출발하자";

		m_iMapMove = 12;
	}
	break;

	//******************************* 13 첫번째 퀘스트 완료 *
	case 13:
	{
		tTalkInfo.fFace = { 2.0f, 17.0f };
		tTalkInfo.wsTalk0 = L"고생했어 유이토!";
		tTalkInfo.wsTalk1 = L"오른쪽에는 임무를 성공하면 주는 보상이야.";
	}
	break;

	case 14:
	{
		tTalkInfo.fFace = { 1.0f, 17.0f };
		tTalkInfo.wsTalk0 = L"이제 대로에 가서 동료들과 합류해 줘.";
		tTalkInfo.wsTalk1 = L"";
	}
	break;

	//******************************* 첫번째 마을 수다. 15 ~ 16
	case 15:
	{
		tTalkInfo.fFace = { 0.0f, 0.0f };
		tTalkInfo.wsTalk0 = L"첫 번째 임무를 무사히 잘 수행한 것 같아서.";
		tTalkInfo.wsTalk1 = L"다행이다.";
	}
	break;

	case 16:
	{
		tTalkInfo.fFace = { 0.0f, 0.0f };
		tTalkInfo.wsTalk0 = L"앞으로도 더 많은 괴이를 처치해서.";
		tTalkInfo.wsTalk1 = L"모두를 지키겠어.";
	}
	break;

	//******************************* 17 불
	case 17:
	{
		tTalkInfo.fFace = { 1.0f, 2.0f };
		tTalkInfo.wsTalk0 = L"유이토 괜찮아!? 이때는 내 초뇌능력";
		tTalkInfo.wsTalk1 = L"불꽃을 이용해봐!";
	}
	break;

	//******************************* 18 투시 *
	case 18:
	{
		tTalkInfo.fFace = { 1.0f, 4.0f };
		tTalkInfo.wsTalk0 = L"유이토 투시를 사용해!";
		tTalkInfo.wsTalk1 = L"내 초뇌능력으로 저 몬스터가 보일거야.";
	}
	break;

	//******************************* 19 루카
	case 19:
	{
		tTalkInfo.fFace = { 1.0f, 5.0f };
		tTalkInfo.wsTalk0 = L"유이토, 제 초뇌능력을 쓰면";
		tTalkInfo.wsTalk1 = L"앞으로 나아갈 수 있을 거에요.";
	}
	break;

	//******************************* 20 시덴
	case 20:
	{
		tTalkInfo.fFace = { 1.0f, 7.0f };
		tTalkInfo.wsTalk0 = L"유이토 내 감전을 써보겠어?";
		tTalkInfo.wsTalk1 = L"많이 유용할거야.";
	}
	break;

	//******************************* 21 쿄카
	case 21:
	{
		tTalkInfo.fFace = { 1.0f, 8.0f };
		tTalkInfo.wsTalk0 = L"유이토 내 분신을 사용해봐.";
		tTalkInfo.wsTalk1 = L"너의 힘이 3배가 될거야.";
	}
	break;

	//******************************* 22 아라시
	case 22:
	{
		tTalkInfo.fFace = { 1.0f, 9.0f };
		tTalkInfo.wsTalk0 = L"유이토! 빨리 내 초뇌능력을 이용해.";
		tTalkInfo.wsTalk1 = L"아주 빨라지는 걸 느끼게 될거야.";
	}
	break;

	//******************************* 23 상점 구매
	case 23:
	{
		tTalkInfo.fFace = { 0.0f, 2.0f };
		tTalkInfo.wsTalk0 = L"유이토! 바로 상점으로 이용하러 가보자!";
		tTalkInfo.wsTalk1 = L"여기 오른쪽 어디 근처였는데...";
	}
	break;
	 
	//******************************* 4번째 퀘스트 갑작스러운 중대장의 명령 24 25
	case 24:
	{
		tTalkInfo.fFace = { 1.0f, 13.0f };
		tTalkInfo.wsTalk0 = L"긴급한 일이 발생했다.";
		tTalkInfo.wsTalk1 = L"내 딸 나오미가 지금 혼자 지하철에 갔어.";
	}
	break;

	case 25:
	{
		tTalkInfo.fFace = { 1.0f, 13.0f };
		tTalkInfo.wsTalk0 = L"지금 지하철 상황이 많이 안 좋다고 하네";
		tTalkInfo.wsTalk1 = L"다들 지금 바로 출발해라.";

		m_iMapMove = 25;
	}
	break;

	//******************************* 3번째 퀘스트 완료로 하나비 에게 꽃 전달 26 ~ 29 *

	case 26:
	{
		tTalkInfo.fFace = { 2.0f, 0.0f };
		tTalkInfo.wsTalk0 = L"하나비 이 꽃은 네 것이야. 미리 생일 축하해~";
		tTalkInfo.wsTalk1 = L"앞으로도 잘 부탁해.";
	}
	break;

	case 27:
	{
		tTalkInfo.fFace = { 2.0f, 4.0f };
		tTalkInfo.wsTalk0 = L"나도... 생일 축하해 하나비.";
		tTalkInfo.wsTalk1 = L"";
	}
	break;

	case 28:
	{
		tTalkInfo.fFace = { 2.0f, 3.0f };
		tTalkInfo.wsTalk0 = L"둘이 보기 좋아~";
		tTalkInfo.wsTalk1 = L"생일 축하한다. 하나비 앞으로도 잘 해보자고.";
	}
	break;

	case 29:
	{
		tTalkInfo.fFace = { 2.0f, 2.0f };
		tTalkInfo.wsTalk0 = L"너무 감동이에요.~";
		tTalkInfo.wsTalk1 = L"다들 정말 감사합니다.~";
	}
	break;

	//******************************* 4 번째 퀘스트 중/// 30 ~ 31 *

	case 30:
	{
		tTalkInfo.fFace = { 1.0f, 7.0f };
		tTalkInfo.wsTalk0 = L"중대장님이 말씀하신 약 있긴 한거야?";
		tTalkInfo.wsTalk1 = L"";
	}
	break;

	case 31:
	{
		tTalkInfo.fFace = { 1.0f, 9.0f };
		tTalkInfo.wsTalk0 = L"그러게 아무리 찾아봐도 안 보여.";
		tTalkInfo.wsTalk1 = L"";
	}
	break;

	//******************************* 중대장의 시간역행 32 ~ 35 *

	case 32:
	{
		tTalkInfo.fFace = { 1.0f, 17.0f };
		tTalkInfo.wsTalk0 = L"큰일이야 지금 중대장이 시간 역행을";
		tTalkInfo.wsTalk1 = L"하려고 하고 있어 아무래도 나오미 때문인가봐.";
	}
	break;

	case 33:
	{
		tTalkInfo.fFace = { 1.0f, 5.0f };
		tTalkInfo.wsTalk0 = L"시간 역행이라면... ";
		tTalkInfo.wsTalk1 = L"미래가 어떻게 바뀔지 아무도 알 수 없어...";
	}
	break;

	case 34:
	{
		tTalkInfo.fFace = { 1.0f, 17.0f };
		tTalkInfo.wsTalk0 = L"맞아 시간 역행을 하게 되면 누구도";
		tTalkInfo.wsTalk1 = L"무사할 수 없을 거야. 어서 중대장을 막아야해.";
	}
	break;

	case 35:
	{
		tTalkInfo.fFace = { 1.0f, 0.0f };
		tTalkInfo.wsTalk0 = L"시간이 없어 얼른 출발하자.";
		tTalkInfo.wsTalk1 = L"어...어..??";
	}
	break;

	// *********************************** Tutorial 레벨에서 수다 36 ~ 37 *
	case 36:
	{
		tTalkInfo.fFace = { 2.0f, 17.0f };
		tTalkInfo.wsTalk0 = L"유이토 역시 소문대로 장난 아닌걸?";
		tTalkInfo.wsTalk1 = L"훈련생 에이스라고 불리는 이유가 있었네.";
	}
	break;

	case 37:
	{
		tTalkInfo.fFace = { 2.0f, 0.0f };
		tTalkInfo.wsTalk0 = L"고마워!";
		tTalkInfo.wsTalk1 = L"아직 많이 부족해 더 열심히 해볼게.";
	}
	break;

	//******************************* 38 겜마
	case 38:
	{
		tTalkInfo.fFace = { 3.0f, 6.0f };
		tTalkInfo.wsTalk0 = L"유이토 내 경질화를 사용해봐,";
		tTalkInfo.wsTalk1 = L"너에게 도움이 될거야.";
	}
	break;

	//******************************* 공사장 3층 수다 39 ~40 *
	case 39:
	{
		tTalkInfo.fFace = { 1.0f, 4.0f };
		tTalkInfo.wsTalk0 = L"여기저기 괴이가 상당히 많네...";
		tTalkInfo.wsTalk1 = L"";
	}
	break;
	case 40:
	{
		tTalkInfo.fFace = { 0.0f, 0.0f };
		tTalkInfo.wsTalk0 = L"그러게.. 앞으로도 많이 있을거야.";
		tTalkInfo.wsTalk1 = L"다들 조심해.";
	}
	break;

	//******************************* 공사장 3층 수다 41 ~ 42 *
	case 41:
	{
		tTalkInfo.fFace = { 2.0f, 2.0f };
		tTalkInfo.wsTalk0 = L"SAS로 연결되는 거, 아프고 감각도 이상하지만";
		tTalkInfo.wsTalk1 = L"힘께 싸우는 중이라는 게 실감이 나!";
	}
	break;
	case 42:
	{
		tTalkInfo.fFace = { 1.0f, 0.0f };
		tTalkInfo.wsTalk0 = L"정말 다 같이 연결되어 있으니까,";
		tTalkInfo.wsTalk1 = L"더욱 강해진 것 같기도해.";
	}
	break;

	//******************************* 공사장 3층 수다 43 *
	case 43:
	{
		tTalkInfo.fFace = { 2.0f, 2.0f };
		tTalkInfo.wsTalk0 = L"우아, 그런  싸움 방식도 있구나!?";
		tTalkInfo.wsTalk1 = L"유이토의 초뇌능력, 멋있다!";
	}
	break;

	//******************************* 공사장 3층 수다 44 ~ 47 *
	case 44:
	{
		tTalkInfo.fFace = { 0.0f, 4.0f };
		tTalkInfo.wsTalk0 = L"겜마님의 초뇌능력은 경질화...";
		tTalkInfo.wsTalk1 = L"라고 하셨죠?";
	}
	break;
	case 45:
	{
		tTalkInfo.fFace = { 1.0f, 3.0f };
		tTalkInfo.wsTalk0 = L"맞아. 내 초뇌능력으로";
		tTalkInfo.wsTalk1 = L"누구보다 단단해 질 수 있지.";
	}
	break;
	case 46:
	{
		tTalkInfo.fFace = { 2.0f, 2.0f };
		tTalkInfo.wsTalk0 = L"우와! 정말 멋져요!!";
		tTalkInfo.wsTalk1 = L"";
	}
	break;
	case 47:
	{
		tTalkInfo.fFace = { 2.0f, 3.0f };
		tTalkInfo.wsTalk0 = L"하하.. 그렇게 말해주니 고맙네";
		tTalkInfo.wsTalk1 = L"";
	}
	break;

	//******************************* 공사장 3층 수다 48 ~ 49 *
	case 48:
	{
		tTalkInfo.fFace = { 2.0f, 2.0f };
		tTalkInfo.wsTalk0 = L"츠구미의 투시 능력 정말 멋지다!";
		tTalkInfo.wsTalk1 = L"";
	}
	break;
	case 49:
	{
		tTalkInfo.fFace = { 2.0f, 4.0f };
		tTalkInfo.wsTalk0 = L"고마워 하나비.";
		tTalkInfo.wsTalk1 = L"너의 초뇌능력도 멋있어.";
	}
	break;

	//******************************* 공사장 3층 수다 50 ~ 53 *
	case 50:
	{
		tTalkInfo.fFace = { 0.0f, 0.0f };
		tTalkInfo.wsTalk0 = L"이 녀석...... 확실히 벅찰 것 같아요.";
		tTalkInfo.wsTalk1 = L"";
	}
	break;
	case 51:
	{
		tTalkInfo.fFace = { 2.0f, 3.0f };
		tTalkInfo.wsTalk0 = L"그런 것 같군, 유이토 위를 봐!";
	tTalkInfo.wsTalk1 = L"자재가 늘어져 있어.";
	}
	break;
	case 52:
	{
		tTalkInfo.fFace = { 1.0f, 2.0f };
		tTalkInfo.wsTalk0 = L"유이토의 염력으로 ";
		tTalkInfo.wsTalk1 = L"쿵 하고 저 녀석한테 떨어뜨려 버리자!";
	}
	break;
	case 53:
	{
		tTalkInfo.fFace = { 0.0f, 0.0f };
		tTalkInfo.wsTalk0 = L"좋아, 맡겨줘.";
		tTalkInfo.wsTalk1 = L"";
	}
	break;

	//******************************* 병원 수다 54 ~ 55
	case 54:
	{
		tTalkInfo.fFace = { 2.0f, 3.0f };
		tTalkInfo.wsTalk0 = L"유이토, 내 초뇌능력은 주위의 물체를 이용하는 것 이니,";
		tTalkInfo.wsTalk1 = L"눈앞의 적에게만 시선을 빼앗기지 마라.";
	}
	break;
	case 55:
	{
		tTalkInfo.fFace = { 0.0f, 0.0f };
		tTalkInfo.wsTalk0 = L"네!";
		tTalkInfo.wsTalk1 = L"";
	}
	break;

	//******************************* 공사장 3층 수다 56 *
	case 56:
	{
		tTalkInfo.fFace = { 1.0f, 3.0f };
		tTalkInfo.wsTalk0 = L"심상치 않은 느낌이 드는군.";
		tTalkInfo.wsTalk1 = L"";
	}
	break;

	//******************************* 공사장 2층 57 ~ 58 *
	case 57:
	{
		tTalkInfo.fFace = { 0.0f, 17.0f };
		tTalkInfo.wsTalk0 = L"유이토 공사장 2층에 괴이가 출현했어.";
		tTalkInfo.wsTalk1 = L"미리 도착했네? 얼른 처리 부탁해.";
	}
	break;
	case 58:
	{
		tTalkInfo.fFace = { 1.0f, 2.0f };
		tTalkInfo.wsTalk0 = L"앗? 뭐야? 아까 히루카에게";
		tTalkInfo.wsTalk1 = L"전달 받고 온거 아니였어?";
	}
	break;

	//******************************* 공사장 2층 59 ~ 60 *
	case 59:
	{
		tTalkInfo.fFace = { 0.0f, 2.0f };
		tTalkInfo.wsTalk0 = L"으... 정말 괴이들은 ";
		tTalkInfo.wsTalk1 = L"이상하게 생기지 않았어? 그렇지 츠구미?";
	}
	break;
	case 60:
	{
		tTalkInfo.fFace = { 0.0f, 4.0f };
		tTalkInfo.wsTalk0 = L"괴이니까...";
		tTalkInfo.wsTalk1 = L"";
	}
	break;

	//******************************* 공사장 2층 61 ~ 63 *
	case 61:
	{
		tTalkInfo.fFace = { 0.0f, 2.0f };
		tTalkInfo.wsTalk0 = L"유이토는 모든 물건에";
		tTalkInfo.wsTalk1 = L"염력을 쓸 수 있는거야?";
	}
	break;
	case 62:
	{
		tTalkInfo.fFace = { 0.0f, 0.0f };
		tTalkInfo.wsTalk0 = L"그렇지? 한 번에 더 많은 물건을 ";
		tTalkInfo.wsTalk1 = L"던질 수 있도록 계속 노력하고 있어.";
	}
	break;
	case 63:
	{
		tTalkInfo.fFace = { 2.0f, 2.0f };
		tTalkInfo.wsTalk0 = L"역시 유이토는 멋있어!";
		tTalkInfo.wsTalk1 = L"";
	}
	break;

	//******************************** 나오미 죽이기 전 64 ~ 66
	case 64:
	{
		tTalkInfo.fFace = { 0.0f, 13.0f };
		tTalkInfo.wsTalk0 = L"다들 멈춰!!";
		tTalkInfo.wsTalk1 = L"지금 ";
	}
	break;
	case 65:
	{
		tTalkInfo.fFace = { 0.0f, 13.0f };
		tTalkInfo.wsTalk0 = L"(괴이를 보니까...)";
		tTalkInfo.wsTalk1 = L"(어쩌면... 있을지도?)";
	}
	break;
	case 66:
	{
		tTalkInfo.fFace = { 0.0f, 2.0f };
		tTalkInfo.wsTalk0 = L"다른 대원분들은 다들 어디 계시는 거지?";
		tTalkInfo.wsTalk1 = L"";

		// 병원으로 이동시켜 줘야 한다...
	}
	break;

	//******************************** 공사장 2층 67 ~ 71
	case 67:
	{
		tTalkInfo.fFace = { 2.0f, 3.0f };
		tTalkInfo.wsTalk0 = L"다들 \"등심\"이랑 \"안심\"중에 뭘 더 좋아하나?";
		tTalkInfo.wsTalk1 = L"";
	}
	break;
	case 68:
	{
		tTalkInfo.fFace = { 2.0f, 2.0f };
		tTalkInfo.wsTalk0 = L"음.. 보통 \"안심\"이 더 부드럽다고 하니까";
		tTalkInfo.wsTalk1 = L"저는 \"안심\"이요!!";
	}
	break;
	case 69:
	{
		tTalkInfo.fFace = { 2.0f, 3.0f };
		tTalkInfo.wsTalk0 = L"나는 \"안심\"이 좀 더 \"안심\" 되던걸";
		tTalkInfo.wsTalk1 = L"하하하.";
	}
	break;
	case 70:
	{
		tTalkInfo.fFace = { 2.0f, 4.0f };
		tTalkInfo.wsTalk0 = L"그럼... \"양고기\"는";
		tTalkInfo.wsTalk1 = L"\"양\" 이 적어서 \"양고기\"인가요...?";
	}
	break;

	//******************************** 지하철 71 ~ 72
	case 71:
	{
		tTalkInfo.fFace = { 0.0f, 8.0f };
		tTalkInfo.wsTalk0 = L"지하철 상태가 많이 심각하네...";
		tTalkInfo.wsTalk1 = L"";
	}
	break;
	case 72:
	{
		tTalkInfo.fFace = { 0.0f, 5.0f };
		tTalkInfo.wsTalk0 = L"정말...";
		tTalkInfo.wsTalk1 = L"어서 이곳에서 나오미를 찾아야 겠어.";
	}
	break;

	//******************************** 지하철 73
	case 73:
	{
		tTalkInfo.fFace = { 2.0f, 6.0f };
		tTalkInfo.wsTalk0 = L"오오~ 나이스 였어~";
		tTalkInfo.wsTalk1 = L"";
	}
	break;

	//******************************** 지하철 74
	case 74:
	{
		tTalkInfo.fFace = { 0.0f, 9.0f };
		tTalkInfo.wsTalk0 = L"유이토 꼼꼼이 잘 살펴봐.";
		tTalkInfo.wsTalk1 = L"";
	}
	break;

	//******************************** 지하철 75
	case 75:
	{
		tTalkInfo.fFace = { 0.0f, 5.0f };
		tTalkInfo.wsTalk0 = L"괴이가 많이 출현했어 다들 조심해!";
		tTalkInfo.wsTalk1 = L"";
	}
	break;

	//******************************** 지하철 76 ~ 78
	case 76:
	{
		tTalkInfo.fFace = { 0.0f, 0.0f };
		tTalkInfo.wsTalk0 = L"이쪽으로 가면 될 것 같아.";
		tTalkInfo.wsTalk1 = L"";
	}
	break;
	case 77:
	{
		tTalkInfo.fFace = { 0.0f, 7.0f };
		tTalkInfo.wsTalk0 = L"당연한 걸 하나하나 지시히지 마.";
		tTalkInfo.wsTalk1 = L"빨리 가라고.";
	}
	break;
	case 78:
	{
		tTalkInfo.fFace = { 0.0f, 6.0f };
		tTalkInfo.wsTalk0 = L"오~ 무서워! 보이는 대로 다 깨물어 들다니.";
		tTalkInfo.wsTalk1 = L"치와와도 아니고.";
	}
	break;

	//******************************** 지하철 79
	case 79:
	{
		tTalkInfo.fFace = { 0.0f, 4.0f };
		tTalkInfo.wsTalk0 = L"어째서지...?";
		tTalkInfo.wsTalk1 = L"내 투시로도 나오미가 보이지 않아.";
	}
	break;

	//******************************** 병원 80 ~ 81
	case 80:
	{
		tTalkInfo.fFace = { 0.0f, 9.0f };
		tTalkInfo.wsTalk0 = L"이 병원......";
		tTalkInfo.wsTalk1 = L"괴이보다, 유령이 나올 것 같은 장소야.";
	}
	break;
	case 81:
	{
		tTalkInfo.fFace = { 0.0f, 4.0f };
		tTalkInfo.wsTalk0 = L"......만약, 유령이 보이면......";
		tTalkInfo.wsTalk1 = L"알려드릴게요......";
	}
	break;

	//******************************** 병원 82 ~ 84
	case 82:
	{
		tTalkInfo.fFace = { 0.0f, 4.0f };
		tTalkInfo.wsTalk0 = L"이상해......";
		tTalkInfo.wsTalk1 = L"왜 병원 안에 괴이가 모여있는 거지?";
	}
	break;
	case 83:
	{
		tTalkInfo.fFace = { 0.0f, 5.0f };
		tTalkInfo.wsTalk0 = L"괴이가 모이는 가장 큰 이유는 먹이를...";
		tTalkInfo.wsTalk1 = L"뇌를 찾기 때문이죠.";
	}
	break;
	break;
	case 84:
	{
		tTalkInfo.fFace = { 2.0f, 3.0f };
		tTalkInfo.wsTalk0 = L"여기에는 괴이가 모여들 만큼의 인간은";
		tTalkInfo.wsTalk1 = L"없는 것 같은데......";
	}
	break;

	//******************************** 병원 85 *
	case 85:
	{
		tTalkInfo.fFace = { 0.0f, 8.0f };
		tTalkInfo.wsTalk0 = L"이쪽은 아닌 것 같 은걸?";
		tTalkInfo.wsTalk1 = L"";
	}
	break;

	//******************************** 병원 투명벽을 넘어갈 때 86 *
	case 86:
	{
		tTalkInfo.fFace = { 0.0f, 2.0f };
		tTalkInfo.wsTalk0 = L"으앗!";
		tTalkInfo.wsTalk1 = L"까, 깜짝이야......";
	}
	break;

	//******************************** 병원 87
	case 87:
	{
		tTalkInfo.fFace = { 0.0f, 4.0f };
		tTalkInfo.wsTalk0 = L"조금은... 성장 했을까?";
		tTalkInfo.wsTalk1 = L"";
	}
	break;

	//******************************** 나오미 리본 공격 88 *
	case 88:
	{
		tTalkInfo.fFace = { 1.0f, 3.0f };
		tTalkInfo.wsTalk0 = L"유이토, 땅속에서도 공격이 온다!";
		tTalkInfo.wsTalk1 = L"발밑에도 주의해. 긴장을 풀지 마!";
	}
	break;
	//******************************** 나오미 안개 89 *
	case 89:
	{
		tTalkInfo.fFace = { 0.0f, 4.0f };
		tTalkInfo.wsTalk0 = L"뭔가 보일지도......";
		tTalkInfo.wsTalk1 = L"유이토, 투시를 사용해봐.";
	}
	break;

	//******************************** 물보스 약점 발견 90 *
	case 90:
	{
		tTalkInfo.fFace = { 0.0f, 9.0f };
		tTalkInfo.wsTalk0 = L"저 괴이 그쪽이 약점인가보네.";
		tTalkInfo.wsTalk1 = L"우리한테 물을 뿌리다니 혼내주지.";
	}
	break;

	//******************************** 나오미 약점 발견 91 *
	case 91:
	{
		tTalkInfo.fFace = { 0.0f, 8.0f };
		tTalkInfo.wsTalk0 = L"여기가 나오미의 약점...";
		tTalkInfo.wsTalk1 = L"나오미한테 괜찮을 걸까...";
	}
	break;

	//******************************** 드릴말 약점 발견 92 *
	case 92:
	{
		tTalkInfo.fFace = { 0.0f, 3.0f };
		tTalkInfo.wsTalk0 = L"그곳이 약점이야 유이토";
		tTalkInfo.wsTalk1 = L"공격하자!";
	}
	break;

	//******************************** 경견페이 약점 발견 93 *
	case 93:
	{
		tTalkInfo.fFace = { 2.0f, 2.0f };
		tTalkInfo.wsTalk0 = L"유이토 나이스!";
		tTalkInfo.wsTalk1 = L"약점을 잘 공격했네!";
	}
	break;

	//******************************** 플레이어 약정공격에 당했을 때 94, 95, 96, 97 *
	case 94:
	{
		_uint iNum = CMathUtils::RandomUInt(2);

		if (0 == iNum)
		{
			tTalkInfo.fFace = { 0.0f, 0.0f };
			tTalkInfo.wsTalk0 = L"으윽.. 뜨거워";
			tTalkInfo.wsTalk1 = L"하나비는 어떻게 그렇게 불을 잘 다루는거지...";
		}
		else
		{
			tTalkInfo.fFace = { 0.0f, 0.0f };
			tTalkInfo.wsTalk0 = L"뜨거워! 통째로 구워버릴 생각인가...!";
			tTalkInfo.wsTalk1 = L"";
		}
	}
	break;

	case 95:
	{
		_uint iNum = CMathUtils::RandomUInt(2);

		if (0 == iNum)
		{
			tTalkInfo.fFace = { 0.0f, 0.0f };
			tTalkInfo.wsTalk0 = L"으... 기름이 가득 묻었어";
			tTalkInfo.wsTalk1 = L"빨리 처치해야겠어.";
		}
		else
		{
			tTalkInfo.fFace = { 1.0f, 0.0f };
			tTalkInfo.wsTalk0 = L"기름이 달라붙어서, 몸이 무거워......";
			tTalkInfo.wsTalk1 = L"";
		}
	}
	break;

	case 96:
	{
		_uint iNum = CMathUtils::RandomUInt(2);

		if (0 == iNum)
		{
			tTalkInfo.fFace = { 0.0f, 0.0f };
			tTalkInfo.wsTalk0 = L"으... 감전된 느낌 이상해";
			tTalkInfo.wsTalk1 = L"시덴은 이런 느낌이 항상 드는걸까...";
		}
		else
		{
			tTalkInfo.fFace = { 0.0f, 0.0f };
			tTalkInfo.wsTalk0 = L"으... 몸이 마음대로 안 움직여.";
			tTalkInfo.wsTalk1 = L"";
		}
	}
	break;

	case 97:
	{
		_uint iNum = CMathUtils::RandomUInt(2);

		if (0 == iNum)
		{
			tTalkInfo.fFace = { 1.0f, 0.0f };
			tTalkInfo.wsTalk0 = L"물 아깝게 물을 가득 뿌리는구만.";
			tTalkInfo.wsTalk1 = L"이왕 뿌려주는거 자연에 뿌려주면 좋을텐데 말이야...";
		}
		else
		{
			tTalkInfo.fFace = { 1.0f, 0.0f };
			tTalkInfo.wsTalk0 = L"몸에 물이 묻어서 움짐이 둔해졌어.";
			tTalkInfo.wsTalk1 = L"다들 조심해!";
		}
	}
	break;

	//******************************** 몬스터가 상태이상 걸렸을 때 98 ~  *
	case 98:
	{
		if (LEVEL_NOW == LEVEL_TUTORIAL) return;

		_uint iNum = CMathUtils::RandomUInt(5);

		if (0 == iNum)
		{
			tTalkInfo.fFace = { 2.0f, 0.0f };
			tTalkInfo.wsTalk0 = L"하나비 봤어?";
			tTalkInfo.wsTalk1 = L"너의 초뇌능력을 이용했어!";
		}
		else if (1 == iNum)
		{
			tTalkInfo.fFace = { 2.0f, 2.0f };
			tTalkInfo.wsTalk0 = L"유이토! 나이스였어~";
			tTalkInfo.wsTalk1 = L"";
		}
		else if (2 == iNum)
		{
			tTalkInfo.fFace = { 1.0f, 2.0f };
			tTalkInfo.wsTalk0 = L"좋았어 유이토!";
			tTalkInfo.wsTalk1 = L"이렇게 더 하면 금방 죽겠는걸?";
		}
		else
		{
			tTalkInfo.fFace = { 2.0f, 2.0f };
			tTalkInfo.wsTalk0 = L"나이스 유이토!";
			tTalkInfo.wsTalk1 = L"내 초뇌능력을 잘 이용해줬구나!";
		}
	}
	break;

	case 99:
	{
		_uint iNum = CMathUtils::RandomUInt(2);

		if (0 == iNum)
		{
			tTalkInfo.fFace = { 0.0f, 4.0f };
			tTalkInfo.wsTalk0 = L"유이토, ";
			tTalkInfo.wsTalk1 = L"하나비의 초뇌능력을 이용해보는게 어때?";
		}
		else
		{
			tTalkInfo.fFace = { 0.0f, 2.0f };
			tTalkInfo.wsTalk0 = L"유이토 내 초뇌능력을 이용해";
			tTalkInfo.wsTalk1 = L"공격이 더 강하게 들어갈거야!";
		}
	}
	break;

	case 100:
	{
		_uint iNum = CMathUtils::RandomUInt(5);

		if (0 == iNum)
		{
			tTalkInfo.fFace = { 0.0f, 7.0f };
			tTalkInfo.wsTalk0 = L"좀 할줄 아는걸?";
			tTalkInfo.wsTalk1 = L"";
		}
		else 	if (1 == iNum)
		{
			tTalkInfo.fFace = { 0.0f, 7.0f };
			tTalkInfo.wsTalk0 = L"좋아. 앞으로 이렇게";
			tTalkInfo.wsTalk1 = L"감전을 잘 이용해봐.";
		}
		else 	if (2 == iNum)
		{
			tTalkInfo.fFace = { 1.0f, 7.0f };
			tTalkInfo.wsTalk0 = L"나이스 좋았어 유이토.";
			tTalkInfo.wsTalk1 = L"";
		}
		else 	if (3 == iNum)
		{
			tTalkInfo.fFace = { 1.0f, 7.0f };
			tTalkInfo.wsTalk0 = L"좋았어 유이토 내 초뇌능력은";
			tTalkInfo.wsTalk1 = L"다른 공격 보다 범위가 넓어. 잘 활용해봐.";
		}
		else
		{
			tTalkInfo.fFace = { 0.0f, 7.0f };
			tTalkInfo.wsTalk0 = L"좋아 그거지!";
			tTalkInfo.wsTalk1 = L"좋았어 유이토";
		}
	}
	break;

	case 101:
	{
		_uint iNum = CMathUtils::RandomUInt(2);

		if (0 == iNum)
		{
			tTalkInfo.fFace = { 0.0f, 3.0f };
			tTalkInfo.wsTalk0 = L"물 상태에상에 빠졌어.";
			tTalkInfo.wsTalk1 = L"지금이야!";
		}
		else
		{
			tTalkInfo.fFace = { 0.0f, 2.0f };
			tTalkInfo.wsTalk0 = L"지금이야 어서 공격하자!";
			tTalkInfo.wsTalk1 = L"";
		}
	}
	break;

	case 102:
	{
		_uint iNum = CMathUtils::RandomUInt(5);

		if (0 == iNum)
		{
			tTalkInfo.fFace = { 1.0f, 2.0f };
			tTalkInfo.wsTalk0 = L"유이토! 조심해!";
			tTalkInfo.wsTalk1 = L"";
		}
		else 	if (1 == iNum)
		{
			tTalkInfo.fFace = { 0.0f, 4.0f };
			tTalkInfo.wsTalk0 = L"유이토 몸 챙겨가면서 해";
			tTalkInfo.wsTalk1 = L"다치면 안 돼.";
		}
		else 	if (2 == iNum)
		{
			tTalkInfo.fFace = { 0.0f, 2.0f };
			tTalkInfo.wsTalk0 = L"후... 나이스 타이밍";
			tTalkInfo.wsTalk1 = L"유이토 체력이 떨어지지 않도록 조심해!";
		}
		else 	if (3 == iNum)
		{
			tTalkInfo.fFace = { 1.0f, 4.0f };
			tTalkInfo.wsTalk0 = L"다치지 않아서 다행이야.";
			tTalkInfo.wsTalk1 = L"";
		}
		else 	if (4 == iNum)
		{
			tTalkInfo.fFace = { 0.0f, 0.0f };
			tTalkInfo.wsTalk0 = L"고마워 하나비, 츠구미";
			tTalkInfo.wsTalk1 = L"덕분에 살았어.";
		}
		else
		{
			tTalkInfo.fFace = { 1.0f, 0.0f };
			tTalkInfo.wsTalk0 = L"후... 동료들 아니었으면 큰일 날 뻔했어.";
			tTalkInfo.wsTalk1 = L"체력을 신경 써가면서 해야겠어.";
		}
	}
	break;

	//******************************** 병원 수다 103 104
	case 103:
	{
		tTalkInfo.fFace = { 0.0f, 5.0f };
		tTalkInfo.wsTalk0 = L"중대장님이 약이 없다는";
		tTalkInfo.wsTalk1 = L"사실을 알게되면 괜찮으실까...";
	}
	break;

	//******************************** 중대장 스토리 
	case 104:
	{
		tTalkInfo.fFace = { 0.0f, 0.0f };
		tTalkInfo.wsTalk0 = L"..?! ";
		tTalkInfo.wsTalk1 = L"다들 어디간거지?!";
	}
	break;
	case 105:
	{
		tTalkInfo.fFace = { 0.0f, 5.0f };
		tTalkInfo.wsTalk0 = L"유이토, 괜찮아?";
		tTalkInfo.wsTalk1 = L"우린 이상한 곳에 격리당했어..";
	}
	break;
	case 106:
	{
		tTalkInfo.fFace = { 0.0f, 5.0f };
		tTalkInfo.wsTalk0 = L"SAS 사용에는 문제가 없으니";
		tTalkInfo.wsTalk1 = L"마음껏 활용해 중대장을 막아줘!!";
	}
	break;

	//******************************** 말 잡고 나서 *
	case 107:
	{
		tTalkInfo.fFace = { 0.0f, 17.0f };
		tTalkInfo.wsTalk0 = L"다들 고생했어!";
		tTalkInfo.wsTalk1 = L"남은 동료들도 지금 마을에 있어.";
	}
	break;
	case 108:
	{
		tTalkInfo.fFace = { 0.0f, 17.0f };
		tTalkInfo.wsTalk0 = L"우리도 가서 합류하자.";
		tTalkInfo.wsTalk1 = L"";
	}
	break;

	////******************************** 나오미 죽었을 때 109 ~ 114
	case 109:
	{
		tTalkInfo.fFace = { 0.0f, 13.0f };
		tTalkInfo.wsTalk0 = L"다들 멈춰!!!!!";
		tTalkInfo.wsTalk1 = L"";
	}
	break;
	case 110:
	{
		tTalkInfo.fFace = { 0.0f, 13.0f };
		tTalkInfo.wsTalk0 = L"지금 나도 믿기지 않지만... 이 괴이는";
		tTalkInfo.wsTalk1 = L"내 딸 나오미야... 무슨일이 있었던거지...";
	}
	break;
	case 111:
	{
		tTalkInfo.fFace = { 1.0f, 4.0f };
		tTalkInfo.wsTalk0 = L"나오미 라구요...?";
		tTalkInfo.wsTalk1 = L"";
	}
	break;
	case 112:
	{
		tTalkInfo.fFace = { 0.0f, 13.0f };
		tTalkInfo.wsTalk0 = L"그렇다. 지금 이게 어떻게 된 상황인지는 모르겠지만...";
		tTalkInfo.wsTalk1 = L"여기는 내가 지킬 테니";
	}
	break;
	case 113:
	{
		tTalkInfo.fFace = { 0.0f, 13.0f };
		tTalkInfo.wsTalk0 = L"다들 병원으로 가서 나오미의 약을";
		tTalkInfo.wsTalk1 = L"찾아주기를 부탁한다.";
	}
	break;
	case 114:
	{
		tTalkInfo.fFace = { 0.0f, 0.0f };
		tTalkInfo.wsTalk0 = L"알겠습니다.";
		tTalkInfo.wsTalk1 = L"";
	}
	break;

	//******************************** 115
	case 115:
	{
		tTalkInfo.fFace = { 0.0f, 5.0f };
		tTalkInfo.wsTalk0 = L"유이토 이제 우리도 합류했어";
		tTalkInfo.wsTalk1 = L"다 같이 시간역행을 막아보자.";
	}
	break;
	//case 116:
	//{
	//	tTalkInfo.fFace = { 0.0f, 0.0f };
	//	tTalkInfo.wsTalk0 = L"알겠습니다.";
	//	tTalkInfo.wsTalk1 = L"";
	//}
	//break;


	/*
	
	#include "GameManager.h"
	CGameManager::GetEnemyReport()->Set_LeftTalk(90);
	
	
	//******************************** 수다
			case 80:
	{
		tTalkInfo.fFace = { 0.0f, 0.0f };
		tTalkInfo.wsTalk0 = L"더욱더 강해질 거야......!";
		tTalkInfo.wsTalk1 = L"";
	}
	break;

	//******************************** 드라이브 쓸 때
					case 80:
	{
		tTalkInfo.fFace = { 0.0f, 0.0f };
		tTalkInfo.wsTalk0 = L"이게 드라이브.....";
		tTalkInfo.wsTalk1 = L"온몽의 신경이 예민해지는 느낌이 들어.";
	}
	break;		
						case 80:
	{
		tTalkInfo.fFace = { 0.0f, 0.0f };
		tTalkInfo.wsTalk0 = L"공격이 먹히고 있어......!";
		tTalkInfo.wsTalk1 = L"지금 노린 곳이 약점인가.";
	}
	break;		

	

	*/

	default:
	{
		tTalkInfo.fFace = { 0.0f, 10.0f };
		tTalkInfo.wsTalk0 = TEXT("쥬신 게임 아카데미 131기");
		tTalkInfo.wsTalk1 = TEXT("......");
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

	while (!m_qCurrentTalk.empty()) m_qCurrentTalk.pop();
}