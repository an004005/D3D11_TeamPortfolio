#include "stdafx.h"
#include "..\public\Canvas_SASSkillMove.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "UI_Manager.h"

CCanvas_SASSkillMove::CCanvas_SASSkillMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_SASSkillMove::CCanvas_SASSkillMove(const CCanvas_SASSkillMove& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_SASSkillMove::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_SASSkillMove::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_MoveCanvas(L"Canvas_SASSkillMove", this);
	CCanvas::UIMove_FSM();
	
	// 처음에 보이지 않을 UI 들

	Find_ChildUI(L"SASSkill_IconNumber1")->SetVisible(false);
	Find_ChildUI(L"SASSkill_IconNumber2")->SetVisible(false);
	Find_ChildUI(L"SASSkill_IconNumber3")->SetVisible(false);
	Find_ChildUI(L"SASSkill_IconNumber4")->SetVisible(false);

	Find_ChildUI(L"SASSKill_ColleagueName1")->SetVisible(false);
	Find_ChildUI(L"SASSKill_ColleagueName2")->SetVisible(false);
	Find_ChildUI(L"SASSKill_ColleagueName3")->SetVisible(false);
	Find_ChildUI(L"SASSKill_ColleagueName4")->SetVisible(false);

	Find_ChildUI(L"SASSKill_SuperPower1")->SetVisible(false);
	Find_ChildUI(L"SASSKill_SuperPower2")->SetVisible(false);
	Find_ChildUI(L"SASSKill_SuperPower3")->SetVisible(false);
	Find_ChildUI(L"SASSKill_SuperPower4")->SetVisible(false);

	return S_OK;
}

void CCanvas_SASSkillMove::BeginTick()
{

}

void CCanvas_SASSkillMove::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);
	m_pUIMoveFSM->Tick(TimeDelta);

	Info_Tick();
	InputCtrl_Tick();
	InputAlt_Tick();
}

void CCanvas_SASSkillMove::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_SASSkillMove::Render()
{
	if (FAILED(CCanvas::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_SASSkillMove::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();
}

void CCanvas_SASSkillMove::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);

}

void CCanvas_SASSkillMove::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);

}

void CCanvas_SASSkillMove::Info_Tick()
{
	if (true == m_bCtrl || true == m_bAlt)
	{
		Find_ChildUI(L"SASSkill_Icon1")->SetVisible(false);
		Find_ChildUI(L"SASSkill_Icon2")->SetVisible(false);
		Find_ChildUI(L"SASSkill_Icon3")->SetVisible(false);
		Find_ChildUI(L"SASSkill_Icon4")->SetVisible(false);

		Find_ChildUI(L"SASSkill_IconNumber1")->SetVisible(true);
		Find_ChildUI(L"SASSkill_IconNumber2")->SetVisible(true);
		Find_ChildUI(L"SASSkill_IconNumber3")->SetVisible(true);
		Find_ChildUI(L"SASSkill_IconNumber4")->SetVisible(true);

	}
	else
	{
		Find_ChildUI(L"SASSkill_Icon1")->SetVisible(true);
		Find_ChildUI(L"SASSkill_Icon2")->SetVisible(true);
		Find_ChildUI(L"SASSkill_Icon3")->SetVisible(true);
		Find_ChildUI(L"SASSkill_Icon4")->SetVisible(true);

		Find_ChildUI(L"SASSkill_IconNumber1")->SetVisible(false);
		Find_ChildUI(L"SASSkill_IconNumber2")->SetVisible(false);
		Find_ChildUI(L"SASSkill_IconNumber3")->SetVisible(false);
		Find_ChildUI(L"SASSkill_IconNumber4")->SetVisible(false);

	}
}

void CCanvas_SASSkillMove::InputCtrl_Tick()
{
	// Ctrel 입력
	if (CGameInstance::GetInstance()->KeyDown(DIK_LCONTROL))
	{
		m_bCtrl = true;
		m_bAlt = false;
	}

	if (CGameInstance::GetInstance()->KeyUp(DIK_LCONTROL))
	{
		m_bCtrl = false;
	}

	Find_ChildUI(L"SASSKill_ColleagueName1")->SetVisible(m_bCtrl);
	Find_ChildUI(L"SASSKill_ColleagueName2")->SetVisible(m_bCtrl);
	Find_ChildUI(L"SASSKill_ColleagueName3")->SetVisible(m_bCtrl);
	Find_ChildUI(L"SASSKill_ColleagueName4")->SetVisible(m_bCtrl);
}

void CCanvas_SASSkillMove::InputAlt_Tick()
{
	// Alt 눌렀을 때 보여야 하는 UI
	if (CGameInstance::GetInstance()->KeyDown(DIK_LALT))
	{
		m_bCtrl = false; 
		m_bAlt = true;
	}
	// Alt 누르지 않을 때 보여야 하는 UI
	if (CGameInstance::GetInstance()->KeyUp(DIK_LALT))
	{
		m_bAlt = false;
	}

	Find_ChildUI(L"SASSKill_SuperPower1")->SetVisible(m_bAlt);
	Find_ChildUI(L"SASSKill_SuperPower2")->SetVisible(m_bAlt);
	Find_ChildUI(L"SASSKill_SuperPower3")->SetVisible(m_bAlt);
	Find_ChildUI(L"SASSKill_SuperPower4")->SetVisible(m_bAlt);
}

CCanvas_SASSkillMove * CCanvas_SASSkillMove::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_SASSkillMove*		pInstance = new CCanvas_SASSkillMove(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_SASSkillMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_SASSkillMove::Clone(void * pArg)
{
	CCanvas_SASSkillMove*		pInstance = new CCanvas_SASSkillMove(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_SASSkillMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_SASSkillMove::Free()
{
	CCanvas::Free();
}
