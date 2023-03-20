#include "stdafx.h"
#include "..\public\Canvas_Party.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "DefaultUI.h"

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

}

void CCanvas_Party::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_Party::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	//_float2 vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Drive_BackGround"))->GetScreenSpaceLeftTop();
	//CGameInstance::GetInstance()->Render_Font(L"Pretendard32", L"DRIVE", vPosition + _float2(40.0f, 99.0f), 0.f, { 0.4f, 0.4f }, { 1.0f, 0.99f, 0.87f, 1.0f });

	//_tchar szChildTag[MAX_PATH] = TEXT("");

	//wsprintf(szChildTag, TEXT("%d"), _int(fPlayerHp.x));
	//CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szChildTag, vPosition + _float2(-100.0f, 61.0f), 0.f, { 0.55f, 0.55f }, { 1.0f, 0.99f, 0.87f, 1.0f });

	return S_OK;
}

void CCanvas_Party::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_Party::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_Party::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
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
