#include "stdafx.h"
#include "..\public\PlayerInfo_HpBackUI.h"
#include "GameInstance.h"
#include "JsonLib.h"
#include "MathUtils.h"

// m_tParams.Ints[2] : 0���� 5��° ���� / 1���� 10��° ���� / 2���� �⺻
// m_tParams.Ints[0] : g_int_2�� 0�� �� 5 / g_int_2�� 1�� �� 10 / g_int_2�� 2�� �� ����
// m_tParams.Floats[0] : ������

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

	m_fCurrentHp = 1.0f;

	return S_OK;
}

void CPlayerInfo_HpBackUI::BeginTick()
{
	m_fCurrentHp = 1.0f;

}

void CPlayerInfo_HpBackUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	_float fReault = m_fHp / m_fMaxHp;
	
	if (m_fCurrentHp > fReault)
		m_fCurrentHp -= _float(TimeDelta) * 0.1f;
	else
		m_fCurrentHp = fReault;

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

void CPlayerInfo_HpBackUI::Object_Tick(const _double & dTimeDetla)
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

void CPlayerInfo_HpBackUI::Zero_Tick(const _double & dTimeDetla)
{
	if (0.2f < m_fCurrentHp)
	{
		RendomHpImage(dTimeDetla);
		m_bVisible = true;
		return;
	}

	NotDraw(0.05f);
}

void CPlayerInfo_HpBackUI::One_Tick(const _double & dTimeDetla)
{
	if (0.35f < m_fCurrentHp)
	{
		RendomHpImage(dTimeDetla);
		m_bVisible = true;
		return;
	}

	NotDraw(0.2f);
}

void CPlayerInfo_HpBackUI::Two_Tick(const _double & dTimeDetla)
{
	if (0.5f <= m_fCurrentHp)
	{
		RendomHpImage(dTimeDetla);
		m_bVisible = true;
		return;
	}

	NotDraw(0.35f);
}

void CPlayerInfo_HpBackUI::Three_Tick(const _double & dTimeDetla)
{
	if (0.65f < m_fCurrentHp)
	{
		RendomHpImage(dTimeDetla);
		m_bVisible = true;
		return;
	}
	
	NotDraw(0.5f);
}

void CPlayerInfo_HpBackUI::Four_Tick(const _double & dTimeDetla)
{
	if (0.8f < m_fCurrentHp)
	{
		RendomHpImage(dTimeDetla);
		m_bVisible = true;
		return;
	}

	NotDraw(0.65f);
}

void CPlayerInfo_HpBackUI::Five_Tick(const _double & dTimeDetla)
{
	if (0.95f < m_fCurrentHp)
	{
		RendomHpImage(dTimeDetla);
		m_bVisible = true;
		return;
	}

	NotDraw(0.8f);
}

void CPlayerInfo_HpBackUI::RendomHpImage(const _double & dTimeDetla)
{
	m_tParams.Floats[0] = 1.0f; 
	
	m_dImageChange_TimeAcc += dTimeDetla;
	if (1.0 < m_dImageChange_TimeAcc)
	{
		m_dImageChange_TimeAcc = 0.0;

		// ü�¿� ���� �������� �̹����� ����ϴ� ������ �޶�����. (m_fCurrentHp ����)
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
			_int fRandomObjectNumber = _int(CMathUtils::RandomFloat(0.0f, 6.0f));	// ���� ��ü ����

			if (m_iObjectNumber == fRandomObjectNumber)
			{
				_int iRandomNumber = _int(CMathUtils::RandomFloat(0.0f, 2.0f)); // 0 or 1 �̹��� ����

				// ���η� �ڸ� ����
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
				m_tParams.Ints[2] = 2;	// �� �ܴ� �⺻
		}
	}
}

void CPlayerInfo_HpBackUI::NotDraw(const _float & fMinHp)
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
