#include "stdafx.h"
#include "..\public\Canvas_Tutorial.h"
#include "GameInstance.h"

#include "TutorialUI.h"
#include "Tutorial_CheckUI.h"
#include "Tutorial_YesNoUI.h"
#include "Tutorial_TipsUI.h"

CCanvas_Tutorial::CCanvas_Tutorial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_Tutorial::CCanvas_Tutorial(const CCanvas_Tutorial& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_Tutorial::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_Tutorial::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	return S_OK;
}

void CCanvas_Tutorial::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	if (CGameInstance::GetInstance()->KeyDown(DIK_0))
	{
		CGameObject::SetDelete();
	}

	Tutorial_Tick();
	Tips_Tick();
	Success_Tick(TimeDelta);
}

void CCanvas_Tutorial::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	if (ImGui::Button("Totorial1"))
	{
		Set_Tutorial(LOCKON);
	}
	ImGui::SameLine();
	if (ImGui::Button("Totorial2"))
	{
		Set_Tutorial(FIGHTINGSTYLE);
	}

	if (ImGui::Button("Tips Open"))
	{
		Set_Tips(TIPS5);
	}
	ImGui::SameLine();
	if (ImGui::Button("Tips Shut"))
	{
		Set_TipsDelete(TIPS5);
	}
	ImGui::SameLine();
	if (ImGui::Button("Success"))
	{
		Set_Success();
	}
}

void CCanvas_Tutorial::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_Tutorial::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

void CCanvas_Tutorial::Tutorial_Tick()
{
	if (m_eTutorial == TUTORIAL_END)
		return;

	_tchar szTag[MAX_PATH];

	switch (m_eTutorial)
	{
	case Client::CCanvas_Tutorial::LOCKON:
		wsprintf(szTag, TEXT("Tutorial0"));
		break;
	case Client::CCanvas_Tutorial::FIGHTINGSTYLE:
		wsprintf(szTag, TEXT("Tutorial1"));
		break;
	case Client::CCanvas_Tutorial::SPECIALATTACK:
		wsprintf(szTag, TEXT("Tutorial3"));
		break;
	case Client::CCanvas_Tutorial::ADDRUSHATTACK:
		wsprintf(szTag, TEXT("Tutorial4"));
		break;
	case Client::CCanvas_Tutorial::ADDPSYCHOKINESISATTACK:
		wsprintf(szTag, TEXT("Tutorial5"));
		break;
	default:
		m_eTutorial = TUTORIAL_END;
		break;
	}

	Tutorial(m_eTutorial, szTag);
}

void CCanvas_Tutorial::Tutorial(const TUTORIAL & eTUTORIAL, const _tchar * pChildTag)
{
	// 외부에서 m_arrTutorial[튜토리얼 타입] 을 true 로 변경하면 실행된다.
	if (false == m_arrTutorial[eTUTORIAL])
		return;

	// 만약에 존재하지 않는 튜토리얼 이라면 메시지 박스를 띄운다. (예전에 테스트 용으로 하나만 만들어 놔서 다른 애들을 누를 때를 위한 예외처리 였다.)
	if (nullptr == Find_ChildUI(pChildTag))
	{
		MSG_BOX("Objects Already Deleted");
		return;
	}

	// 이제 위에서 Set 한 튜토리얼 타입을 Set 해준다.
	if (false == m_bTutorialOpen)
	{
		m_bTutorialOpen = true;
		dynamic_cast<CTutorialUI*>(Find_ChildUI(pChildTag))->Set_OnTutorial();
		return;
	}

	vector<wstring> except { PLAYERTEST_LAYER_FRONTUI };
	CGameInstance::GetInstance()->SetTimeRatio(0.0f, &except);

	if (FIGHTINGSTYLE == m_arrTutorial[eTUTORIAL])
	{
		_bool	bNextPage = dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_ZNextPage"))->Get_NextPage();

		if (false == bNextPage)	// bNextPage : false->Tutorial1 / true->Tutorial2
		{
			Find_ChildUI(L"Tutorial_ZNextPage")->Set_Position({ 207.0f, -190.0f });
			dynamic_cast<CTutorialUI*>(Find_ChildUI(L"Tutorial1"))->SetVisible(true);
			dynamic_cast<CTutorialUI*>(Find_ChildUI(L"Tutorial2"))->SetVisible(false);
		}
		else
		{
			Find_ChildUI(L"Tutorial_ZNextPage")->Set_Position({ 135.0f, -190.0f });
			dynamic_cast<CTutorialUI*>(Find_ChildUI(L"Tutorial1"))->SetVisible(false);
			dynamic_cast<CTutorialUI*>(Find_ChildUI(L"Tutorial2"))->SetVisible(true);

			_bool	bInvisblePush = dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_InvisibleBox"))->Get_Invisible();

			if (true == bInvisblePush)
			{
				if (false == m_bCheckOpen)
				{
					m_bCheckOpen = true;
					Find_ChildUI(L"Tutorial_YesBox")->SetVisible(true);
					Find_ChildUI(L"Tutorial_Icon0")->SetVisible(true);
					Find_ChildUI(L"Tutorial_Icon1")->SetVisible(true);
					Find_ChildUI(L"Tutorial_Icon2")->SetVisible(true);
					dynamic_cast<CTutorial_CheckUI*>(Find_ChildUI(L"Tutorial_Check0"))->Set_OnTutorial();
				}

				KeyInput_Yes();
				KeyInput_No();
				Check_Tick();
			}
		}
	}
	else
	{
		_bool	bInvisblePush = dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_InvisibleBox"))->Get_Invisible();

		if (true == bInvisblePush)
		{
			if (false == m_bCheckOpen)
			{
				m_bCheckOpen = true;
				Find_ChildUI(L"Tutorial_YesBox")->SetVisible(true);
				Find_ChildUI(L"Tutorial_Icon0")->SetVisible(true);
				Find_ChildUI(L"Tutorial_Icon1")->SetVisible(true);
				Find_ChildUI(L"Tutorial_Icon2")->SetVisible(true);
				dynamic_cast<CTutorial_CheckUI*>(Find_ChildUI(L"Tutorial_Check0"))->Set_OnTutorial();
			}

			KeyInput_Yes();
			KeyInput_No();
			Check_Tick();
		}
	}

	// 초기화
	if (2 == m_iYesCount)
	{
		if (false == m_bCheckClose)
		{
			m_bCheckClose = true;
			dynamic_cast<CTutorial_CheckUI*>(Find_ChildUI(L"Tutorial_Check0"))->Set_OffTutorial();

			Find_ChildUI(L"Tutorial_YesBox")->SetVisible(false);
			Find_ChildUI(L"Tutorial_NoBox")->SetVisible(false);
			Find_ChildUI(L"Tutorial_Icon0")->SetVisible(false);
			Find_ChildUI(L"Tutorial_Icon1")->SetVisible(false);
			Find_ChildUI(L"Tutorial_Icon2")->SetVisible(false); 
		}

		if (true == dynamic_cast<CTutorial_CheckUI*>(Find_ChildUI(L"Tutorial_Check0"))->Get_End())
		{
			Find_ChildUI(L"Tutorial_Check0")->SetVisible(false);
			dynamic_cast<CTutorialUI*>(Find_ChildUI(pChildTag))->Set_OffTutorial();
		}

		if (true == dynamic_cast<CTutorialUI*>(Find_ChildUI(pChildTag))->Get_End())
		{
			Find_ChildUI(pChildTag)->SetDelete();
			dynamic_cast<CTutorialUI*>(Find_ChildUI(pChildTag))->Set_End();
			dynamic_cast<CTutorial_CheckUI*>(Find_ChildUI(L"Tutorial_Check0"))->Set_End();
			dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_InvisibleBox"))->Set_Invisible();

			m_iYesCount = 0;
			m_arrTutorial[eTUTORIAL] = false;
			m_bTutorialOpen = false;
			m_bCheckOpen = false;
			m_bCheckClose = false;

			CGameInstance::GetInstance()->ResetDefaultTimeRatio();
			SetDelete();
		}
	}

	if (2 == m_iNoCount)
	{
		if (false == m_bCheckClose)
		{
			m_bCheckClose = true;
			dynamic_cast<CTutorial_CheckUI*>(Find_ChildUI(L"Tutorial_Check0"))->Set_OffTutorial();

			Find_ChildUI(L"Tutorial_YesBox")->SetVisible(false);
			Find_ChildUI(L"Tutorial_NoBox")->SetVisible(false);
			Find_ChildUI(L"Tutorial_Icon0")->SetVisible(false);
			Find_ChildUI(L"Tutorial_Icon1")->SetVisible(false);
			Find_ChildUI(L"Tutorial_Icon2")->SetVisible(false);
		}

		if (true == dynamic_cast<CTutorial_CheckUI*>(Find_ChildUI(L"Tutorial_Check0"))->Get_End())
		{
			Find_ChildUI(L"Tutorial_Check0")->SetVisible(false);
			Find_ChildUI(L"Tutorial_NoBox")->SetVisible(false);
			dynamic_cast<CTutorial_CheckUI*>(Find_ChildUI(L"Tutorial_Check0"))->Set_End();
			dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_InvisibleBox"))->Set_Invisible();

			dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon0"))->Set_Position(_float2(-50.0f, -19.0f));
			dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon1"))->Set_Position(_float2(-51.0f, -19.0f));
			dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon2"))->Set_Position(_float2(-49.0f, -19.0f));

			m_iNoCount = 0;
			m_bCheckOpen = false;
			m_bCheckClose = false;
		}
	}
}

void CCanvas_Tutorial::Check_Tick()
{
	// 깜박이 한 번 되고 나면
	if (1 == dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon1"))->Get_OneRenderCount())
		dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon2"))->Set_TwoRneder();

	if (true == dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon2"))->Get_TwoRnederEnd())
	{
		dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon1"))->Set_OneTwoAlpha();
		dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon2"))->Set_OneTwoAlpha();
	}

	if (true == dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon2"))->Get_AlphaEnd())
	{
		dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon1"))->Set_OneReset();
		dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon2"))->Set_TwoReset();
	}
}

void CCanvas_Tutorial::KeyInput_Yes()
{
	if (false == dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_YesBox"))->Get_InputYes())
	{
		m_bYesPush = false;
		return;
	}

	if (false == m_bYesPush)
	{
		m_iNoCount = 0;
		m_bYesPush = true;
		
		++m_iYesCount;
		if (2 == m_iYesCount)
		{
			dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon0"))->Set_ZeroShader();
		}

		Find_ChildUI(L"Tutorial_YesBox")->SetVisible(true);
		Find_ChildUI(L"Tutorial_NoBox")->SetVisible(false);

		dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon0"))->Set_Position(_float2(-50.0f, -19.0f));
		dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon1"))->Set_Position(_float2(-51.0f, -19.0f));
		dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon2"))->Set_Position(_float2(-49.0f, -19.0f));
	}
}

void CCanvas_Tutorial::KeyInput_No()
{
	if (false == dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_NoBox"))->Get_InputNo())
	{
		m_bNoPush = false;
		return;
	}

	if (false == m_bNoPush)
	{
		m_iYesCount = 0;
		m_bNoPush = true;

		++m_iNoCount;
		if (2 == m_iNoCount)
		{
			dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon0"))->Set_ZeroShader();
		}

		Find_ChildUI(L"Tutorial_YesBox")->SetVisible(false);
		Find_ChildUI(L"Tutorial_NoBox")->SetVisible(true);

		dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon0"))->Set_Position(_float2(-50.0f, -52.0f));
		dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon1"))->Set_Position(_float2(-51.0f, -52.0f));
		dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon2"))->Set_Position(_float2(-49.0f, -52.0f));
	}
}

void CCanvas_Tutorial::NextInput()
{

}

void CCanvas_Tutorial::Tips_Tick()
{
	if (TIPS_END == m_eTips)
		return;

	_tchar szTag[MAX_PATH];

	switch (m_eTips)
	{
	case Client::CCanvas_Tutorial::TIPS0:
		wsprintf(szTag, TEXT("Tutorial_Tips0"));
		break;
	case Client::CCanvas_Tutorial::TIPS1:
		wsprintf(szTag, TEXT("Tutorial_Tips1"));
		break;
	case Client::CCanvas_Tutorial::TIPS2:
		wsprintf(szTag, TEXT("Tutorial_Tips2"));
		break;
	case Client::CCanvas_Tutorial::TIPS3:
		wsprintf(szTag, TEXT("Tutorial_Tips3"));
		break;
	case Client::CCanvas_Tutorial::TIPS4:
		wsprintf(szTag, TEXT("Tutorial_Tips4"));
		break;
	case Client::CCanvas_Tutorial::TIPS5:
		wsprintf(szTag, TEXT("Tutorial_Tips5"));
		break;
	case Client::CCanvas_Tutorial::TIPS6:
		wsprintf(szTag, TEXT("Tutorial_Tips6"));
		break;
	case Client::CCanvas_Tutorial::TIPS7:
		wsprintf(szTag, TEXT("Tutorial_Tips7"));
		break;
	case Client::CCanvas_Tutorial::TIPS8:
		wsprintf(szTag, TEXT("Tutorial_Tips8"));
		break;
	case Client::CCanvas_Tutorial::TIPS9:
		wsprintf(szTag, TEXT("Tutorial_Tips9"));
		break;
	case Client::CCanvas_Tutorial::TIPS10:
		wsprintf(szTag, TEXT("Tutorial_Tips10"));
		break;
	default:
		m_eTips = TIPS_END;
		break;
	}

	Tips(m_eTips, szTag);
}

void CCanvas_Tutorial::Tips(const TIPS & eTIPS, const _tchar * pChildTag)
{
	if (nullptr == Find_ChildUI(pChildTag))
		return;

	if (false == m_iTipsOpen)
	{
		m_iTipsOpen = true;
		dynamic_cast<CTutorial_TipsUI*>(Find_ChildUI(pChildTag))->Set_OnTutorial();
	}

	if (false == m_arrTips[eTIPS])
		return;

	if (true == m_arrTips[eTIPS])
	{
		dynamic_cast<CTutorial_TipsUI*>(Find_ChildUI(pChildTag))->Set_OffTutorial();

		if (true == dynamic_cast<CTutorial_TipsUI*>(Find_ChildUI(pChildTag))->Get_End())
		{
			m_eTips = TIPS_END;
			m_iTipsOpen = false;
			m_arrTips[eTIPS] = false;
			SetDelete();
		}
	}
}

void CCanvas_Tutorial::Success_Tick(const _double & TimeDelta)
{
	if (false == m_bSuccess)
		return;

	Find_ChildUI(L"Tutorial_Success")->SetVisible(true);

	m_bSuccess_TimeAcc += TimeDelta;
	if (1.5 < m_bSuccess_TimeAcc)
	{
		m_bSuccess_TimeAcc = 0.0;
		m_bSuccess = false;
		Find_ChildUI(L"Tutorial_Success")->SetVisible(false);
		SetDelete();
	}
}

CCanvas_Tutorial * CCanvas_Tutorial::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_Tutorial*		pInstance = new CCanvas_Tutorial(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_Tutorial");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_Tutorial::Clone(void * pArg)
{
	CCanvas_Tutorial*		pInstance = new CCanvas_Tutorial(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_Tutorial");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_Tutorial::Free()
{
	CCanvas::Free();
}
