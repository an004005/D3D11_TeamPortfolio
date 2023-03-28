#include "stdafx.h"
#include "..\public\Item_IconUI.h"
#include "GameInstance.h"

CItem_IconUI::CItem_IconUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CItem_IconUI::CItem_IconUI(const CItem_IconUI& rhs)
	: CUI(rhs)
{
}

HRESULT CItem_IconUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_IconUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_fStartSize.x = m_fSizeX;
	m_fStartSize.y = m_fSizeY;

	return S_OK;
}

void CItem_IconUI::BeginTick()
{


}

void CItem_IconUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	if (m_bUse == false) return;

	m_fSizeX += _float(TimeDelta) * 13.0f;
	m_fSizeY += _float(TimeDelta) * 13.0f;

	m_dSizeChange_TimeAcc += TimeDelta;
	if (1.0 < m_dSizeChange_TimeAcc)
	{
		m_bUse = false;
		m_fSizeX = m_fStartSize.x;
		m_fSizeY = m_fStartSize.y;
		m_dSizeChange_TimeAcc = 0.0;
	}
}

void CItem_IconUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CItem_IconUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CItem_IconUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CItem_IconUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CItem_IconUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CItem_IconUI::Set_IconIndex(const _float2 & fIconIndex)
{
	m_tParams.Float2s[0] = fIconIndex;
}

CItem_IconUI * CItem_IconUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CItem_IconUI*		pInstance = new CItem_IconUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItem_IconUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CItem_IconUI::Clone(void * pArg)
{
	CItem_IconUI*		pInstance = new CItem_IconUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CItem_IconUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CItem_IconUI::Free()
{
	CUI::Free();

}
