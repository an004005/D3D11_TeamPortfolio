#include "stdafx.h"
#include "..\public\Drive_GaugeUI.h"
#include "GameInstance.h"
#include "UI_Manager.h"

//m_tParams.Floats[0] = Gauge

CDrive_GaugeUI::CDrive_GaugeUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CDrive_GaugeUI::CDrive_GaugeUI(const CDrive_GaugeUI& rhs)
	: CUI(rhs)
{
}

HRESULT CDrive_GaugeUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDrive_GaugeUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CDrive_GaugeUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	// ������ ����ϸ鼭 ���� �������� ä���.
	// ���� ������ ä��� ����̺� ����� ����������.
	// ����̺긦 ����ϸ� �ܺο��� �޾ƿ� ��ü �ð��� �̿��ؼ� �������� ���δ�.

	DrivaGaugePlus_Tick(TimeDelta);
	DrivaGaugeMinus_Tick(TimeDelta);
	DirveRightDot_Tick(TimeDelta);
}

HRESULT CDrive_GaugeUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CDrive_GaugeUI::Imgui_RenderProperty()
{
	CUI::Imgui_RenderProperty();

}

void CDrive_GaugeUI::SaveToJson(Json & json)
{
	CUI::SaveToJson(json);

}

void CDrive_GaugeUI::LoadFromJson(const Json & json)
{
	CUI::LoadFromJson(json);

}

void CDrive_GaugeUI::DrivaGaugePlus_Tick(const _double & dTimeDelta)
{
	if (false == m_bDriveGauge_Plus)
		return;

	// ����̺� �������� ����. (0->1)

	_float fDriveGauge = CUI_Manager::GetInstance()->Get_DriveGauge();
	if (m_tParams.Floats[0] < fDriveGauge)
	{
		m_tParams.Floats[0] += _float(dTimeDelta) * 0.05f;
	}
	else	// UITODO : �׽�Ʈ �뵵 �� ���� else �� �߰��Ѵ�.
	{
		m_tParams.Floats[0] = fDriveGauge;
	}

	if (1.0f < m_tParams.Floats[0])
	{
		m_tParams.Floats[0] = 1.0f;
		m_bGaugeFull = true;
		m_bDriveGauge_Plus = false;
	}
}

void CDrive_GaugeUI::DirveRightDot_Tick(const _double & dTimeDelta)
{
	if (false == m_bGaugeFull)
		return;

	// Gauge �� ���� á�� �� Right Dot �� ���� ����ٰ� ���ִ� �뵵
	// ������� Ŭ������ �ϳ� �� ������ ������ ���� �ִٰ� ���� ��� �ۿ� ��� ���⼭ �����Դ�.

	m_dGaugeFull_TimeAcc += dTimeDelta;
	if (1.5f < m_dGaugeFull_TimeAcc)
	{
		m_bGaugeFull = false;
		m_dGaugeFull_TimeAcc = 0.0;
	}
}

void CDrive_GaugeUI::DrivaGaugeMinus_Tick(const _double & dTimeDelta)
{
	if (false == m_bDriveGauge_Minus)
		return;

	// ����̺� �������� �پ���. (1->0)

	static _double m_dMinus_TimeAcc;
	m_dMinus_TimeAcc += dTimeDelta;
	_float fDriveFullTime = CUI_Manager::GetInstance()->Get_DriveFullTime();
	m_tParams.Floats[0] = 1 - (_float(m_dMinus_TimeAcc) / fDriveFullTime);

	if (0.0f > m_tParams.Floats[0])
	{
		m_tParams.Floats[0] = 0.0f;
		m_bDriveGauge_Minus = false;

		CUI_Manager::GetInstance()->Set_DriveGaugePlus(0.0f);
	}
}

CDrive_GaugeUI * CDrive_GaugeUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CDrive_GaugeUI*		pInstance = new CDrive_GaugeUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDrive_GaugeUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CDrive_GaugeUI::Clone(void * pArg)
{
	CDrive_GaugeUI*		pInstance = new CDrive_GaugeUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDrive_GaugeUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDrive_GaugeUI::Free()
{
	CUI::Free();

}
