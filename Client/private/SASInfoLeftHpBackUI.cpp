#include "stdafx.h"
#include "..\public\SASInfoLeftHpBackUI.h"
#include "GameInstance.h"

// m_tParams.Ints[2] : 0번은 5번째 까지 / 1번은 10번째 까지 / 2번은 기본
// m_tParams.Ints[0] : g_int_2가 0일 때 5 / g_int_2가 1일 때 10 / g_int_2가 2일 때 노상관
// m_tParams.Floats[0] : 게이지

CSASInfoLeftHpBackUI::CSASInfoLeftHpBackUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSASInfoLeftHpBackUI::CSASInfoLeftHpBackUI(const CSASInfoLeftHpBackUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSASInfoLeftHpBackUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASInfoLeftHpBackUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	static _int iObjectCount;
	m_iObjectNumber = iObjectCount;
	++iObjectCount;

	if (3 == iObjectCount) iObjectCount = 0;

	m_tParams.Floats[0] = 0.0f;

	return S_OK;
}

void CSASInfoLeftHpBackUI::BeginTick()
{
	m_fHp = 1.0f;
	m_fCurrentHp = 1.0f;
	m_tParams.Floats[0] = 1.0f;
}

void CSASInfoLeftHpBackUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	if (false == m_bSpeed)
	{
		if (m_fCurrentHp < m_fHp)
			m_fCurrentHp += _float(TimeDelta) * 0.1f;
		else
			m_fCurrentHp -= _float(TimeDelta) * 0.1f;
	}
	else
	{
		if (m_fCurrentHp < m_fHp)
			m_fCurrentHp += _float(TimeDelta);
		else
			m_fCurrentHp -= _float(TimeDelta);
	}

	Object_Tick(TimeDelta);
}

void CSASInfoLeftHpBackUI::Late_Tick(_double TimeDelta)
{
	CUI::Late_Tick(TimeDelta);
}

HRESULT CSASInfoLeftHpBackUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CSASInfoLeftHpBackUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CSASInfoLeftHpBackUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CSASInfoLeftHpBackUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CSASInfoLeftHpBackUI::Object_Tick(const _double & dTimeDelta)
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

void CSASInfoLeftHpBackUI::Zero_Tick(const _double & dTimeDelta)
{
	if (0.35f < m_fCurrentHp)
		return;

	NotDraw(0.05f);
}

void CSASInfoLeftHpBackUI::One_Tick(const _double & dTimeDelta)
{
	if (0.65f < m_fCurrentHp)
		return;

	NotDraw(0.35f);
}

void CSASInfoLeftHpBackUI::Two_Tick(const _double & dTimeDelta)
{
	if (0.95f < m_fCurrentHp)
		return;

	NotDraw(0.65f);
}

void CSASInfoLeftHpBackUI::RendomHpImage(const _int & iImageNumber)
{
	if (0 == iImageNumber)
		m_tParams.Ints[0] = 5;
	else
		m_tParams.Ints[0] = 10;

	m_tParams.Ints[2] = iImageNumber;
}

void CSASInfoLeftHpBackUI::NotDraw(const _float & fMinHp)
{
	_float fCurrentHp = m_fCurrentHp - fMinHp;
	m_tParams.Floats[0] = _float(fCurrentHp / 0.3f);
	m_tParams.Ints[2] = 2;
}

CSASInfoLeftHpBackUI * CSASInfoLeftHpBackUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASInfoLeftHpBackUI*		pInstance = new CSASInfoLeftHpBackUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASInfoLeftHpBackUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSASInfoLeftHpBackUI::Clone(void * pArg)
{
	CSASInfoLeftHpBackUI*		pInstance = new CSASInfoLeftHpBackUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASInfoLeftHpBackUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASInfoLeftHpBackUI::Free()
{
	CUI::Free();

}
