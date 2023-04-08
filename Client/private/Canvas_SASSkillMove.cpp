#include "stdafx.h"
#include "..\public\Canvas_SASSkillMove.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "UI_Manager.h"
#include "PlayerInfoManager.h"

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

	//// 처음에 보이지 않을 UI 들
	//Find_ChildUI(L"SASSkill_IconNumber1")->SetVisible(false);
	//Find_ChildUI(L"SASSkill_IconNumber2")->SetVisible(false);
	//Find_ChildUI(L"SASSkill_IconNumber3")->SetVisible(false);
	//Find_ChildUI(L"SASSkill_IconNumber4")->SetVisible(false);

	//Find_ChildUI(L"SASSKill_ColleagueName1")->SetVisible(false);
	//Find_ChildUI(L"SASSKill_ColleagueName2")->SetVisible(false);
	//Find_ChildUI(L"SASSKill_ColleagueName3")->SetVisible(false);
	//Find_ChildUI(L"SASSKill_ColleagueName4")->SetVisible(false);

	//Find_ChildUI(L"SASSKill_SuperPower1")->SetVisible(false);
	//Find_ChildUI(L"SASSKill_SuperPower2")->SetVisible(false);
	//Find_ChildUI(L"SASSKill_SuperPower3")->SetVisible(false);
	//Find_ChildUI(L"SASSKill_SuperPower4")->SetVisible(false);

	m_eSASType[ONE0] = ESASType::SAS_FIRE;
	m_eSASType[TWO0] = ESASType::SAS_PENETRATE;
	m_eSASType[THREE0] = ESASType::SAS_HARDBODY;
	m_eSASType[FOUR0] = ESASType::SAS_NOT;
	m_eSASType[ONE1] = ESASType::SAS_NOT;
	m_eSASType[TWO1] = ESASType::SAS_NOT;
	m_eSASType[THREE1] = ESASType::SAS_NOT;
	m_eSASType[FOUR1] = ESASType::SAS_NOT;

	Set_IconType(1, m_eSASType[ONE0]);
	Set_IconType(2, m_eSASType[TWO0]);
	Set_IconType(3, m_eSASType[THREE0]);
	Set_IconType(4, m_eSASType[FOUR0]);

	return S_OK;
}

void CCanvas_SASSkillMove::BeginTick()
{
	CCanvas::BeginTick();

}

void CCanvas_SASSkillMove::Tick(_double TimeDelta)
{
#pragma region 동료를 만나고 나서부터 Tick 을 돈다. (첫번 쨰 만남)
	// 처음에 하나비를 만난 기준으로 부터 SAS SKilll 창이 보인다.
	if (false == m_bMember0)
	{
		if (false == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::HANABI) && 
			false == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::TSUGUMI)&&
			false == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::KYOTO))
			return;

		m_bMember0 = true;

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
	if (true == m_bMember0 && m_bMember1 == false)
	{
		if (false == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::LUCA) &&
			false == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::SEEDEN) &&
			false == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::ARASHI))
			return;

		m_bMember1 = true;

		m_eSASType[FOUR0] = ESASType::SAS_TELEPORT;
		m_eSASType[ONE1] = ESASType::SAS_ELETRIC;
		m_eSASType[TWO1] = ESASType::SAS_SUPERSPEED;
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

void CCanvas_SASSkillMove::Set_SkillInfo(const SKILLINDEX eSKILLINDEX, const ESASType eESASType)
{
	if (ESASType::SAS_END == m_eSASType[eSKILLINDEX])
	{
		m_eSASType[eSKILLINDEX] = eESASType;
		return;
	}

	for (_uint i = 0; i < SKILLINDEX_END; ++i)
	{
		if (m_eSASType[i] == eESASType)
		{
			m_eSASType[i] = m_eSASType[eSKILLINDEX];
			m_eSASType[eSKILLINDEX] = eESASType;
			return;
		}
	}

	m_eSASType[eSKILLINDEX] = eESASType;
}

void CCanvas_SASSkillMove::Set_IconType(const _uint iIndex, const ESASType & eESASType)
{
	_float fCurrentEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eESASType)].Energy;
	_float fMinEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eESASType)].MinEnergy;

	_bool bUsable = false;
	if (fCurrentEnergy > fMinEnergy)
		bUsable = true;
	else
		bUsable = false;

	//_tchar szText[MAX_PATH] = TEXT("");
	//
	//for (_uint i = 1; i < 5; i++)
	//{
	//	wsprintf(szText, TEXT("SASSkill_Icon%u"), i);
	//	dynamic_cast<CSASSkillIconUI*>(Find_ChildUI(szText))->Set_IconType(eESASType, bUsable);
	//	wsprintf(szText, TEXT("SASSKill_SuperPower%u"), i);
	//	dynamic_cast<CSASSkillNameUI*>(Find_ChildUI(szText))->Set_SuperPower(eESASType, bUsable);
	//	wsprintf(szText, TEXT("SASSKill_ColleagueName%u"), i);
	//	dynamic_cast<CSASSkillNameUI*>(Find_ChildUI(szText))->Set_ColleagueName(eESASType, bUsable);
	//}

	if (1 == iIndex)
	{
		dynamic_cast<CSASSkillIconUI*>(Find_ChildUI(L"SASSkill_Icon1"))->Set_IconType(eESASType, bUsable);
		dynamic_cast<CSASSkillNameUI*>(Find_ChildUI(L"SASSKill_SuperPower1"))->Set_SuperPower(eESASType, bUsable);
		dynamic_cast<CSASSkillNameUI*>(Find_ChildUI(L"SASSKill_ColleagueName1"))->Set_ColleagueName(eESASType, bUsable);
	}
	else if (2 == iIndex)
	{
		dynamic_cast<CSASSkillIconUI*>(Find_ChildUI(L"SASSkill_Icon2"))->Set_IconType(eESASType, bUsable);
		dynamic_cast<CSASSkillNameUI*>(Find_ChildUI(L"SASSKill_SuperPower2"))->Set_SuperPower(eESASType, bUsable);
		dynamic_cast<CSASSkillNameUI*>(Find_ChildUI(L"SASSKill_ColleagueName2"))->Set_ColleagueName(eESASType, bUsable);
	}
	else if (3 == iIndex)
	{
		dynamic_cast<CSASSkillIconUI*>(Find_ChildUI(L"SASSkill_Icon3"))->Set_IconType(eESASType, bUsable);
		dynamic_cast<CSASSkillNameUI*>(Find_ChildUI(L"SASSKill_SuperPower3"))->Set_SuperPower(eESASType, bUsable);
		dynamic_cast<CSASSkillNameUI*>(Find_ChildUI(L"SASSKill_ColleagueName3"))->Set_ColleagueName(eESASType, bUsable);
	}
	else if (4 == iIndex)
	{
		dynamic_cast<CSASSkillIconUI*>(Find_ChildUI(L"SASSkill_Icon4"))->Set_IconType(eESASType, bUsable);
		dynamic_cast<CSASSkillNameUI*>(Find_ChildUI(L"SASSKill_SuperPower4"))->Set_SuperPower(eESASType, bUsable);
		dynamic_cast<CSASSkillNameUI*>(Find_ChildUI(L"SASSKill_ColleagueName4"))->Set_ColleagueName(eESASType, bUsable);
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

void CCanvas_SASSkillMove::UseSkill_Tick()
{
	// 1
	if (true == m_pPlayer->Get_SASSkillInput(0))
	{
		ESASType eSASType = m_eSASType[ONE0];

		_float fCurrentEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eSASType)].Energy;
		_float fMaxEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eSASType)].MaxEnergy;
		_float fRatio = fCurrentEnergy / fMaxEnergy;

		if (ESASType::SAS_NOT != eSASType)
		{
			dynamic_cast<CSASSkillLightUI*>(Find_ChildUI(L"SASSkill_Light1"))->Set_LightType(eSASType, true);
			dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(L"SASSkill_Gauge1"))->Set_GaugeType(eSASType, fRatio);
			dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(L"SASSkill_Gauge1Back"))->Set_GaugeBackType(eSASType, fRatio);
		}
	}
	else
	{
		ESASType eSASType = m_eSASType[ONE0];

		if (ESASType::SAS_NOT != eSASType)
		{
			_float fCurrentEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eSASType)].Energy;
			_float fMaxEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eSASType)].MaxEnergy;
			_float fRatio = fCurrentEnergy / fMaxEnergy;

			dynamic_cast<CSASSkillLightUI*>(Find_ChildUI(L"SASSkill_Light1"))->Set_LightType(eSASType, false);
			dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(L"SASSkill_Gauge1"))->Set_GaugeType(eSASType, fRatio);
			dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(L"SASSkill_Gauge1Back"))->Set_GaugeBackType(eSASType, fRatio);
		}
	}

	// 2
	if (true == m_pPlayer->Get_SASSkillInput(1))
	{
		ESASType eSASType = m_eSASType[TWO0];

		_float fCurrentEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eSASType)].Energy;
		_float fMaxEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eSASType)].MaxEnergy;
		_float fRatio = fCurrentEnergy / fMaxEnergy;

		if (ESASType::SAS_NOT != eSASType)
		{
			dynamic_cast<CSASSkillLightUI*>(Find_ChildUI(L"SASSkill_Light2"))->Set_LightType(eSASType, true);
			dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(L"SASSkill_Gauge2"))->Set_GaugeType(eSASType, fRatio);
			dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(L"SASSkill_Gauge2Back"))->Set_GaugeBackType(eSASType, fRatio);
		}
	}
	else
	{
		ESASType eSASType = m_eSASType[TWO0];

		if (ESASType::SAS_NOT != eSASType)
		{
			_float fCurrentEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eSASType)].Energy;
			_float fMaxEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eSASType)].MaxEnergy;
			_float fRatio = fCurrentEnergy / fMaxEnergy;

			dynamic_cast<CSASSkillLightUI*>(Find_ChildUI(L"SASSkill_Light2"))->Set_LightType(eSASType, false);
			dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(L"SASSkill_Gauge2"))->Set_GaugeType(eSASType, fRatio);
			dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(L"SASSkill_Gauge2Back"))->Set_GaugeBackType(eSASType, fRatio);
		}
	}

	// 3
	if (true == m_pPlayer->Get_SASSkillInput(2))
	{
		ESASType eSASType = m_eSASType[THREE0];

		_float fCurrentEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eSASType)].Energy;
		_float fMaxEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eSASType)].MaxEnergy;
		_float fRatio = fCurrentEnergy / fMaxEnergy;

		if (ESASType::SAS_NOT != eSASType)
		{
			dynamic_cast<CSASSkillLightUI*>(Find_ChildUI(L"SASSkill_Light3"))->Set_LightType(eSASType, true);
			dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(L"SASSkill_Gauge3"))->Set_GaugeType(eSASType, fRatio);
			dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(L"SASSkill_Gauge3Back"))->Set_GaugeBackType(eSASType, fRatio);
		}
	}
	else
	{
		ESASType eSASType = m_eSASType[THREE0];

		if (ESASType::SAS_NOT != eSASType)
		{
			_float fCurrentEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eSASType)].Energy;
			_float fMaxEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eSASType)].MaxEnergy;
			_float fRatio = fCurrentEnergy / fMaxEnergy;

			dynamic_cast<CSASSkillLightUI*>(Find_ChildUI(L"SASSkill_Light3"))->Set_LightType(eSASType, false);
			dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(L"SASSkill_Gauge3"))->Set_GaugeType(eSASType, fRatio);
			dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(L"SASSkill_Gauge3Back"))->Set_GaugeBackType(eSASType, fRatio);
		}
	}

	// 4
	if (true == m_pPlayer->Get_SASSkillInput(3))
	{
		ESASType eSASType = m_eSASType[FOUR0];

		_float fCurrentEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eSASType)].Energy;
		_float fMaxEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eSASType)].MaxEnergy;
		_float fRatio = fCurrentEnergy / fMaxEnergy;

		if (ESASType::SAS_NOT != eSASType)
		{
			dynamic_cast<CSASSkillLightUI*>(Find_ChildUI(L"SASSkill_Light4"))->Set_LightType(eSASType, true);
			dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(L"SASSkill_Gauge4"))->Set_GaugeType(eSASType, fRatio);
			dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(L"SASSkill_Gauge4Back"))->Set_GaugeBackType(eSASType, fRatio);
		}
	}
	else
	{
		ESASType eSASType = m_eSASType[FOUR0];

		if (ESASType::SAS_NOT != eSASType)
		{
			_float fCurrentEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eSASType)].Energy;
			_float fMaxEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(eSASType)].MaxEnergy;
			_float fRatio = fCurrentEnergy / fMaxEnergy;

			dynamic_cast<CSASSkillLightUI*>(Find_ChildUI(L"SASSkill_Light4"))->Set_LightType(eSASType, false);
			dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(L"SASSkill_Gauge4"))->Set_GaugeType(eSASType, fRatio);
			dynamic_cast<CSASSkillGaugeUI*>(Find_ChildUI(L"SASSkill_Gauge4Back"))->Set_GaugeBackType(eSASType, fRatio);
		}
	}
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
