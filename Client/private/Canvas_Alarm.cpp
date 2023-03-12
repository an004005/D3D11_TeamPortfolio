#include "stdafx.h"
#include "..\public\Canvas_Alarm.h"
#include "GameInstance.h"

#include "NextMapNameUI.h"

CCanvas_Alarm::CCanvas_Alarm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_Alarm::CCanvas_Alarm(const CCanvas_Alarm& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_Alarm::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_Alarm::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCanvas_Alarm::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);
	m_bVisible = true;
}

void CCanvas_Alarm::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_Alarm::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;


	return S_OK;
}

void CCanvas_Alarm::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	if (ImGui::Button("Open Next Map Name UI"))
	{
		Set_OpenNextRoomName(L"ภüภฮบน ธÞที");
	}
	ImGui::SameLine();
	if (ImGui::Button("Close Next Map Name UI"))
	{
		Set_CloseNextRoomName();
	}
}

void CCanvas_Alarm::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_Alarm::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

void CCanvas_Alarm::Set_OpenNextRoomName(const _tchar * pNextRoomName)
{
	dynamic_cast<CNextMapNameUI*>(Find_ChildUI(L"NextMapName"))->Set_NextRoolName(pNextRoomName);

}

void CCanvas_Alarm::Set_CloseNextRoomName()
{
	Find_ChildUI(L"NextMapName")->SetVisible(false);
}

CCanvas_Alarm * CCanvas_Alarm::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_Alarm*      pInstance = new CCanvas_Alarm(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_Alarm");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_Alarm::Clone(void * pArg)
{
	CCanvas_Alarm*      pInstance = new CCanvas_Alarm(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_Alarm");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_Alarm::Free()
{
	CCanvas::Free();
}