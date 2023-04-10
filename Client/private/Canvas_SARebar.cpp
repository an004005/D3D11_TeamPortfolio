#include "stdafx.h"
#include "..\public\Canvas_SARebar.h"
#include "GameInstance.h"

#include "ShaderUI.h"
#include "SA_AxisUI.h"

CCanvas_SARebar::CCanvas_SARebar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SARebar::CCanvas_SARebar(const CCanvas_SARebar& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SARebar::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_SARebar::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;
	
	Find_ChildUI(L"Circle")->SetVisible(false);
	Find_ChildUI(L"CircleLine2")->SetVisible(false);
	dynamic_cast<CSA_AxisUI*>(Find_ChildUI(L"Arrow"))->Set_AxisInitialize(AXIS::RIGHT_ROTATION, 10.0f);

	return S_OK;
}

void CCanvas_SARebar::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	//KeyInput();
	MouseLight_Tick(TimeDelta);
	Action_Tick(TimeDelta);

	Find_ChildUI(L"Arrow_Light")->Set_Position(Find_ChildUI(L"Arrow")->Get_Position());	// 계속 그냥 따라다니도록
	Find_ChildUI(L"Base_Light")->Set_Position(Find_ChildUI(L"Base")->Get_Position());
}

void CCanvas_SARebar::KeyInput()
{
	if (CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB))
	{
		m_bInput = true;
	}
}

void CCanvas_SARebar::MouseLight_Tick(const _double& TimeDelta)
{
	_float fSpeed = 3.0f;
	if (true == m_fAlphaDown)
	{
		if (0.1f > m_fMouseLightAlpha)
			m_fAlphaDown = false;
	}
	else
	{
		fSpeed *= -1;
		if (0.3f < m_fMouseLightAlpha)
			m_fAlphaDown = true;
	}
	m_fMouseLightAlpha -= _float(TimeDelta) * fSpeed;

	dynamic_cast<CShaderUI*>(Find_ChildUI(L"Base_Light"))->Set_Floats0(m_fMouseLightAlpha);
}

void CCanvas_SARebar::Action_Tick(const _double& TimeDelta)
{
	if (m_bInput == false)
	{	
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

	_float fSizeSpeed = 100.0f;

	// 그냥 동그라미
	_float2 fCircle_Size = Find_ChildUI(L"Circle")->Get_Size();
	if (100.0f > fCircle_Size.x)
	{
		Find_ChildUI(L"Circle")->SetVisible(true);

		fCircle_Size += {_float(TimeDelta)* fSizeSpeed, _float(TimeDelta)* fSizeSpeed};
		Find_ChildUI(L"Circle")->Set_Size(fCircle_Size);

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

		fCircle2_Size += {_float(TimeDelta)* fSizeSpeed, _float(TimeDelta)* fSizeSpeed};
		Find_ChildUI(L"CircleLine2")->Set_Size(fCircle2_Size);
	}
	else
	{
		_float fAlpha = dynamic_cast<CShaderUI*>(Find_ChildUI(L"CircleLine2"))->Get_Floats0();
		fAlpha -= _float(TimeDelta) * 2.0f;
		dynamic_cast<CShaderUI*>(Find_ChildUI(L"CircleLine2"))->Set_Floats0(fAlpha);
		
		if (0.0f > fAlpha)
			CGameObject::SetDelete();
	}
}

CCanvas_SARebar* CCanvas_SARebar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCanvas_SARebar* pInstance = new CCanvas_SARebar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SARebar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas* CCanvas_SARebar::Clone(void* pArg)
{
	CCanvas_SARebar* pInstance = new CCanvas_SARebar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SARebar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SARebar::Free()
{
	CCanvas::Free();

}