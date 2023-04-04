#include "stdafx.h"
#include "..\public\Canvas_SASSkill.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CCanvas_SASSkill::CCanvas_SASSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SASSkill::CCanvas_SASSkill(const CCanvas_SASSkill& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SASSkill::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_SASSkill::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_Canvas(L"Canvas_SASSkill", this);

	// 처음에 보이지 않을 UI 들
	Find_ChildUI(L"SASSkill_CtrlText1")->SetVisible(false);
	Find_ChildUI(L"SASSkill_CtrlTexture1")->SetVisible(false);

	Find_ChildUI(L"SASSkill_AltText1")->SetVisible(false);
	Find_ChildUI(L"SASSkill_AltTexture1")->SetVisible(false);

	Find_ChildUI(L"SASSkill_XRight")->SetVisible(false);
	Find_ChildUI(L"SASSkill_XInput")->SetVisible(false);

	Find_ChildUI(L"SASSkill_FullCircle1")->SetVisible(false);
	Find_ChildUI(L"SASSkill_FullCircle2")->SetVisible(false);

	return S_OK;
}

void CCanvas_SASSkill::Tick(_double TimeDelta)
{
	if (true == Find_ChildUI(L"SASSkill_BackGround")->Get_TempOff())
		return;

	__super::Tick(TimeDelta);
	
	InputAlt();
	InputCtrl();
	InputX_Tick(TimeDelta);
}

void CCanvas_SASSkill::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

}

void CCanvas_SASSkill::SaveToJson(Json& json)
{
	__super::SaveToJson(json);

}

void CCanvas_SASSkill::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

}

void CCanvas_SASSkill::InputX_Tick(const _double & dTimeDelta)
{
	if (CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB))
	{
		m_bChangeX = !m_bChangeX;
		m_bChangeXButton = true;

		// [m_bChangeX] false -> 오른쪽 true -> 왼쪽
		Find_ChildUI(L"SASSkill_XLeft")->SetVisible(!m_bChangeX);
		Find_ChildUI(L"SASSkill_XRight")->SetVisible(m_bChangeX);
	}

	if (true == m_bChangeXButton)
	{
		if (0.0 < m_dChangeX_TimcAcc)
		{
			Find_ChildUI(L"SASSkill_XInput")->SetVisible(true);
		}

		m_dChangeX_TimcAcc += dTimeDelta;

		if (0.2 < m_dChangeX_TimcAcc)
		{
			m_bChangeXButton = false;
			m_dChangeX_TimcAcc = 0.0;
			Find_ChildUI(L"SASSkill_XInput")->SetVisible(false);
		}
	}
}

void CCanvas_SASSkill::InputAlt()
{
	if (CGameInstance::GetInstance()->KeyPressing(DIK_LALT))
	{
		Find_ChildUI(L"SASSkill_AltText0")->SetVisible(false);
		Find_ChildUI(L"SASSkill_AltTexture0")->SetVisible(false);

		Find_ChildUI(L"SASSkill_AltText1")->SetVisible(true);
		Find_ChildUI(L"SASSkill_AltTexture1")->SetVisible(true);

		// Ctrl
		Find_ChildUI(L"SASSkill_CtrlText0")->SetVisible(true);
		Find_ChildUI(L"SASSkill_CtrlTexture0")->SetVisible(true);

		Find_ChildUI(L"SASSkill_CtrlText1")->SetVisible(false);
		Find_ChildUI(L"SASSkill_CtrlTexture1")->SetVisible(false);
	}
	if (CGameInstance::GetInstance()->KeyUp(DIK_LALT))
	{
		Find_ChildUI(L"SASSkill_AltText0")->SetVisible(true);
		Find_ChildUI(L"SASSkill_AltTexture0")->SetVisible(true);

		Find_ChildUI(L"SASSkill_AltText1")->SetVisible(false);
		Find_ChildUI(L"SASSkill_AltTexture1")->SetVisible(false);
	}
}

void CCanvas_SASSkill::InputCtrl()
{
	if (CGameInstance::GetInstance()->KeyPressing(DIK_LCONTROL))
	{
		Find_ChildUI(L"SASSkill_CtrlText0")->SetVisible(false);
		Find_ChildUI(L"SASSkill_CtrlTexture0")->SetVisible(false);

		Find_ChildUI(L"SASSkill_CtrlText1")->SetVisible(true);
		Find_ChildUI(L"SASSkill_CtrlTexture1")->SetVisible(true);

		// Alt
		Find_ChildUI(L"SASSkill_AltText0")->SetVisible(true);
		Find_ChildUI(L"SASSkill_AltTexture0")->SetVisible(true);

		Find_ChildUI(L"SASSkill_AltText1")->SetVisible(false);
		Find_ChildUI(L"SASSkill_AltTexture1")->SetVisible(false);
	}
	if (CGameInstance::GetInstance()->KeyUp(DIK_LCONTROL))
	{
		Find_ChildUI(L"SASSkill_CtrlText0")->SetVisible(true);
		Find_ChildUI(L"SASSkill_CtrlTexture0")->SetVisible(true);

		Find_ChildUI(L"SASSkill_CtrlText1")->SetVisible(false);
		Find_ChildUI(L"SASSkill_CtrlTexture1")->SetVisible(false);
	}
}

CCanvas_SASSkill * CCanvas_SASSkill::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_SASSkill*		pInstance = new CCanvas_SASSkill(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SASSkill");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_SASSkill::Clone(void * pArg)
{
	CCanvas_SASSkill*		pInstance = new CCanvas_SASSkill(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SASSkill");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SASSkill::Free()
{
	__super::Free();

}
