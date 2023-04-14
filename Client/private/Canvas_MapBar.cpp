#include "stdafx.h"
#include "..\public\Canvas_MapBar.h"
#include "GameInstance.h"

#include "Main_PickUI.h"

CCanvas_MapBar::CCanvas_MapBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_MapBar::CCanvas_MapBar(const CCanvas_MapBar& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_MapBar::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_MapBar::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	m_bVisible = false;

	return S_OK;
}

void CCanvas_MapBar::Tick(_double TimeDelta)
{
	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);

	if (false == m_bVisible) return;

	CCanvas::Tick(TimeDelta);
}

void CCanvas_MapBar::Late_Tick(_double TimeDelta)
{
	if (false == m_bVisible) return;

	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_MapBar::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_float2 vPosition = Find_ChildUI(L"PickBar")->GetScreenSpaceLeftTop();
	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", m_wsName.c_str(), vPosition + _float2(25.0f, 8.0f), 0.f, { 0.45f, 0.45f }, { 1.0f, 0.99f, 0.87f, 1.0f });

	return S_OK;
}

void CCanvas_MapBar::Set_PosY(const _float& fPosY)
{
	Find_ChildUI(L"PickBar")->Set_Position({ Find_ChildUI(L"PickBar")->Get_Position().x, fPosY });
	Find_ChildUI(L"BaseBar")->Set_Position({ Find_ChildUI(L"BaseBar")->Get_Position().x, fPosY });
}

_bool CCanvas_MapBar::Get_OnButton()
{
	return 	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"PickBar"))->Get_OnButton();
}

void CCanvas_MapBar::Set_OnButton()
{
	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"PickBar"))->Set_OnButton();
}

void CCanvas_MapBar::Set_OnAlpha()
{
	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"PickBar"))->Set_OnAlpha();
}

CCanvas_MapBar * CCanvas_MapBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_MapBar*		pInstance = new CCanvas_MapBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_MapBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_MapBar::Clone(void * pArg)
{
	CCanvas_MapBar*		pInstance = new CCanvas_MapBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_MapBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_MapBar::Free()
{
	CCanvas::Free();

}
