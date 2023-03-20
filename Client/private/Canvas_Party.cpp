#include "stdafx.h"
#include "..\public\Canvas_Party.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "PlayerInfoManager.h"

#include "DefaultUI.h"
#include "Main_PickUI.h"

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

	return S_OK;
}

void CCanvas_Party::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);

	PickInfo();
}

void CCanvas_Party::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_Party::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_float4 vColor = { 1.0f, 0.99f, 0.87f, 1.0f };
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// 오른쪽 상단 배틀 멤버
	_float2 vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Party_BattleMembers_B"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"배틀 멤버의 SAS", vPosition + _float2(m_vPosition), 0.f, { 0.4f, 0.4f }, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"배틀 멤버의 SAS는 효과 시간이 길어지며 /n SAS 게이지 회복 속도도 빨라진다.",
		vPosition + _float2(40.0f, 99.0f), 0.f, { 0.4f, 0.4f }, vColor);

	_tchar szText[MAX_PATH] = TEXT("");
	_float2 vFontBigSize = { 0.5f, 0.5f };
	_float2 vFontSmaillSize = { 0.3f, 0.3f };

	// 오른쪽 하단 정보
	vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Party_BasicInfo_B"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"유이토 스메라기", vPosition + _float2(40.0f, 99.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"WSY567369", vPosition + _float2(40.0f, 99.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"레벨", vPosition + _float2(40.0f, 99.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"앞으로", vPosition + _float2(40.0f, 99.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"체력", vPosition + _float2(40.0f, 99.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"초뇌능력", vPosition + _float2(40.0f, 99.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"공격력", vPosition + _float2(40.0f, 99.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"방어력", vPosition + _float2(40.0f, 99.0f), 0.f, vFontSmaillSize, vColor);

	// -------------------------------------------------------------------
	vFontBigSize = { 0.35f, 0.35f };
	vFontSmaillSize = { 0.2f, 0.2f };

	// 유이토
	vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_01_Leader_B"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"유이토 스메라기", vPosition + _float2(40.0f, 99.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"레벨", vPosition + _float2(40.0f, 99.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"체력", vPosition + _float2(40.0f, 99.0f), 0.f, vFontSmaillSize, vColor);

	PLAYER_STAT tPlayerStat = CPlayerInfoManager::GetInstance()->Get_PlayerStat();
	wsprintf(szText, TEXT("%d"), tPlayerStat.iLevel);
	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(-100.0f, 61.0f), 0.f, vFontBigSize, vColor);
	wsprintf(szText, TEXT("%d / %d"), tPlayerStat.m_iHP, tPlayerStat.m_iMaxHP);
	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(-100.0f, 61.0f), 0.f, vFontSmaillSize, vColor);

	// 하아비
	vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_02_Member_B"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"하나비 이치조", vPosition + _float2(40.0f, 99.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"레벨", vPosition + _float2(40.0f, 99.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"체력", vPosition + _float2(40.0f, 99.0f), 0.f, vFontSmaillSize, vColor);

	HANABI_STAT tHanabiStat = CPlayerInfoManager::GetInstance()->Get_HanabiStat();
	wsprintf(szText, TEXT("%d"), tHanabiStat.iLevel);
	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(-100.0f, 61.0f), 0.f, vFontBigSize, vColor);
	wsprintf(szText, TEXT("%d / %d"), tHanabiStat.iHP, tHanabiStat.iMaxHP);
	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(-100.0f, 61.0f), 0.f, vFontSmaillSize, vColor);

	// 츠구미
	vPosition = dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_03_Member_B"))->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"츠구미 나자르", vPosition + _float2(40.0f, 99.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"레벨", vPosition + _float2(40.0f, 99.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"체력", vPosition + _float2(40.0f, 99.0f), 0.f, vFontSmaillSize, vColor);

	TSUGUMI_STAT tTsugumiStat = CPlayerInfoManager::GetInstance()->Get_TsugumiStat();
	wsprintf(szText, TEXT("%d"), tTsugumiStat.iLevel);
	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(-100.0f, 61.0f), 0.f, vFontBigSize, vColor);
	wsprintf(szText, TEXT("%d / %d"), tTsugumiStat.iHP, tTsugumiStat.iMaxHP);
	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(-100.0f, 61.0f), 0.f, vFontSmaillSize, vColor);







	//_tchar szChildTag[MAX_PATH] = TEXT("");
	//wsprintf(szChildTag, TEXT("%d"), _int(fPlayerHp.x));
	//CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szChildTag, vPosition + _float2(-100.0f, 61.0f), 0.f, { 0.55f, 0.55f }, { 1.0f, 0.99f, 0.87f, 1.0f });

	return S_OK;
}

void CCanvas_Party::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	ImGui::DragFloat("X", &m_vPosition.x);
	ImGui::DragFloat("Y", &m_vPosition.y);
}

void CCanvas_Party::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_Party::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

void CCanvas_Party::PickInfo()
{
	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_01_Leader_B"))->Get_OnButton())
	{
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"Party_01_Leader_B"))->Set_OnButton();



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
