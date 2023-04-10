#include "stdafx.h"
#include "..\public\Canvas_SASSkillMove.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "UI_Manager.h"
#include "PlayerInfoManager.h"

#include "Canvas_SASSkill.h"
#include "Player.h"
#include "SASSkillIconUI.h"
#include "SASSkillLightUI.h"
#include "SASSkillGaugeUI.h"
#include "SASSkillNameUI.h"

CCanvas_SASSkillMove::CCanvas_SASSkillMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SASSkillMove::CCanvas_SASSkillMove(const CCanvas_SASSkillMove& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SASSkillMove::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_SASSkillMove::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_MoveCanvas(L"Canvas_SASSkillMove", this);
	m_vMaxDestination = { 7.0f, -7.0f };
	CCanvas::UIMove_FSM();
	
	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	m_eSASType[ONE0] = ESASType::SAS_NOT;
	m_eSASType[TWO0] = ESASType::SAS_PENETRATE;
	m_eSASType[THREE0] = ESASType::SAS_HARDBODY;
	m_eSASType[FOUR0] = ESASType::SAS_FIRE;
	m_eSASType[ONE1] = ESASType::SAS_NOT;
	m_eSASType[TWO1] = ESASType::SAS_NOT;
	m_eSASType[THREE1] = ESASType::SAS_NOT;
	m_eSASType[FOUR1] = ESASType::SAS_NOT;
	
	return S_OK;
}

void CCanvas_SASSkillMove::BeginTick()
{
	CCanvas::BeginTick();

}

void CCanvas_SASSkillMove::Tick(_double TimeDelta)
{
#pragma region 동료를 만나고 나서부터 Tick 을 돈다. (첫번 째 만남)
	// 처음에 하나비를 만난 기준으로 부터 SAS SKilll 창이 보인다.
	if (false == m_bMember0)
	{
		if (false == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::HANABI) || 
			false == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::TSUGUMI) ||
			false == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::KYOTO))
			return;

		m_bMember0 = true;

		Set_IconTypeNotUsed(2, m_eSASType[TWO0]);
		Set_IconTypeNotUsed(3, m_eSASType[THREE0]);
		Set_IconTypeNotUsed(4, m_eSASType[FOUR0]);

		for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		{
			if ((*iter).first == L"SASSkill_IconNumber1" ||
				(*iter).first == L"SASSkill_IconNumber2" ||
				(*iter).first == L"SASSkill_IconNumber3" ||
				(*iter).first == L"SASSkill_IconNumber4" ||
				(*iter).first == L"SASSKill_ColleagueName1" ||
				(*iter).first == L"SASSKill_ColleagueName2" ||
				(*iter).first == L"SASSKill_ColleagueName3" ||
				(*iter).first == L"SASSKill_ColleagueName4" ||
				(*iter).first == L"SASSKill_SuperPower1" ||
				(*iter).first == L"SASSKill_SuperPower2" ||
				(*iter).first == L"SASSKill_SuperPower3" ||
				(*iter).first == L"SASSKill_SuperPower4")
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
			m_eSASType[ONE0] = ESASType::SAS_TELEPORT;
			Set_IconTypeNotUsed(1, m_eSASType[FOUR0]);
		}

		if (true == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::LUCA) &&
			true == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::SEEDEN) &&
			true == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::ARASHI))
		{
			m_bMember1 = true;

			m_eSASType[ONE1] = ESASType::SAS_SUPERSPEED;
			m_eSASType[TWO1] = ESASType::SAS_ELETRIC;

		}
	}
#pragma endregion

	if (true == Find_ChildUI(L"SASSkill_Icon1")->Get_TempOff())	return;

	CCanvas::Tick(TimeDelta);
	m_pUIMoveFSM->Tick(TimeDelta);
	CCanvas::UIHit(TimeDelta);
	
	InputIcon_Tick();
	InputCtrl_Tick();
	InputAlt_Tick();
	UseSkill_Tick();
}

void CCanvas_SASSkillMove::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	if (ImGui::Button("Hanabi"))
	{
		CPlayerInfoManager::GetInstance()->Set_SASMember(SASMEET::HANABI);
	}
	ImGui::SameLine();
	if (ImGui::Button("Tsugumi"))
	{
		CPlayerInfoManager::GetInstance()->Set_SASMember(SASMEET::TSUGUMI);
	}
	ImGui::SameLine();
	if (ImGui::Button("GEMMA"))
	{
		CPlayerInfoManager::GetInstance()->Set_SASMember(SASMEET::KYOTO);
	}
	ImGui::SameLine();
	if (ImGui::Button("LUCA"))
	{
		CPlayerInfoManager::GetInstance()->Set_SASMember(SASMEET::LUCA);
	}
	ImGui::SameLine();
	if (ImGui::Button("SEEDEN"))
	{
		CPlayerInfoManager::GetInstance()->Set_SASMember(SASMEET::SEEDEN);
	}
	ImGui::SameLine();
	if (ImGui::Button("ARASHI"))
	{
		CPlayerInfoManager::GetInstance()->Set_SASMember(SASMEET::ARASHI);
	}
}

void CCanvas_SASSkillMove::Set_SkillAll(const _bool bXKey)
{
	_tchar szText[MAX_PATH] = TEXT("");

	m_bXKey = bXKey; // false 이면 ONE0~ / true이면 ONE1~
	if (false == m_bXKey)
	{
		m_iSASType = 0;
	}
	else
	{
		m_iSASType = 4;
	}

	for (_uint i = 1; i < 5; i++)
	{
		ESASType eSasType = m_eSASType[static_cast<SKILLINDEX>(m_iSASType)];

		wsprintf(szText, TEXT("SASSkill_Icon%u"), i);
		dynamic_cast<CSASSkillIconUI*>(Find_ChildUI(szText))->Set_IconType(eSasType, true);
		wsprintf(szText, TEXT("SASSKill_SuperPower%u"), i);
		dynamic_cast<CSASSkillNameUI*>(Find_ChildUI(szText))->Set_SuperPower(eSasType, true);
		wsprintf(szText, TEXT("SASSKill_ColleagueName%u"), i);
		dynamic_cast<CSASSkillNameUI*>(Find_ChildUI(szText))->Set_ColleagueName(eSasType, true);
		wsprintf(szText, TEXT("SASSkill_Light%u"), i);
		dynamic_cast<CSASSkillLightUI*>(Find_ChildUI(szText))->Set_LightType(eSasType, false);

		_float fCurrentEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eSasType)].Energy;
		_float fMaxEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eSasType)].MaxEnergy;
		_float fRatio = fCurrentEnergy / fMaxEnergy;
		if (ESASType::SAS_NOT == eSasType) fRatio = 0.0f;
		wsprintf(szText, TEXT("SASSkill_Gauge%u"), i);
		dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(szText))->Set_GaugeType(eSasType, fRatio);
		wsprintf(szText, TEXT("SASSkill_Gauge%uBack"), i);
		dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(szText))->Set_GaugeBackType(eSasType, fRatio);

		++m_iSASType;
	}
}

//void CCanvas_SASSkillMove::Set_SkillInfo(const SKILLINDEX eSKILLINDEX, const ESASType eESASType)
//{
//	if (ESASType::SAS_END == m_eSASType[eSKILLINDEX])
//	{
//		m_eSASType[eSKILLINDEX] = eESASType;
//		return;
//	}
//
//	for (_uint i = 0; i < SKILLINDEX_END; ++i)
//	{
//		if (m_eSASType[i] == eESASType)
//		{
//			m_eSASType[i] = m_eSASType[eSKILLINDEX];
//			m_eSASType[eSKILLINDEX] = eESASType;
//			return;
//		}
//	}
//
//	m_eSASType[eSKILLINDEX] = eESASType;
//}

void CCanvas_SASSkillMove::Set_IconTypeUse(const _uint iIndex, const ESASType& eESASType)
{
	_float fCurrentEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eESASType)].Energy;
	_float fMaxEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eESASType)].MaxEnergy;
	_float fRatio = fCurrentEnergy / fMaxEnergy;
	if (ESASType::SAS_NOT == eESASType) fRatio = 0.0f;

	_tchar szText[MAX_PATH] = TEXT("");
	wsprintf(szText, TEXT("SASSkill_Icon%u"), iIndex);
	dynamic_cast<CSASSkillIconUI*>(Find_ChildUI(szText))->Set_IconType(eESASType, true);
	wsprintf(szText, TEXT("SASSKill_SuperPower%u"), iIndex);
	dynamic_cast<CSASSkillNameUI*>(Find_ChildUI(szText))->Set_SuperPower(eESASType, true);
	wsprintf(szText, TEXT("SASSKill_ColleagueName%u"), iIndex);
	dynamic_cast<CSASSkillNameUI*>(Find_ChildUI(szText))->Set_ColleagueName(eESASType, true);
	wsprintf(szText, TEXT("SASSkill_Light%u"), iIndex);
	dynamic_cast<CSASSkillLightUI*>(Find_ChildUI(szText))->Set_LightType(eESASType, true);
	wsprintf(szText, TEXT("SASSkill_Gauge%u"), iIndex);
	dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(szText))->Set_GaugeType(eESASType, fRatio);
	wsprintf(szText, TEXT("SASSkill_Gauge%uBack"), iIndex);
	dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(szText))->Set_GaugeBackType(eESASType, fRatio);
}

void CCanvas_SASSkillMove::Set_IconTypeNotUsed(const _uint iIndex, const ESASType& eESASType)
{
	_float fCurrentEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eESASType)].Energy;
	_float fMinEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eESASType)].MinEnergy;
	_float fMaxEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eESASType)].MaxEnergy;
	_float fRatio = fCurrentEnergy / fMaxEnergy;
	if (ESASType::SAS_NOT == eESASType) fRatio = 0.0f;

	_bool bUsable = false;
	if (fCurrentEnergy > fMinEnergy && ESASType::SAS_NOT != eESASType)
	{
		bUsable = true;
	}
	else
	{
		bUsable = false;
	}

	_tchar szText[MAX_PATH] = TEXT("");
	wsprintf(szText, TEXT("SASSkill_Icon%u"), iIndex);
	dynamic_cast<CSASSkillIconUI*>(Find_ChildUI(szText))->Set_IconType(eESASType, bUsable);
	wsprintf(szText, TEXT("SASSKill_SuperPower%u"), iIndex);
	dynamic_cast<CSASSkillNameUI*>(Find_ChildUI(szText))->Set_SuperPower(eESASType, bUsable);
	wsprintf(szText, TEXT("SASSKill_ColleagueName%u"), iIndex);
	dynamic_cast<CSASSkillNameUI*>(Find_ChildUI(szText))->Set_ColleagueName(eESASType, bUsable);
	wsprintf(szText, TEXT("SASSkill_Light%u"), iIndex);
	dynamic_cast<CSASSkillLightUI*>(Find_ChildUI(szText))->Set_LightType(eESASType, false);
	wsprintf(szText, TEXT("SASSkill_Gauge%u"), iIndex);
	dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(szText))->Set_GaugeType(eESASType, fRatio);
	wsprintf(szText, TEXT("SASSkill_Gauge%uBack"), iIndex);
	dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(szText))->Set_GaugeBackType(eESASType, fRatio);
}

void CCanvas_SASSkillMove::UseSkill_Tick()
{
	if (nullptr == m_pPlayer) return;

	if (false == m_bXKey)
	{
		// 1
		if (true == m_pPlayer->Get_SASSkillInput(0))
		{
			ESASType eSASType = m_eSASType[ONE0];
			Set_IconTypeUse(1, eSASType);
		}
		else
		{
			ESASType eSASType = m_eSASType[ONE0];
			Set_IconTypeNotUsed(1, eSASType);
		}

		// 2
		if (true == m_pPlayer->Get_SASSkillInput(1))
		{
			ESASType eSASType = m_eSASType[TWO0];
			Set_IconTypeUse(2, eSASType);
		}
		else
		{
			ESASType eSASType = m_eSASType[TWO0];
			Set_IconTypeNotUsed(2, eSASType);
		}

		// 3
		if (true == m_pPlayer->Get_SASSkillInput(2))
		{
			ESASType eSASType = m_eSASType[THREE0];
			Set_IconTypeUse(3, eSASType);
		}
		else
		{
			ESASType eSASType = m_eSASType[THREE0];
			Set_IconTypeNotUsed(3, eSASType);
		}

		// 4
		if (true == m_pPlayer->Get_SASSkillInput(3))
		{
			ESASType eSASType = m_eSASType[FOUR0];
			Set_IconTypeUse(4, eSASType);
		}
		else
		{
			ESASType eSASType = m_eSASType[FOUR0];
			Set_IconTypeNotUsed(4, eSASType);
		}
	}
	else
	{
		// 1
		if (true == m_pPlayer->Get_SASSkillInput(4))
		{
			ESASType eSASType = m_eSASType[ONE1];
			Set_IconTypeUse(1, eSASType);
		}
		else
		{
			ESASType eSASType = m_eSASType[ONE1];
			Set_IconTypeNotUsed(1, eSASType);
		}

		// 2
		if (true == m_pPlayer->Get_SASSkillInput(5))
		{
			ESASType eSASType = m_eSASType[TWO1];
			Set_IconTypeUse(2, eSASType);
		}
		else
		{
			ESASType eSASType = m_eSASType[TWO1];
			Set_IconTypeNotUsed(2, eSASType);
		}

		// 3
		ESASType eSASType = m_eSASType[THREE1];
		Set_IconTypeUse(3, eSASType);

		// 4
		eSASType = m_eSASType[FOUR1];
		Set_IconTypeUse(4, eSASType);
	}
}

void CCanvas_SASSkillMove::InputIcon_Tick()
{
	if (true == m_bCtrl || true == m_bAlt)
	{
		Find_ChildUI(L"SASSkill_Icon1")->SetVisible(false);
		Find_ChildUI(L"SASSkill_Icon2")->SetVisible(false);
		Find_ChildUI(L"SASSkill_Icon3")->SetVisible(false);
		Find_ChildUI(L"SASSkill_Icon4")->SetVisible(false);

		Find_ChildUI(L"SASSkill_IconNumber1")->SetVisible(true);
		Find_ChildUI(L"SASSkill_IconNumber2")->SetVisible(true);
		Find_ChildUI(L"SASSkill_IconNumber3")->SetVisible(true);
		Find_ChildUI(L"SASSkill_IconNumber4")->SetVisible(true);

	}
	else
	{
		Find_ChildUI(L"SASSkill_Icon1")->SetVisible(true);
		Find_ChildUI(L"SASSkill_Icon2")->SetVisible(true);
		Find_ChildUI(L"SASSkill_Icon3")->SetVisible(true);
		Find_ChildUI(L"SASSkill_Icon4")->SetVisible(true);

		Find_ChildUI(L"SASSkill_IconNumber1")->SetVisible(false);
		Find_ChildUI(L"SASSkill_IconNumber2")->SetVisible(false);
		Find_ChildUI(L"SASSkill_IconNumber3")->SetVisible(false);
		Find_ChildUI(L"SASSkill_IconNumber4")->SetVisible(false);

	}
}

void CCanvas_SASSkillMove::InputCtrl_Tick()
{
	// Ctrel 입력
	if (CGameInstance::GetInstance()->KeyDown(DIK_LCONTROL))
	{
		m_bCtrl = true;
		m_bAlt = false;
	}

	if (CGameInstance::GetInstance()->KeyUp(DIK_LCONTROL))
	{
		m_bCtrl = false;
	}

	Find_ChildUI(L"SASSKill_ColleagueName1")->SetVisible(m_bCtrl);
	Find_ChildUI(L"SASSKill_ColleagueName2")->SetVisible(m_bCtrl);
	Find_ChildUI(L"SASSKill_ColleagueName3")->SetVisible(m_bCtrl);
	Find_ChildUI(L"SASSKill_ColleagueName4")->SetVisible(m_bCtrl);
}

void CCanvas_SASSkillMove::InputAlt_Tick()
{
	// Alt 눌렀을 때 보여야 하는 UI
	if (CGameInstance::GetInstance()->KeyDown(DIK_LALT))
	{
		m_bCtrl = false;
		m_bAlt = true;
	}
	// Alt 누르지 않을 때 보여야 하는 UI
	if (CGameInstance::GetInstance()->KeyUp(DIK_LALT))
	{
		m_bAlt = false;
	}

	Find_ChildUI(L"SASSKill_SuperPower1")->SetVisible(m_bAlt);
	Find_ChildUI(L"SASSKill_SuperPower2")->SetVisible(m_bAlt);
	Find_ChildUI(L"SASSKill_SuperPower3")->SetVisible(m_bAlt);
	Find_ChildUI(L"SASSKill_SuperPower4")->SetVisible(m_bAlt);
}

CCanvas_SASSkillMove * CCanvas_SASSkillMove::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_SASSkillMove*		pInstance = new CCanvas_SASSkillMove(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SASSkillMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_SASSkillMove::Clone(void * pArg)
{
	CCanvas_SASSkillMove*		pInstance = new CCanvas_SASSkillMove(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SASSkillMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SASSkillMove::Free()
{
	CCanvas::Free();
}
