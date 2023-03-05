#include "stdafx.h"
#include "..\public\SASInfoLeftHpUI.h"
#include "GameInstance.h"

// m_tParams.Ints[2] : 0번은 5번째 까지 / 1번은 10번째 까지 / 2번은 기본
// m_tParams.Ints[0] : g_int_2가 0일 때 5 / g_int_2가 1일 때 10 / g_int_2가 2일 때 노상관
// m_tParams.Floats[0] : 게이지

CSASInfoLeftHpUI::CSASInfoLeftHpUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSASInfoLeftHpUI::CSASInfoLeftHpUI(const CSASInfoLeftHpUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSASInfoLeftHpUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASInfoLeftHpUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	static _int iObjectCount;
	m_iObjectNumber = iObjectCount;
	++iObjectCount;

	m_tParams.Floats[0] = 0.0f;

	return S_OK;
}

void CSASInfoLeftHpUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	if (m_fCurrentHp < m_fHp)
		m_fCurrentHp += _float(TimeDelta) * 0.1f;
	else
		m_fCurrentHp = m_fHp;

	Object_Tick(TimeDelta);
	
}

void CSASInfoLeftHpUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CSASInfoLeftHpUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASInfoLeftHpUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CSASInfoLeftHpUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CSASInfoLeftHpUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CSASInfoLeftHpUI::Object_Tick(const _double & dTimeDelta)
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
	default:
		// Pass
		break;
	}
}

void CSASInfoLeftHpUI::Zero_Tick(const _double & dTimeDetla)
{
	if (0.35f < m_fCurrentHp)
		return;

	NotDraw(0.05f);
}

void CSASInfoLeftHpUI::One_Tick(const _double & dTimeDetla)
{
	if (0.65f < m_fCurrentHp)
		return;

	NotDraw(0.35f);
}

void CSASInfoLeftHpUI::Two_Tick(const _double & dTimeDetla)
{
	if (0.95f < m_fCurrentHp)
		return;

	NotDraw(0.65f);
}

void CSASInfoLeftHpUI::RendomHpImage(const _int & iImageNumber)
{
	if (0 == iImageNumber)
		m_tParams.Ints[0] = 5;
	else
		m_tParams.Ints[0] = 10;

	m_tParams.Ints[2] = iImageNumber;
}

void CSASInfoLeftHpUI::NotDraw(const _float & fMinHp)
{
	_float fCurrentHp = m_fCurrentHp - fMinHp;
	m_tParams.Floats[0] = _float(fCurrentHp / 0.3f);
	m_tParams.Ints[2] = 2;
}

CSASInfoLeftHpUI * CSASInfoLeftHpUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASInfoLeftHpUI*		pInstance = new CSASInfoLeftHpUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASInfoLeftHpUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSASInfoLeftHpUI::Clone(void * pArg)
{
	CSASInfoLeftHpUI*		pInstance = new CSASInfoLeftHpUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASInfoLeftHpUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASInfoLeftHpUI::Free()
{
	CUI::Free();

}
