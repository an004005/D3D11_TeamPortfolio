#include "stdafx.h"
#include "..\public\AcquisitionUI.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include <random>

CAcquisitionUI::CAcquisitionUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CAcquisitionUI::CAcquisitionUI(const CAcquisitionUI& rhs)
	: CUI(rhs)
{
}

HRESULT CAcquisitionUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAcquisitionUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_bVisible = false;

	return S_OK;
}

void CAcquisitionUI::Tick(_double TimeDelta)
{
	if (m_bRunning == false) return;

	__super::Tick(TimeDelta);

	m_dDead_TimeAcc += TimeDelta;
	if (0.5 > m_dDead_TimeAcc)
	{
		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(m_wsText.begin(), m_wsText.end(), g); // ¹®ÀÚ¿­À» ·£´ýÇÏ°Ô ¼¯À½
	}
	else
	{
		m_wsText = m_wsStartText;
	}

	if (5.0 < m_dDead_TimeAcc)
	{
		m_bDead = true;
	}
}

void CAcquisitionUI::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CAcquisitionUI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", m_wsText.c_str(), GetScreenSpaceLeftTop() + _float2(150.0f, 5.0f), 0.f, { 0.35f, 0.35f });

	return S_OK;
}

void CAcquisitionUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CAcquisitionUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CAcquisitionUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);
	m_wsText = s2ws(json["Text"]);
	m_wsStartText = m_wsText;

	//m_szText = new _tchar[wstr.size() + 1];
	//lstrcpy(m_szText, wstr.c_str());

	//const _tchar* pp = m_szText;
}

CAcquisitionUI * CAcquisitionUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CAcquisitionUI*		pInstance = new CAcquisitionUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAcquisitionUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CAcquisitionUI::Clone(void * pArg)
{
	CAcquisitionUI*		pInstance = new CAcquisitionUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAcquisitionUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAcquisitionUI::Free()
{
	__super::Free();

}
