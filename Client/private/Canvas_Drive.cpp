#include "stdafx.h"
#include "..\public\Canvas_Drive.h"
#include "GameInstance.h"

CCanvas_Drive::CCanvas_Drive(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_Drive::CCanvas_Drive(const CCanvas_Drive& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_Drive::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_Drive::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	// 처음에 보이지 않을 UI 들
	Find_ChildUI(L"Drive_B")->SetVisible(false);
	Find_ChildUI(L"Drive_Circle0")->SetVisible(false);
	Find_ChildUI(L"Drive_Circle1")->SetVisible(false);

	return S_OK;
}

void CCanvas_Drive::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	Find_ChildUI(L"Drive_Circle0")->SetVisible(m_bDriveB_OFF);

	Find_ChildUI(L"Drive_B")->SetVisible(m_bDriveB_ON);
	Find_ChildUI(L"Drive_Circle1")->SetVisible(m_bDriveB_ON);
}

void CCanvas_Drive::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	ImGui::Checkbox("DriveB_OFF", &m_bDriveB_OFF);
	ImGui::SameLine();
	ImGui::Checkbox("DriveB_ON", &m_bDriveB_ON);
}

void CCanvas_Drive::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_Drive::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

CCanvas_Drive * CCanvas_Drive::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_Drive*		pInstance = new CCanvas_Drive(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_Drive");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_Drive::Clone(void * pArg)
{
	CCanvas_Drive*		pInstance = new CCanvas_Drive(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_Drive");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_Drive::Free()
{
	CCanvas::Free();
}
