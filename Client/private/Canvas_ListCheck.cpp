#include "stdafx.h"
#include "..\public\Canvas_ListCheck.h"
#include "GameInstance.h"
#include "PlayerInfoManager.h"
#include "Item_Manager.h"

#include "Tutorial_YesNoUI.h"
#include "ShaderUI.h"

CCanvas_ListCheck::CCanvas_ListCheck(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_ListCheck::CCanvas_ListCheck(const CCanvas_ListCheck& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_ListCheck::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_ListCheck::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	return S_OK;
}

void CCanvas_ListCheck::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
	{
		if (m_bVisible == true)
		{
			if ((*iter).first == L"Purchase_Window_Color" ||
				(*iter).first == L"Tutorial_YesBox" ||
				(*iter).first == L"Tutorial_NoBox" ||
				(*iter).first == L"Transferring_Color" ||
				(*iter).first == L"ListCheck_Icon" ||
				(*iter).first == L"ListCheck_Bar"	)
				continue;
		}

		iter->second->SetVisible(m_bVisible);
	}

	if (m_bVisible == false) return;

	CheckIcon_Tick();
	KeyInput_Yes();
	KeyInput_No();
	TransferringBar_Tick(TimeDelta);
}

void CCanvas_ListCheck::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_ListCheck::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;
	
	_tchar szText[MAX_PATH] = TEXT("");
	_float2 vFontBigSize = { 0.5f, 0.5f };
	_float2 vFontSmaillSize = { 0.4f, 0.4f };
	_float4 vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	vector<pair<wstring, CItem_Manager::ITEMINFO>> vecItemInfo = CItem_Manager::GetInstance()->Get_ItmeInfo();

	_float2 vPosition = Find_ChildUI(L"Purchase_Window")->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"구입하시겠습니까?", vPosition + _float2(145.0f, 27.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"예", vPosition + _float2(212.0f, 95.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"아니오", vPosition + _float2(195.0f, 133.0f), 0.f, vFontBigSize, vColor);

	if (m_bTransferring == true) {
		if (1.0f < m_fTransferringBar)
			pGameInstance->Render_Font(L"Pretendard32", L"전송 완료", vPosition + _float2(195.0f, 245.0f), 0.f, vFontSmaillSize, vColor);
		else
			pGameInstance->Render_Font(L"Pretendard32", L"전송 중", vPosition + _float2(205.0f, 245.0f), 0.f, vFontSmaillSize, vColor);
		return S_OK;
	}

	pGameInstance->Render_Font(L"Pretendard32", L"소지금", vPosition + _float2(55.0f, 193.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"구입 후 소지금", vPosition + _float2(55.0f, 219.0f), 0.f, vFontSmaillSize, vColor);

	wsprintf(szText, TEXT("%u"), CPlayerInfoManager::GetInstance()->Get_PlayerStat().iCoin);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(340.0f, 193.0f), 0.f, vFontSmaillSize, vColor);

	_int iResult = static_cast<_int>(CPlayerInfoManager::GetInstance()->Get_PlayerStat().iCoin) - (vecItemInfo[m_iItemIndex].second.iPrice * m_iItemCount);
	if (0 >= iResult)
		vColor = { 1.0f, 0.458f, 0.38f, 1.0f };
	else
		vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	wsprintf(szText, TEXT("%d"), iResult);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(340.0f, 219.0f), 0.f, vFontSmaillSize, vColor);

	vColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	_uint iItemValueResult = vecItemInfo[m_iItemIndex].second.iPrice * m_iItemCount;
	wsprintf(szText, TEXT("%d"), iItemValueResult);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(340.0f, 249.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"구입 금액", vPosition + _float2(55.0f, 249.0f), 0.f, vFontSmaillSize, vColor);

	return S_OK;
}

void CCanvas_ListCheck::Set_ItemInfo(const size_t iItemIndex, const _uint iItemCount)
{
	m_bTransferring = false;
	m_iItemIndex = iItemIndex;
	m_iItemCount = iItemCount;

	Find_ChildUI(L"Tutorial_YesBox")->SetVisible(true);
	Find_ChildUI(L"Purchase_Window_Color")->SetVisible(true);
	Find_ChildUI(L"Tutorial_Icon0")->Set_Position(_float2(-48.0f, 51.0f));
	Find_ChildUI(L"Tutorial_Icon1")->Set_Position(_float2(-49.0f, 51.0f));
	Find_ChildUI(L"Tutorial_Icon2")->Set_Position(_float2(-47.0f, 51.0f));
}

void CCanvas_ListCheck::CheckIcon_Tick()
{
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

void CCanvas_ListCheck::KeyInput_Yes()
{
	if (true == dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_YesBox"))->Get_InputYes())
	{
		m_iNoCount = 0;
		++m_iYesCount;

		if (2 == m_iYesCount)
		{
			dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon0"))->Set_ZeroShader();
			m_iYesCount = 0;
			Purchase();
		}

		Find_ChildUI(L"Tutorial_YesBox")->SetVisible(true);
		Find_ChildUI(L"Tutorial_NoBox")->SetVisible(false);

		Find_ChildUI(L"Tutorial_Icon0")->Set_Position(_float2(-48.0f, 51.0f));
		Find_ChildUI(L"Tutorial_Icon1")->Set_Position(_float2(-49.0f, 51.0f));
		Find_ChildUI(L"Tutorial_Icon2")->Set_Position(_float2(-47.0f, 51.0f));
	}
}

void CCanvas_ListCheck::KeyInput_No()
{
	if (true == dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_NoBox"))->Get_InputNo())
	{
		m_iYesCount = 0;
		++m_iNoCount;

		if (2 == m_iNoCount)
		{
			dynamic_cast<CTutorial_YesNoUI*>(Find_ChildUI(L"Tutorial_Icon0"))->Set_ZeroShader();
			m_iNoCount = 0;
			m_bVisible = false;
		}

		Find_ChildUI(L"Tutorial_YesBox")->SetVisible(false);
		Find_ChildUI(L"Tutorial_NoBox")->SetVisible(true);

		Find_ChildUI(L"Tutorial_Icon0")->Set_Position(_float2(-48.0f, 15.0f));
		Find_ChildUI(L"Tutorial_Icon1")->Set_Position(_float2(-49.0f, 15.0f));
		Find_ChildUI(L"Tutorial_Icon2")->Set_Position(_float2(-47.0f, 15.0f));
	}
}

void CCanvas_ListCheck::Purchase()
{
	m_bTransferring = true;
	Find_ChildUI(L"Purchase_Window_Color")->SetVisible(false);
	
	Find_ChildUI(L"Transferring_Color")->SetVisible(true);
	Find_ChildUI(L"ListCheck_Icon")->SetVisible(true);
	Find_ChildUI(L"ListCheck_Bar")->SetVisible(true);
}

void CCanvas_ListCheck::TransferringBar_Tick(const _double& TimeDelta)
{
	if (m_bTransferring == false) return;

	if (0.0 == m_fTransferringBar)
	{
		// 플레이어의 소지금 감소, 아이템 구매
		vector<pair<wstring, CItem_Manager::ITEMINFO>> vecItemInfo = CItem_Manager::GetInstance()->Get_ItmeInfo();
		_uint iItemValueResult = vecItemInfo[m_iItemIndex].second.iPrice * m_iItemCount;
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().iCoin -= iItemValueResult;

		CItem_Manager::GetInstance()->Set_ItemCount(vecItemInfo[m_iItemIndex].first, +m_iItemCount);
	}

	m_fTransferringBar += _float(TimeDelta) * 0.5f;

	if(1.0f > m_fTransferringBar)
		dynamic_cast<CShaderUI*>(Find_ChildUI(L"ListCheck_Bar"))->Set_Floats0(m_fTransferringBar);

	if (1.3f < m_fTransferringBar)
	{
		m_fTransferringBar = 0.0f;
		m_bVisible = false;
	}
}

CCanvas_ListCheck * CCanvas_ListCheck::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_ListCheck*      pInstance = new CCanvas_ListCheck(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_ListCheck");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_ListCheck::Clone(void * pArg)
{
	CCanvas_ListCheck*      pInstance = new CCanvas_ListCheck(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_ListCheck");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_ListCheck::Free()
{
	CCanvas::Free();
}