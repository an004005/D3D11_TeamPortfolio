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

	// 0 : 시간 되면 넘기기
	m_dTalk_TimeAcc += TimeDelta;
	if (m_dTalk_TimeAcc >= 3.0 && m_bNextTalk == false)
		Show_Talk();

	// 1 ; 키 입력으로 넘기기
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
		// 퀘스트 있는 경우
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
		//******************************************************** 상점
	case 0:
	{
		tTalkInfo.wsName = { L"상점인 사토리" };
		tTalkInfo.wsTalk0 = { L"내 영혼의 비극과 새로운 의지를 모아 판매한다." };
		tTalkInfo.wsTalk1 = { L"계속해서, 난 공기니까......" };
	}
	break;

	//******************************************************** [0] 1 ~ 11
	case 1:
	{
		tTalkInfo.wsName = { L"하나비" };
		tTalkInfo.wsTalk0 = { L"유이토! 첫 임무는 잘 하고 왔어?" };
		tTalkInfo.wsTalk1 = { L"어땠어? 어땠어? 괜찮았어?" };
	}
	break;

	case 2:
	{
		tTalkInfo.wsName = { L"유이토" };
		tTalkInfo.wsTalk0 = { L"응! 하나비 너는  괜찮아?" };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 3:
	{
		tTalkInfo.wsName = { L"하나비" };
		tTalkInfo.wsTalk0 = { L"웅웅 나 정말 잘 할 수 있을 것 같아!" };
		tTalkInfo.wsTalk1 = { L"그렇죠? 겜마 선배?" };
	}
	break;

	case 4:
	{
		tTalkInfo.wsName = { L"겜마" };
		tTalkInfo.wsTalk0 = { L"하나비 잘했어 그렇지만 항상 조심해야해" };
		tTalkInfo.wsTalk1 = { L"아까 츠구미 아니였으면 다칠뻔 했어" };
	}
	break;

	case 5:
	{
		tTalkInfo.wsName = { L"하나비" };
		tTalkInfo.wsTalk0 = { L"네! 주의 할게요 감사합니다! " };
		tTalkInfo.wsTalk1 = { L"그리고 항상 고마워 츠구미!" };
	}
	break;

	case 6:
	{
		tTalkInfo.wsName = { L"츠구미" };
		tTalkInfo.wsTalk0 = { L"응..." };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 7:
	{
		tTalkInfo.wsName = { L"하나비" };
		tTalkInfo.wsTalk0 = { L"아! 유이토 사토리씨 이야기 들었어?" };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 8:
	{
		tTalkInfo.wsName = { L"유이토" };
		tTalkInfo.wsTalk0 = { L"사토리씨 라면 그 상점인 사토리씨?" };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 9:
	{
		tTalkInfo.wsName = { L"하나비" };
		tTalkInfo.wsTalk0 = { L"응!! 맞아!! 아까 겜마 선배가 알려주셨어." };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 10:
	{
		tTalkInfo.wsName = { L"겜마" };
		tTalkInfo.wsTalk0 = { L"맞아 유이토 상점인 사토리 씨께서는" };
		tTalkInfo.wsTalk1 = { L"우리가 괴이랑 싸울 때 필요한 아이템들을 사거나 판매를 하시지." };
	}
	break;

	case 11:
	{
		tTalkInfo.wsName = { L"유이토" };
		tTalkInfo.wsTalk0 = { L"아 그렇군요 좋은 정보 감사드려요. 겜마 선배." };
		tTalkInfo.wsTalk1 = { L"" };

		m_iQuestNum = 11;
	}
	break;

	//******************************************************** [0] 12 ~ 21 *
	case 12:
	{
		tTalkInfo.wsName = { L"겜마" };
		tTalkInfo.wsTalk0 = { L"유이토 고민이 많아 보이네? 무슨일 있어?" };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 13:
	{
		tTalkInfo.wsName = { L"츠구미" };
		tTalkInfo.wsTalk0 = { L"하나비 생일...?" };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 14:
	{
		tTalkInfo.wsName = { L"유이토" };
		tTalkInfo.wsTalk0 = { L"하하.. 들켜버렸나 그렇게 티가 났어요?" };
		tTalkInfo.wsTalk1 = { L"하나비 생일이 다가오는데 뭘해주면 좋아해 줄지 잘 모르겠어요." };
	}
	break;

	case 15:
	{
		tTalkInfo.wsName = { L"겜마" };
		tTalkInfo.wsTalk0 = { L"하나비 생일 선물 때문에 고민이였구나." };
		tTalkInfo.wsTalk1 = { L"그러게... 하나비는 유이토 너가 준거라면 뭐든 좋아할 것 같은데?" };
	}
	break;

	case 16:
	{
		tTalkInfo.wsName = { L"츠구미" };
		tTalkInfo.wsTalk0 = { L"공사장 2층... 에 특별한 꽃이 있대... " };
		tTalkInfo.wsTalk1 = { L"\"히마와리\" 였나..." };
	}
	break;

	case 17:
	{
		tTalkInfo.wsName = { L"겜마" };
		tTalkInfo.wsTalk0 = { L"하하. 나도 들어본 것 같네 그 꽃을 좋아하는 사람 한테 전해주면," };
		tTalkInfo.wsTalk1 = { L"사랑이 이루어지는 전설이 있다고 하더나... 그런데 아마 구하기 정말 어려울껄?" };
	}
	break;

	case 18:
	{
		tTalkInfo.wsName = { L"유이토" };
		tTalkInfo.wsTalk0 = { L"정말요? 하나비가 받으면 정말 좋아할 것 같은데..." };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 19:
	{
		tTalkInfo.wsName = { L"츠구미" };
		tTalkInfo.wsTalk0 = { L"도와줄게 유이토..." };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 20:
	{
		tTalkInfo.wsName = { L"겜마" };
		tTalkInfo.wsTalk0 = { L"당연하지 동료의 기쁨은 모두의 기쁨이니까." };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 21:
	{
		tTalkInfo.wsName = { L"유이토" };
		tTalkInfo.wsTalk0 = { L"다들 정말 감사해요!" };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	//******************************************************** [1] 22 ~ 26 * 

	case 22:
	{
		tTalkInfo.wsName = { L"하나비" };
		tTalkInfo.wsTalk0 = { L"마침 딱 만났네요!" };
		tTalkInfo.wsTalk1 = { L"유이토! 츠구미! 중대장님이 아주 좋은걸 알려주셨어." };
	}
	break;

	case 23:
	{
		tTalkInfo.wsName = { L"겜마" };
		tTalkInfo.wsTalk0 = { L"중대장님이 좋은걸 알려주셨다면..." };
		tTalkInfo.wsTalk1 = { L"브레인 맵을 말하는 거구나?" };
	}
	break;

	case 24:
	{
		tTalkInfo.wsName = { L"하나비" };
		tTalkInfo.wsTalk0 = { L"맞아요! 겜마선배!" };
		tTalkInfo.wsTalk1 = { L"브레인 맵에서는 우리가 가지고 있는 초 뇌능력을 더욱 더 높일 수 있대!" };
	}
	break;

	case 25:
	{
		tTalkInfo.wsName = { L"겜마" };
		tTalkInfo.wsTalk0 = { L"맞아 각자의 초능력에 맞게 서로 다른 브레인 맵이 존재해." };
		tTalkInfo.wsTalk1 = { L"자기에게 맞는 쪽으로 잘 확인하고 찍는게 중요하지." };
	}
	break;

	case 26:
	{
		tTalkInfo.wsName = { L"유이토" };
		tTalkInfo.wsTalk0 = { L"듣기만 해도 벌써 기대 되는걸." };
		tTalkInfo.wsTalk1 = { L"얼른 브레인 맵을 확인해 보자." };
	}
	break;

	//******************************************************** [2] 27 ~ 28
	case 27:
	{
		tTalkInfo.wsName = { L"유이토" };
		tTalkInfo.wsTalk0 = { L"다들 여기 계셨네요." };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 28:
	{
		tTalkInfo.wsName = { L"유이토" };
		tTalkInfo.wsTalk0 = { L"???" };
		tTalkInfo.wsTalk1 = { L"" };

		m_iQuestNum = 28;
	}
	break;

	//******************************************************** 마지막 대사 29 ~ 31
	case 29:
	{
		tTalkInfo.wsName = { L"중대장" };
		tTalkInfo.wsTalk0 = { L"윽... 으윽...." };
		tTalkInfo.wsTalk1 = { L"나오미... 우리 나오미..." };
	}
	break;

	case 30:
	{
		tTalkInfo.wsName = { L"유이토" };
		tTalkInfo.wsTalk0 = { L"중대장님 정말 죄송합니다." };
		tTalkInfo.wsTalk1 = { L"모두를 위해서 시간역행을 도와드릴 수가 없었습니다." };
	}
	break;

	case 31:
	{
		tTalkInfo.wsName = { L"중대장" };
		tTalkInfo.wsTalk0 = { L"다들 내가 미안하네... 욕심 때문에 이렇게 되었어." };
		tTalkInfo.wsTalk1 = { L"앞으로의 미래를 잘 부탁하네." };
	}
	break;

	case 32:
	{
		tTalkInfo.wsName = { L"중대장" };
		tTalkInfo.wsTalk0 = { L"그렇다... 어쩌다 내 딸이 이렇게 되었는지는 모르겠지만," };
		tTalkInfo.wsTalk1 = { L"여기는 내가 지킬 테니." };
	}
	break;

	case 33:
	{
		tTalkInfo.wsName = { L"중대장" };
		tTalkInfo.wsTalk0 = { L"다들 구 토벌 병원으로 가서 나오미의 약을 찾아주기를 부탁한다." };
		tTalkInfo.wsTalk1 = { L"거기라면 해결할 방법이 있을 거야." };
	}
	break;
	
	case 34:
	{
		tTalkInfo.wsName = { L"유이토" };
		tTalkInfo.wsTalk0 = { L"알겠습니다." };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	//******************************************************** [4] 35 ~ 42
	case 35:
	{
		tTalkInfo.wsName = { L"중대장" };
		tTalkInfo.wsTalk0 = { L"유이토. 너의 초뇌능력이 필요해. 한시라도 빨리 시간을 돌려서.. " };
		tTalkInfo.wsTalk1 = { L"나오미가 그 지하철역으로 가는 걸.. 막아야만 해." };
	}
	break;
	
	case 36:
	{
		tTalkInfo.wsName = { L"중대장" };
		tTalkInfo.wsTalk0 = { L"나오미... 나오미를 위해서..." };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 37:
	{
		tTalkInfo.wsName = { L"유이토" };
		tTalkInfo.wsTalk0 = { L"중대장님, 멈춰주세요." };
		tTalkInfo.wsTalk1 = { L"시간 역행을 하게 되면 미래가 어떻게 변할지 모르는거 알고 계시잖아요." };
	}
	break;

	// case 38:
	// {
	// 	tTalkInfo.wsName = { L"루카" };
	// 	tTalkInfo.wsTalk0 = { L"맞아요, 중대장님. 나오미를 돌아오게 할 방법이 있을 거예요." };
	// 	tTalkInfo.wsTalk1 = { L"저희가 도와드릴게요." };
	// }
	// break;
	
	case 39:
	{
		tTalkInfo.wsName = { L"중대장" };
		tTalkInfo.wsTalk0 = { L"조용히 해!!!" };
		tTalkInfo.wsTalk1 = { L"" };
	}
	break;

	case 40:
	{
		tTalkInfo.wsName = { L"중대장" };
		tTalkInfo.wsTalk0 = { L"네가 딸을 잃은 아픔을 알기나해?" };
		tTalkInfo.wsTalk1 = { L"그깟 미래? 나오미가 없는 미래는 아무래도 상관없어." };
	}
	break;

	case 41:
	{
		tTalkInfo.wsName = { L"유이토" };
		tTalkInfo.wsTalk0 = { L"...!!" };
		tTalkInfo.wsTalk1 = { L"그렇다면 저희가 가만있지 않을겁니다." };
	}
	break;

	case 42:
	{
		tTalkInfo.wsName = { L"중대장" };
		tTalkInfo.wsTalk0 = { L"...그렇군." };
		tTalkInfo.wsTalk1 = { L"너희는 억지로라도 내 계획의 장기말로 써주겠다!" };
	}
	break;

	//******************************************************** 
	default:
	{
		tTalkInfo.wsName = { L"131" };
		tTalkInfo.wsTalk0 = { L"쥬신 게임 아카데미 \" | \' " };
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