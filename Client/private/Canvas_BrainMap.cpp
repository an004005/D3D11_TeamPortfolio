#include "stdafx.h"
#include "..\public\Canvas_BrainMap.h"
#include "GameInstance.h"
#include "PlayerInfoManager.h"
#include "Item_Manager.h"

#include "DefaultUI.h"
#include "Main_OnMouseUI.h"
#include "Main_BrainMapIconPickUI.h"

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

	return S_OK;
}

HRESULT CCanvas_BrainMap::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	m_bVisible = false;

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
			++m_iIconCount;
			m_vecIconUI.push_back(dynamic_cast<CMain_OnMouseUI*>((*iter).second));
		}

		wstring	wsLink = L"";
		if (10 > m_iLinkCount)
			wsLink = L"Icon__0" + to_wstring(m_iLinkCount);
		else
			wsLink = L"Icon__" + to_wstring(m_iLinkCount);

		if (iter->first == wsLink)
		{
			++m_iLinkCount;
			m_vecLinkUI.push_back(iter->second);
		}
	}

	return S_OK;
}

void CCanvas_BrainMap::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);

	OnIcon_Tick();

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
	_float2 vFontBigSize = { 0.45f, 0.45f };
	_float2 vFontSmaillSize = { 0.35f, 0.35f };
	_float4 vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_float2 vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"BrainExplanation_Face"))->GetScreenSpaceLeftTop();
	wsprintf(szText, TEXT("%d"), CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iHP);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(385.0f, 10.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"ü��", vPosition + _float2(120.0f, 10.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d"), CPlayerInfoManager::GetInstance()->Get_PlayerStat().iSprbrPower);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(385.0f, 31.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"�ʳ��ɷ�", vPosition + _float2(120.0f, 31.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d"), CPlayerInfoManager::GetInstance()->Get_PlayerStat().iAttack);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(385.0f, 52.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"���ݷ�", vPosition + _float2(120.0f, 52.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d"), CPlayerInfoManager::GetInstance()->Get_PlayerStat().iDefense);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(385.0f, 75.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"����", vPosition + _float2(120.0f, 75.0f), 0.f, vFontSmaillSize, vColor);

	// ���콺 Ŀ�� �� �ö��� �� �Ǵ� Ŭ���� �Ǿ��� �� ������ ���� �Ѵ�.
	/*if ()
		return S_OK;*/

	vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"BrainExplanation"))->GetScreenSpaceLeftTop();

	CItem_Manager::BRAININFO tBrainInfo = CItem_Manager::GetInstance()->Get_BrainInfo()[1];
	pGameInstance->Render_Font(L"Pretendard32", tBrainInfo.szBrainName, vPosition + _float2(100.0f, 120.0f), 0.f, { 0.6f, 0.6f }, vColor);
	pGameInstance->Render_Font(L"Pretendard32", tBrainInfo.szBrainEx[0], vPosition + _float2(100.0f, 510.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", tBrainInfo.szBrainEx[1], vPosition + _float2(100.0f, 540.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", tBrainInfo.szBrainEx[2], vPosition + _float2(100.0f, 570.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", tBrainInfo.szBrainEx[3], vPosition + _float2(100.0f, 600.0f), 0.f, vFontBigSize, vColor);

	if (true == m_bAcquired)
	{
		pGameInstance->Render_Font(L"Pretendard32", L"���� �Ϸ�", vPosition + _float2(80.0f, 21.0f), 0.f, vFontBigSize, vColor);
	}
	else
	{
		// �� ���� BP
		wsprintf(szText, TEXT("/      %d BP"), CPlayerInfoManager::GetInstance()->Get_PlayerStat().iBP);
		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(141.0f, 675.0f), 0.f, vFontBigSize, vColor);

		

		// UITODO : �� �������� BP �� ����־�� �Ѵ�. + �� BP�� �����ϴٸ� vColor �� ������ �������ش�.
		wsprintf(szText, TEXT("%d"), 55);
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

void CCanvas_BrainMap::OnIcon_Tick()
{
	for (size_t i = 0; i < m_vecIconUI.size(); ++i)
	{
		// ������ ���� ���콺�� �ö��� ��
		if (true == m_vecIconUI[i]->Get_OnMouse())
			Find_ChildUI(L"Icon_Pick")->Set_Position(m_vecIconUI[i]->Get_Position());

		// �������� Ŭ�� ���� ��
		if (true == m_vecIconUI[i]->Get_OnButton())
			dynamic_cast<CMain_BrainMapIconPickUI*>(Find_ChildUI(L"Icon_Pick"))->Set_Pick();
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
}
