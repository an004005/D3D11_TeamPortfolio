#include "stdafx.h"
#include "..\public\Canvas_Party.h"
#include "GameInstance.h"
#include "PlayerInfoManager.h"

#include "DefaultUI.h"
#include "Main_PickUI.h"
#include "Main_FaceUI.h"
#include "Main_BarUI.h"
#include "Main_SkillIconUI.h"
#include "Main_SkillNameUI.h"

CCanvas_Party::CCanvas_Party(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_Party::CCanvas_Party(const CCanvas_Party& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_Party::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_Party::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	m_bVisible = false;
	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_01_Leader_B"))->Set_InitializeAlpha();
	PickInfo();

	return S_OK;
}

void CCanvas_Party::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	ChildRender_Tick();

	if (false == m_bVisible) return;

	CurrentPick_Tick();
}

void CCanvas_Party::Late_Tick(_double TimeDelta)
{
	if (false == m_bVisible) return;
	
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_Party::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_float4 vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// 오른쪽 상단 배틀 멤버
	_float2 vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Party_BattleMembers_B"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"배틀 멤버의 SAS", vPosition + _float2(70.0f, 68.0f), 0.f, { 0.45f, 0.45f }, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"배틀 멤버의 SAS는 효과 시간이 길어지며",	vPosition + _float2(70.0f, 125.0f), 0.f, { 0.4f, 0.4f }, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"SAS 게이지 회복 속도도 빨라진다.", vPosition + _float2(70.0f, 155.0f), 0.f, { 0.4f, 0.4f }, vColor);

	_tchar szText[MAX_PATH] = TEXT("");
	_float2 vFontBigSize = { 0.5f, 0.5f };
	_float2 vFontSmaillSize = { 0.3f, 0.3f };

	// 오른쪽 하단 정보
	vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Party_BasicInfo_B"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", m_tPickIngo.szName, vPosition + _float2(95.0f, 52.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", m_tPickIngo.szSASNumber, vPosition + _float2(433.0f, 56.0f), 0.f, vFontBigSize * 0.5f, vColor);

	wsprintf(szText, TEXT("%d"), m_tPickIngo.iLevel);
	pGameInstance->Render_Font(L"Pretendard32", L"레벨", vPosition + _float2(95.0f, 95.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(125.0f, 87.0f), 0.f, vFontBigSize, vColor);
	wsprintf(szText, TEXT("%d"), m_tPickIngo.iRemainingExp);
	pGameInstance->Render_Font(L"Pretendard32", L"앞으로", vPosition + _float2(203.0f, 95.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(280.0f, 95.0f), 0.f, vFontSmaillSize, vColor);
	if (m_eSASMember == YUITO)	wsprintf(szText, TEXT("-")); else	wsprintf(szText, TEXT("%d"), m_tPickIngo.iBondLevel);	// 유대 관계 레벨 (유이토는 -)
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(359.0f, 94.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d / %d"), m_tPickIngo.iHP, m_tPickIngo.iMaxHP);
	pGameInstance->Render_Font(L"Pretendard32", L"체력", vPosition + _float2(203.0f, 125.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(297.0f, 125.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d"), m_tPickIngo.iSprbrPower);
	pGameInstance->Render_Font(L"Pretendard32", L"초뇌능력", vPosition + _float2(203.0f, 161.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(350.0f, 160.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d"), m_tPickIngo.iAttack);
	pGameInstance->Render_Font(L"Pretendard32", L"공격력", vPosition + _float2(203.0f, 183.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(350.0f, 182.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d"), m_tPickIngo.iDefense);
	pGameInstance->Render_Font(L"Pretendard32", L"방어력", vPosition + _float2(203.0f, 207.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(350.0f, 205.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"작전", vPosition + _float2(96.0f, 228.0f), 0.f, vFontSmaillSize, vColor);
	if (m_eSASMember == YUITO)	pGameInstance->Render_Font(L"Pretendard32", L"----", vPosition + _float2(276.0f, 255.0f), 0.f, { 0.4f, 0.4f }, vColor);
	else pGameInstance->Render_Font(L"Pretendard32", L"자유롭게 싸워라", vPosition + _float2(246.0f, 255.0f), 0.f, { 0.4f, 0.4f }, vColor);

	// -------------------------------------------------------------------
	vFontBigSize = { 0.35f, 0.35f };
	vFontSmaillSize = { 0.25f, 0.25f };

	// 유이토
	vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_01_Leader_B"))->GetScreenSpaceLeftTop();
	PLAYER_STAT tPlayerStat = CPlayerInfoManager::GetInstance()->Get_PlayerStat();
	pGameInstance->Render_Font(L"Pretendard32", L"ㅡ", vPosition + _float2(331.0f, 49.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d"), tPlayerStat.iLevel);
	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(207.0f, 46.0f), 0.f, vFontBigSize, vColor);
	wsprintf(szText, TEXT("%d / %d"), tPlayerStat.m_iHP, tPlayerStat.m_iMaxHP);
	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(283.0f, 71.0f), 0.f, vFontSmaillSize, vColor);
	dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_01_Leader_HpBar"))->Set_Ber(_float(tPlayerStat.m_iHP / tPlayerStat.m_iMaxHP));

	pGameInstance->Render_Font(L"Pretendard32", L"유이토 스메라기", vPosition + _float2(178.0f, 21.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"레벨", vPosition + _float2(180.0f, 48.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"체력", vPosition + _float2(180.0f, 72.0f), 0.f, vFontSmaillSize, vColor);

	// 하아비
	HANABI_STAT tHanabiStat = CPlayerInfoManager::GetInstance()->Get_HanabiStat();
	if (true == tHanabiStat.bMember)
	{
		vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_02_Member_B"))->GetScreenSpaceLeftTop();
		wsprintf(szText, TEXT("%d"), tHanabiStat.iBondLevel);
		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(332.0f, 49.0f), 0.f, vFontSmaillSize, vColor);
		wsprintf(szText, TEXT("%d"), tHanabiStat.iLevel);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(207.0f, 46.0f), 0.f, vFontBigSize, vColor);
		wsprintf(szText, TEXT("%d / %d"), tHanabiStat.iHP, tHanabiStat.iMaxHP);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(283.0f, 71.0f), 0.f, vFontSmaillSize, vColor);
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_02_Member_HpBar"))->Set_Ber(_float(tHanabiStat.iHP / tHanabiStat.iMaxHP));

		pGameInstance->Render_Font(L"Pretendard32", L"하나비 이치조", vPosition + _float2(178.0f, 21.0f), 0.f, vFontBigSize, vColor);
		pGameInstance->Render_Font(L"Pretendard32", L"레벨", vPosition + _float2(180.0f, 48.0f), 0.f, vFontSmaillSize, vColor);
		pGameInstance->Render_Font(L"Pretendard32", L"체력", vPosition + _float2(180.0f, 72.0f), 0.f, vFontSmaillSize, vColor);
	}

	// 츠구미
	TSUGUMI_STAT tTsugumiStat = CPlayerInfoManager::GetInstance()->Get_TsugumiStat();
	if (true == tTsugumiStat.bMember)
	{
		vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_03_Member_B"))->GetScreenSpaceLeftTop();
		wsprintf(szText, TEXT("%d"), tHanabiStat.iBondLevel);
		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(333.0f, 50.0f), 0.f, vFontSmaillSize, vColor);
		wsprintf(szText, TEXT("%d"), tTsugumiStat.iLevel);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(207.0f, 46.0f), 0.f, vFontBigSize, vColor);
		wsprintf(szText, TEXT("%d / %d"), tTsugumiStat.iHP, tTsugumiStat.iMaxHP);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(283.0f, 71.0f), 0.f, vFontSmaillSize, vColor);
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_03_Member_HpBar"))->Set_Ber(_float(tTsugumiStat.iHP / tTsugumiStat.iMaxHP));

		pGameInstance->Render_Font(L"Pretendard32", L"츠구미 나자르", vPosition + _float2(178.0f, 21.0f), 0.f, vFontBigSize, vColor);
		pGameInstance->Render_Font(L"Pretendard32", L"레벨", vPosition + _float2(180.0f, 48.0f), 0.f, vFontSmaillSize, vColor);
		pGameInstance->Render_Font(L"Pretendard32", L"체력", vPosition + _float2(180.0f, 72.0f), 0.f, vFontSmaillSize, vColor);
	}

	return S_OK;
}

void CCanvas_Party::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	ImGui::DragFloat("X", &m_vPosition.x);
	ImGui::DragFloat("Y", &m_vPosition.y);

	if (ImGui::Button("Hanabi"))
	{
		CPlayerInfoManager::GetInstance()->Set_HanabiMemvber();
	}
	ImGui::SameLine();
	if (ImGui::Button("Tsugumi"))
	{
		CPlayerInfoManager::GetInstance()->Set_TsugumiMemvber();
	}
}

void CCanvas_Party::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_Party::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

void CCanvas_Party::ChildRender_Tick()
{
	// Canvas 를 만들 때 항상 켜야하는 애들이 많은지, 새로 켜야 하는 애들이 많은지 비교해서 구현하기

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
	{
		if (iter->first == L"Party_02_Member_HpBar" || 
			iter->first == L"Party_02_Member_HpBar_B" || 
			iter->first == L"Party_02_Member_Info")
		{
			if (false == CPlayerInfoManager::GetInstance()->Get_HanabiStat().bMember) continue;
		}

		if (iter->first == L"Party_03_Member_HpBar" ||
			iter->first == L"Party_03_Member_HpBar_B" ||
			iter->first == L"Party_03_Member_Info")
		{
			if (false == CPlayerInfoManager::GetInstance()->Get_TsugumiStat().bMember) continue;
		}

		if (iter->first == L"Party_XX0_Reserve")
			continue;

		iter->second->SetVisible(m_bVisible);
	}
}

void CCanvas_Party::CurrentPick_Tick()
{
	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_01_Leader_B"))->Get_OnButton())
	{
		m_eSASMember = YUITO;

		PickInfo();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_01_Leader_B"))->Set_OnButton();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_02_Member_B"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_03_Member_B"))->Set_OnAlpha();
		Find_ChildUI(L"Party_InfoPick")->Set_Position(Find_ChildUI(L"Party_01_Leader_B")->Get_Position());

	}

	if (true == CPlayerInfoManager::GetInstance()->Get_HanabiStat().bMember)
	{
		if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_02_Member_B"))->Get_OnButton())
		{
			m_eSASMember = HANABI;

			PickInfo();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_02_Member_B"))->Set_OnButton();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_01_Leader_B"))->Set_OnAlpha();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_03_Member_B"))->Set_OnAlpha();
			Find_ChildUI(L"Party_InfoPick")->Set_Position(Find_ChildUI(L"Party_02_Member_B")->Get_Position());

		}
	}
	else
	{
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_02_Member_B"))->Set_OnAlpha();
	}

	if (true == CPlayerInfoManager::GetInstance()->Get_TsugumiStat().bMember)
	{
		if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_03_Member_B"))->Get_OnButton())
		{
			m_eSASMember = TSUGUMI;

			PickInfo();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_03_Member_B"))->Set_OnButton();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_01_Leader_B"))->Set_OnAlpha();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_02_Member_B"))->Set_OnAlpha();
			Find_ChildUI(L"Party_InfoPick")->Set_Position(Find_ChildUI(L"Party_03_Member_B")->Get_Position());

		}

	}
	else
	{
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_03_Member_B"))->Set_OnAlpha();
	}
}

void CCanvas_Party::PickInfo()
{
	switch (m_eSASMember)
	{
	case Client::YUITO:
	{
		PLAYER_STAT tStat = CPlayerInfoManager::GetInstance()->Get_PlayerStat();

		m_tPickIngo.szName = L"유이토 스메라기";
		m_tPickIngo.szSASNumber = L"WSY567369";
		m_tPickIngo.iLevel = tStat.iLevel;
		m_tPickIngo.iRemainingExp = tStat.iMaxExp - tStat.iExp;
		m_tPickIngo.iHP = tStat.m_iHP;
		m_tPickIngo.iMaxHP = tStat.m_iMaxHP;
		m_tPickIngo.iSprbrPower = tStat.iSprbrPower;
		m_tPickIngo.iAttack = tStat.iAttack;
		m_tPickIngo.iDefense = tStat.iDefense;

		dynamic_cast<CMain_FaceUI*>(Find_ChildUI(L"Party_BasicInfo_Face"))->Set_Face(0.0f);
		dynamic_cast<CMain_SkillIconUI*>(Find_ChildUI(L"Party_BasicInfo_Icon"))->Set_Icon(0.0f);
		dynamic_cast<CMain_SkillNameUI*>(Find_ChildUI(L"Party_BasicInfo_SkillName"))->Set_Name(0.0f);
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_BasicInfo_HpBar"))->Set_Ber(_float(tStat.m_iHP / tStat.m_iMaxHP));
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_BasicInfo_LevelBer"))->Set_Ber(_float(tStat.iExp / tStat.iMaxExp));
	}
		break;
	case Client::HANABI:
	{
		HANABI_STAT tStat = CPlayerInfoManager::GetInstance()->Get_HanabiStat();

		m_tPickIngo.szName = L"하나비 이치조";
		m_tPickIngo.szSASNumber = L"RIH567145";
		m_tPickIngo.iLevel = tStat.iLevel;
		m_tPickIngo.iRemainingExp = tStat.iMaxExp - tStat.iExp;
		m_tPickIngo.iBondLevel = tStat.iBondLevel;
		m_tPickIngo.iHP = tStat.iHP;
		m_tPickIngo.iMaxHP = tStat.iMaxHP;
		m_tPickIngo.iSprbrPower = tStat.iSprbrPower;
		m_tPickIngo.iAttack = tStat.iAttack;
		m_tPickIngo.iDefense = tStat.iDefense;

		dynamic_cast<CMain_FaceUI*>(Find_ChildUI(L"Party_BasicInfo_Face"))->Set_Face(2.0f);
		dynamic_cast<CMain_SkillIconUI*>(Find_ChildUI(L"Party_BasicInfo_Icon"))->Set_Icon(2.0f);
		dynamic_cast<CMain_SkillNameUI*>(Find_ChildUI(L"Party_BasicInfo_SkillName"))->Set_Name(2.0f);
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_BasicInfo_HpBar"))->Set_Ber(_float(tStat.iHP / tStat.iMaxHP));
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_BasicInfo_LevelBer"))->Set_Ber(_float(tStat.iExp / tStat.iMaxExp));
	}
		break;
	case Client::TSUGUMI:
	{
		TSUGUMI_STAT tStat = CPlayerInfoManager::GetInstance()->Get_TsugumiStat();

		m_tPickIngo.szName = L"츠구미 나자르";
		m_tPickIngo.szSASNumber = L"SNT555228";
		m_tPickIngo.iLevel = tStat.iLevel;
		m_tPickIngo.iRemainingExp = tStat.iMaxExp - tStat.iExp;
		m_tPickIngo.iBondLevel = tStat.iBondLevel;
		m_tPickIngo.iHP = tStat.iHP;
		m_tPickIngo.iMaxHP = tStat.iMaxHP;
		m_tPickIngo.iSprbrPower = tStat.iSprbrPower;
		m_tPickIngo.iAttack = tStat.iAttack;
		m_tPickIngo.iDefense = tStat.iDefense;

		dynamic_cast<CMain_FaceUI*>(Find_ChildUI(L"Party_BasicInfo_Face"))->Set_Face(4.0f);
		dynamic_cast<CMain_SkillIconUI*>(Find_ChildUI(L"Party_BasicInfo_Icon"))->Set_Icon(4.0f);
		dynamic_cast<CMain_SkillNameUI*>(Find_ChildUI(L"Party_BasicInfo_SkillName"))->Set_Name(4.0f);
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_BasicInfo_HpBar"))->Set_Ber(_float(tStat.iHP / tStat.iMaxHP));
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_BasicInfo_LevelBer"))->Set_Ber(_float(tStat.iExp / tStat.iMaxExp));
	}
		break;
	default:
		Assert("No Members");
		break;
	}
}

CCanvas_Party * CCanvas_Party::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_Party*		pInstance = new CCanvas_Party(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_Party");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_Party::Clone(void * pArg)
{
	CCanvas_Party*		pInstance = new CCanvas_Party(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_Party");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_Party::Free()
{
	CCanvas::Free();
}
