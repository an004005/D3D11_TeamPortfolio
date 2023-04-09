#include "stdafx.h"
#include "..\public\Canvas_BrainMap.h"
#include "GameInstance.h"
#include "PlayerInfoManager.h"
#include "UI_Manager.h"

#include "Canvas_PlayerInfoMove.h"
#include "DefaultUI.h"
#include "Main_PickUI.h"
#include "Main_OnMouseUI.h"
#include "Main_BrainMapIconPickUI.h"
#include "Main_BrainGaugeUI.h"

CCanvas_BrainMap::CCanvas_BrainMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_BrainMap::CCanvas_BrainMap(const CCanvas_BrainMap& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_BrainMap::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	m_iStartPriority = m_iPriority;

	return S_OK;
}

HRESULT CCanvas_BrainMap::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	m_bVisible = false;
	Brain_Intiialize();
	ChildUI_Intiialize();
	Link_Initialize();

	m_arrCurrentHighLevel.fill(0);

	return S_OK;
}

void CCanvas_BrainMap::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
	{
		if (true == m_bVisible)
		{
			// Lick 의 m_bVisible 은 이 캔버스가 true 일 때는 건들이지 않는다.
			wstring	wbSLink = L"";
			if (10 > m_iLinkCount)
				wbSLink = L"Link__0" + to_wstring(m_iLinkCount);
			else
				wbSLink = L"Link__" + to_wstring(m_iLinkCount);

			if (iter->first == wbSLink)
			{
				++m_iLinkCount;
				if (56 == m_iLinkCount)
					m_iLinkCount = 0;
				continue;
			}

			wstring	wbSLinkS = L"Link__S_0" + to_wstring(m_iLinkSCount);
			if (iter->first == wbSLinkS)
			{
				++m_iLinkSCount;
				if (4 == m_iLinkSCount)
					m_iLinkSCount = 0;
				continue;
			}
		}

		iter->second->SetVisible(m_bVisible);
	}

	if (false == m_bVisible) return;
	OnIcon_Tick();
	SkillAcquisition_Tick();
	OnLick_Tick();
}

void CCanvas_BrainMap::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_BrainMap::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_tchar szText[MAX_PATH] = TEXT("");
	_float2 vPosition = { 0.0f, 0.0f };
	_float2 vFontBigSize = { 0.45f, 0.45f };
	_float2 vFontSmaillSize = { 0.35f, 0.35f };
	_float4 vColor = { 0.0f, 0.0f, 0.0f, 0.0f };

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (false == m_bSkillAcquisition)
	{
		m_iPriority = m_iStartPriority;
		vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
	}
	else
	{
		m_iPriority = 20;
		vColor = { 0.752f, 0.752f, 0.596f, 1.0f };

		vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"SkillAcquisition"))->GetScreenSpaceLeftTop();
		wsprintf(szText, TEXT("<%s>"), m_CurrentBrainInfo.szBrainName);
		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(130.0f, 150.0f), 0.f, vFontSmaillSize * 1.5f, vColor);
		pGameInstance->Render_Font(L"Pretendard32", m_szAlarmText, vPosition + _float2(130.0f, 200.0f), 0.f, vFontSmaillSize * 1.5f, vColor);

		vColor = { 0.488f, 0.427f, 0.384f, 1.0f };
	}

	vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"BrainExplanation_Face"))->GetScreenSpaceLeftTop();
	wsprintf(szText, TEXT("%d"), CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iHP);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(385.0f, 10.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"체력", vPosition + _float2(120.0f, 10.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d"), CPlayerInfoManager::GetInstance()->Get_PlayerStat().iSprbrPower);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(385.0f, 31.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"초뇌능력", vPosition + _float2(120.0f, 31.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d"), CPlayerInfoManager::GetInstance()->Get_PlayerStat().iAttack);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(385.0f, 52.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"공격력", vPosition + _float2(120.0f, 52.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d"), CPlayerInfoManager::GetInstance()->Get_PlayerStat().iDefense);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(385.0f, 75.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"방어력", vPosition + _float2(120.0f, 75.0f), 0.f, vFontSmaillSize, vColor);

	// 마우스 커서 가 올라갔을 때 또는 클릭이 되었을 때 정보가 떠야 한다.
	if (BRAINTYPE_END ==  m_CurrentBrainInfo.eType)
		return S_OK;

	vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"BrainExplanation"))->GetScreenSpaceLeftTop();

	pGameInstance->Render_Font(L"Pretendard32", m_CurrentBrainInfo.szBrainName, vPosition + _float2(100.0f, 120.0f), 0.f, { 0.6f, 0.6f }, vColor);
	pGameInstance->Render_Font(L"Pretendard32", m_CurrentBrainInfo.szBrainEx[0], vPosition + _float2(100.0f, 510.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", m_CurrentBrainInfo.szBrainEx[1], vPosition + _float2(100.0f, 540.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", m_CurrentBrainInfo.szBrainEx[2], vPosition + _float2(100.0f, 570.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", m_CurrentBrainInfo.szBrainEx[3], vPosition + _float2(100.0f, 600.0f), 0.f, vFontBigSize, vColor);

	if (true == m_CurrentBrainInfo.bUse)
	{
		pGameInstance->Render_Font(L"Pretendard32", L"습득 완료", vPosition + _float2(100.0f, 675.0f), 0.f, vFontBigSize, vColor);
	}
	else
	{
		// 내 현재 BP
		wsprintf(szText, TEXT("/      %d BP"), CPlayerInfoManager::GetInstance()->Get_PlayerStat().iBP);
		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(141.0f, 675.0f), 0.f, vFontBigSize, vColor);

		if (false == m_bSkillAcquisition) // 위에 BP가 부족할 때는 무조건 위에서 지정한 색상으로 출력한다.
		{
			// Icon 의 BP
			if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().iBP >= m_vecIconUI[m_iCurrentIndex]->Get_BrainInfo().iBP)
				vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
			else
				vColor = { 0.488f, 0.427f, 0.384f, 1.0f };
		}

		wsprintf(szText, TEXT("%d"), m_CurrentBrainInfo.iBP);
		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(112.0f, 675.0f), 0.f, vFontBigSize, vColor);
	}

	return S_OK;
}

void CCanvas_BrainMap::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_BrainMap::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_BrainMap::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

void CCanvas_BrainMap::Brain_Intiialize()
{
	BRAININFO	tBrainInfo;
	tBrainInfo.arrNeighbor.fill(-1);

	// 드라이브
	// 0
	tBrainInfo.iBP = 1;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 3.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 2.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 1, 8, -1 };
	tBrainInfo.szBrainName = L"드라이브 게이지 축적량 상승";
	tBrainInfo.szBrainEx[0] = L"드라이브 게이지 축적량 +20%";
	tBrainInfo.szBrainEx[1] = L"적을 쓰러뜨리거나 적에게 대미지를 받는 등";
	tBrainInfo.szBrainEx[2] = L"특정 행동으로 인한 축적량이 증가하여";
	tBrainInfo.szBrainEx[3] = L"드라이브 게이지가 쌓이기 쉬워진다.";
	m_vecBrain.push_back(tBrainInfo);

	// 1
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.bUse = false;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 4, 2, -1 };
	tBrainInfo.szBrainName = L"드라이브: 체력 자동 회복";
	tBrainInfo.szBrainEx[0] = L"드라이브 중 체력이 서서히 회복된다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 2
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 3, -1, -1 };
	tBrainInfo.szBrainName = L"드라이브: 상태이상 회복";
	tBrainInfo.szBrainEx[0] = L"드라이브 발동하면 상태이상을 회복한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 3
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"드라이브: 연속 어설트 비전";
	tBrainInfo.szBrainEx[0] = L"브라이브 중 <어설트 비전> 이";
	tBrainInfo.szBrainEx[1] = L"연속으로 발생하게 된다.";
	tBrainInfo.szBrainEx[2] = L"<어설트 비전>은 유대 레벨이 상승하면 발생";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 4
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 3.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 2.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 5, -1, -1 };
	tBrainInfo.szBrainName = L"드라이브: 드라이브 게이지 회복1";
	tBrainInfo.szBrainEx[0] = L"드라이브 중 브레인  크러시로 적을 쓰러뜨리면";
	tBrainInfo.szBrainEx[1] = L"브라이브 게이지가 조금 회복된다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 5
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 6, -1, -1 };
	tBrainInfo.szBrainName = L"드라이브: 보너스 배율 증가량 상승";
	tBrainInfo.szBrainEx[0] = L"드라이브 중 획득 EXP의 보너스 배율의";
	tBrainInfo.szBrainEx[1] = L"증가량이 상승한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 6
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 7, -1, -1 };
	tBrainInfo.szBrainName = L"드라이브: 공격 크러시 효과 상승";
	tBrainInfo.szBrainEx[0] = L"드라이브 중 공격으로 크러시 게이지를";
	tBrainInfo.szBrainEx[1] = L"많이 깎을 수 있다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 7
	tBrainInfo.iBP = 6;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 3.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 2.0f, 3.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"드라이브: 드라이브 게이지 회복 2";
	tBrainInfo.szBrainEx[0] = L"드라이브 중 브레인 크러시로 적을 쓰러뜨리면";
	tBrainInfo.szBrainEx[1] = L"드라이브 게이지가 조금 회복된다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 8
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 9, -1, -1 };
	tBrainInfo.szBrainName = L"드라이브: 스테이터스 상승";
	tBrainInfo.szBrainEx[0] = L"드라이브 중 초뇌능력, 공격력, 방어력 +10%";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 9
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 10, -1, -1 };
	tBrainInfo.szBrainName = L"드라이브: SAS 게이지 회복";
	tBrainInfo.szBrainEx[0] = L"드라이브를 발동하면";
	tBrainInfo.szBrainEx[1] = L"모든 SAS 게이지가 모두 회복된다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 강화
	// 10
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 11, 15, -1 };
	tBrainInfo.szBrainName = L"염력 게이지 회복량 상승(공중 공격)";
	tBrainInfo.szBrainEx[0] = L"공중 공격으로 인한 염력 게이지의 회복량이 증가한다.";
	tBrainInfo.szBrainEx[1] = L"!![스페이스바]+[왼쪽 마루스] 올려베기 공격은 포함되지 않는다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 11
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 12, -1, -1 };
	tBrainInfo.szBrainName = L"무기 공격 크러시 효과 상승2";
	tBrainInfo.szBrainEx[0] = L"무기 공격으로 크러시 게이지를 많이 깍을 수 있다.";
	tBrainInfo.szBrainEx[1] = L"크러시 효과가 5% 증가한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 12
	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 3.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 2.0f, 3.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"염력 게이지 최대치 증가";
	tBrainInfo.szBrainEx[0] = L"같은 이름의 스킬이 2개 있으며, 1개를 습득하면 50%,";
	tBrainInfo.szBrainEx[1] = L"2개를 습득하면 100% 염력 게이지의 최대치가 증가한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 13
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 5.0f };
	tBrainInfo.arrNeighbor = { 14, 17, 19 };
	tBrainInfo.szBrainName = L"공격력 상승 1";
	tBrainInfo.szBrainEx[0] = L"공격력 +5%";
	tBrainInfo.szBrainEx[1] = L"무기 공격으로 주는 대미지가 증가한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 14
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 15, -1, -1 };
	tBrainInfo.szBrainName = L"후속타";
	tBrainInfo.szBrainEx[0] = L"다운 중인 적에게 주는 대미지가 증가한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 15
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 3.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 2.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 10, 16, -1 };
	tBrainInfo.szBrainName = L"소생 속도 상승";
	tBrainInfo.szBrainEx[0] = L"동료를 소생하는 속도가 상승한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 16
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 3.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"무기 공격 크러시 효과 상승 1";
	tBrainInfo.szBrainEx[0] = L"무기 공격으로 크러시 게이지를 많이 깍을 수 있다.";
	tBrainInfo.szBrainEx[1] = L"크러시 효과가 5% 증가한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 17
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 5.0f };
	tBrainInfo.arrNeighbor = { 18, -1, -1 };
	tBrainInfo.szBrainName = L"공격력 상승 2";
	tBrainInfo.szBrainEx[0] = L"공격력 +5%";
	tBrainInfo.szBrainEx[1] = L"무기 공격으로 주는 대미지가 증가한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 18
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 3.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 2.0f, 3.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"염력 게이지 최대치 증가";
	tBrainInfo.szBrainEx[0] = L"같은 이름의 스킬이 2개 있으며, 1개를 습득하면 50%,";
	tBrainInfo.szBrainEx[1] = L"2개를 습득하면 100% 염력 게이지의 최대치가 증가한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 19
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 20, -1, -1 };
	tBrainInfo.szBrainName = L"초뇌능력 상승";
	tBrainInfo.szBrainEx[0] = L"초뇌능력 +5%";
	tBrainInfo.szBrainEx[1] = L"염력 공격으로 주는 대미지가 증가한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 20
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.bUse = false;
	tBrainInfo.arrNeighbor = { 21, -1, -1 };
	tBrainInfo.vOnIconIndex = { 2.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 3.0f };
	tBrainInfo.szBrainName = L"염력 공격 크러시 효과 상승";
	tBrainInfo.szBrainEx[0] = L"염력 공격으로 크러시 게이지를 많이 깎을 수 있다.";
	tBrainInfo.szBrainEx[1] = L"크러시 효과가 5% 증가한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 확장
	// 21
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 5.0f };
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"내려치기 공격";
	tBrainInfo.szBrainEx[0] = L"체공 중에 [C] 길게 누르기로";
	tBrainInfo.szBrainEx[1] = L"공중에서 하강하며 무기 공격을 한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 22
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 5.0f };
	tBrainInfo.arrNeighbor = { 21, 24, 30 };
	tBrainInfo.szBrainName = L"차지 공격";
	tBrainInfo.szBrainEx[0] = L"[C] 길게 누르기로 힘을 모으고, [C]를 떼면";
	tBrainInfo.szBrainEx[1] = L"넓은 범위에 위력을 높인 무기 공격을 한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 23

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 22, 31, -1 };
	tBrainInfo.szBrainName = L"낙법";
	tBrainInfo.szBrainEx[0] = L"날려질 때 [위]로 자세를 바로 잡아 다운을 방지한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 24
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 3.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 2.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 25, -1, -1 };
	tBrainInfo.szBrainName = L"저스트 회피 염력 게이지 회복";
	tBrainInfo.szBrainEx[0] = L"저스트 회피 성공 시 염력 게이지를 조금 회복한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 25
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 5.0f };
	tBrainInfo.arrNeighbor = { 26, -1, -1 };
	tBrainInfo.szBrainName = L"무기 콤보 4단";
	tBrainInfo.szBrainEx[0] = L"무기 공격을 4회까지 연속해서 할 수 있다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 26
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 5.0f };
	tBrainInfo.arrNeighbor = { 27, 29, -1 };
	tBrainInfo.szBrainName = L"2단 차지 공격";
	tBrainInfo.szBrainEx[0] = L"[C] 길게 누르기로 힘을 모은 후, 한 단계 더 힘을 모으고";
	tBrainInfo.szBrainEx[1] = L"[C] 를 떼면 <차지 공격> 의 위력이 높아진다. ";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 27
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 28, -1, -1 };
	tBrainInfo.szBrainName = L"차지 공격 강화";
	tBrainInfo.szBrainEx[0] = L"<차지 공격> 과 <2단 차지 공격>의";
	tBrainInfo.szBrainEx[1] = L"공격 범위가 넓어지고 위력이 높아진다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 28
	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 5.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"염력 후속타";
	tBrainInfo.szBrainEx[0] = L"다운된 적에게 [오름쪽 마우스]로 염력 공격을 하면";
	tBrainInfo.szBrainEx[1] = L"내리치듯이 염력 공격을 한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 29
	tBrainInfo.iBP = 6;
	tBrainInfo.iLevel = 6;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 5.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"무기 콤보 5탄";
	tBrainInfo.szBrainEx[0] = L"무기 공격을 5회까지 연속해서 할 수 있다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 30
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = GREEN;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 2.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"아이템 끌어당기기";
	tBrainInfo.szBrainEx[0] = L"근처에 떨어져있는 드롭 아이템을 자동으로 끌어당긴다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 31
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 32, -1, -1 };
	tBrainInfo.szBrainName = L"저스트 회피 공격";
	tBrainInfo.szBrainEx[0] = L"저스트 회피 성공 시 [왼쪽 마우스]로 무기 공격을 한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 32
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 33, 37, -1 };
	tBrainInfo.szBrainName = L"공궁 점프";
	tBrainInfo.szBrainEx[0] = L"공중 [스페이스바]로 한번 더 점프할 수 있다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 33
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 1.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 34, -1, -1 };
	tBrainInfo.szBrainName = L"추가 공격 확장 1";
	tBrainInfo.szBrainEx[0] = L"염력 추가 공격과 러시 추가 공격을";
	tBrainInfo.szBrainEx[1] = L"3회까지 연속해서 할 수 있다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 34
	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 5.0f };
	tBrainInfo.arrNeighbor = { 35, 36, -1 };
	tBrainInfo.szBrainName = L"회전 무빙 공격 확장";
	tBrainInfo.szBrainEx[0] = L"[C]로 이동하면서 넓은 범위에 휩쓰는";
	tBrainInfo.szBrainEx[1] = L"<회전 무빙 공격>을";
	tBrainInfo.szBrainEx[2] = L"한 콤보 중에 2번까지 할 수 있다.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 35
	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 1.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 4.0f }; 
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"공중 추가 공격";
	tBrainInfo.szBrainEx[0] = L"공중 무기 콤보 중에 [오름쪽 마우스]로 공중 추가 공격을 한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 36
	tBrainInfo.iBP = 6;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 1.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 4.0f }; 
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"추가 공격 확장 2";
	tBrainInfo.szBrainEx[0] = L"염력 추가 공격과 러시 추가 공격을";
	tBrainInfo.szBrainEx[1] = L"4회까지 연속해서 할 수 있다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 37
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 3.0f }; 
	tBrainInfo.arrNeighbor = { 38, 41, -1 };
	tBrainInfo.szBrainName = L"공중 스텝";
	tBrainInfo.szBrainEx[0] = L"공중 [위]로 공중 스텝을 할 수 있다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 38
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 5.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"공중 대시 공격";
	tBrainInfo.szBrainEx[0] = L"<공중 스텝> 중 [왼쪽 마우스]로 전진하면서 무기 공격을 한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 보조
	// 39
	tBrainInfo.iBP = 1;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 40, 46, -1 };
	tBrainInfo.szBrainName = L"플러그인 장비 슬롯 추가 1";
	tBrainInfo.szBrainEx[0] = L"장비 화면에서 적용할 수 있는 플러그인이 하나 증가한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 40
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = GREEN;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 2.0f };
	tBrainInfo.arrNeighbor = { 41, -1, -1 };
	tBrainInfo.szBrainName = L"아이템 사용 제한 단축";
	tBrainInfo.szBrainEx[0] = L"아이템 사용 후의 사용 제한 시간을 단축시킨다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 41
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 1.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 37, 42, -1 };
	tBrainInfo.szBrainName = L"브레인 크러시 SAS 회복";
	tBrainInfo.szBrainEx[0] = L"SAS 회과 중에 브레인 크러시로 적을 쓰러뜨리면";
	tBrainInfo.szBrainEx[1] = L"사용 중인 SAS의 SAS 게이지가 조금 회복한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 42
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 1.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 43, 44, -1 };
	tBrainInfo.szBrainName = L"브레인 크러시 획득 금액 보너스";
	tBrainInfo.szBrainEx[0] = L"브레인 크러시로 적을 쓰러뜨리면";
	tBrainInfo.szBrainEx[1] = L"적을 쓰러뜨렸을 때 입수할 수 있는 돈이 증가한다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 43
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 1.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 3.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"브레인 크러시 획득 EXP 보너스";
	tBrainInfo.szBrainEx[0] = L"브레인 크러시로 적을 쓰러뜨리면 획득 EXP가 증가한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 44
	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 45, -1, -1 };
	tBrainInfo.szBrainName = L"플러그인 장비 슬롯 추가 2";
	tBrainInfo.szBrainEx[0] = L"장비 화면에서 적용할 수 있는 플러그인이 하나 증가한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 45
	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"SAS 동시 발동";
	tBrainInfo.szBrainEx[0] = L"같은 이름의 스킬이 2개 있으며,";
	tBrainInfo.szBrainEx[1] = L"하나를 습득하면, SAS를 동시에 2개까지 발동할 수 있으며,";
	tBrainInfo.szBrainEx[2] = L"2개를 습득하면, SAS를 동시에 4개까지 발동할 수 있다.";
	tBrainInfo.szBrainEx[3] = L"!!SAS 발화와 SAS 방전은 동시에 발동할 수 없다.";
	m_vecBrain.push_back(tBrainInfo);

	// 46
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 47, 48, -1 };
	tBrainInfo.szBrainName = L"오버 킬";
	tBrainInfo.szBrainEx[0] = L"크러시 게이지가 다 깍여 활동을 정지한";
	tBrainInfo.szBrainEx[1] = L"적에게 공격을 명중시키면 EXP 보너스가 증가한다.";
	tBrainInfo.szBrainEx[2] = L"단, 브레인 크러시로밖에 쓰러뜨릴 수 없는 강적의 경우,";
	tBrainInfo.szBrainEx[3] = L"체력을 다 깍은 상황에서는 효과가 없다.";
	m_vecBrain.push_back(tBrainInfo);

	// 47
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 1.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 3.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"브레인 크러시 찬스 연장";
	tBrainInfo.szBrainEx[0] = L"적의 크러시 게이지를 다 깎은 후";
	tBrainInfo.szBrainEx[1] = L"적이 더 오랜 시간 활동을 정지하게 되어";
	tBrainInfo.szBrainEx[2] = L"브레인 크러시 가능한 시간이 연장한다.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 48
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"SAS 동시 발동";
	tBrainInfo.szBrainEx[0] = L"같은 이름의 스킬이 2개 있으며,";
	tBrainInfo.szBrainEx[1] = L"하나를 습득하면, SAS를 동시에 2개까지 발동할 수 있으며,";
	tBrainInfo.szBrainEx[2] = L"2개를 습득하면, SAS를 동시에 4개까지 발동할 수 있다.";
	tBrainInfo.szBrainEx[3] = L"!!SAS 발화와 SAS 방전은 동시에 발동할 수 없다.";
	m_vecBrain.push_back(tBrainInfo);
	// 잠시 다른 곳

	// 브레인 필드
	// 49
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = BRAINFIELD;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 3.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 2.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 50, 54, 58 };
	tBrainInfo.szBrainName = L"브레인 필드: 시간 연장 1";
	tBrainInfo.szBrainEx[0] = L"브레인 필드의 전개 시간을 5초 연장한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 50
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = BRAINFIELD;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 51, 53, -1 };
	tBrainInfo.szBrainName = L"브레인 필드: 차지 투하 레벨 상승";
	tBrainInfo.szBrainEx[0] = L"자신이 브레인 필드를 전대하고 있을 때";
	tBrainInfo.szBrainEx[1] = L"[G] 길게 누르기로 모이는 오브젝트가 증가하여";
	tBrainInfo.szBrainEx[2] = L"오브젝트를 낙하시켜서 주는 대미지가 증가한다.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 보조 _2
	// 51
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = GREEN;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 2.0f };
	tBrainInfo.arrNeighbor = { 48, 52, -1 };
	tBrainInfo.szBrainName = L"아이템 즉시 효과";
	tBrainInfo.szBrainEx[0] = L"아이템 사용 시의 동작을 생략해";
	tBrainInfo.szBrainEx[1] = L"즉시 효과를 얻을 수 있다.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 52
	tBrainInfo.iBP = 6;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"체력 자동 회복";
	tBrainInfo.szBrainEx[0] = L"비전투 시 체력이 서서히 회복된다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 다시 브레인 필드
	// 53
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"브레인 필드: 대미지 경감";
	tBrainInfo.szBrainEx[0] = L"자신이 브레인 필드를 전ㅐ하고 있을 때 받는 데미지가";
	tBrainInfo.szBrainEx[1] = L"경감된다. 같은 이름의 스킬이 2개 있으며";
	tBrainInfo.szBrainEx[2] = L"1개 습득하면 대미지가 반감,";
	tBrainInfo.szBrainEx[3] = L"2개 습득하면 대미자가 무효화된다.";
	m_vecBrain.push_back(tBrainInfo);

	// 54
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 55, -1, -1 };
	tBrainInfo.szBrainName = L"브레인 필드: 오버 킬";
	tBrainInfo.szBrainEx[0] = L"브레인 필드 전개 시, 크러시 게이지가 다 깎인";
	tBrainInfo.szBrainEx[1] = L"적에게 공격을 명중시키면 EXP 보너스가 증가한다.";
	tBrainInfo.szBrainEx[2] = L"단' 브레인 크러시로밖에 쓰러뜨릴 수 없는 강적의 경우";
	tBrainInfo.szBrainEx[3] = L"체력을 다 깎은 상황에서는 효과가 없다.";
	m_vecBrain.push_back(tBrainInfo);

	// 55
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 56, 57, -1 };
	tBrainInfo.szBrainName = L"브레인 필드: 타격 레벨 상승";
	tBrainInfo.szBrainEx[0] = L"자신의 브레인 필드를 전개하고 있을 때";
	tBrainInfo.szBrainEx[1] = L"[왼쪽 마우스]로 휘두르는 오브젝트가 증가하여";
	tBrainInfo.szBrainEx[2] = L"대미지를 줄 수 있는 범위가 넓어진다.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 56
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"브레인 필드: 드라이브 중 스톡 소비 없음";
	tBrainInfo.szBrainEx[0] = L"브레인 필드의 준비 아이콘이 표시된 상태에서";
	tBrainInfo.szBrainEx[1] = L"드라이브를 발동했을 경우, 드라이브 중 스톡을";
	tBrainInfo.szBrainEx[2] = L"소비하지 않고 브레인 필드를 전개할 수 있다.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 57
	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"브레인 필드: 대미지 경감";
	tBrainInfo.szBrainEx[0] = L"자신이 브레인 필드를 전ㅐ하고 있을 때 받는 데미지가";
	tBrainInfo.szBrainEx[1] = L"경감된다. 같은 이름의 스킬이 2개 있으며";
	tBrainInfo.szBrainEx[2] = L"1개 습득하면 대미지가 반감,";
	tBrainInfo.szBrainEx[3] = L"2개 습득하면 대미자가 무효화된다.";
	m_vecBrain.push_back(tBrainInfo);

	// 58
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = BRAINFIELD;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 59, -1, -1 };
	tBrainInfo.szBrainName = L"브레인 필드: 발사 레벨 상승 1";
	tBrainInfo.szBrainEx[0] = L"자신이 브레인 필드를 전개하고 일을 때,";
	tBrainInfo.szBrainEx[1] = L"[왼쪽 마우스 2] [오른쪽 마우스] 나 ";
	tBrainInfo.szBrainEx[2] = L"[왼쪽 마우스 3] [오른쪽 마우스]의 [오른쪽 마우스]로";
	tBrainInfo.szBrainEx[3] = L"날리는 오브젝트가 증가한다.";
	m_vecBrain.push_back(tBrainInfo);

	// 59
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 60, -1, -1 };
	tBrainInfo.szBrainName = L"브레인 필드: 발사 레벨 상승 2";
	tBrainInfo.szBrainEx[0] = L"자신이 브레인 필드를 전개하고 있을 때";
	tBrainInfo.szBrainEx[1] = L"[왼쪽 마우스 3] [오른쪽 마우스]의";
	tBrainInfo.szBrainEx[2] = L"[오른쪽 마우스]로 날리는 오브젝트의 수가 증가한다.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 60
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 4.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"브레인 필드: 시간 연장 2";
	tBrainInfo.szBrainEx[0] = L"브레인 필드 전개 시간을 5초 연장한다.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);
}

void CCanvas_BrainMap::ChildUI_Intiialize()
{
	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
	{
		iter->second->SetVisible(false);

		wstring	wsIcon = L"";
		if (10 > m_iIconCount)
			wsIcon = L"Icon__0" + to_wstring(m_iIconCount);
		else
			wsIcon = L"Icon__" + to_wstring(m_iIconCount);

		if (iter->first == wsIcon)
		{
			CMain_OnMouseUI* pMouseUI = dynamic_cast<CMain_OnMouseUI*>((*iter).second);
			pMouseUI->Set_BrainInfo(m_vecBrain[m_iIconCount]);
			
			if(0 == m_vecBrain[m_iIconCount].iLevel)
				pMouseUI->Set_IconIndex(m_vecBrain[m_iIconCount].vOnIconIndex);
			else
				pMouseUI->Set_IconIndex(m_vecBrain[m_iIconCount].vOffIconIndex);

			m_vecIconUI.push_back(pMouseUI);

			++m_iIconCount;
		}
	}
}

void CCanvas_BrainMap::Link_Initialize()
{
	LINKINFO	tLinkInfo;

	tLinkInfo.bSLink = false;

	tLinkInfo.iIconIndex0 = 0;
	tLinkInfo.iIconIndex1 = 1;
	tLinkInfo.iLickIndex = 0;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 1;
	tLinkInfo.iIconIndex1 = 4;
	tLinkInfo.iLickIndex = 1;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 4;
	tLinkInfo.iIconIndex1 = 5;
	tLinkInfo.iLickIndex = 2;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 5;
	tLinkInfo.iIconIndex1 = 6;
	tLinkInfo.iLickIndex = 3;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 6;
	tLinkInfo.iIconIndex1 = 7;
	tLinkInfo.iLickIndex = 4;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 6;
	tLinkInfo.iIconIndex1 = 7;
	tLinkInfo.iLickIndex = 4;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 1;
	tLinkInfo.iIconIndex1 = 2;
	tLinkInfo.iLickIndex = 5;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 2;
	tLinkInfo.iIconIndex1 = 3;
	tLinkInfo.iLickIndex = 6;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 59;
	tLinkInfo.iIconIndex1 = 60;
	tLinkInfo.iLickIndex = 7;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 8;
	tLinkInfo.iIconIndex1 = 9;
	tLinkInfo.iLickIndex = 8;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 10;
	tLinkInfo.iIconIndex1 = 11;
	tLinkInfo.iLickIndex = 9;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 11;
	tLinkInfo.iIconIndex1 = 12;
	tLinkInfo.iLickIndex = 10;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 8;
	tLinkInfo.iIconIndex1 = 0;
	tLinkInfo.iLickIndex = 11;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 10;
	tLinkInfo.iIconIndex1 = 15;
	tLinkInfo.iLickIndex = 12;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 13;
	tLinkInfo.iIconIndex1 = 14;
	tLinkInfo.iLickIndex = 13;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 15;
	tLinkInfo.iIconIndex1 = 14;
	tLinkInfo.iLickIndex = 14;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 16;
	tLinkInfo.iIconIndex1 = 15;
	tLinkInfo.iLickIndex = 15;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 13;
	tLinkInfo.iIconIndex1 = 17;
	tLinkInfo.iLickIndex = 16;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 18;
	tLinkInfo.iIconIndex1 = 17;
	tLinkInfo.iLickIndex = 17;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 13;
	tLinkInfo.iIconIndex1 = 19;
	tLinkInfo.iLickIndex = 18;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 19;
	tLinkInfo.iIconIndex1 = 20;
	tLinkInfo.iLickIndex = 19;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 21;
	tLinkInfo.iIconIndex1 = 22;
	tLinkInfo.iLickIndex = 20;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 22;
	tLinkInfo.iIconIndex1 = 24;
	tLinkInfo.iLickIndex = 21;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 24;
	tLinkInfo.iIconIndex1 = 25;
	tLinkInfo.iLickIndex = 22;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 25;
	tLinkInfo.iIconIndex1 = 26;
	tLinkInfo.iLickIndex = 23;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 26;
	tLinkInfo.iIconIndex1 = 27;
	tLinkInfo.iLickIndex = 24;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 27;
	tLinkInfo.iIconIndex1 = 28;
	tLinkInfo.iLickIndex = 25;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 26;
	tLinkInfo.iIconIndex1 = 29;
	tLinkInfo.iLickIndex = 26;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 22;
	tLinkInfo.iIconIndex1 = 23;
	tLinkInfo.iLickIndex = 27;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 22;
	tLinkInfo.iIconIndex1 = 30;
	tLinkInfo.iLickIndex = 28;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 23;
	tLinkInfo.iIconIndex1 = 31;
	tLinkInfo.iLickIndex = 29;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 31;
	tLinkInfo.iIconIndex1 = 32;
	tLinkInfo.iLickIndex = 30;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 32;
	tLinkInfo.iIconIndex1 = 33;
	tLinkInfo.iLickIndex = 31;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 33;
	tLinkInfo.iIconIndex1 = 34;
	tLinkInfo.iLickIndex = 32;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 35;
	tLinkInfo.iIconIndex1 = 34;
	tLinkInfo.iLickIndex = 33;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 34;
	tLinkInfo.iIconIndex1 = 36;
	tLinkInfo.iLickIndex = 34;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 32;
	tLinkInfo.iIconIndex1 = 37;
	tLinkInfo.iLickIndex = 35;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 37;
	tLinkInfo.iIconIndex1 = 38;
	tLinkInfo.iLickIndex = 36;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 39;
	tLinkInfo.iIconIndex1 = 40;
	tLinkInfo.iLickIndex = 37;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 40;
	tLinkInfo.iIconIndex1 = 41;
	tLinkInfo.iLickIndex = 38;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 41;
	tLinkInfo.iIconIndex1 = 42;
	tLinkInfo.iLickIndex = 39;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 42;
	tLinkInfo.iIconIndex1 = 43;
	tLinkInfo.iLickIndex = 40;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 42;
	tLinkInfo.iIconIndex1 = 44;
	tLinkInfo.iLickIndex = 41;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 44;
	tLinkInfo.iIconIndex1 = 45;
	tLinkInfo.iLickIndex = 42;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 39;
	tLinkInfo.iIconIndex1 = 46;
	tLinkInfo.iLickIndex = 43;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 46;
	tLinkInfo.iIconIndex1 = 48;
	tLinkInfo.iLickIndex = 44;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 46;
	tLinkInfo.iIconIndex1 = 47;
	tLinkInfo.iLickIndex = 45;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 51;
	tLinkInfo.iIconIndex1 = 48;
	tLinkInfo.iLickIndex = 46;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 51;
	tLinkInfo.iIconIndex1 = 52;
	tLinkInfo.iLickIndex = 47;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 49;
	tLinkInfo.iIconIndex1 = 50;
	tLinkInfo.iLickIndex = 48;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 50;
	tLinkInfo.iIconIndex1 = 53;
	tLinkInfo.iLickIndex = 49;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 49;
	tLinkInfo.iIconIndex1 = 54;
	tLinkInfo.iLickIndex = 50;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 54;
	tLinkInfo.iIconIndex1 = 55;
	tLinkInfo.iLickIndex = 51;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 55;
	tLinkInfo.iIconIndex1 = 56;
	tLinkInfo.iLickIndex = 52;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 55;
	tLinkInfo.iIconIndex1 = 57;
	tLinkInfo.iLickIndex = 53;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 49;
	tLinkInfo.iIconIndex1 = 58;
	tLinkInfo.iLickIndex = 54;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 58;
	tLinkInfo.iIconIndex1 = 59;
	tLinkInfo.iLickIndex = 55;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	// S 자 모양 Link
	tLinkInfo.bSLink = true;

	tLinkInfo.iIconIndex0 = 9;
	tLinkInfo.iIconIndex1 = 10;
	tLinkInfo.iLickIndex = 0;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 20;
	tLinkInfo.iIconIndex1 = 21;
	tLinkInfo.iLickIndex = 1;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 37;
	tLinkInfo.iIconIndex1 = 41;
	tLinkInfo.iLickIndex = 2;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 50;
	tLinkInfo.iIconIndex1 = 51;
	tLinkInfo.iLickIndex = 3;
	m_vecOnLinkInfo.push_back(tLinkInfo);

}

void CCanvas_BrainMap::OnIcon_Tick()
{
	// 자식 UI를 돌면서 변해야 할 것 을 확인해야 한다.
	for (size_t i = 0; i < m_vecIconUI.size(); ++i)
	{
		if (m_bSkillAcquisition == false)
		{
			// 아이콘 위에 마우스가 올라갔을 때
			if (true == m_vecIconUI[i]->Get_OnMouse())
			{
				m_iCurrentIndex = i;
				Find_ChildUI(L"Icon_Pick")->Set_Position(m_vecIconUI[i]->Get_Position());
				m_CurrentBrainInfo = m_vecIconUI[i]->Get_BrainInfo();
			}
		}

		if (false == m_vecIconUI[i]->Get_BrainInfo().bUse)
		{
			// 아이콘을 클릭 했을 때
			if (true == m_vecIconUI[i]->Get_OnButton())
			{
				IconPick(i);
				m_iCurrentIndex = i;
				m_vecIconUI[i]->Set_OnButton();
				dynamic_cast<CMain_BrainMapIconPickUI*>(Find_ChildUI(L"Icon_Pick"))->Set_Pick();
			}
		}
	}
}

void CCanvas_BrainMap::IconPick(const size_t iIndex)
{
	// 현재 구매할 수 있는 Level 이 되어야 살 수 있도록 한다.
	if (m_vecBrain[iIndex].vOnIconIndex != m_vecIconUI[iIndex]->Get_CurrentIconIndex())
	{
		m_bSkillAcquisition = true;
		m_szAlarmText = L"습득 조건을 충족하지 않습니다.";
		return;
	}

	// 현재 플레이어가 가지고 있는 BP를 확인해서 Icon 의 BP 보다 큰 경우에만 구매할 수 있다.
	if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().iBP >= m_vecIconUI[iIndex]->Get_BrainInfo().iBP)
	{			
		// 게이지가 가득 차야 아이템을 구매할 수 있다.
		dynamic_cast<CMain_BrainGaugeUI*>(Find_ChildUI(L"IconGauge_0"))->Set_ChargeGauge(true);
		dynamic_cast<CMain_BrainGaugeUI*>(Find_ChildUI(L"IconGauge_1"))->Set_ChargeGauge(true);
		Find_ChildUI(L"IconGauge_0")->Set_Position(m_vecIconUI[iIndex]->Get_Position());
		Find_ChildUI(L"IconGauge_1")->Set_Position(m_vecIconUI[iIndex]->Get_Position());

		if (true == dynamic_cast<CMain_BrainGaugeUI*>(Find_ChildUI(L"IconGauge_0"))->Get_GaugeFull())
		{
			m_bSkillAcquisition = true;
			m_szAlarmText = L"BP를 소모했습니다.";
			m_vecIconUI[iIndex]->Set_BrainUse();

			_uint iResultBP = CPlayerInfoManager::GetInstance()->Get_PlayerStat().iBP - m_vecIconUI[iIndex]->Get_BrainInfo().iBP;
			CPlayerInfoManager::GetInstance()->Set_BP(iResultBP);	 // 플레이어 BP 감소하기

			if (iIndex == 12 || iIndex == 18) 	// 플레이어 염력 레벨 증가
			{
				CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iKineticEnergyLevel += 1;
				dynamic_cast<CCanvas_PlayerInfoMove*>(CUI_Manager::GetInstance()->Find_MoveCanvas(L"Canvas_PlayerInfoMove"))->Set_PsychokinesisType();
			}

			// 레벨에 따른 아이콘 구매할 수 있는 정도
			for (_int j = 0; j < 3; ++j)
			{
				if (-1 == m_vecIconUI[iIndex]->Get_BrainInfo().arrNeighbor[j])
					break;

				m_vecIconUI[m_vecIconUI[iIndex]->Get_BrainInfo().arrNeighbor[j]]->Set_OnIcon();
			}
			return;
		}
	}
	else
	{
		if (true == m_vecIconUI[iIndex]->Get_BrainInfo().bUse)
			return;

		m_bSkillAcquisition = true;
		m_szAlarmText = L"BP가 부족합니다.";
		return;
	}
}

void CCanvas_BrainMap::SkillAcquisition_Tick()
{
	Find_ChildUI(L"SkillAcquisition")->SetVisible(m_bSkillAcquisition);
	Find_ChildUI(L"SkillAcquisitionBorderLine")->SetVisible(m_bSkillAcquisition);
	Find_ChildUI(L"SkillAcquisitionBorderLine_B")->SetVisible(m_bSkillAcquisition);
	Find_ChildUI(L"SkillAcquisition_BackGround")->SetVisible(m_bSkillAcquisition);
	Find_ChildUI(L"SkillAcquisition_Button")->SetVisible(m_bSkillAcquisition);
	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"SkillAcquisition_Button"))->Set_ColorType(2);

	if (false == m_bSkillAcquisition)
		return;

	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"SkillAcquisition_Button"))->Get_OnButton() ||
		CGameInstance::GetInstance()->KeyDown(DIK_RETURN))
	{
		m_bSkillAcquisition = false;
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"SkillAcquisition_Button"))->Set_OnButton();

		dynamic_cast<CMain_BrainGaugeUI*>(Find_ChildUI(L"IconGauge_0"))->Set_ChargeGauge(false);
		dynamic_cast<CMain_BrainGaugeUI*>(Find_ChildUI(L"IconGauge_1"))->Set_ChargeGauge(false);
		return;
	}
}

void CCanvas_BrainMap::OnLick_Tick()
{
	for (size_t i = 0; i < m_vecOnLinkInfo.size(); ++i)
	{
		size_t iIconIndex0 = m_vecOnLinkInfo[i].iIconIndex0;
		size_t iIconIndex1 = m_vecOnLinkInfo[i].iIconIndex1;

		_bool	bIconUse0 = m_vecIconUI[iIconIndex0]->Get_BrainInfo().bUse;
		_bool	bIconUse1 = m_vecIconUI[iIconIndex1]->Get_BrainInfo().bUse;

		if (true == bIconUse0 && true == bIconUse1)
		{
			_tchar szText[MAX_PATH] = TEXT("");
			if (true == m_vecOnLinkInfo[i].bSLink)
			{
				wsprintf(szText, TEXT("Link__S_0%d"), static_cast<_int>(m_vecOnLinkInfo[i].iLickIndex));
			}
			else
			{
				if (10 > m_vecOnLinkInfo[i].iLickIndex)
					wsprintf(szText, TEXT("Link__0%d"), static_cast<_int>(m_vecOnLinkInfo[i].iLickIndex));
				else
					wsprintf(szText, TEXT("Link__%d"), static_cast<_int>(m_vecOnLinkInfo[i].iLickIndex));
			}
			Find_ChildUI(szText)->SetVisible(true);
		}
	}
}

CCanvas_BrainMap * CCanvas_BrainMap::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_BrainMap*		pInstance = new CCanvas_BrainMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_BrainMap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_BrainMap::Clone(void * pArg)
{
	CCanvas_BrainMap*		pInstance = new CCanvas_BrainMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_BrainMap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_BrainMap::Free()
{
	CCanvas::Free();

	m_vecBrain.clear();
	m_vecIconUI.clear();
	m_vecOnLinkInfo.clear();
}
