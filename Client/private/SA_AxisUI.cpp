#include "stdafx.h"
#include "..\public\SA_AxisUI.h"
#include "GameInstance.h"
#include "JsonStorage.h"

// 상, 하, 좌, 우로 움직일 UI를 생성시에 정해주고, 이동시킨다.

CSA_AxisUI::CSA_AxisUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSA_AxisUI::CSA_AxisUI(const CSA_AxisUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSA_AxisUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSA_AxisUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	m_bMove = true;
	m_fStartX = m_fX;
	m_fStartY = m_fY;

	return S_OK;
}

void CSA_AxisUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);

	// 평상시에는 그냥 이동하고 있다가. 키 입력이 들어오면 이동하지 않는다.
	if (m_bMove == false) return;
	Type_Tick(TimeDelta);
	
}

void CSA_AxisUI::Type_Tick(const _double& TimeDelta)
{
	switch (m_eAxis)
	{
	case Client::AXIS::LEFT:
		Left_Tick(TimeDelta);
		break;
	case Client::AXIS::RIGHT:
		Right_Tick(TimeDelta);
		break;
	case Client::AXIS::UP:
		Up_Tick(TimeDelta);
		break;
	case Client::AXIS::DOWN:
		Down_Tick(TimeDelta);
		break;
	case Client::AXIS::RIGHT_ROTATION:
		Right_Rotation_Tick(TimeDelta);
		break;
	case Client::AXIS::LEFT_ROTATION:
		Left_Rotation_Tick(TimeDelta);
		break;
	default:
		//assert(!"No Axis Type"); 
		break;
	}
}

void CSA_AxisUI::Left_Tick(const _double& TimeDelta)
{
	m_fMove_TimeAcc += TimeDelta;
	if (m_fMoveTime < m_fMove_TimeAcc)
	{
		m_fX += _float(TimeDelta) * m_fSpeed;

		if (m_fX > m_fStartX)
		{
			m_fX = m_fStartX;
			m_fMove_TimeAcc = 0.0;
		}
	}
	else
	{
		m_fX -= _float(TimeDelta) * m_fSpeed;
	}
}

void CSA_AxisUI::Right_Tick(const _double& TimeDelta)
{
	m_fMove_TimeAcc += TimeDelta;
	if (m_fMoveTime < m_fMove_TimeAcc)
	{
		m_fX -= _float(TimeDelta) * m_fSpeed;

		if (m_fX < m_fStartX)
		{
			m_fX = m_fStartX;
			m_fMove_TimeAcc = 0.0;
		}
	}
	else
	{
		m_fX += _float(TimeDelta) * m_fSpeed;
	}
}

void CSA_AxisUI::Up_Tick(const _double& TimeDelta)
{
	m_fMove_TimeAcc += TimeDelta;
	if (m_fMoveTime < m_fMove_TimeAcc)
	{
		m_fY -= _float(TimeDelta) * m_fSpeed;

		if (m_fY < m_fStartY)
		{
			m_fY = m_fStartY;
			m_fMove_TimeAcc = 0.0;
		}
	}
	else
	{
		m_fY += _float(TimeDelta) * m_fSpeed;
	}
}

void CSA_AxisUI::Down_Tick(const _double& TimeDelta)
{
	m_fMove_TimeAcc += TimeDelta;
	if (m_fMoveTime < m_fMove_TimeAcc)
	{
		m_fY += _float(TimeDelta) * m_fSpeed;

		if (m_fY > m_fStartY)
		{
			m_fY = m_fStartY;
			m_fMove_TimeAcc = 0.0;
		}
	}
	else
	{
		m_fY -= _float(TimeDelta) * m_fSpeed;
	}
}

void CSA_AxisUI::Right_Rotation_Tick(const _double& TimeDelta)
{
	// Z 축을 기준으로 회전
	m_fRadianRotation -= _float(TimeDelta) * m_fSpeed;
	m_pTransformCom->Rotation({ 0.0f, 0.0f, 1.0f, 0.0f }, m_fRadianRotation);
}

void CSA_AxisUI::Left_Rotation_Tick(const _double& TimeDelta)
{
	// Z 축을 기준으로 회전
	m_fRadianRotation += _float(TimeDelta) * m_fSpeed;
	m_pTransformCom->Rotation({ 0.0f, 0.0f, 1.0f, 0.0f }, m_fRadianRotation);
}

CSA_AxisUI * CSA_AxisUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSA_AxisUI*		pInstance = new CSA_AxisUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSA_AxisUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSA_AxisUI::Clone(void * pArg)
{
	CSA_AxisUI*		pInstance = new CSA_AxisUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSA_AxisUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSA_AxisUI::Free()
{
	CUI::Free();

}
