#include "stdafx.h"
#include "..\public\Canvas_SASInfoLeft.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "PlayerInfoManager.h"

#include "DefaultUI.h"
#include "Canvas_SASInfoLeftMove.h"

CCanvas_SASInfoLeft::CCanvas_SASInfoLeft(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SASInfoLeft::CCanvas_SASInfoLeft(const CCanvas_SASInfoLeft& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SASInfoLeft::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_SASInfoLeft::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_Canvas(L"Canvas_SASInfoLeft", this);
	
	m_bVisible = false;
	
	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	return S_OK;
}

void CCanvas_SASInfoLeft::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

}

void CCanvas_SASInfoLeft::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_SASInfoLeft::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_float2 vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"SASInfo_Left_BackGround"))->GetScreenSpaceLeftTop();
	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", L"츠구미 나자르", vPosition + _float2(160.0f, 90.0f), 0.f, { 0.35f, 0.35f }, { 1.0f, 0.99f, 0.87f, 1.0f });

	_uint iHp = CPlayerInfoManager::GetInstance()->Get_TsugumiStat().iHP;
	_uint iMaxHP = CPlayerInfoManager::GetInstance()->Get_TsugumiStat().iMaxHP;
	_tchar szChildTag[MAX_PATH] = TEXT("");

	_float2 fFontSize = { 0.45f, 0.45f };
	if (1000 <= iHp)
	{
		wsprintf(szChildTag, TEXT("%u"), iHp);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szChildTag, vPosition + _float2(224.0f, 27.0f), 0.f, fFontSize, { 1.0f, 0.99f, 0.87f, 1.0f });
		wsprintf(szChildTag, TEXT("/ %u"), iMaxHP);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szChildTag, vPosition + _float2(275.0f, 27.0f), 0.f, fFontSize, { 1.0f, 0.99f, 0.87f, 1.0f });
	}
	else
	{
		wsprintf(szChildTag, TEXT("%u"), iHp);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szChildTag, vPosition + _float2(245.0f, 27.0f), 0.f, fFontSize, { 1.0f, 0.99f, 0.87f, 1.0f });
		wsprintf(szChildTag, TEXT("/ %u"), iMaxHP);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szChildTag, vPosition + _float2(285.0f, 27.0f), 0.f, fFontSize, { 1.0f, 0.99f, 0.87f, 1.0f });
	}

	return S_OK;
}

void CCanvas_SASInfoLeft::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	static _float fPosition[2];
	ImGui::DragFloat2("Font Position", fPosition);
	m_vFontPos = { fPosition[0], fPosition[1] };
}

void CCanvas_SASInfoLeft::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_SASInfoLeft::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

void CCanvas_SASInfoLeft::Set_Render()
{	
	m_bVisible = true;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(true);
}

CCanvas_SASInfoLeft * CCanvas_SASInfoLeft::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_SASInfoLeft*		pInstance = new CCanvas_SASInfoLeft(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SASInfoLeft");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_SASInfoLeft::Clone(void * pArg)
{
	CCanvas_SASInfoLeft*		pInstance = new CCanvas_SASInfoLeft(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SASInfoLeft");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SASInfoLeft::Free()
{
	CCanvas::Free();
}
