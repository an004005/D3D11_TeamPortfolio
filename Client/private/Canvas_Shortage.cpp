#include "stdafx.h"
#include "..\public\Canvas_Shortage.h"
#include "GameInstance.h"

#include "ButtonUI.h"

CCanvas_Shortage::CCanvas_Shortage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_Shortage::CCanvas_Shortage(const CCanvas_Shortage& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_Shortage::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_Shortage::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	return S_OK;
}

void CCanvas_Shortage::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);

	if (m_bVisible == false) return;

	if (true == dynamic_cast<CButtonUI*>(Find_ChildUI(L"Shop_ShortageB"))->Get_Input())
	{
		m_bVisible = false;
		dynamic_cast<CButtonUI*>(Find_ChildUI(L"Shop_ShortageB"))->Set_Input();
	}
}

void CCanvas_Shortage::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_Shortage::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_float2 vFontSize = { 0.5f, 0.5f };
	_float4 vColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	_float2 vPosition = Find_ChildUI(L"Shop_ShortageB")->GetScreenSpaceLeftTop();
	pGameInstance->Render_Font(L"Pretendard32", L"�������� �����Ͽ�  ������ �� �����ϴ�.", vPosition + _float2(55.0f, 55.0f), 0.f, vFontSize, vColor);

	return S_OK;
}

void CCanvas_Shortage::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_Shortage::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_Shortage::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_Shortage * CCanvas_Shortage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_Shortage*      pInstance = new CCanvas_Shortage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_Shortage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_Shortage::Clone(void * pArg)
{
	CCanvas_Shortage*      pInstance = new CCanvas_Shortage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_Shortage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_Shortage::Free()
{
	CCanvas::Free();
}