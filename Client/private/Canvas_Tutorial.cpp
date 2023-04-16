#include "stdafx.h"
#include "..\public\Canvas_Tutorial.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "PlayerInfoManager.h"
#include "GameManager_Tutorial.h"
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

	CUI_Manager::GetInstance()->PlaySound("TABLarge");

	return S_OK;
}

void CCanvas_Tutorial::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	if (CGameInstance::GetInstance()->KeyDown(DIK_0))
	{
		CGameInstance::GetInstance()->ResetDefaultTimeRatio();
		CGameObject::SetDelete();
	}

	Tutorial_Tick();
	Tips_Tick(TimeDelta);
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

void CCanvas_Tutorial::LoadFromJson(const Json& json)
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
	case Client::CCanvas_Tutorial::ADDRUSHADDPSYCHOKINESIS:
		wsprintf(szTag, TEXT("Tutorial6"));
		break;
	default:
		m_eTutorial = TUTORIAL_END;
		break;
	}

	Tutorial(m_eTutorial, szTag);
}

void CCanvas_Tutorial::Tutorial(const TUTORIAL& eTUTORIAL, const _tchar* pChildTag)
{
	// �ܺο��� m_arrTutorial[Ʃ�丮�� Ÿ��] �� true �� �����ϸ� ����ȴ�.
	if (false == m_arrTutorial[eTUTORIAL])
		return;

	// ���࿡ �������� �ʴ� Ʃ�丮�� �̶�� �޽��� �ڽ��� ����. (������ �׽�Ʈ ������ �ϳ��� ����� ���� �ٸ� �ֵ��� ���� ���� ���� ����ó�� ����.)
	if (nullptr == Find_ChildUI(pChildTag))
	{
		MSG_BOX("Objects Already Deleted");
		return;
	}

	// ���� ������ Set �� Ʃ�丮�� Ÿ���� Set ���ش�.
	if (false == m_bTutorialOpen)
	{
		CUI_Manager::GetInstance()->SetMouseActive(true);
		m_bTutorialOpen = true;
		dynamic_cast<CTutorialUI*>(Find_ChildUI(pChildTag))->Set_OnTutorial();

		if (FIGHTINGSTYLE == eTUTORIAL)
			dynamic_cast<CTutorialUI*>(Find_ChildUI(L"Tutorial2"))->Set_OnTutorial();

		return;
	}

	vector<wstring> except{ PLAYERTEST_LAYER_FRONTUI };
	CGameInstance::GetInstance()->SetTimeRatio(0.0f, &except);

	if (FIGHTINGSTYLE == eTUTORIAL)
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

			if (CGameInstance::GetInstance()->KeyDown(DIK_RETURN))
				m_bEnter = !m_bEnter;

			if (true == bInvisblePush || true == m_bEnter)
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

		if (CGameInstance::GetInstance()->KeyDown(DIK_RETURN))
			m_bEnter = !m_bEnter;

		if (true == bInvisblePush || true == m_bEnter)
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

	// �ʱ�ȭ
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

			if (FIGHTINGSTYLE == eTUTORIAL)
			{
				dynamic_cast<CTutorialUI*>(Find_ChildUI(L"Tutorial2"))->Set_OffTutorial();
				dynamic_cast<CTutorialUI*>(Find_ChildUI(pChildTag))->Set_OffTutorial();
			}
			else
				dynamic_cast<CTutorialUI*>(Find_ChildUI(pChildTag))->Set_OffTutorial();
		}

		if (true == dynamic_cast<CTutorialUI*>(Find_ChildUI(pChildTag))->Get_End())
		{
			CUI_Manager::GetInstance()->PlaySound("TABSmall");
		
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
			CGameObject::SetDelete();
			CUI_Manager::GetInstance()->SetMouseActive(true);
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
	// ������ �� �� �ǰ� ����
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
	if (CGameInstance::GetInstance()->KeyDown(DIK_UP))
	{
		Find_ChildUI(L"Tutorial_YesBox")->SetVisible(true);
		Find_ChildUI(L"Tutorial_NoBox")->SetVisible(false);

		dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon0"))->Set_Position(_float2(-50.0f, -19.0f));
		dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon1"))->Set_Position(_float2(-51.0f, -19.0f));
		dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon2"))->Set_Position(_float2(-49.0f, -19.0f));
	}

	if (false == dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_YesBox"))->Get_InputYes() /*||
		Find_ChildUI(L"Tutorial_Icon0")->Get_Position().x == -19.0f && true == CGameInstance::GetInstance()->KeyDown(DIK_RETURN)*/)
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
	if (CGameInstance::GetInstance()->KeyDown(DIK_DOWN))
	{
		Find_ChildUI(L"Tutorial_YesBox")->SetVisible(false);
		Find_ChildUI(L"Tutorial_NoBox")->SetVisible(true);

		dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon0"))->Set_Position(_float2(-50.0f, -52.0f));
		dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon1"))->Set_Position(_float2(-51.0f, -52.0f));
		dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon2"))->Set_Position(_float2(-49.0f, -52.0f));
	}

	if (false == dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_NoBox"))->Get_InputNo() ||
		Find_ChildUI(L"Tutorial_Icon0")->Get_Position().x == -52.0f && true == CGameInstance::GetInstance()->KeyDown(DIK_RETURN))
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

void CCanvas_Tutorial::Tips_Tick(const _double& TimeDelta)
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

	Tips(m_eTips, szTag, TimeDelta);
}

void CCanvas_Tutorial::Tips(const TIPS& eTIPS, const _tchar* pChildTag, const _double& TimeDelta)
{
	if (nullptr == Find_ChildUI(pChildTag))
		return;

	if (false == m_iTipsOpen)
	{
		m_iTipsOpen = true;
		dynamic_cast<CTutorial_TipsUI*>(Find_ChildUI(pChildTag))->Set_OnTutorial();
	}

	// ���ϴ� Ÿ�ֿ̹� Tips �� ���� ���� �� if �� �߰�
	if (TIPS4 == m_eTips)
	{
		m_dTips_TimeAcc += TimeDelta;
		if (3.0 < m_dTips_TimeAcc)
		{
			dynamic_cast<CTutorial_TipsUI*>(Find_ChildUI(pChildTag))->Set_OffTutorial();

			if (true == dynamic_cast<CTutorial_TipsUI*>(Find_ChildUI(pChildTag))->Get_End())
				SetDelete();
		}
	}
	if (TIPS0 == m_eTips)
	{
		if (true == dynamic_cast<CGameManager_Tutorial*>(CGameManager::GetInstance())->Get_KineticAttackCheck())
		{
			dynamic_cast<CTutorial_TipsUI*>(Find_ChildUI(pChildTag))->Set_OffTutorial();
			m_bSuccess = true;

			if (true == dynamic_cast<CTutorial_TipsUI*>(Find_ChildUI(pChildTag))->Get_End())
			{
				//m_bSuccess = true;
			}
		}
	}
	if (TIPS1 == m_eTips)
	{
		m_dTips_TimeAcc += TimeDelta;
		if (3.0 < m_dTips_TimeAcc)
		{
			dynamic_cast<CTutorial_TipsUI*>(Find_ChildUI(pChildTag))->Set_OffTutorial();

			if (true == dynamic_cast<CTutorial_TipsUI*>(Find_ChildUI(pChildTag))->Get_End())
			{
				m_eTips = TIPS2;
				dynamic_cast<CTutorial_TipsUI*>(Find_ChildUI(L"Tutorial_Tips2"))->Set_OnTutorial();
			}
		}
	}
	if (TIPS2 == m_eTips)
	{
		dynamic_cast<CGameManager_Tutorial*>(CGameManager::GetInstance())->Set_KineticAttackAndLockOn();
		if (true == dynamic_cast<CGameManager_Tutorial*>(CGameManager::GetInstance())->Get_KineticAttackAndLockOn() &&
			nullptr != CPlayerInfoManager::GetInstance()->Get_TargetedMonster())
		{
			dynamic_cast<CTutorial_TipsUI*>(Find_ChildUI(pChildTag))->Set_OffTutorial();
			m_bSuccess = true;
		}
	}
	if (TIPS3 == m_eTips)
	{
		dynamic_cast<CGameManager_Tutorial*>(CGameManager::GetInstance())->Set_FlatHit();
		if (true == dynamic_cast<CGameManager_Tutorial*>(CGameManager::GetInstance())->Get_FlatHit())
		{
			dynamic_cast<CTutorial_TipsUI*>(Find_ChildUI(pChildTag))->Set_OffTutorial();

			if (true == dynamic_cast<CTutorial_TipsUI*>(Find_ChildUI(pChildTag))->Get_End())
			{
				Set_Tutorial(ADDPSYCHOKINESISATTACK);
				m_eTips = TIPS_END;
			}
		}
	}
	if (TIPS7 == m_eTips)
	{
		if (true == dynamic_cast<CGameManager_Tutorial*>(CGameManager::GetInstance())->Get_HitAir())
		{
			dynamic_cast<CTutorial_TipsUI*>(Find_ChildUI(pChildTag))->Set_OffTutorial();
			m_bSuccess = true;

			if (true == dynamic_cast<CTutorial_TipsUI*>(Find_ChildUI(pChildTag))->Get_End())
			{
				//m_bSuccess = true;
			}
		}
	}
	// �ܺο��� ���ִ� ���
	else
	{
		if (true == m_arrTips[eTIPS])
		{
			dynamic_cast<CTutorial_TipsUI*>(Find_ChildUI(pChildTag))->Set_OffTutorial();

			if (true == dynamic_cast<CTutorial_TipsUI*>(Find_ChildUI(pChildTag))->Get_End())
			{
				m_eTips = TIPS_END;
				m_iTipsOpen = false;
				m_arrTips[eTIPS] = false;
				m_bSuccess = true;
			}
		}
	}
}

void CCanvas_Tutorial::Success_Tick(const _double& TimeDelta)
{
	if (false == m_bSuccess || m_eTutorial == ADDPSYCHOKINESISATTACK)
		return;

	Find_ChildUI(L"Tutorial_Success")->SetVisible(true);

	m_bSuccess_TimeAcc += TimeDelta;
	if (2.0 < m_bSuccess_TimeAcc)
	{
		m_bSuccess = false;
		m_bSuccess_TimeAcc = 0.0;
		Find_ChildUI(L"Tutorial_Success")->SetVisible(false);
		SetDelete();
	}
}

CCanvas_Tutorial* CCanvas_Tutorial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCanvas_Tutorial* pInstance = new CCanvas_Tutorial(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_Tutorial");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas* CCanvas_Tutorial::Clone(void* pArg)
{
	CCanvas_Tutorial* pInstance = new CCanvas_Tutorial(*this);

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
