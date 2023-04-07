#include "stdafx.h"
#include "..\public\Canvas_SAGragting_Go.h"
#include "GameInstance.h"

#include "ShaderUI.h"
#include "SA_AxisUI.h"

CCanvas_SAGragting_Go::CCanvas_SAGragting_Go(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SAGragting_Go::CCanvas_SAGragting_Go(const CCanvas_SAGragting_Go& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SAGragting_Go::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_SAGragting_Go::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;
	
	Find_ChildUI(L"Circle")->SetVisible(false);
	Find_ChildUI(L"CircleLine2")->SetVisible(false);
	dynamic_cast<CSA_AxisUI*>(Find_ChildUI(L"Arrow"))->Set_AxisInitialize(AXIS::UP);

	return S_OK;
}

void CCanvas_SAGragting_Go::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	KeyInput();
	MouseLight_Tick(TimeDelta);
	Action_Tick(TimeDelta);

	Find_ChildUI(L"Arrow_Light")->Set_Position(Find_ChildUI(L"Arrow")->Get_Position());	// 계속 그냥 따라다니도록
}

void CCanvas_SAGragting_Go::KeyInput()
{
	if (true == m_bKeyInput && 6 != m_iInputCount)
	{
		m_bKeyInput = false;
		++m_iInputCount;
		dynamic_cast<CShaderUI*>(Find_ChildUI(L"Gauge"))->Set_Float2s({ _float(m_iInputCount), 0.0f });

		if (6 == m_iInputCount)
		{
			Find_ChildUI(L"CircleLine3")->SetVisible(false);
			m_bInput = true;
		}
	}
}

void CCanvas_SAGragting_Go::MouseLight_Tick(const _double& TimeDelta)
{
	_float fSpeed = 1.0f;
	if (true == m_fAlphaDown)
	{
		if (0.1f > m_fMouseLightAlpha)
			m_fAlphaDown = false;
	}
	else
	{
		fSpeed *= -1;
		if (0.2f < m_fMouseLightAlpha)
			m_fAlphaDown = true;
	}
	m_fMouseLightAlpha -= _float(TimeDelta) * fSpeed;

	dynamic_cast<CShaderUI*>(Find_ChildUI(L"Base_Light"))->Set_Floats0(m_fMouseLightAlpha);
}

void CCanvas_SAGragting_Go::Action_Tick(const _double& TimeDelta)
{
	if (m_bInput == false)
	{
		// 삐쭉삐쭉 움직이기
		m_dMuseMove_TimeAcc += TimeDelta;
		if (1.0 < m_dMuseMove_TimeAcc)
		{
			m_dMuseMove_TimeAcc = 0.0;
			m_fY = -7.0f;
		}
		else
		{
			m_fY = -12.0f;
		}

		// 동그라미선 3개
		_float fSizeSpeed = 150.0f;
		_float2 fBase_Size = Find_ChildUI(L"CircleLine3")->Get_Size();
		if (150.0f > fBase_Size.x)
		{
			fBase_Size += {_float(TimeDelta)* fSizeSpeed, _float(TimeDelta)* fSizeSpeed};
			Find_ChildUI(L"CircleLine3")->Set_Size(fBase_Size);
		}
		else
		{
			Find_ChildUI(L"CircleLine3")->Set_Size({ 10.0f, 10.0f });
		}

		return;
	}

	dynamic_cast<CSA_AxisUI*>(Find_ChildUI(L"Arrow"))->Set_Move();	// 화살표 움직임 멈춰!

	_float fSizeSpeed = 100.0f;

	// S 와 화살표 크기 조절
	_float2 fBase_Size = Find_ChildUI(L"Base")->Get_Size();
	if (100.0f > fBase_Size.x)
	{
		fBase_Size += {_float(TimeDelta)* fSizeSpeed, _float(TimeDelta)* fSizeSpeed};
		Find_ChildUI(L"Base")->Set_Size(fBase_Size);
	}

	fBase_Size = Find_ChildUI(L"Arrow")->Get_Size();
	if (65.0f > fBase_Size.y)
	{
		fBase_Size += {_float(TimeDelta)* fSizeSpeed, _float(TimeDelta)* fSizeSpeed};
		Find_ChildUI(L"Arrow")->Set_Size(fBase_Size);
	}

	// 그냥 동그라미
	_float2 fCircle_Size = Find_ChildUI(L"Circle")->Get_Size();
	if (100.0f > fCircle_Size.x)
	{
		Find_ChildUI(L"Circle")->SetVisible(true);

		// 크기
		fCircle_Size += {_float(TimeDelta)* fSizeSpeed, _float(TimeDelta)* fSizeSpeed};
		Find_ChildUI(L"Circle")->Set_Size(fCircle_Size);

		// 알파값
		_float fAlpha = dynamic_cast<CShaderUI*>(Find_ChildUI(L"Circle"))->Get_Floats0();
		fAlpha -= _float(TimeDelta);
		dynamic_cast<CShaderUI*>(Find_ChildUI(L"Circle"))->Set_Floats0(fAlpha);
	}
	else
	{
		Find_ChildUI(L"Circle")->SetVisible(false);
	}

	fSizeSpeed = 500.0f;
	// 동그라미선 2개
	_float2 fCircle2_Size = Find_ChildUI(L"CircleLine2")->Get_Size();
	if (500.0f > fCircle2_Size.x)
	{
		Find_ChildUI(L"CircleLine2")->SetVisible(true);

		// 크기
		fCircle2_Size += {_float(TimeDelta)* fSizeSpeed, _float(TimeDelta)* fSizeSpeed};
		Find_ChildUI(L"CircleLine2")->Set_Size(fCircle2_Size);
	}
	else
	{
		// 알파값
		_float fAlpha = dynamic_cast<CShaderUI*>(Find_ChildUI(L"CircleLine2"))->Get_Floats0();
		fAlpha -= _float(TimeDelta) * 2.0f;
		dynamic_cast<CShaderUI*>(Find_ChildUI(L"CircleLine2"))->Set_Floats0(fAlpha);
		if (0.0f > fAlpha)
			CGameObject::SetDelete();
	}
}

CCanvas_SAGragting_Go* CCanvas_SAGragting_Go::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCanvas_SAGragting_Go* pInstance = new CCanvas_SAGragting_Go(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SAGragting_Go");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas* CCanvas_SAGragting_Go::Clone(void* pArg)
{
	CCanvas_SAGragting_Go* pInstance = new CCanvas_SAGragting_Go(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SAGragting_Go");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SAGragting_Go::Free()
{
	CCanvas::Free();

}