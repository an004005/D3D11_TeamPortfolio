#include "stdafx.h"
#include "..\public\Canvas_Drive.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "DefaultUI.h"

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

	CUI_Manager::GetInstance()->Add_Canvas(L"Canvas_Drive", this);

	// 처음에 보이지 않을 UI 들
	Find_ChildUI(L"Drive_B")->SetVisible(false);
	Find_ChildUI(L"Drive_Circle0")->SetVisible(false);
	Find_ChildUI(L"Drive_Circle1")->SetVisible(false);

	m_bVisible = true;

	return S_OK;
}

void CCanvas_Drive::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

}

void CCanvas_Drive::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_Drive::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_float2 vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"Drive_BackGround"))->GetScreenSpaceLeftTop();
	CGameInstance::GetInstance()->Render_Font(L"Pretendard32", L"DRIVE", vPosition + _float2(40.0f, 99.0f), 0.f, { 0.33f, 0.35f }, { 1.0f, 0.99f, 0.87f, 1.0f });

	return S_OK;
}

void CCanvas_Drive::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	static _float fPosition[2];
	ImGui::DragFloat2("Font Position", fPosition);
	m_vFontPos = { fPosition[0], fPosition[1] };
}

void CCanvas_Drive::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_Drive::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

void CCanvas_Drive::Set_Drive(const _bool Drive)
{
	Find_ChildUI(L"Drive_Circle0")->SetVisible(Drive);	// 드라이브 계산을 시작하면!
}

void CCanvas_Drive::Set_DriveB(const _bool DriveB)
{
	Find_ChildUI(L"Drive_B")->SetVisible(DriveB);			// 드라이게이지를 사용하고 반 이하로 남았을 때
	Find_ChildUI(L"Drive_Circle1")->SetVisible(DriveB);
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
