#include "stdafx.h"
#include "..\public\PlayerInfo_HpBackUI.h"
#include "GameInstance.h"

// m_tParams.Ints[2] : 0번은 5번째 까지 / 1번은 10번째 까지 / 2번은 기본
// m_tParams.Ints[0] : g_int_2가 0일 때 5 / g_int_2가 1일 때 10 / g_int_2가 2일 때 노상관
// m_tParams.Floats[0] : 게이지

CPlayerInfo_HpBackUI::CPlayerInfo_HpBackUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CPlayerInfo_HpBackUI::CPlayerInfo_HpBackUI(const CPlayerInfo_HpBackUI& rhs)
	: CUI(rhs)
{
}

HRESULT CPlayerInfo_HpBackUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CPlayerInfo_HpBackUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	static _int iObjectCount;
	m_iObjectNumber = iObjectCount;
	++iObjectCount;

	m_tParams.Floats[0] = 0.0f;
	
	return S_OK;
}

void CPlayerInfo_HpBackUI::BeginTick()
{

}

void CPlayerInfo_HpBackUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	if (m_fCurrentHp < m_fHp)
		m_fCurrentHp += _float(TimeDelta) * 0.1f;
	else
		m_fCurrentHp -= _float(TimeDelta) * 0.1f;

	Object_Tick(TimeDelta);
}

void CPlayerInfo_HpBackUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CPlayerInfo_HpBackUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CPlayerInfo_HpBackUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CPlayerInfo_HpBackUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CPlayerInfo_HpBackUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CPlayerInfo_HpBackUI::Object_Tick(const _double & dTimeDelta)
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

void CPlayerInfo_HpBackUI::Zero_Tick(const _double & dTimeDelta)
{
	if (0.2f < m_fCurrentHp)
		return;

	NotDraw(0.05f);
}

void CPlayerInfo_HpBackUI::One_Tick(const _double & dTimeDelta)
{
	if (0.35f < m_fCurrentHp)
		return;

	NotDraw(0.2f);
}

void CPlayerInfo_HpBackUI::Two_Tick(const _double & dTimeDelta)
{
	if (0.5f < m_fCurrentHp)
		return;

	NotDraw(0.35f);
}

void CPlayerInfo_HpBackUI::Three_Tick(const _double & dTimeDelta)
{
	if (0.65f < m_fCurrentHp)
		return;
	
	NotDraw(0.5f);
}

void CPlayerInfo_HpBackUI::Four_Tick(const _double & dTimeDelta)
{
	if (0.8f < m_fCurrentHp)
		return;

	NotDraw(0.65f);
}

void CPlayerInfo_HpBackUI::Five_Tick(const _double & dTimeDelta)
{
	if (0.95f < m_fCurrentHp)
		return;

	NotDraw(0.8f);
}

void CPlayerInfo_HpBackUI::RendomHpImage(const _int & iImageNumber)
{
	// 캔버스에서 호출하는 함수로, 이미지를 정하고 일부객체를 그린다.

	if (0 == iImageNumber)
		m_tParams.Ints[0] = 5;
	else
		m_tParams.Ints[0] = 10;

	m_tParams.Ints[2] = iImageNumber;
}

void CPlayerInfo_HpBackUI::NotDraw(const _float & fMinHp)
{
	_float fCurrentHp = m_fCurrentHp - fMinHp;
	m_tParams.Floats[0] = _float(fCurrentHp / 0.15f);
	m_tParams.Ints[2] = 2;
}

CPlayerInfo_HpBackUI * CPlayerInfo_HpBackUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CPlayerInfo_HpBackUI*		pInstance = new CPlayerInfo_HpBackUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayerInfo_HpBackUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CPlayerInfo_HpBackUI::Clone(void * pArg)
{
	CPlayerInfo_HpBackUI*		pInstance = new CPlayerInfo_HpBackUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayerInfo_HpBackUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerInfo_HpBackUI::Free()
{
	CUI::Free();

}
