#include "stdafx.h"
#include "..\public\Canvas_SASInfoRight.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "PlayerInfoManager.h"

#include "DefaultUI.h"
#include "Canvas_SASInfoRightMove.h"

CCanvas_SASInfoRight::CCanvas_SASInfoRight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SASInfoRight::CCanvas_SASInfoRight(const CCanvas_SASInfoRight& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SASInfoRight::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CCanvas_SASInfoRight::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_Canvas(L"Canvas_SASInfoRight", this);
	
	m_bVisible = false;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);
	
	return S_OK;
}

void CCanvas_SASInfoRight::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

}

void CCanvas_SASInfoRight::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_SASInfoRight::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_float2 vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"SASInfo_Right_BackGround"))->GetScreenSpaceLeftTop();
	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", L"하나비 이치조", vPosition + _float2(150.0f, 90.0f), 0.f, { 0.35f, 0.35f }, { 1.0f, 0.99f, 0.87f, 1.0f });

	_uint iHp = CPlayerInfoManager::GetInstance()->Get_HanabiStat().iHP;
	_uint iMaxHP = CPlayerInfoManager::GetInstance()->Get_HanabiStat().iMaxHP;
	_tchar szChildTag[MAX_PATH] = TEXT("");
	
	_float2 fFontSize = { 0.45f, 0.45f };
	if (1000 <= iHp)
	{
		wsprintf(szChildTag, TEXT("%u"), iHp);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szChildTag, vPosition + _float2(230.0f, 30.0f), 0.f, fFontSize, { 1.0f, 0.99f, 0.87f, 1.0f });
		wsprintf(szChildTag, TEXT("/ %u"), iMaxHP);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szChildTag, vPosition + _float2(281.0f, 30.5f), 0.f, fFontSize, { 1.0f, 0.99f, 0.87f, 1.0f });
	}
	else
	{
		wsprintf(szChildTag, TEXT("%u"), iHp);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szChildTag, vPosition + _float2(251.0f, 30.0f), 0.f, fFontSize, { 1.0f, 0.99f, 0.87f, 1.0f });
		wsprintf(szChildTag, TEXT("/ %u"), iMaxHP);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szChildTag, vPosition + _float2(291.0f, 30.5f), 0.f, fFontSize, { 1.0f, 0.99f, 0.87f, 1.0f });
	}

	return S_OK;
}

void CCanvas_SASInfoRight::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_SASInfoRight::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_SASInfoRight::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

void CCanvas_SASInfoRight::Set_Render()
{
	//if (true == m_bVisible)
	//	return;

	m_bVisible = true;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(true);
}

CCanvas_SASInfoRight * CCanvas_SASInfoRight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_SASInfoRight*		pInstance = new CCanvas_SASInfoRight(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SASInfoRight");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_SASInfoRight::Clone(void * pArg)
{
	CCanvas_SASInfoRight*		pInstance = new CCanvas_SASInfoRight(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SASInfoRight");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SASInfoRight::Free()
{
	CCanvas::Free();
}
