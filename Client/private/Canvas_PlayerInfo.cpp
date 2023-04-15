#include "stdafx.h"
#include "..\public\Canvas_PlayerInfo.h"
#include "GameInstance.h"
#include "PlayerInfoManager.h"
#include "UI_Manager.h"

#include "DefaultUI.h"
#include "PlayerInfo_GaugeBackGround.h"

CCanvas_PlayerInfo::CCanvas_PlayerInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_PlayerInfo::CCanvas_PlayerInfo(const CCanvas_PlayerInfo& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_PlayerInfo::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_PlayerInfo::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_Canvas(L"Canvas_PlayerInfo", this);
	
	m_bVisible = true;

	return S_OK;
}

void CCanvas_PlayerInfo::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

}

void CCanvas_PlayerInfo::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_PlayerInfo::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_float2 vPosition = Find_ChildUI(L"PlayerInfo_GaugeBack")->GetScreenSpaceLeftTop();
	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", L"유이토 스메라기", vPosition + _float2(30.0f, 77.0f), 0.f, { 0.4f, 0.4f }, { 1.0f, 0.99f, 0.87f, 1.0f });

	_uint iPlayerHp = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iHP;
	_uint iPlayerMaxHp = CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iMaxHP;
	_tchar szChildTag[MAX_PATH] = TEXT("");

	if (1000 <= iPlayerHp)
	{
		wsprintf(szChildTag, TEXT("%u"), iPlayerHp);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szChildTag, vPosition + _float2(300.0f, 33.0f), 0.f, { 0.45f, 0.45f }, { 1.0f, 0.99f, 0.87f, 1.0f });
		wsprintf(szChildTag, TEXT("/%u"), iPlayerMaxHp);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szChildTag, vPosition + _float2(353.0f, 36.0f), 0.f, { 0.35f, 0.35f }, { 1.0f, 0.99f, 0.87f, 1.0f });
	}
	else
	{
		wsprintf(szChildTag, TEXT("%u"), iPlayerHp);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szChildTag, vPosition + _float2(320.0f, 34.0f), 0.f, { 0.45f, 0.45f }, { 1.0f, 0.99f, 0.87f, 1.0f });
		wsprintf(szChildTag, TEXT("/%u"), iPlayerMaxHp);
		CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szChildTag, vPosition + _float2(360.0f, 37.0f), 0.f, { 0.35f, 0.35f }, { 1.0f, 0.99f, 0.87f, 1.0f });
	}

	return S_OK;
}

void CCanvas_PlayerInfo::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	ImGui::DragFloat ("X", &mm.x);
	ImGui::DragFloat("Y", &mm.y);
}

void CCanvas_PlayerInfo::Set_Type(const _uint iLevel)
{
	dynamic_cast<CPlayerInfo_GaugeBackGround*>(Find_ChildUI(L"PlayerInfo_GaugeBack"))->Set_PsychokinesisGauge(iLevel);
}

CCanvas_PlayerInfo * CCanvas_PlayerInfo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_PlayerInfo*		pInstance = new CCanvas_PlayerInfo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_PlayerInfo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_PlayerInfo::Clone(void * pArg)
{
	CCanvas_PlayerInfo*		pInstance = new CCanvas_PlayerInfo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_PlayerInfo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_PlayerInfo::Free()
{
	CCanvas::Free();
}
