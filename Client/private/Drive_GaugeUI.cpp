#include "stdafx.h"
#include "..\public\Drive_GaugeUI.h"
#include "GameInstance.h"
#include "PlayerInfoManager.h"

// Drive �������� Drive Back �� ���ÿ� ���ǰ� �ֽ��ϴ�. (�� �� Ŭ������ ������ ��ü�� 2�� �Դϴ�.)
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
	DrivaGaugeUse_Tick(TimeDelta);
	DirveRightDot_Tick(TimeDelta);
}

void CDrive_GaugeUI::DrivaGaugePlus_Tick(const _double & dTimeDelta)
{
	if (0.0f == m_fDriveGauge)	// �ܺο��� �������� �����ϱ� �����ϸ� �Ʒ��� ����� �����Ѵ�.
		return;

	// ����̺� �������� ����. (0->1)

	if (m_tParams.Floats[0] < m_fDriveGauge)
	{
		m_tParams.Floats[0] += _float(dTimeDelta) * 0.05f;
	}
	else	// UITODO : �׽�Ʈ �뵵 �� ���� else �� �߰��Ѵ�.
	{
		m_tParams.Floats[0] = m_fDriveGauge;
	}

	if (1.0f < m_tParams.Floats[0])
	{
		m_tParams.Floats[0] = 1.0f;
		m_bGaugeFull = true;
	}
}

void CDrive_GaugeUI::DirveRightDot_Tick(const _double & dTimeDelta)
{
	if (false == m_bGaugeFull)
		return;

	// Gauge �� ���� á�� �� Right Dot �� ���� ����ٰ� ���ִ� �뵵
	// ������� Ŭ������ �ϳ� �� ������ ������ ���� �ִٰ� ���� ��� �ۿ� ��� ���⼭ �����Դ�.

	m_dGaugeFull_TimeAcc += dTimeDelta;
	if (2.0f < m_dGaugeFull_TimeAcc)
	{
		m_bGaugeFull = false;
		m_dGaugeFull_TimeAcc = 0.0;
	}
}

void CDrive_GaugeUI::DrivaGaugeUse_Tick(const _double & dTimeDelta)
{
	if (false == m_bDriveGauge_Use)		// �ܺο��� ����̺긦 ����ϸ� ���´�.
		return;

	// ����̺� �������� �پ���. (1->0)

	m_dCurrentDriveTime += dTimeDelta;
	m_tParams.Floats[0] = m_fCurrentDriveGauge = 1.0f - (_float(m_dCurrentDriveTime) / m_fDriveFullTime);
	CPlayerInfoManager::GetInstance()->Set_DriveEnergy(m_tParams.Floats[0]);

	if (0.0f > m_fCurrentDriveGauge)
	{
		m_tParams.Floats[0] = 0.0f;
		m_fDriveGauge = 0.0f;
		m_fCurrentDriveGauge = 0.0f;
		m_dCurrentDriveTime = 0.0;
		m_bDriveGauge_Use = false;
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
