#include "stdafx.h"
#include "..\public\Canvas_SAMouseLeft.h"
#include "GameInstance.h"

#include "ShaderUI.h"
#include "SA_AxisUI.h"

// Mouse : 아무것도 하지 않아도 된다.
// Arrow_Light : 아무것도 하지 않아도 된다.
// Mouse_Light : 사이 로 알파값 조절하기
// Arrow : 객체 내 에서 이루어짐. Set_Move() 을 통해 움직임을 멈출 수 있다.

CCanvas_SAMouseLeft::CCanvas_SAMouseLeft(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SAMouseLeft::CCanvas_SAMouseLeft(const CCanvas_SAMouseLeft& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SAMouseLeft::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_SAMouseLeft::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	Find_ChildUI(L"Circle")->SetVisible(false);
	Find_ChildUI(L"CircleLine2")->SetVisible(false);
	dynamic_cast<CSA_AxisUI*>(Find_ChildUI(L"Arrow"))->Set_AxisInitialize(AXIS::LEFT);

	return S_OK;
}

void CCanvas_SAMouseLeft::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	//KeyInput();
	MouseLight_Tick(TimeDelta);
	Action_Tick(TimeDelta);

	Find_ChildUI(L"Arrow_Light")->Set_Position(Find_ChildUI(L"Arrow")->Get_Position());	// 계속 그냥 따라다니도록
}

void CCanvas_SAMouseLeft::KeyInput()
{
	if (CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB))
	{
		m_bInput = true;
	}
}

void CCanvas_SAMouseLeft::MouseLight_Tick(const _double& TimeDelta)
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

	dynamic_cast<CShaderUI*>(Find_ChildUI(L"Mouse_Light"))->Set_Floats0(m_fMouseLightAlpha);
}

void CCanvas_SAMouseLeft::Action_Tick(const _double& TimeDelta)
{
	if (m_bInput == false)
	{
		// 마우스 삐쭉삐쭉 움직이기
		m_dMuseMove_TimeAcc += TimeDelta;
		if (1.0 < m_dMuseMove_TimeAcc)
		{
			m_dMuseMove_TimeAcc = 0.0;
			Find_ChildUI(L"Mouse")->Set_Position({20.0f, Find_ChildUI(L"Mouse")->Get_Position().y});
		}
		else
		{
			Find_ChildUI(L"Mouse")->Set_Position({ 25.0f, Find_ChildUI(L"Mouse")->Get_Position().y });
		}

		return;
	}

	dynamic_cast<CSA_AxisUI*>(Find_ChildUI(L"Arrow"))->Set_Move();	// 화살표 움직임 멈춰!

	if (0.5f > m_fMoveDistance) 
	{
		m_fMoveDistance += _float(TimeDelta);
		m_fX -= m_fMoveDistance * 20.0f;
	}

	_float fSizeSpeed = 100.0f;

	// 그냥 동그라미
	_float2 fCircle_Size = Find_ChildUI(L"Circle")->Get_Size();
	if (100.0f > fCircle_Size.x)
	{
		Find_ChildUI(L"Circle")->SetVisible(true);

		// 크기
		fCircle_Size += {_float(TimeDelta) * fSizeSpeed, _float(TimeDelta) * fSizeSpeed};
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
		fCircle2_Size += {_float(TimeDelta) * fSizeSpeed, _float(TimeDelta) * fSizeSpeed};
		Find_ChildUI(L"CircleLine2")->Set_Size(fCircle2_Size);
	}
	else
	{
		// 알파값
		_float fAlpha = dynamic_cast<CShaderUI*>(Find_ChildUI(L"CircleLine2"))->Get_Floats0();
		fAlpha -= _float(TimeDelta) * 2.0f;
		dynamic_cast<CShaderUI*>(Find_ChildUI(L"CircleLine2"))->Set_Floats0(fAlpha);
		if(0.0f > fAlpha)
			CGameObject::SetDelete();
	}
}

CCanvas_SAMouseLeft* CCanvas_SAMouseLeft::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCanvas_SAMouseLeft* pInstance = new CCanvas_SAMouseLeft(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SAMouseLeft");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas* CCanvas_SAMouseLeft::Clone(void* pArg)
{
	CCanvas_SAMouseLeft* pInstance = new CCanvas_SAMouseLeft(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SAMouseLeft");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SAMouseLeft::Free()
{
	CCanvas::Free();

}