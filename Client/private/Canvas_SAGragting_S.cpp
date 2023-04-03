#include "stdafx.h"
#include "..\public\Canvas_SAGragting_S.h"
#include "GameInstance.h"

#include "ShaderUI.h"
#include "SA_AxisUI.h"

// S: Ű �Է��� ������ Ŀ����.
// Arrow_Light : ȭ��ǥ�� ��� ����ٴѴ�.
// S_Light : ���İ� �����ϱ�
// Arrow : ��ü �� ���� �̷����. Set_Move() �� ���� �������� ���� �� �ִ�.

CCanvas_SAGragting_S::CCanvas_SAGragting_S(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SAGragting_S::CCanvas_SAGragting_S(const CCanvas_SAGragting_S& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SAGragting_S::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_SAGragting_S::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	Find_ChildUI(L"Circle")->SetVisible(false);
	Find_ChildUI(L"CircleLine2")->SetVisible(false);
	dynamic_cast<CSA_AxisUI*>(Find_ChildUI(L"Arrow"))->Set_AxisInitialize(AXIS::UP);

	return S_OK;
}

void CCanvas_SAGragting_S::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	KeyInput();
	MouseLight_Tick(TimeDelta);
	Action_Tick(TimeDelta);

	Find_ChildUI(L"Arrow_Light")->Set_Position(Find_ChildUI(L"Arrow")->Get_Position());	// ��� �׳� ����ٴϵ���
}

void CCanvas_SAGragting_S::KeyInput()
{
	if (CGameInstance::GetInstance()->KeyDown(DIK_X))
	{
		m_bInput = true;
	}
}

void CCanvas_SAGragting_S::MouseLight_Tick(const _double& TimeDelta)
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

void CCanvas_SAGragting_S::Action_Tick(const _double& TimeDelta)
{
	if (m_bInput == false)
	{
		// ���߻��� �����̱�
		m_dMuseMove_TimeAcc += TimeDelta;
		if (1.0 < m_dMuseMove_TimeAcc)
		{
			m_dMuseMove_TimeAcc = 0.0;
			Find_ChildUI(L"Base")->Set_Position({ Find_ChildUI(L"Base")->Get_Position().x, -95.0f });
		}
		else
		{
			Find_ChildUI(L"Base")->Set_Position({ Find_ChildUI(L"Base")->Get_Position().x,  -100.0f });
		}

		return;
	}

	dynamic_cast<CSA_AxisUI*>(Find_ChildUI(L"Arrow"))->Set_Move();	// ȭ��ǥ ������ ����!

	_float fSizeSpeed = 100.0f;

	// S �� ȭ��ǥ ũ�� ����
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

	// �׳� ���׶��
	_float2 fCircle_Size = Find_ChildUI(L"Circle")->Get_Size();
	if (100.0f > fCircle_Size.x)
	{
		Find_ChildUI(L"Circle")->SetVisible(true);

		// ũ��
		fCircle_Size += {_float(TimeDelta) * fSizeSpeed, _float(TimeDelta) * fSizeSpeed};
		Find_ChildUI(L"Circle")->Set_Size(fCircle_Size);

		// ���İ�
		_float fAlpha = dynamic_cast<CShaderUI*>(Find_ChildUI(L"Circle"))->Get_Floats0();
		fAlpha -= _float(TimeDelta);
		dynamic_cast<CShaderUI*>(Find_ChildUI(L"Circle"))->Set_Floats0(fAlpha);
	}
	else
	{
		Find_ChildUI(L"Circle")->SetVisible(false);
	}

	fSizeSpeed = 500.0f;
	// ���׶�̼� 2��
	_float2 fCircle2_Size = Find_ChildUI(L"CircleLine2")->Get_Size();
	if (500.0f > fCircle2_Size.x)
	{
		Find_ChildUI(L"CircleLine2")->SetVisible(true);

		// ũ��
		fCircle2_Size += {_float(TimeDelta) * fSizeSpeed, _float(TimeDelta) * fSizeSpeed};
		Find_ChildUI(L"CircleLine2")->Set_Size(fCircle2_Size);
	}
	else
	{
		// ���İ�
		_float fAlpha = dynamic_cast<CShaderUI*>(Find_ChildUI(L"CircleLine2"))->Get_Floats0();
		fAlpha -= _float(TimeDelta) * 2.0f;
		dynamic_cast<CShaderUI*>(Find_ChildUI(L"CircleLine2"))->Set_Floats0(fAlpha);
		if(0.0f > fAlpha)
			CGameObject::SetDelete();
	}
}

CCanvas_SAGragting_S* CCanvas_SAGragting_S::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCanvas_SAGragting_S* pInstance = new CCanvas_SAGragting_S(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SAGragting_S");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas* CCanvas_SAGragting_S::Clone(void* pArg)
{
	CCanvas_SAGragting_S* pInstance = new CCanvas_SAGragting_S(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SAGragting_S");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SAGragting_S::Free()
{
	CCanvas::Free();

}