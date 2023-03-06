#include "stdafx.h"
#include "..\public\Canvas_PlayerInfo.h"
#include "GameInstance.h"

#include "UI_Manager.h"
#include "DefaultUI.h"
#include "Canvas_PlayerInfoMove.h"

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

	_float2 vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"PlayerInfo_BackGround"))->GetScreenSpaceLeftTop();
	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", L"유이토 스메라기", vPosition + _float2(110.0f, 99.0f), 0.f, { 0.35f, 0.35f }, { 1.0f, 0.99f, 0.87f, 1.0f });

	//_float2 fPlayerHp = dynamic_cast<CCanvas_PlayerInfoMove*>(CUI_Manager::GetInstance()->Find_MoveCanvas(L"Canvas_PlayerInfoMove"))->Get_PlayerHp();
	//_tchar szChildTag[MAX_PATH] = TEXT("");
	//wsprintf(szChildTag, TEXT("%d/"), fPlayerHp.x);
	//CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szChildTag, vPosition + m_vFontPos, 0.f, { 0.45f, 0.45f }, { 1.0f, 0.99f, 0.87f, 1.0f });
	//wsprintf(szChildTag, TEXT("%d"), fPlayerHp.y);
	//CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szChildTag, vPosition + m_vFontPos, 0.f, { 0.35f, 0.35f }, { 1.0f, 0.99f, 0.87f, 1.0f });

	return S_OK;
}

void CCanvas_PlayerInfo::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	static _float fPosition[2];
	ImGui::DragFloat2("Font Position", fPosition);
	m_vFontPos = { fPosition[0], fPosition[1] };
}

void CCanvas_PlayerInfo::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);

}

void CCanvas_PlayerInfo::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);

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
