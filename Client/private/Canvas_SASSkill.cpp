#include "stdafx.h"
#include "..\public\Canvas_SASSkill.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "PlayerInfoManager.h"

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
			false == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::KYOTO))
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
				(*iter).first == L"SASSkill_XInput")
				continue;

			iter->second->SetVisible(true);
		}
	}
#pragma endregion

#pragma region (2번째 만남)
	if (true == m_bMember0 && false == m_bMember1)
	{
		if (true == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::LUCA) &&
			true == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::SEEDEN) &&
			true == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::ARASHI))
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

void CCanvas_SASSkill::Set_Grow(const _uint iNumber, const _bool bVisible)
{
	_tchar szText[MAX_PATH] = TEXT("");
	wsprintf(szText, TEXT("FullCircle%i"), iNumber);
	dynamic_cast<CSASSkillFullCircleUI*>(Find_ChildUI(szText))->Set_Grow();
	Find_ChildUI(szText)->SetVisible(bVisible);
}

void CCanvas_SASSkill::InputX_Tick(const _double & dTimeDelta)
{
	if (false == m_bMember1) return;

	if (CGameInstance::GetInstance()->KeyDown(DIK_X))
	{
		m_bChangeX = !m_bChangeX;
		m_bChangeXButton = true;

		// [m_bChangeX] false -> 왼쪽 true -> 오른쪽
		Find_ChildUI(L"SASSkill_XLeft")->SetVisible(m_bChangeX);
		Find_ChildUI(L"SASSkill_XRight")->SetVisible(!m_bChangeX);

		dynamic_cast<CCanvas_SASSkillMove*>(CUI_Manager::GetInstance()->Find_MoveCanvas(L"Canvas_SASSkillMove"))->Set_SkillAll(m_bChangeX);
	}

	if (true == m_bChangeXButton)
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
		}
	}
}

void CCanvas_SASSkill::InputAlt()
{
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
