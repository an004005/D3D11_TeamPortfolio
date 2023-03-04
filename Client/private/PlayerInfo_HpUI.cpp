#include "stdafx.h"
#include "..\public\PlayerInfo_HpUI.h"
#include "GameInstance.h"
#include "JsonLib.h"
#include "MathUtils.h"

// m_tParams.Ints[2] : 0번은 5번째 까지 / 1번은 10번째 까지 / 2번은 기본
// m_tParams.Ints[0] : g_int_2가 0일 때 5 / g_int_2가 1일 때 10 / g_int_2가 2일 때 노상관
// m_tParams.Floats[0] : 게이지

CPlayerInfo_HpUI::CPlayerInfo_HpUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CPlayerInfo_HpUI::CPlayerInfo_HpUI(const CPlayerInfo_HpUI& rhs)
	: CUI(rhs)
{
}

HRESULT CPlayerInfo_HpUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CPlayerInfo_HpUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	static _int iObjectCount;
	m_iObjectNumber = iObjectCount;
	++iObjectCount;

	return S_OK;
}

void CPlayerInfo_HpUI::BeginTick()
{


}

void CPlayerInfo_HpUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	m_fCurrentHp = m_fHp / m_fMaxHp;

	Object_Tick(TimeDelta);
}

void CPlayerInfo_HpUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CPlayerInfo_HpUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CPlayerInfo_HpUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CPlayerInfo_HpUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CPlayerInfo_HpUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CPlayerInfo_HpUI::Object_Tick(const _double & dTimeDelta)
{
	switch (m_iObjectNumber)
	{
	case 0:
		Zero_Tick(dTimeDelta);
		break;
	case 1:
		One_Tick(dTimeDelta);
		break;
	case 2:
		Two_Tick(dTimeDelta);
		break;
	case 3:
		Three_Tick(dTimeDelta);
		break;
	case 4:
		Four_Tick(dTimeDelta);
		break;
	case 5:
		Five_Tick(dTimeDelta);
		break;
	default:
		// Pass
		break;
	}
}

void CPlayerInfo_HpUI::Zero_Tick(const _double & dTimeDelta)
{
	if (0.2f < m_fCurrentHp)
		return;

	NotDraw(0.05f);
}

void CPlayerInfo_HpUI::One_Tick(const _double & dTimeDelta)
{
	if (0.35f < m_fCurrentHp)
		return;

	NotDraw(0.2f);
}

void CPlayerInfo_HpUI::Two_Tick(const _double & dTimeDelta)
{
	if (0.5f <= m_fCurrentHp)
		return;

	NotDraw(0.35f);
}

void CPlayerInfo_HpUI::Three_Tick(const _double & dTimeDelta)
{
	if (0.65f < m_fCurrentHp)
		return;
	
	NotDraw(0.5f);
}

void CPlayerInfo_HpUI::Four_Tick(const _double & dTimeDelta)
{
	if (0.8f < m_fCurrentHp)
		return;

	NotDraw(0.65f);
}

void CPlayerInfo_HpUI::Five_Tick(const _double & dTimeDelta)
{
	if (0.95f < m_fCurrentHp)
		return;

	NotDraw(0.8f);
}

void CPlayerInfo_HpUI::RendomHpImage(const _int & iImageNumber)
{
	// 캔버스에서 호출하는 함수로, Hp를 그리며, 이미지를 정하고 일부객체를 그린다.
	m_bVisible = true;

	if (0 == iImageNumber)
		m_tParams.Ints[0] = 5;
	else
		m_tParams.Ints[0] = 10;

	m_tParams.Floats[0] = 1.0f;
	m_tParams.Ints[2] = iImageNumber;
}

void CPlayerInfo_HpUI::NotDraw(const _float & fMinHp)
{
	if (0.0f > m_tParams.Ints[2])
	{
		m_bVisible = false;
		return;
	}

	_float fCurrentHp = m_fCurrentHp - fMinHp;
	m_tParams.Floats[0] = _float(fCurrentHp / 0.15f);
	m_tParams.Ints[2] = 2;
}

CPlayerInfo_HpUI * CPlayerInfo_HpUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CPlayerInfo_HpUI*		pInstance = new CPlayerInfo_HpUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerInfo_HpUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CPlayerInfo_HpUI::Clone(void * pArg)
{
	CPlayerInfo_HpUI*		pInstance = new CPlayerInfo_HpUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayerInfo_HpUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerInfo_HpUI::Free()
{
	CUI::Free();

}
