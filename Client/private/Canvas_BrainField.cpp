#include "stdafx.h"
#include "..\public\Canvas_BrainField.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "PlayerInfoManager.h"

#include "Canvas_BrainFieldMove.h"

CCanvas_BrainField::CCanvas_BrainField(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_BrainField::CCanvas_BrainField(const CCanvas_BrainField& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_BrainField::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_BrainField::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	//for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
	//	iter->second->SetVisible(false);

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_BrainFieldMove.json");
	m_pCanvas_BrainFieldMove = dynamic_cast<CCanvas_BrainFieldMove*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"Canvas_BrainFieldMove", &json));
	assert(m_pCanvas_BrainFieldMove != nullptr && "Failed to Clone : CCanvas_BrainFieldMove");

	m_bVisible = true;

	return S_OK;
}

void CCanvas_BrainField::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);



}

void CCanvas_BrainField::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_BrainField::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_float fBrain = CPlayerInfoManager::GetInstance()->Get_PlayerStat().fBrainFieldMaintain;

	_tchar szText[MAX_PATH] = TEXT("");
	wsprintf(szText, TEXT("%d"), _int(fBrain));
	_float2 vPosition = Find_ChildUI(L"BrainFieldBase")->GetScreenSpaceLeftTop();

	_float fPosX = 0.0f;
	if (10 > fBrain) fPosX = 35.0f;
	else fPosX = 30.0f;
	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", szText, vPosition + _float2(fPosX, 96.0f), 0.f, { 0.4f, 0.4f }, { 1.0f, 0.99f, 0.87f, 1.0f });
	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", L"Sec", vPosition + _float2(58.0f, 98.0f), 0.f, { 0.35f, 0.35f }, { 1.0f, 0.99f, 0.87f, 1.0f });

	return S_OK;
}

void CCanvas_BrainField::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	ImGui::DragFloat("X", &mm.x);
	ImGui::DragFloat("Y", &mm.y);
}

void CCanvas_BrainField::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_BrainField::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_BrainField * CCanvas_BrainField::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_BrainField*		pInstance = new CCanvas_BrainField(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_BrainField");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_BrainField::Clone(void * pArg)
{
	CCanvas_BrainField*		pInstance = new CCanvas_BrainField(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_BrainField");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_BrainField::Free()
{
	CCanvas::Free();

	if(CGameInstance::GetInstance()->Check_ObjectAlive(m_pCanvas_BrainFieldMove))
		m_pCanvas_BrainFieldMove->SetDelete();
}
