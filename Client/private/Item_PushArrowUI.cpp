#include "stdafx.h"
#include "..\public\Item_PushArrowUI.h"
#include "GameInstance.h"
#include "JsonLib.h"

CItem_PushArrowUI::CItem_PushArrowUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CItem_PushArrowUI::CItem_PushArrowUI(const CItem_PushArrowUI& rhs)
	: CUI(rhs)
{
}

HRESULT CItem_PushArrowUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_PushArrowUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_fStartY = m_fY;

	return S_OK;
}

void CItem_PushArrowUI::BeginTick()
{


}

void CItem_PushArrowUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	if (false == m_bInput)	return;

	m_fMoveTimeAcc += TimeDelta;
	if (0.15 < m_fMoveTimeAcc)
	{
		m_fY += _float(TimeDelta) * 100.0f;

		if (m_fY > m_fStartY)
		{
			m_fY = m_fStartY;
			m_fMoveTimeAcc = 0.0;
			m_bInput = false;
		}
	}
	else
	{
		m_fY -= _float(TimeDelta) * 100.0f;
	}
}

void CItem_PushArrowUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CItem_PushArrowUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CItem_PushArrowUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CItem_PushArrowUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CItem_PushArrowUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

CItem_PushArrowUI * CItem_PushArrowUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CItem_PushArrowUI*		pInstance = new CItem_PushArrowUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItem_PushArrowUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CItem_PushArrowUI::Clone(void * pArg)
{
	CItem_PushArrowUI*		pInstance = new CItem_PushArrowUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CItem_PushArrowUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CItem_PushArrowUI::Free()
{
	CUI::Free();

}
