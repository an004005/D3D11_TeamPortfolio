#include "stdafx.h"
#include "..\public\Canvas_SASSkill.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "PlayerInfoManager.h"

#include "Player.h"
#include "Canvas_SASSkillMove.h"
#include "SASSkillFullCircleUI.h"

CCanvas_SASSkill::CCanvas_SASSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SASSkill::CCanvas_SASSkill(const CCanvas_SASSkill& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SASSkill::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_SASSkill::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_Canvas(L"Canvas_SASSkill", this);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	return S_OK;
}

void CCanvas_SASSkill::Tick(_double TimeDelta)
{
#pragma region 동료를 만나고 나서부터 Tick 을 돈다.
	// 처음에 하나비를 만난 기준으로 부터 SAS SKilll 창이 보인다.
	if (false == m_bMember0)
	{
		if (false == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::HANABI) ||
			false == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::TSUGUMI) ||
			false == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::GEMMA))
			return;

		m_bMember0 = true;

		for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		{
			if ((*iter).first == L"SASSkill_CtrlText1" || 
				(*iter).first == L"SASSkill_CtrlTexture1" ||
				(*iter).first == L"SASSkill_AltText1" ||
				(*iter).first == L"SASSkill_AltTexture1" ||
				(*iter).first == L"SASSkill_XLeft" ||
				(*iter).first == L"SASSkill_XRight" ||
				(*iter).first == L"SASSkill_XInput" ||
				(*iter).first == L"FullCircle1" ||
				(*iter).first == L"FullCircle5" ||
				(*iter).first == L"FullCircle6" ||
				(*iter).first == L"FullCircle7")
				continue;

			iter->second->SetVisible(true);
		}
	}
#pragma endregion

#pragma region (2번째 만남)
	if (true == m_bMember0 && false == m_bMember1)
	{
		if (true == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::LUCA))
		{
			Find_ChildUI(L"FullCircle1")->SetVisible(true);
			dynamic_cast<CSASSkillFullCircleUI*>(Find_ChildUI(L"FullCircle1"))->Set_TelepotiCircle();
		}
		if(true == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::SEEDEN) &&
			true == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::ARASHI) &&
			true == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::KYOTO))
		{
			m_bMember1 = true;

			Find_ChildUI(L"SASSkill_XLeft")->SetVisible(true);
			Find_ChildUI(L"SASSkill_XRight")->SetVisible(true);
		}
	}
#pragma endregion

	if (true == Find_ChildUI(L"SASSkill_BackGround")->Get_TempOff()) return;

	__super::Tick(TimeDelta);
	
	InputAlt();
	InputCtrl();
	InputX_Tick(TimeDelta);
}

void CCanvas_SASSkill::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

}

void CCanvas_SASSkill::InputX_Tick(const _double & dTimeDelta)
{
	if (false == m_bMember1) return;

	if (CGameInstance::GetInstance()->KeyDown(DIK_1) || 
		CGameInstance::GetInstance()->KeyDown(DIK_2) ||
		CGameInstance::GetInstance()->KeyDown(DIK_3) ||
		CGameInstance::GetInstance()->KeyDown(DIK_4))
	{
		CUI_Manager::GetInstance()->PlaySound("X");
		m_bChangeX = false;
		InputData();

		m_bX = false;
	}

	if (CGameInstance::GetInstance()->KeyDown(DIK_5) ||
		CGameInstance::GetInstance()->KeyDown(DIK_6) ||
		CGameInstance::GetInstance()->KeyDown(DIK_7))
	{
		CUI_Manager::GetInstance()->PlaySound("X");
		m_bChangeX = true;
		InputData();

		m_bX = true;
	}

	if (CGameInstance::GetInstance()->KeyDown(DIK_X))
	{
		CUI_Manager::GetInstance()->PlaySound("X");
		m_bChangeX = !m_bChangeX;
		InputData();

		m_bX = (false == m_bPreX) ? true : false;
	}

	if (true == m_bChangeXButton && m_bX != m_bPreX)
	{
		if (0.0 < m_dChangeX_TimcAcc)
		{
			Find_ChildUI(L"SASSkill_XInput")->SetVisible(true);
		}

		m_dChangeX_TimcAcc += dTimeDelta;

		if (0.2 < m_dChangeX_TimcAcc)
		{
			m_bChangeXButton = false;
			m_dChangeX_TimcAcc = 0.0;
			Find_ChildUI(L"SASSkill_XInput")->SetVisible(false);
		
			m_bPreX = m_bX;
		}
	}
}

void CCanvas_SASSkill::InputData()
{
	m_bChangeXButton = true;

	// [m_bChangeX] false -> 왼쪽 true -> 오른쪽
	Find_ChildUI(L"SASSkill_XLeft")->SetVisible(m_bChangeX);
	Find_ChildUI(L"SASSkill_XRight")->SetVisible(!m_bChangeX);

	dynamic_cast<CCanvas_SASSkillMove*>(CUI_Manager::GetInstance()->Find_MoveCanvas(L"Canvas_SASSkillMove"))->Set_SkillAll(m_bChangeX);
	FullCircle();
}

void CCanvas_SASSkill::FullCircle()
{
	// 원 들이게 X키의 정보를 넘겨준다.
	_tchar szText[MAX_PATH] = TEXT("");

	for (_uint i = 1; i <= 7; i++)
	{
		wsprintf(szText, TEXT("FullCircle%u"), i);
		dynamic_cast<CSASSkillFullCircleUI*>(Find_ChildUI(szText))->Set_ChangeX(m_bChangeX);

		if (false == m_bChangeX)
		{
			if (1 == i || 2 == i || 3 == i || 4 == i)
				Find_ChildUI(szText)->SetVisible(true);
			else
				Find_ChildUI(szText)->SetVisible(false);
		}
		else
		{
			if (1 == i || 2 == i || 3 == i || 4 == i)
				Find_ChildUI(szText)->SetVisible(false);
			else
				Find_ChildUI(szText)->SetVisible(true);
		}
	}
}

void CCanvas_SASSkill::InputAlt()
{
	if (CGameInstance::GetInstance()->KeyDown(DIK_LALT))
		CUI_Manager::GetInstance()->PlaySound("Alt");

	if (CGameInstance::GetInstance()->KeyPressing(DIK_LALT))
	{
		Find_ChildUI(L"SASSkill_AltText0")->SetVisible(false);
		Find_ChildUI(L"SASSkill_AltTexture0")->SetVisible(false);

		Find_ChildUI(L"SASSkill_AltText1")->SetVisible(true);
		Find_ChildUI(L"SASSkill_AltTexture1")->SetVisible(true);

		// Ctrl
		Find_ChildUI(L"SASSkill_CtrlText0")->SetVisible(true);
		Find_ChildUI(L"SASSkill_CtrlTexture0")->SetVisible(true);

		Find_ChildUI(L"SASSkill_CtrlText1")->SetVisible(false);
		Find_ChildUI(L"SASSkill_CtrlTexture1")->SetVisible(false);
	}
	if (CGameInstance::GetInstance()->KeyUp(DIK_LALT))
	{
		Find_ChildUI(L"SASSkill_AltText0")->SetVisible(true);
		Find_ChildUI(L"SASSkill_AltTexture0")->SetVisible(true);

		Find_ChildUI(L"SASSkill_AltText1")->SetVisible(false);
		Find_ChildUI(L"SASSkill_AltTexture1")->SetVisible(false);
	}
}

void CCanvas_SASSkill::InputCtrl()
{
	if (CGameInstance::GetInstance()->KeyDown(DIK_LCONTROL))
		CUI_Manager::GetInstance()->PlaySound("Ctrl");

	if (CGameInstance::GetInstance()->KeyPressing(DIK_LCONTROL))
	{
		Find_ChildUI(L"SASSkill_CtrlText0")->SetVisible(false);
		Find_ChildUI(L"SASSkill_CtrlTexture0")->SetVisible(false);

		Find_ChildUI(L"SASSkill_CtrlText1")->SetVisible(true);
		Find_ChildUI(L"SASSkill_CtrlTexture1")->SetVisible(true);

		// Alt
		Find_ChildUI(L"SASSkill_AltText0")->SetVisible(true);
		Find_ChildUI(L"SASSkill_AltTexture0")->SetVisible(true);

		Find_ChildUI(L"SASSkill_AltText1")->SetVisible(false);
		Find_ChildUI(L"SASSkill_AltTexture1")->SetVisible(false);
	}
	if (CGameInstance::GetInstance()->KeyUp(DIK_LCONTROL))
	{
		Find_ChildUI(L"SASSkill_CtrlText0")->SetVisible(true);
		Find_ChildUI(L"SASSkill_CtrlTexture0")->SetVisible(true);

		Find_ChildUI(L"SASSkill_CtrlText1")->SetVisible(false);
		Find_ChildUI(L"SASSkill_CtrlTexture1")->SetVisible(false);
	}
}

CCanvas_SASSkill * CCanvas_SASSkill::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_SASSkill*		pInstance = new CCanvas_SASSkill(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SASSkill");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_SASSkill::Clone(void * pArg)
{
	CCanvas_SASSkill*		pInstance = new CCanvas_SASSkill(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SASSkill");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SASSkill::Free()
{
	__super::Free();

}
