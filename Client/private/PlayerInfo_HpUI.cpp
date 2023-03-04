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

	m_tParams.Floats[0] = 1.0f;
	//if (iObjectCount == 1 || iObjectCount == 2)
	//	m_iObjectNumber = 0;
	//else if(iObjectCount == 3 || iObjectCount == 4)
	//	m_iObjectNumber = 1;
	//else if (iObjectCount == 5 || iObjectCount == 6)
	//	m_iObjectNumber = 2;
	//else if (iObjectCount == 7 || iObjectCount == 8)
	//	m_iObjectNumber = 3;
	//else if (iObjectCount == 9 || iObjectCount == 10)
	//	m_iObjectNumber = 4;
	//else if (iObjectCount == 11 || iObjectCount == 12)
	//	m_iObjectNumber = 5;

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

void CPlayerInfo_HpUI::Object_Tick(const _double & dTimeDetla)
{
	switch (m_iObjectNumber)
	{
	case 0:
		Zero_Tick(dTimeDetla);
		break;
	case 1:
		One_Tick(dTimeDetla);
		break;
	case 2:
		Two_Tick(dTimeDetla);
		break;
	case 3:
		Three_Tick(dTimeDetla);
		break;
	case 4:
		Four_Tick(dTimeDetla);
		break;
	case 5:
		Five_Tick(dTimeDetla);
		break;
	default:
		// Pass
		break;
	}
}

void CPlayerInfo_HpUI::Zero_Tick(const _double & dTimeDetla)
{
	if (0.2f < m_fCurrentHp)
	{
		RendomHpImage(dTimeDetla);
		m_bVisible = true;
		return;
	}

	NotDraw(0.05f);
}

void CPlayerInfo_HpUI::One_Tick(const _double & dTimeDetla)
{
	if (0.35f < m_fCurrentHp)
	{
		RendomHpImage(dTimeDetla);
		m_bVisible = true;
		return;
	}

	NotDraw(0.2f);
}

void CPlayerInfo_HpUI::Two_Tick(const _double & dTimeDetla)
{
	if (0.5f <= m_fCurrentHp)
	{
		RendomHpImage(dTimeDetla);
		m_bVisible = true;
		return;
	}

	NotDraw(0.35f);
}

void CPlayerInfo_HpUI::Three_Tick(const _double & dTimeDetla)
{
	if (0.65f < m_fCurrentHp)
	{
		RendomHpImage(dTimeDetla);
		m_bVisible = true;
		return;
	}
	
	NotDraw(0.5f);
}

void CPlayerInfo_HpUI::Four_Tick(const _double & dTimeDetla)
{
	if (0.8f < m_fCurrentHp)
	{
		RendomHpImage(dTimeDetla);
		m_bVisible = true;
		return;
	}

	NotDraw(0.65f);
}

void CPlayerInfo_HpUI::Five_Tick(const _double & dTimeDetla)
{
	if (0.95f < m_fCurrentHp)
	{
		RendomHpImage(dTimeDetla);
		m_bVisible = true;
		return;
	}

	NotDraw(0.8f);
}

void CPlayerInfo_HpUI::RendomHpImage(const _double & dTimeDetla)
{
	m_tParams.Floats[0] = 1.0f; 
	
	m_dImageChange_TimeAcc += dTimeDetla;
	if (1.0 < m_dImageChange_TimeAcc)
	{
		m_dImageChange_TimeAcc = 0.0;

		// 체력에 따라서 랜덤으로 이미지를 출력하는 개수가 달라진다. (m_fCurrentHp 기준)
		// 0.05~0.95 : 3 / 0.05~0.65 : 2 / 0.05~0.35 : 1

		static _int iRendomCount;
		if (0.65f < m_fCurrentHp)
			iRendomCount = 3;
		else if(0.35f < m_fCurrentHp)
			iRendomCount = 2;
		else if (0.05f < m_fCurrentHp)
			iRendomCount = 1;
		else
			iRendomCount = 0;

		for (_int i = 0; i < iRendomCount; i++)
		{
			_int fRandomObjectNumber = _int(CMathUtils::RandomFloat(0.0f, 6.0f));	// 랜덤 객체 고르기

			if (m_iObjectNumber == fRandomObjectNumber)
			{
				_int iRandomNumber = _int(CMathUtils::RandomFloat(0.0f, 2.0f)); // 0 or 1 이미지 고르기

				// 가로로 자를 숫자
				if (0 == iRandomNumber)
				{
					m_tParams.Ints[2] = iRandomNumber;
					m_tParams.Ints[0] = _int(CMathUtils::RandomFloat(0.0f, 6.0f));
				}
				else
				{
					m_tParams.Ints[2] = iRandomNumber;
					m_tParams.Ints[0] = _int(CMathUtils::RandomFloat(0.0f, 11.0f));
				}

			}
			else
				m_tParams.Ints[2] = 2;	// 그 외는 기본
		}
	}
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
