#include "stdafx.h"
#include "..\public\Canvas_Party.h"
#include "GameInstance.h"
#include "PlayerInfoManager.h"

#include "DefaultUI.h"
#include "ShaderUI.h"
#include "ButtonUI.h"
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
	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_01_Leader_B"))->Set_PickInitialize();

	m_eSASMember = CCanvas_Party::SASMEMBER::YUITO;
	PickInfo();
	SASMemberInfo_Initialize();
	m_bFrontPage = true;

	return S_OK;
}

void CCanvas_Party::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	ChildRender_Tick();

	if (false == m_bVisible) return;

	CurrentPick_Tick();
	Reserve_Tick();
	ReserveArrow_Tick();
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

#pragma region 오른쪽 하단 정보
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
	if (m_eSASMember == SASMEMBER::YUITO)	wsprintf(szText, TEXT("-")); else	wsprintf(szText, TEXT("%d"), m_tPickIngo.iBondLevel);	// 유대 관계 레벨 (유이토는 -)
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
	if (m_eSASMember == SASMEMBER::YUITO)	pGameInstance->Render_Font(L"Pretendard32", L"----", vPosition + _float2(276.0f, 255.0f), 0.f, { 0.4f, 0.4f }, vColor);
	else pGameInstance->Render_Font(L"Pretendard32", L"자유롭게 싸워라", vPosition + _float2(246.0f, 255.0f), 0.f, { 0.4f, 0.4f }, vColor);
#pragma endregion

#pragma region 유이토
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
	dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_01_Leader_HpBar"))->Set_Ber(_float(tPlayerStat.m_iHP) / tPlayerStat.m_iMaxHP);

	pGameInstance->Render_Font(L"Pretendard32", L"유이토 스메라기", vPosition + _float2(178.0f, 21.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"레벨", vPosition + _float2(180.0f, 48.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"체력", vPosition + _float2(180.0f, 72.0f), 0.f, vFontSmaillSize, vColor);
#pragma endregion

#pragma region 하나비
	// 하나비
	HANABI_STAT tHanabiStat = CPlayerInfoManager::GetInstance()->Get_HanabiStat();
	if (true == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::HANABI))
	{
		vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_02_Member_B"))->GetScreenSpaceLeftTop();
		wsprintf(szText, TEXT("%d"), tHanabiStat.iBondLevel);
		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(332.0f, 49.0f), 0.f, vFontSmaillSize, vColor);
		wsprintf(szText, TEXT("%d"), tHanabiStat.iLevel);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(207.0f, 46.0f), 0.f, vFontBigSize, vColor);
		wsprintf(szText, TEXT("%d / %d"), tHanabiStat.iHP, tHanabiStat.iMaxHP);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(283.0f, 71.0f), 0.f, vFontSmaillSize, vColor);
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_02_Member_HpBar"))->Set_Ber(_float(tHanabiStat.iHP) / tHanabiStat.iMaxHP);

		pGameInstance->Render_Font(L"Pretendard32", L"하나비 이치조", vPosition + _float2(178.0f, 21.0f), 0.f, vFontBigSize, vColor);
		pGameInstance->Render_Font(L"Pretendard32", L"레벨", vPosition + _float2(180.0f, 48.0f), 0.f, vFontSmaillSize, vColor);
		pGameInstance->Render_Font(L"Pretendard32", L"체력", vPosition + _float2(180.0f, 72.0f), 0.f, vFontSmaillSize, vColor);
	}
#pragma endregion

#pragma region 츠구미
	// 츠구미
	TSUGUMI_STAT tTsugumiStat = CPlayerInfoManager::GetInstance()->Get_TsugumiStat();
	if (true == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::TSUGUMI))
	{
		vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_03_Member_B"))->GetScreenSpaceLeftTop();
		wsprintf(szText, TEXT("%d"), tHanabiStat.iBondLevel);
		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(333.0f, 50.0f), 0.f, vFontSmaillSize, vColor);
		wsprintf(szText, TEXT("%d"), tTsugumiStat.iLevel);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(207.0f, 46.0f), 0.f, vFontBigSize, vColor);
		wsprintf(szText, TEXT("%d / %d"), tTsugumiStat.iHP, tTsugumiStat.iMaxHP);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(283.0f, 71.0f), 0.f, vFontSmaillSize, vColor);
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_03_Member_HpBar"))->Set_Ber(_float(tTsugumiStat.iHP) / tTsugumiStat.iMaxHP);

		pGameInstance->Render_Font(L"Pretendard32", L"츠구미 나자르", vPosition + _float2(178.0f, 21.0f), 0.f, vFontBigSize, vColor);
		pGameInstance->Render_Font(L"Pretendard32", L"레벨", vPosition + _float2(180.0f, 48.0f), 0.f, vFontSmaillSize, vColor);
		pGameInstance->Render_Font(L"Pretendard32", L"체력", vPosition + _float2(180.0f, 72.0f), 0.f, vFontSmaillSize, vColor);
	}
#pragma endregion

#pragma region XX
	if (m_bFrontPage == true)
	{
		for (size_t i = 0; i < 2; i++)
		{
			if (true == m_arrReserve_Index[i])
			{
				if (i == 0)
				{
					vPosition = Find_ChildUI(L"Party_XX0_ReserveB")->GetScreenSpaceLeftTop();
					dynamic_cast<CShaderUI*>(Find_ChildUI(L"Party_XX0_Reserve_Info"))->Set_Float2sX(m_arrReserve[i].fShaderInfoIndex);
				}
				else
				{
					vPosition = Find_ChildUI(L"Party_XX1_ReserveB")->GetScreenSpaceLeftTop();
					dynamic_cast<CShaderUI*>(Find_ChildUI(L"Party_XX1_Reserve_Info"))->Set_Float2sX(m_arrReserve[i].fShaderInfoIndex);
				}

				wsprintf(szText, TEXT("%d"), m_arrReserve[i].iBondLevel);
				pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(336.0f, 48.0f), 0.f, vFontSmaillSize, vColor);
				wsprintf(szText, TEXT("%d"), m_arrReserve[i].iLevel);
				CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(207.0f, 46.0f), 0.f, vFontBigSize, vColor);
				wsprintf(szText, TEXT("%d / %d"), m_arrReserve[i].iHP, m_arrReserve[i].iMaxHP);
				CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(283.0f, 70.0f), 0.f, vFontSmaillSize, vColor);
				dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_03_Member_HpBar"))->Set_Ber(_float(tTsugumiStat.iHP) / tTsugumiStat.iMaxHP);

				pGameInstance->Render_Font(L"Pretendard32", m_arrReserve[i].szName, vPosition + _float2(178.0f, 22.0f), 0.f, vFontBigSize, vColor);
				pGameInstance->Render_Font(L"Pretendard32", L"레벨", vPosition + _float2(180.0f, 48.0f), 0.f, vFontSmaillSize, vColor);
				pGameInstance->Render_Font(L"Pretendard32", L"체력", vPosition + _float2(180.0f, 70.0f), 0.f, vFontSmaillSize, vColor);
			}
		}
	}
	else
	{
		for (size_t i = 2; i < 4; i++)
		{
			if (true == m_arrReserve_Index[i])
			{
				if (i == 2)
				{
					vPosition = Find_ChildUI(L"Party_XX0_ReserveB")->GetScreenSpaceLeftTop();
					dynamic_cast<CShaderUI*>(Find_ChildUI(L"Party_XX0_Reserve_Info"))->Set_Float2sX(m_arrReserve[i].fShaderInfoIndex);
				}
				else
				{
					vPosition = Find_ChildUI(L"Party_XX1_ReserveB")->GetScreenSpaceLeftTop();
					dynamic_cast<CShaderUI*>(Find_ChildUI(L"Party_XX1_Reserve_Info"))->Set_Float2sX(m_arrReserve[i].fShaderInfoIndex);
				}

				wsprintf(szText, TEXT("%d"), m_arrReserve[i].iBondLevel);
				pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(336.0f, 48.0f), 0.f, vFontSmaillSize, vColor);
				wsprintf(szText, TEXT("%d"), m_arrReserve[i].iLevel);
				CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(207.0f, 46.0f), 0.f, vFontBigSize, vColor);
				wsprintf(szText, TEXT("%d / %d"), m_arrReserve[i].iHP, m_arrReserve[i].iMaxHP);
				CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(283.0f, 70.0f), 0.f, vFontSmaillSize, vColor);
				dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_03_Member_HpBar"))->Set_Ber(_float(tTsugumiStat.iHP) / tTsugumiStat.iMaxHP);

				pGameInstance->Render_Font(L"Pretendard32", m_arrReserve[i].szName, vPosition + _float2(178.0f, 22.0f), 0.f, vFontBigSize, vColor);
				pGameInstance->Render_Font(L"Pretendard32", L"레벨", vPosition + _float2(180.0f, 48.0f), 0.f, vFontSmaillSize, vColor);
				pGameInstance->Render_Font(L"Pretendard32", L"체력", vPosition + _float2(180.0f, 70.0f), 0.f, vFontSmaillSize, vColor);
			}
		}
	}
#pragma endregion

	return S_OK;
}

void CCanvas_Party::Imgui_RenderProperty()
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
	if (ImGui::Button("KYOTO"))
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

void CCanvas_Party::SASMemberInfo_Initialize()
{
	PICKINFO	tPickInfo;

	tPickInfo.szName = L"쿄카 에덴";
	tPickInfo.szSASNumber = L"KEK538232";
	tPickInfo.iLevel = 13;
	tPickInfo.iRemainingExp = 608;
	tPickInfo.iBondLevel = 2;
	tPickInfo.iHP = 2023;
	tPickInfo.iMaxHP = 2023;
	tPickInfo.iSprbrPower = 99;
	tPickInfo.iAttack = 148;
	tPickInfo.iDefense = 129;
	tPickInfo.fShaderInfoIndex = { 3.0f };
	tPickInfo.iSasMember = 3;
	m_arrSASInfo[0] = tPickInfo;

	tPickInfo.szName = L"루카 트래버스";
	tPickInfo.szSASNumber = L"KTL545301";
	tPickInfo.iLevel = 20;
	tPickInfo.iRemainingExp = 2239;
	tPickInfo.iBondLevel = 5;
	tPickInfo.iHP = 1515;
	tPickInfo.iMaxHP = m_tPickIngo.iHP;
	tPickInfo.iSprbrPower = 99;
	tPickInfo.iAttack = 148;
	tPickInfo.iDefense = 51;
	tPickInfo.fShaderInfoIndex = { 1.0f };
	tPickInfo.iSasMember = 4;
	m_arrSASInfo[1] = tPickInfo;

	tPickInfo.szName = L"시덴 리터";
	tPickInfo.szSASNumber = L"SR5553101";
	tPickInfo.iLevel = 18;
	tPickInfo.iRemainingExp = 645;
	tPickInfo.iBondLevel = 5;
	tPickInfo.iHP = 826;
	tPickInfo.iMaxHP = m_tPickIngo.iHP;
	tPickInfo.iSprbrPower = 93;
	tPickInfo.iAttack = 185;
	tPickInfo.iDefense = 47;
	tPickInfo.fShaderInfoIndex = { 2.0f };
	tPickInfo.iSasMember = 5;
	m_arrSASInfo[2] = tPickInfo;

	tPickInfo.szName = L"아라시 스프링";
	tPickInfo.szSASNumber = L"NSAS538201";
	tPickInfo.iLevel = 6;
	tPickInfo.iRemainingExp = 782;
	tPickInfo.iBondLevel = 6;
	tPickInfo.iHP = 1253;
	tPickInfo.iMaxHP = m_tPickIngo.iHP;
	tPickInfo.iSprbrPower = 112;
	tPickInfo.iAttack = 132;
	tPickInfo.iDefense = 32;
	tPickInfo.fShaderInfoIndex = { 4.0f };
	tPickInfo.iSasMember = 6;
	m_arrSASInfo[3] = tPickInfo;
}

void CCanvas_Party::ChildRender_Tick()
{
	// Canvas 를 만들 때 항상 켜야하는 애들이 많은지, 새로 켜야 하는 애들이 많은지 비교해서 구현하기

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
	{
#pragma region 캔버스의 Visible 을 따르지 않는 객체
		if (m_bVisible)
		{
			if (iter->first == L"Party_02_Member_HpBar" ||
				iter->first == L"Party_02_Member_HpBar_B")
			{
				if (false == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::HANABI)) continue;
				else dynamic_cast<CShaderUI*>(Find_ChildUI(L"Party_02_Member_Info"))->Set_Float2sX(1.0f);
			}

			if (iter->first == L"Party_03_Member_HpBar" ||
				iter->first == L"Party_03_Member_HpBar_B")
			{
				if (false== CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::TSUGUMI)) continue;
				else dynamic_cast<CShaderUI*>(Find_ChildUI(L"Party_03_Member_Info"))->Set_Float2sX(1.0f);
			}

			if (iter->first == L"Party_XXDwonArrow" ||
				iter->first == L"Party_XXDwonArrowB" ||
				iter->first == L"Party_XXUpArrow" ||
				iter->first == L"Party_XXUpArrowB")
			{
				continue;
			}
		}
#pragma endregion

		iter->second->SetVisible(m_bVisible);
	}
}

void CCanvas_Party::CurrentPick_Tick()
{
	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_01_Leader_B"))->Get_OnButton())
	{
		m_eSASMember = SASMEMBER::YUITO;

		PickInfo();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_01_Leader_B"))->Set_OnButton();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_02_Member_B"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_03_Member_B"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_XX0_ReserveB"))->Set_OnAlpha();
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_XX1_ReserveB"))->Set_OnAlpha();
		Find_ChildUI(L"Party_InfoPick")->Set_Position(Find_ChildUI(L"Party_01_Leader_B")->Get_Position());
	}

	if (true == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::HANABI))
	{
		if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_02_Member_B"))->Get_OnButton())
		{
			m_eSASMember = SASMEMBER::HANABI;

			PickInfo();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_02_Member_B"))->Set_OnButton();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_01_Leader_B"))->Set_OnAlpha();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_03_Member_B"))->Set_OnAlpha();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_XX0_ReserveB"))->Set_OnAlpha();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_XX1_ReserveB"))->Set_OnAlpha();
			Find_ChildUI(L"Party_InfoPick")->Set_Position(Find_ChildUI(L"Party_02_Member_B")->Get_Position());
		}
	}
	else
	{
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_02_Member_B"))->Set_OnAlpha();	// 멤버로 추가되지 않았으면 불이 들어올 수 없게 한다.
	}

	if (true == CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::TSUGUMI))
	{
		if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_03_Member_B"))->Get_OnButton())
		{
			m_eSASMember = SASMEMBER::TSUGUMI;

			PickInfo();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_03_Member_B"))->Set_OnButton();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_01_Leader_B"))->Set_OnAlpha();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_02_Member_B"))->Set_OnAlpha();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_XX0_ReserveB"))->Set_OnAlpha();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_XX1_ReserveB"))->Set_OnAlpha();
			Find_ChildUI(L"Party_InfoPick")->Set_Position(Find_ChildUI(L"Party_03_Member_B")->Get_Position());
		}
	}
	else
	{
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_03_Member_B"))->Set_OnAlpha();
	}

	if (true == m_arrReserve_Index[0])
	{
		if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_XX0_ReserveB"))->Get_OnButton())
		{
			if (m_bFrontPage)
				m_eSASMember = SASMEMBER(m_arrReserve[0].iSasMember);
			else
				m_eSASMember = SASMEMBER(m_arrReserve[2].iSasMember);

			PickInfo();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_XX0_ReserveB"))->Set_OnButton();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_01_Leader_B"))->Set_OnAlpha();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_02_Member_B"))->Set_OnAlpha();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_03_Member_B"))->Set_OnAlpha();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_XX1_ReserveB"))->Set_OnAlpha();
			Find_ChildUI(L"Party_InfoPick")->Set_Position(Find_ChildUI(L"Party_XX0_ReserveB")->Get_Position());
		}
	}
	else
	{
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_XX0_ReserveB"))->Set_OnAlpha();
	}

	if (true == m_arrReserve_Index[1])
	{
		if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_XX1_ReserveB"))->Get_OnButton())
		{
			if (m_bFrontPage)
				m_eSASMember = SASMEMBER(m_arrReserve[1].iSasMember);
			else
				m_eSASMember = SASMEMBER(m_arrReserve[3].iSasMember);

			PickInfo();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_XX1_ReserveB"))->Set_OnButton();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_01_Leader_B"))->Set_OnAlpha();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_02_Member_B"))->Set_OnAlpha();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_03_Member_B"))->Set_OnAlpha();
			dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_XX0_ReserveB"))->Set_OnAlpha();
			Find_ChildUI(L"Party_InfoPick")->Set_Position(Find_ChildUI(L"Party_XX1_ReserveB")->Get_Position());
		}
	}
	else
	{
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_XX1_ReserveB"))->Set_OnAlpha();
	}
}

void CCanvas_Party::PickInfo()
{
	switch (m_eSASMember)
	{
	case Client::CCanvas_Party::SASMEMBER::YUITO :
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
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_BasicInfo_HpBar"))->Set_Ber(_float(tStat.m_iHP) / tStat.m_iMaxHP);
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_BasicInfo_LevelBer"))->Set_Ber(_float(tStat.iExp) / tStat.iMaxExp);
	}
		break;
	case Client::CCanvas_Party::SASMEMBER::HANABI:
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
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_BasicInfo_HpBar"))->Set_Ber(_float(tStat.iHP) / tStat.iMaxHP);
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_BasicInfo_LevelBer"))->Set_Ber(_float(tStat.iExp) / tStat.iMaxExp);
	}
		break;
	case Client::CCanvas_Party::SASMEMBER::TSUGUMI:
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
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_BasicInfo_HpBar"))->Set_Ber(_float(tStat.iHP) / tStat.iMaxHP);
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_BasicInfo_LevelBer"))->Set_Ber(_float(tStat.iExp) / tStat.iMaxExp);
	}
		break;
	case Client::CCanvas_Party::SASMEMBER::KYOTO:
	{
		m_tPickIngo.szName = m_arrSASInfo[0].szName;
		m_tPickIngo.szSASNumber = m_arrSASInfo[0].szSASNumber;
		m_tPickIngo.iLevel = m_arrSASInfo[0].iLevel;
		m_tPickIngo.iRemainingExp = m_arrSASInfo[0].iRemainingExp;
		m_tPickIngo.iBondLevel = m_arrSASInfo[0].iBondLevel;
		m_tPickIngo.iHP = m_arrSASInfo[0].iHP;
		m_tPickIngo.iMaxHP = m_arrSASInfo[0].iMaxHP;
		m_tPickIngo.iSprbrPower = m_arrSASInfo[0].iSprbrPower;
		m_tPickIngo.iAttack = m_arrSASInfo[0].iAttack;
		m_tPickIngo.iDefense = m_arrSASInfo[0].iDefense;

		dynamic_cast<CMain_FaceUI*>(Find_ChildUI(L"Party_BasicInfo_Face"))->Set_Face(8.0f);
		dynamic_cast<CMain_SkillIconUI*>(Find_ChildUI(L"Party_BasicInfo_Icon"))->Set_Icon(3.0f);
		dynamic_cast<CMain_SkillNameUI*>(Find_ChildUI(L"Party_BasicInfo_SkillName"))->Set_Name(3.0f);
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_BasicInfo_HpBar"))->Set_Ber(1.0f);
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_BasicInfo_LevelBer"))->Set_Ber(1.0f);
	}
	break;
	case Client::CCanvas_Party::SASMEMBER::LUCA:
	{
		m_tPickIngo.szName = m_arrSASInfo[1].szName;
		m_tPickIngo.szSASNumber = m_arrSASInfo[1].szSASNumber;
		m_tPickIngo.iLevel = m_arrSASInfo[1].iLevel;
		m_tPickIngo.iRemainingExp = m_arrSASInfo[1].iRemainingExp;
		m_tPickIngo.iBondLevel = m_arrSASInfo[1].iBondLevel;
		m_tPickIngo.iHP = m_arrSASInfo[1].iHP;
		m_tPickIngo.iMaxHP = m_arrSASInfo[1].iMaxHP;
		m_tPickIngo.iSprbrPower = m_arrSASInfo[1].iSprbrPower;
		m_tPickIngo.iAttack = m_arrSASInfo[1].iAttack;
		m_tPickIngo.iDefense = m_arrSASInfo[1].iDefense;

		dynamic_cast<CMain_FaceUI*>(Find_ChildUI(L"Party_BasicInfo_Face"))->Set_Face(5.0f);
		dynamic_cast<CMain_SkillIconUI*>(Find_ChildUI(L"Party_BasicInfo_Icon"))->Set_Icon(5.0f);
		dynamic_cast<CMain_SkillNameUI*>(Find_ChildUI(L"Party_BasicInfo_SkillName"))->Set_Name(5.0f);
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_BasicInfo_HpBar"))->Set_Ber(1.0f);
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_BasicInfo_LevelBer"))->Set_Ber(1.0f);
	}
	break;
	case Client::CCanvas_Party::SASMEMBER::SEEDEN:
	{
		m_tPickIngo.szName = m_arrSASInfo[2].szName;
		m_tPickIngo.szSASNumber = m_arrSASInfo[2].szSASNumber;
		m_tPickIngo.iLevel = m_arrSASInfo[2].iLevel;
		m_tPickIngo.iRemainingExp = m_arrSASInfo[2].iRemainingExp;
		m_tPickIngo.iBondLevel = m_arrSASInfo[2].iBondLevel;
		m_tPickIngo.iHP = m_arrSASInfo[2].iHP;
		m_tPickIngo.iMaxHP = m_arrSASInfo[2].iMaxHP;
		m_tPickIngo.iSprbrPower = m_arrSASInfo[2].iSprbrPower;
		m_tPickIngo.iAttack = m_arrSASInfo[2].iAttack;
		m_tPickIngo.iDefense = m_arrSASInfo[2].iDefense;

		dynamic_cast<CMain_FaceUI*>(Find_ChildUI(L"Party_BasicInfo_Face"))->Set_Face(7.0f);
		dynamic_cast<CMain_SkillIconUI*>(Find_ChildUI(L"Party_BasicInfo_Icon"))->Set_Icon(7.0f);
		dynamic_cast<CMain_SkillNameUI*>(Find_ChildUI(L"Party_BasicInfo_SkillName"))->Set_Name(7.0f);
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_BasicInfo_HpBar"))->Set_Ber(1.0f);
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_BasicInfo_LevelBer"))->Set_Ber(1.0f);
	}
	break;
	case Client::CCanvas_Party::SASMEMBER::ARASHI:
	{
		m_tPickIngo.szName = m_arrSASInfo[3].szName;
		m_tPickIngo.szSASNumber = m_arrSASInfo[3].szSASNumber;
		m_tPickIngo.iLevel = m_arrSASInfo[3].iLevel;
		m_tPickIngo.iRemainingExp = m_arrSASInfo[3].iRemainingExp;
		m_tPickIngo.iBondLevel = m_arrSASInfo[3].iBondLevel;
		m_tPickIngo.iHP = m_arrSASInfo[3].iHP;
		m_tPickIngo.iMaxHP = m_arrSASInfo[3].iMaxHP;
		m_tPickIngo.iSprbrPower = m_arrSASInfo[3].iSprbrPower;
		m_tPickIngo.iAttack = m_arrSASInfo[3].iAttack;
		m_tPickIngo.iDefense = m_arrSASInfo[3].iDefense;

		dynamic_cast<CMain_FaceUI*>(Find_ChildUI(L"Party_BasicInfo_Face"))->Set_Face(9.0f);
		dynamic_cast<CMain_SkillIconUI*>(Find_ChildUI(L"Party_BasicInfo_Icon"))->Set_Icon(9.0f);
		dynamic_cast<CMain_SkillNameUI*>(Find_ChildUI(L"Party_BasicInfo_SkillName"))->Set_Name(9.0f);
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_BasicInfo_HpBar"))->Set_Ber(1.0f);
		dynamic_cast<CMain_BarUI*>(Find_ChildUI(L"Party_BasicInfo_LevelBer"))->Set_Ber(1.0f);
	}
	break;
	default:
		Assert("No Members");
		break;
	}
}

void CCanvas_Party::Reserve_Tick()
{
	if (m_iReserve_Count >= 4) return;		// 3 과 같아졌다면 모든 데이터를 채운 것 이기 때문에 반복문을 돌지 않도록 한다.

	if (CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::KYOTO) == true)
	{
		if (m_arrReserve_Index[0] == false)
		{
			m_arrReserve_Index[0] = true;
			m_arrReserve[m_iReserve_Count] = m_arrSASInfo[0]; // m_arrReserve[m_iReserve_Count] : Reserve_Index 에 빈 순서대로 / m_arrSASInfo[0] 즉, 쿄카의 정보가 들어간다.
			++m_iReserve_Count;	// 현재까지 들어온 Reserve 의 개수를 센다.
		}
	}

	if (CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::LUCA) == true)
	{
		if (m_arrReserve_Index[1] == false)
		{
			m_arrReserve_Index[1] = true;
			m_arrReserve[m_iReserve_Count] = m_arrSASInfo[1];
			++m_iReserve_Count;
		}
	}

	if (CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::SEEDEN) == true)
	{
		if (m_arrReserve_Index[2] == false)
		{
			m_arrReserve_Index[2] = true;
			m_arrReserve[m_iReserve_Count] = m_arrSASInfo[2];
			++m_iReserve_Count;
		}
	}

	if (CPlayerInfoManager::GetInstance()->Get_SASMember(SASMEET::ARASHI) == true)
	{
		if (m_arrReserve_Index[3] == false)
		{
			m_arrReserve_Index[3] = true;
			m_arrReserve[m_iReserve_Count] = m_arrSASInfo[3];
			++m_iReserve_Count;
		}
	}

	// m_iReserve_Count 가 3개 이상 이라면 화살표가 On 된다.
	if (3 <= m_iReserve_Count)
	{
		Find_ChildUI(L"Party_XXDwonArrow")->SetVisible(true);
		Find_ChildUI(L"Party_XXDwonArrowB")->SetVisible(true);
		Find_ChildUI(L"Party_XXUpArrow")->SetVisible(true);
		Find_ChildUI(L"Party_XXUpArrowB")->SetVisible(true);
	}
}

void CCanvas_Party::ReserveArrow_Tick()
{
	if (3 > m_iReserve_Count) return;

	if (true == dynamic_cast<CButtonUI*>(Find_ChildUI(L"Party_XXUpArrowB"))->Get_Input())
	{
		dynamic_cast<CButtonUI*>(Find_ChildUI(L"Party_XXUpArrowB"))->Set_Input();
		m_bFrontPage = true;
	}

	if (true == dynamic_cast<CButtonUI*>(Find_ChildUI(L"Party_XXDwonArrowB"))->Get_Input())
	{
		dynamic_cast<CButtonUI*>(Find_ChildUI(L"Party_XXDwonArrowB"))->Set_Input();
		m_bFrontPage = false;
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
