#include "stdafx.h"
#include "..\public\Canvas_BossHpMove.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "UI_Manager.h"

#include "Canvas_BossHp.h"
#include "Boss_HpUI.h"
#include "Boss_HpBackUI.h"
#include "Boss_ShildUI.h"

CCanvas_BossHpMove::CCanvas_BossHpMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_BossHpMove::CCanvas_BossHpMove(const CCanvas_BossHpMove& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_BossHpMove::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_BossHpMove::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_MoveCanvas(L"Canvas_BossHpMove", this);
	m_vMaxDestination = { 0.0f, -5.0f };
	CCanvas::UIMove_FSM();

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	return S_OK;
}

void CCanvas_BossHpMove::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	m_pUIMoveFSM->Tick(TimeDelta);	// UI 의 움직임
	CCanvas::UIHit(TimeDelta);

}

void CCanvas_BossHpMove::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_BossHpMove::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_BossHpMove::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	static _float fHp;
	ImGui::DragFloat("Hp", &fHp);
	Set_BossHp(fHp);

	static _float fShild;
	ImGui::DragFloat("Shild", &fShild);
	Set_BossShild(fShild);
}

void CCanvas_BossHpMove::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_BossHpMove::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

void CCanvas_BossHpMove::Set_BossHp(const _float & fHp)
{
	dynamic_cast<CCanvas_BossHp*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_BossHp"))->Set_BossHp();

	Find_ChildUI(L"Boss_Hp")->SetVisible(true);
	Find_ChildUI(L"Boss_HPBack")->SetVisible(true);
	dynamic_cast<CBoss_HpUI*>(Find_ChildUI(L"Boss_Hp"))->Set_BossHp(fHp);
	dynamic_cast<CBoss_HpBackUI*>(Find_ChildUI(L"Boss_HPBack"))->Set_BossHp(fHp);
}

void CCanvas_BossHpMove::Set_BossShild(const _float & fShild)
{
	Find_ChildUI(L"Boss_Shild")->SetVisible(true);
	dynamic_cast<CBoss_ShildUI*>(Find_ChildUI(L"Boss_Shild"))->Set_Shild(fShild);
}

CCanvas_BossHpMove * CCanvas_BossHpMove::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_BossHpMove*		pInstance = new CCanvas_BossHpMove(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_BossHpMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_BossHpMove::Clone(void * pArg)
{
	CCanvas_BossHpMove*		pInstance = new CCanvas_BossHpMove(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_BossHpMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_BossHpMove::Free()
{
	CCanvas::Free();
}
