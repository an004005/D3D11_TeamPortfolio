#include "stdafx.h"
#include "..\public\Canvas_BossHp.h"
#include "GameInstance.h"

#include "Boss_HpUI.h"
#include "Boss_HpBackUI.h"
#include "Boss_ShildUI.h"

CCanvas_BossHp::CCanvas_BossHp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_BossHp::CCanvas_BossHp(const CCanvas_BossHp& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_BossHp::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_BossHp::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	return S_OK;
}

void CCanvas_BossHp::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

}

void CCanvas_BossHp::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_BossHp::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_BossHp::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	static _float fHp;
	ImGui::DragFloat("Hp", &fHp);
	Set_BossHp(fHp);

	static _float fShild;
	ImGui::DragFloat("Shild", &fShild);
	Set_BossShild(fShild);
}

void CCanvas_BossHp::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_BossHp::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

void CCanvas_BossHp::Set_BossUIRender()
{
	Find_ChildUI(L"Boss_Hp")->SetVisible(false);
	Find_ChildUI(L"Boss_HPBack")->SetVisible(false);
	Find_ChildUI(L"Boss_Shild")->SetVisible(false);
}

void CCanvas_BossHp::Set_BossHp(const _float & fHp)
{
	Find_ChildUI(L"Boss_Hp")->SetVisible(true);
	Find_ChildUI(L"Boss_HPBack")->SetVisible(true);
	dynamic_cast<CBoss_HpUI*>(Find_ChildUI(L"Boss_Hp"))->Set_BossHp(fHp);
	dynamic_cast<CBoss_HpBackUI*>(Find_ChildUI(L"Boss_HPBack"))->Set_BossHp(fHp);
}

void CCanvas_BossHp::Set_BossShild(const _float & fShild)
{
	Find_ChildUI(L"Boss_Shild")->SetVisible(true);
	dynamic_cast<CBoss_ShildUI*>(Find_ChildUI(L"Boss_Shild"))->Set_Shild(fShild);
}

CCanvas_BossHp * CCanvas_BossHp::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_BossHp*		pInstance = new CCanvas_BossHp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_BossHp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_BossHp::Clone(void * pArg)
{
	CCanvas_BossHp*		pInstance = new CCanvas_BossHp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_BossHp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_BossHp::Free()
{
	CCanvas::Free();
}
