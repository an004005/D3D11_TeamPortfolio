#include "stdafx.h"
#include "..\public\Canvas_Tutorial.h"
#include "GameInstance.h"

#include "TutorialUI.h"
#include "Tutorial_CheckUI.h"
#include "Tutorial_YesNoUI.h"

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

	Tutorial_Tick();
	Check_Tick();

	if (CGameInstance::GetInstance()->KeyDown(DIK_0))
		Set_Tutorial(LOCKON);
}

void CCanvas_Tutorial::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

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
	switch (m_eTutorial)
	{
	case Client::CCanvas_Tutorial::LOCKON:
		Lockon();
		break;
	case Client::CCanvas_Tutorial::FIGHTINGSTYLE:
		break;
	case Client::CCanvas_Tutorial::SPECIALATTACK:
		break;
	case Client::CCanvas_Tutorial::ADDRUSHATTACK:
		break;
	case Client::CCanvas_Tutorial::ADDPSYCHOKINESISATTACK:
		break;
	case Client::CCanvas_Tutorial::STRENGTHENATTACK:
		break;
	default:
		return; // No Tutorials
		break;
	}
}

void CCanvas_Tutorial::Lockon()
{
	if (false == m_arrTutorial[LOCKON])
		return;

	if (nullptr == Find_ChildUI(L"Tutorial0"))
	{
		MSG_BOX("Objects Already Deleted");
		return;
	}

	if (false == m_bTutorialOpen)
	{
		m_bTutorialOpen = true;
		dynamic_cast<CTutorialUI*>(Find_ChildUI(L"Tutorial0"))->Set_OnTutorial();
		return;
	}

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
			dynamic_cast<CTutorialUI*>(Find_ChildUI(L"Tutorial0"))->Set_OffTutorial();
		}

		if (true == dynamic_cast<CTutorialUI*>(Find_ChildUI(L"Tutorial0"))->Get_End())
		{
			Find_ChildUI(L"Tutorial0")->SetDelete();
			dynamic_cast<CTutorialUI*>(Find_ChildUI(L"Tutorial0"))->Set_End();
			dynamic_cast<CTutorial_CheckUI*>(Find_ChildUI(L"Tutorial_Check0"))->Set_End();
			dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_InvisibleBox"))->Set_Invisible();

			m_iYesCount = 0;
			m_arrTutorial[LOCKON] = false;
			m_bTutorialOpen = false;
			m_bCheckOpen = false;
			m_bCheckClose = false;
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
