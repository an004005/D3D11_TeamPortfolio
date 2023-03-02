#include "stdafx.h"
#include "..\public\Canvas_SASSkill.h"
#include "GameInstance.h"
#include "GameUtils.h"

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

void CCanvas_SASSkill::BeginTick()
{


}

void CCanvas_SASSkill::Tick(_double TimeDelta)
{
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
	// TODO : Text 용 삭제해야 합니당.
	// 만약에 UI 에서 하는 것 이라면 매개변수로 _bool 받아와서 할 것! -> 계속 Tick() 을 돌고 있음 생각하기
	if (CGameInstance::GetInstance()->KeyDown(DIK_X))
	{
		m_bChangeX = !m_bChangeX;
		m_bChangeXButton = true;
	}
	// 요기까지

	if (true == m_bChangeX)	// 오른쪽
	{
		Find_ChildUI(L"SASSkill_XLeft")->SetVisible(false);
		Find_ChildUI(L"SASSkill_XRight")->SetVisible(true);
	}
	else					// 왼쪽
	{
		Find_ChildUI(L"SASSkill_XLeft")->SetVisible(true);
		Find_ChildUI(L"SASSkill_XRight")->SetVisible(false);
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
	if (CGameInstance::GetInstance()->KeyDown(DIK_LALT))
	{
		m_bKeyInput[0] = true;
		m_bKeyInput[1] = false;
	}
	if (CGameInstance::GetInstance()->KeyUp(DIK_LALT))
	{
		m_bKeyInput[0] = false;
	}

	Find_ChildUI(L"SASSkill_AltText0")->SetVisible(!m_bKeyInput[0]);
	Find_ChildUI(L"SASSkill_AltTexture0")->SetVisible(!m_bKeyInput[0]);

	Find_ChildUI(L"SASSkill_AltText1")->SetVisible(m_bKeyInput[0]);
	Find_ChildUI(L"SASSkill_AltTexture1")->SetVisible(m_bKeyInput[0]);
}

void CCanvas_SASSkill::InputCtrl()
{
	if (CGameInstance::GetInstance()->KeyDown(DIK_LCONTROL))
	{
		m_bKeyInput[0] = false;
		m_bKeyInput[1] = true;
	}
	if (CGameInstance::GetInstance()->KeyUp(DIK_LCONTROL))
	{
		m_bKeyInput[1] = false;
	}

	Find_ChildUI(L"SASSkill_CtrlText0")->SetVisible(!m_bKeyInput[1]);
	Find_ChildUI(L"SASSkill_CtrlTexture0")->SetVisible(!m_bKeyInput[1]);

	Find_ChildUI(L"SASSkill_CtrlText1")->SetVisible(m_bKeyInput[1]);
	Find_ChildUI(L"SASSkill_CtrlTexture1")->SetVisible(m_bKeyInput[1]);
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
