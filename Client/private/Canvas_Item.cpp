#include "stdafx.h"
#include "..\public\Canvas_Item.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "PlayerInfoManager.h"
#include "MathUtils.h"
#include "VFX_Manager.h"
#include "EffectSystem.h"
#include "GameManager.h"

#include "Canvas_ItemMove.h"
#include "Canvas_PlayerInfoMove.h"
#include "Canvas_SASInfoRightMove.h"
#include "Canvas_SASInfoLeftMove.h"
#include "Item_IconUI.h"
#include "Item_LightUI.h"
#include "Item_NameUI.h"
#include "ItemIconUI.h"
#include "Player.h"

CCanvas_Item::CCanvas_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_Item::CCanvas_Item(const CCanvas_Item& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_Item::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_Item::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_Canvas(L"Canvas_Item", this);
	m_bVisible = true;
	m_bInput = true;

	return S_OK;
}

void CCanvas_Item::BeginTick()
{
	__super::BeginTick();

}

void CCanvas_Item::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	BattleItem_Choice();
	PlayerHpManagement_Tick();
}

HRESULT CCanvas_Item::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	if (m_bBattleItmeEmpty == true) return S_OK;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_tchar szText[MAX_PATH] = TEXT("");
	_float2 vFontSize = { 0.5f, 0.5f };
	_float2	vPosition = Find_ChildUI(L"Item_BackGround")->GetScreenSpaceLeftTop();

	m_vecBattleItme= CItem_Manager::GetInstance()->Get_BattleItem();
	_int iTtemSize = static_cast<_int>(m_vecBattleItme.size());
	if (iTtemSize == 1)
	{
		_int iCurrentItemCount = m_vecBattleItme[wrap(m_iCurrentIndex, 0, iTtemSize)].iCount;
		if (10 <= iCurrentItemCount)
			wsprintf(szText, TEXT("%d"), iCurrentItemCount);
		else
			wsprintf(szText, TEXT("0%d"), iCurrentItemCount);

		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(139.0f, 144.0f), 0.f, vFontSize);
	}
	else if (iTtemSize == 2)
	{
		_int iCurrentItemCount = m_vecBattleItme[wrap(m_iCurrentIndex, 0, iTtemSize)].iCount;
		if (10 <= iCurrentItemCount)
			wsprintf(szText, TEXT("%d"), iCurrentItemCount);
		else
			wsprintf(szText, TEXT("0%d"), iCurrentItemCount);

		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(139.0f, 144.0f), 0.f, vFontSize);

		_int iRightCount = m_vecBattleItme[wrap(m_iCurrentIndex - 1, 0, iTtemSize)].iCount;
		if (10 <= iRightCount)
			wsprintf(szText, TEXT("%d"), iRightCount);
		else
			wsprintf(szText, TEXT("0%d"), iRightCount);

		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(212.0f, 137.0f), 0.f, vFontSize);
	}
	else if (iTtemSize >= 3)
	{
		_int iCurrentItemCount = m_vecBattleItme[wrap(m_iCurrentIndex, 0, iTtemSize)].iCount;
		if (10 <= iCurrentItemCount)
			wsprintf(szText, TEXT("%d"), iCurrentItemCount);
		else
			wsprintf(szText, TEXT("0%d"), iCurrentItemCount);

		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(139.0f, 144.0f), 0.f, vFontSize);

		_int iRightCount = m_vecBattleItme[wrap(m_iCurrentIndex - 1, 0, iTtemSize)].iCount;
		if (10 <= iRightCount)
			wsprintf(szText, TEXT("%d"), iRightCount);
		else
			wsprintf(szText, TEXT("0%d"), iRightCount);

		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(212.0f, 137.0f), 0.f, vFontSize);

		_int iLeftCount = m_vecBattleItme[wrap(m_iCurrentIndex + 1, 0, iTtemSize)].iCount;
		if (10 <= iLeftCount)
			wsprintf(szText, TEXT("%d"), iLeftCount);
		else
			wsprintf(szText, TEXT("0%d"), iLeftCount);

		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(50.0f, 143.0f), 0.f, vFontSize);
	}

	return S_OK;
}

void CCanvas_Item::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	if (ImGui::Button("Jelly5"))
	{
		CItem_Manager::GetInstance()->Set_ItemCount(L"회복(소) 젤리", 5);
	}

	if (ImGui::Button("Jelly-1"))
	{
		CItem_Manager::GetInstance()->Set_ItemCount(L"회복(소) 젤리", -1);
	}

	if (ImGui::Button("test"))
	{
		CItem_Manager::GetInstance()->Set_ItemCount(L"SAS 보급수", 1);
	}
	
	if (ImGui::Button("test2"))
	{
		CItem_Manager::GetInstance()->Set_ItemCount(L"전체: 회복(소) 젤리", 1);
	}

	if (ImGui::Button("test4"))
	{
		CItem_Manager::GetInstance()->Set_ItemCount(L"회복(중) 젤리", 3);
	}
}

void CCanvas_Item::Set_ItmeUse()
{
	if (m_vecBattleItme.empty()) return;

	dynamic_cast<CItem_LightUI*>(Find_ChildUI(L"Item_Light"))->Set_ItemLightUse();
	dynamic_cast<CItem_IconUI*>(Find_ChildUI(L"Item_Icon"))->Set_ItemIconUse();

	m_vecBattleItme = CItem_Manager::GetInstance()->Get_BattleItem();
	wstring wsItemName = m_vecBattleItme[wrap(m_iCurrentIndex, 0, m_vecBattleItme.size())].wsName;
	CItem_Manager::GetInstance()->Set_ItemCount(wsItemName, -1);

	if (wsItemName == L"회복(소) 젤리")
	{
		PlayerHpSet(0.3f);
		return;
	}
	else if (wsItemName == L"회복(중) 젤리")
	{
		PlayerHpSet(0.6f);
		return;
	}
	else if (wsItemName == L"회복(대) 젤리")
	{
		PlayerHpSet(1.0f);
		return;
	}
	else if (wsItemName == L"전체: 회복(소) 젤리")
	{
		PlayerHpSet(0.3f);
		HanabiHpSet(0.3f);
		TsugumiSet(0.3f);
		return;
	}
	else if (wsItemName == L"전체: 회복(중) 젤리")
	{
		PlayerHpSet(0.6f);
		HanabiHpSet(0.6f);
		TsugumiSet(0.6f);
		return;
	}
	else if (wsItemName == L"전체: 회복(대) 젤리")
	{
		PlayerHpSet(1.0f);
		HanabiHpSet(1.0f);
		TsugumiSet(1.0f);
		return;
	}
	else if (wsItemName == L"SAS 보급수")
	{
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[0].Energy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[0].MaxEnergy;
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[1].Energy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[1].MaxEnergy;
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[2].Energy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[2].MaxEnergy;
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[3].Energy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[3].MaxEnergy;
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[4].Energy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[4].MaxEnergy;
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[5].Energy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[5].MaxEnergy;

		return;
	}
}

void CCanvas_Item::Set_IconLight(const _bool	bVisble)
{
	Find_ChildUI(L"Item_Light")->SetVisible(bVisble);
}

void CCanvas_Item::Set_LightReset()
{
	dynamic_cast<CItem_LightUI*>(Find_ChildUI(L"Item_Light"))->Set_LightReset();
}

void CCanvas_Item::BattleItem_Choice()
{
	m_vecBattleItme= CItem_Manager::GetInstance()->Get_BattleItem();
	
	if (m_vecBattleItme.empty())	m_bBattleItmeEmpty = true;
	else m_bBattleItmeEmpty = false;

	if (true == m_bBattleItmeEmpty || false == m_bFullGauge) return;

	if (CGameInstance::GetInstance()->KeyDown(DIK_RIGHT))
	{
		CUI_Manager::GetInstance()->PlaySound("ItemSwitch");
		++m_iCurrentIndex;
		m_bInput = true;

	}

	if (CGameInstance::GetInstance()->KeyDown(DIK_LEFT))
	{
		CUI_Manager::GetInstance()->PlaySound("ItemSwitch");
		--m_iCurrentIndex;
		m_bInput = true;
	}

	if (m_bInput == true)
	{
		m_bInput = false;

		_int iTtemSize = static_cast<_int>(m_vecBattleItme.size());
		dynamic_cast<CItem_LightUI*>(Find_ChildUI(L"Item_Light"))->Set_LightColor(LightColorIndex(wrap(m_iCurrentIndex, 0, iTtemSize)));
		dynamic_cast<CItem_NameUI*>(Find_ChildUI(L"Item_Name"))->Set_ItemName(NameIndex(wrap(m_iCurrentIndex, 0, iTtemSize)));

		if (iTtemSize == 1)
		{
			dynamic_cast<CItem_IconUI*>(Find_ChildUI(L"Item_Icon"))->Set_IconIndex({ m_vecBattleItme[0].vIconIndex });
			dynamic_cast<CItemIconUI*>(Find_ChildUI(L"Item_IconLeft"))->Set_IconIndex({ 0.0f, 0.0f });
			dynamic_cast<CItemIconUI*>(Find_ChildUI(L"Item_IconRight"))->Set_IconIndex({ 0.0f, 0.0f });
		}
		else if (iTtemSize == 2)
		{
			dynamic_cast<CItem_IconUI*>(Find_ChildUI(L"Item_Icon"))->Set_IconIndex({ m_vecBattleItme[wrap(m_iCurrentIndex, 0, iTtemSize)].vIconIndex });
			dynamic_cast<CItemIconUI*>(Find_ChildUI(L"Item_IconRight"))->Set_IconIndex({ m_vecBattleItme[wrap(m_iCurrentIndex + 1, 0, iTtemSize)].vIconIndex });
			dynamic_cast<CItemIconUI*>(Find_ChildUI(L"Item_IconLeft"))->Set_IconIndex({ 0.0f, 0.0f });
		}
		else if (iTtemSize >= 3)
		{
			dynamic_cast<CItem_IconUI*>(Find_ChildUI(L"Item_Icon"))->Set_IconIndex({ m_vecBattleItme[wrap(m_iCurrentIndex, 0, iTtemSize)].vIconIndex });
			dynamic_cast<CItemIconUI*>(Find_ChildUI(L"Item_IconRight"))->Set_IconIndex({ m_vecBattleItme[wrap(m_iCurrentIndex + 1, 0, iTtemSize)].vIconIndex });
			dynamic_cast<CItemIconUI*>(Find_ChildUI(L"Item_IconLeft"))->Set_IconIndex({ m_vecBattleItme[wrap(m_iCurrentIndex - 1, 0, iTtemSize)].vIconIndex });
		}

		dynamic_cast<CCanvas_ItemMove*>(CUI_Manager::GetInstance()->Find_MoveCanvas(L"Canvas_ItemMove"))->Set_GaugeColor(ColorIndex(false));
		dynamic_cast<CCanvas_ItemMove*>(CUI_Manager::GetInstance()->Find_MoveCanvas(L"Canvas_ItemMove"))->Set_BackGaugeColor(ColorIndex(true));
	}
}

_float CCanvas_Item::NameIndex(const size_t iIndex)
{
	if (m_bBattleItmeEmpty == true) return 0.0f;

	m_vecBattleItme= CItem_Manager::GetInstance()->Get_BattleItem();
	wstring wsBattleItmeName = m_vecBattleItme[iIndex].wsName;

	if (wsBattleItmeName == L"회복(소) 젤리")
		return 1.0f;
	else if (wsBattleItmeName == L"회복(중) 젤리")
		return 2.0f;
	else if (wsBattleItmeName == L"회복(대) 젤리")
		return 3.0f;
	else if (wsBattleItmeName == L"전체: 회복(소) 젤리")
		return 4.0f;
	else if (wsBattleItmeName == L"전체: 회복(중) 젤리")
		return 5.0f;
	else if (wsBattleItmeName == L"전체: 회복(대) 젤리")
		return 6.0f;
	else if (wsBattleItmeName == L"정상화 태블릿")
		return 7.0f;
	else if (wsBattleItmeName == L"전체: 정상화 태블릿")
		return 8.0f;
	else if (wsBattleItmeName == L"브레인 필드 기어")
		return 9.0f;
	else if (wsBattleItmeName == L"SAS 보급수")
		return 10.0f;
	else
		return 0.0f;

	return 0.0f;
}

_float3 CCanvas_Item::LightColorIndex(const size_t iIndex)
{
	if (m_bBattleItmeEmpty == true) return { 0.0f, 0.0f, 0.0f };

	m_vecBattleItme= CItem_Manager::GetInstance()->Get_BattleItem();
	wstring wsBattleItmeName = m_vecBattleItme[iIndex].wsName;

	if (wsBattleItmeName == L"회복(소) 젤리" || wsBattleItmeName == L"회복(중) 젤리" || wsBattleItmeName == L"회복(대) 젤리" ||
		wsBattleItmeName == L"전체: 회복(소) 젤리" || wsBattleItmeName == L"전체: 회복(중) 젤리" || wsBattleItmeName == L"전체: 회복(대) 젤리")
		return { 0.4f, 1.0f, 0.4f };
	else if (wsBattleItmeName == L"정상화 태블릿" || wsBattleItmeName == L"전체: 정상화 태블릿")
		return  { 0.34f, 0.897f, 1.0f };
	else if (wsBattleItmeName == L"브레인 필드 기어")
		return  { 1.0f, 0.34f, 1.0f };
	else if (wsBattleItmeName == L"SAS 보급수")
		return  { 1.0f, 0.8f, 0.3f };

	return { 0.0f, 0.0f, 0.0f };
}

_float4 CCanvas_Item::ColorIndex(const _bool	bBack)
{
	if (m_bBattleItmeEmpty == true || m_vecBattleItme.empty()) return { 0.0f, 0.0f, 0.0f, 0.0f };

	// Canvas_ItemMove 에서 사용하기 위해서 Move 에서는 m_CurrentIndex 을 모르기 때문에 그냥 구해서 넘겨주자
	m_vecBattleItme= CItem_Manager::GetInstance()->Get_BattleItem();
	_int iReault = wrap(m_iCurrentIndex, 0, m_vecBattleItme.size());
	wstring wsBattleItmeName = m_vecBattleItme[iReault].wsName;

	// 게이지지 색상이 2개 존재하는데 bBack가 true 라면 뒤에 있는 게이지를 뜻한다.
	if (bBack == true)
	{
		if (wsBattleItmeName == L"회복(소) 젤리" || wsBattleItmeName == L"회복(중) 젤리" || wsBattleItmeName == L"회복(대) 젤리" ||
			wsBattleItmeName == L"전체: 회복(소) 젤리" || wsBattleItmeName == L"전체: 회복(중) 젤리" || wsBattleItmeName == L"전체: 회복(대) 젤리")
			return { 0.5f, 1.0f, 0.4f, 1.0f };
		else if (wsBattleItmeName == L"정상화 태블릿" || wsBattleItmeName == L"전체: 정상화 태블릿")
			return  { 0.612f, 1.0f, 1.0f, 1.0f };
		else if (wsBattleItmeName == L"브레인 필드 기어")
			return  { 1.0f, 0.7f, 1.0f, 1.0f };
		else if (wsBattleItmeName == L"SAS 보급수")
			return  { 1.0f, 0.8f, 0.7f, 1.0f };

		return { 1.0f, 1.0f, 1.0f, 1.0f };
	}

	if (wsBattleItmeName == L"회복(소) 젤리" || wsBattleItmeName == L"회복(중) 젤리" || wsBattleItmeName == L"회복(대) 젤리" ||
		wsBattleItmeName == L"전체: 회복(소) 젤리" || wsBattleItmeName == L"전체: 회복(중) 젤리" || wsBattleItmeName == L"전체: 회복(대) 젤리")
		return { 0.6f, 1.0f, 0.6f, 1.0f };
	else if (wsBattleItmeName == L"정상화 태블릿" || wsBattleItmeName == L"전체: 정상화 태블릿")
		return  { 0.635f, 0.866, 1.0f, 1.0f };
	else if (wsBattleItmeName == L"브레인 필드 기어")
		return  { 0.918f, 0.547f, 1.0f, 1.0f };
	else if (wsBattleItmeName == L"SAS 보급수")
		return  { 1.0f, 0.9f, 0.5f, 1.0f };

	return{ 1.0f, 1.0f, 1.0f, 1.0f };
}

_int CCanvas_Item::wrap(_int x, _int low, _int high)
{
	if (0 == low && 0 == high) return 0;

	assert(low < high);
	const int n = (x - low) % (high - low);
	return (n >= 0) ? (n + low) : (n + high);
}

void CCanvas_Item::PlayerFontHp(_uint iHp)
{
	if (nullptr == m_pPlayer) return;

	array<_int, 4> SaveNum;

	//10을 넣어주면 숫자가 안뜬다고 함
	SaveNum.fill(10);

	_int iCount = 0;
	while (iHp != 0)
	{
		SaveNum[iCount++] = iHp % 10;
		iHp = iHp / 10;
	}

	_float4 vHitPosition = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);

	vHitPosition.x += CMathUtils::RandomFloat(-0.25f, 0.25f);
	vHitPosition.y += CMathUtils::RandomFloat(-1.2f, 1.5f);

	CEffectGroup* pFont = CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_UI, L"HillNum");
	pFont->Start_AttachOnlyPos(vHitPosition);

	pFont->GetFirstEffect()->GetParams().Float2s[0] = { _float(SaveNum[3]), 0.0f };
	pFont->GetSecondEffect()->GetParams().Float2s[0] = { _float(SaveNum[2]), 0.0f };
	pFont->GetThirdEffect()->GetParams().Float2s[0] = { _float(SaveNum[1]), 0.0f };
	pFont->GetFourthEffect()->GetParams().Float2s[0] = { _float(SaveNum[0]), 0.0f };
}

void CCanvas_Item::PlayerHpSet(const _float & fPercentage)
{
	_uint iHp = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iHP;
	_uint iMaxHp = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iMaxHP;
	
	_int iAddHp = 0;
	if(1.0 == fPercentage || iHp * 0.2f >= iHp) iAddHp = iMaxHp;
	else iAddHp = static_cast<_int>(static_cast<_float>(iMaxHp) * fPercentage);

	if (iHp == iMaxHp) return;

	if (iMaxHp < iHp + iAddHp)
	{
		_uint iResult = iMaxHp - iHp;
		PlayerFontHp(iResult);
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iHP = iMaxHp;
	}
	else
	{
		PlayerFontHp(iAddHp);
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iHP += iAddHp;
	}

	// Front Hp Bar 에게 전달
	dynamic_cast<CCanvas_PlayerInfoMove*>(CUI_Manager::GetInstance()->Find_MoveCanvas(L"Canvas_PlayerInfoMove"))->Set_HillBar();
}

void CCanvas_Item::HanabiHpSet(const _float& fPercentage)
{
	_uint iHp = CPlayerInfoManager::GetInstance()->Get_HanabiStat().iHP;
	_uint iMaxHp = CPlayerInfoManager::GetInstance()->Get_HanabiStat().iMaxHP;

	_int iAddHp = 0;
	if (1.0 == fPercentage || iHp * 0.2f >= iHp) iAddHp = iMaxHp;
	else iAddHp = static_cast<_int>(static_cast<_float>(iMaxHp) * fPercentage);

	if (iHp == iMaxHp) return;

	if (iMaxHp < iHp + iAddHp)
	{
		_uint iResult = iMaxHp - iHp;
		CPlayerInfoManager::GetInstance()->Get_HanabiStat().iHP = iMaxHp;
	}
	else
	{
		CPlayerInfoManager::GetInstance()->Get_HanabiStat().iHP += iAddHp;
	}

	// Front Hp Bar 에게 전달
	dynamic_cast<CCanvas_SASInfoRightMove*>(CUI_Manager::GetInstance()->Find_MoveCanvas(L"Canvas_SASInfoRightMove"))->Set_HillBar();
}

void CCanvas_Item::TsugumiSet(const _float& fPercentage)
{
	_uint iHp = CPlayerInfoManager::GetInstance()->Get_TsugumiStat().iHP;
	_uint iMaxHp = CPlayerInfoManager::GetInstance()->Get_TsugumiStat().iMaxHP;

	_int iAddHp = 0;
	if (1.0 == fPercentage || iHp * 0.2f >= iHp) iAddHp = iMaxHp;
	else iAddHp = static_cast<_int>(static_cast<_float>(iMaxHp) * fPercentage);

	if (iHp == iMaxHp) return;

	if (iMaxHp < iHp + iAddHp)
	{
		_uint iResult = iMaxHp - iHp;
		CPlayerInfoManager::GetInstance()->Get_TsugumiStat().iHP = iMaxHp;
	}
	else
	{
		CPlayerInfoManager::GetInstance()->Get_TsugumiStat().iHP += iAddHp;
	}

	// Front Hp Bar 에게 전달
	dynamic_cast<CCanvas_SASInfoLeftMove*>(CUI_Manager::GetInstance()->Find_MoveCanvas(L"Canvas_SASInfoLeftMove"))->Set_HillBar();
}

void CCanvas_Item::PlayerHpManagement_Tick()
{
	// 플레이어 체력을 게속 확인하면서 부족한 20% 이하로 떨어졌을 경우 체력을 회복시켜준다.
	_uint iHp = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iHP;
	_uint iMaxHp = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iMaxHP;

	if (iHp <= iMaxHp * 0.1)
	{
		PlayerHpSet(1.0f);
		CGameManager::GetInstance()->Set_LeftTalk(102);	// 추가로 왼쪽에 BrainTalk 를 띄운다.
	}

	iHp = CPlayerInfoManager::GetInstance()->Get_HanabiStat().iHP;
	iMaxHp = CPlayerInfoManager::GetInstance()->Get_HanabiStat().iMaxHP;

	if (iHp <= iMaxHp * 0.2)
	{
		HanabiHpSet(1.0f);
	}

	iHp = CPlayerInfoManager::GetInstance()->Get_TsugumiStat().iHP;
	iMaxHp = CPlayerInfoManager::GetInstance()->Get_TsugumiStat().iMaxHP;

	if (iHp <= iMaxHp * 0.2)
	{
		TsugumiSet(1.0f);
	}
}

CCanvas_Item * CCanvas_Item::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_Item*		pInstance = new CCanvas_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_Item");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_Item::Clone(void * pArg)
{
	CCanvas_Item*		pInstance = new CCanvas_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_Item");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_Item::Free()
{
	CCanvas::Free();
}
