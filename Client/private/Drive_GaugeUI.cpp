#include "stdafx.h"
#include "..\public\Drive_GaugeUI.h"
#include "GameInstance.h"
#include "PlayerInfoManager.h"

// Drive 게이지와 Drive Back 가 동시에 계산되고 있습니다. (즉 이 클래스로 생성된 객체가 2개 입니다.)
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

	// 염력을 사용하면서 염력 게이지를 채운다.
	// 일정 염력을 채우면 드라이브 사용이 가능해진다.
	// 드라이브를 사용하면 외부에서 받아온 전체 시간을 이용해서 게이지를 줄인다.

	DrivaGaugePlus_Tick(TimeDelta);
	DrivaGaugeUse_Tick(TimeDelta);
	DirveRightDot_Tick(TimeDelta);
}

void CDrive_GaugeUI::DrivaGaugePlus_Tick(const _double & dTimeDelta)
{
	if (0.0f == m_fDriveGauge)	// 외부에서 게이지가 증가하기 시작하면 아래서 계산을 시작한다.
		return;

	// 드라이브 게이지가 찬다. (0->1)

	if (m_tParams.Floats[0] < m_fDriveGauge)
	{
		m_tParams.Floats[0] += _float(dTimeDelta) * 0.05f;
	}
	else	// UITODO : 테스트 용도 일 때만 else 을 추가한다.
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

	// Gauge 가 가득 찼을 때 Right Dot 를 몇초 띄웠다가 없애는 용도
	// 원래라면 클래스를 하나 더 만들어야 하지만 몇초 있다가 끄는 기능 밖에 없어서 여기서 구현함다.

	m_dGaugeFull_TimeAcc += dTimeDelta;
	if (2.0f < m_dGaugeFull_TimeAcc)
	{
		m_bGaugeFull = false;
		m_dGaugeFull_TimeAcc = 0.0;
	}
}

void CDrive_GaugeUI::DrivaGaugeUse_Tick(const _double & dTimeDelta)
{
	if (false == m_bDriveGauge_Use)		// 외부에서 드라이브를 사용하면 들어온다.
		return;

	// 드라이브 게이지가 줄어든다. (1->0)

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
