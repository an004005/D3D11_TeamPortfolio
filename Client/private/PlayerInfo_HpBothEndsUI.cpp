#include "stdafx.h"
#include "..\public\PlayerInfo_HpBothEndsUI.h"
#include "GameInstance.h"

// m_tParams.Floats[0] : 게이지

CPlayerInfo_HpBothEndsUI::CPlayerInfo_HpBothEndsUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CPlayerInfo_HpBothEndsUI::CPlayerInfo_HpBothEndsUI(const CPlayerInfo_HpBothEndsUI& rhs)
	: CUI(rhs)
{
}

HRESULT CPlayerInfo_HpBothEndsUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CPlayerInfo_HpBothEndsUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	// 0: EndHP, 1:  EndHpBack, 2: StartHp, 3: StartHpBack
	static _int iObjectCount;
	m_iObjectNumber = iObjectCount;
	++iObjectCount;


	return S_OK;
}

void CPlayerInfo_HpBothEndsUI::BeginTick()
{
	m_tParams.Floats[0] = 1.0f;
}

void CPlayerInfo_HpBothEndsUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	if (0 == m_iObjectNumber || 2 == m_iObjectNumber)
	{
		if (m_fCurrentHp < m_fHp)
			m_fCurrentHp += _float(TimeDelta) * 0.1f;
		else
			m_fCurrentHp = m_fHp;
	}
	else
	{
		if (m_fCurrentHp < m_fHp)
			m_fCurrentHp += _float(TimeDelta) * 0.1f;
		else
			m_fCurrentHp -= _float(TimeDelta) * 0.1f;
	}


	if(2 == m_iObjectNumber || 3 == m_iObjectNumber)
		Start_Tick();
	else
		End_Tick();
}

void CPlayerInfo_HpBothEndsUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CPlayerInfo_HpBothEndsUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;


	return S_OK;
}

void CPlayerInfo_HpBothEndsUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CPlayerInfo_HpBothEndsUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CPlayerInfo_HpBothEndsUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CPlayerInfo_HpBothEndsUI::RendomHpImage(const _int & iImageNumber)
{
	// 캔버스에서 호출하는 함수로, Hp를 그리며, 이미지를 정하고 일부객체를 그린다.
	m_bVisible = true;
	m_tParams.Floats[0] = 1.0f;
}

void CPlayerInfo_HpBothEndsUI::Start_Tick()
{
	_float fCurHp = Clamp<_float>(m_fCurrentHp, 0.0f, 0.05f);
	m_tParams.Floats[0] = Remap<float>(fCurHp, 0.0f, 0.05f, 0.0f, 1.0f);
}

void CPlayerInfo_HpBothEndsUI::End_Tick()
{
	_float fCurrentHp = m_fCurrentHp - 0.95f;
	m_tParams.Floats[0] = _float(fCurrentHp / 0.05f);
}

CPlayerInfo_HpBothEndsUI * CPlayerInfo_HpBothEndsUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CPlayerInfo_HpBothEndsUI*		pInstance = new CPlayerInfo_HpBothEndsUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerInfo_HpBothEndsUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CPlayerInfo_HpBothEndsUI::Clone(void * pArg)
{
	CPlayerInfo_HpBothEndsUI*		pInstance = new CPlayerInfo_HpBothEndsUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayerInfo_HpBothEndsUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerInfo_HpBothEndsUI::Free()
{
	CUI::Free();

}
