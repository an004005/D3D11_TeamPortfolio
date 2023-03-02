#include "stdafx.h"
#include "..\public\Canvas_DriveMove.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "UI_Manager.h"

#include "Drive_GaugeUI.h"

CCanvas_DriveMove::CCanvas_DriveMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_DriveMove::CCanvas_DriveMove(const CCanvas_DriveMove& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_DriveMove::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_DriveMove::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_CCanvas(L"Canvas_DriveMove", this);
	CCanvas::UIMove_FSM();

	// Drive UI �� ��� ó������ ��� UI �� ������ �ʰ� �����Ѵ�.	
	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
	{
		iter->second->SetVisible(false);
	}

	return S_OK;
}

void CCanvas_DriveMove::BeginTick()
{

}

void CCanvas_DriveMove::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	m_pUIMoveFSM->Tick(TimeDelta);	// UI �� ������

	// ����̺� �������� ��� ����
	if (0.0f == CUI_Manager::GetInstance()->Get_DriveGauge())
	{
		m_bOnDrive_OneCheck = false;
		m_bOnDriveStart_OneCheck = false;
		return;
	}

	if (false == m_bOnDriveStart_OneCheck)
	{
		m_bOnDriveStart_OneCheck = true;

		dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_Gauge"))->Set_DriveGauge_Plus();
		dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_GaugeBack"))->Set_DriveGauge_Plus();
		Find_ChildUI(L"Drive_Gauge")->SetVisible(true);
		Find_ChildUI(L"Drive_GaugeBack")->SetVisible(true);
	}

	DriveGaugeFull_Tick();
	
	// ����̺긦 ����ϱ� �����ϸ�
	if (false == m_bOnDrive)
		return;

	if (m_bOnDrive_OneCheck == false)
	{
		m_bOnDrive_OneCheck = true;

		dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_Gauge"))->Set_DriveGauge_Minus();
		dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_GaugeBack"))->Set_DriveGauge_Minus();

		// �׳� Drive �ð����� ����ϸ� �ȴ�.
		Find_ChildUI(L"Drive_LeftDot")->SetVisible(m_bOnDrive);

		// Ư�� �ð��� ����ؾ��Ѵ�.
		Find_ChildUI(L"Drive_OnCircle")->SetVisible(m_bOnDrive);

		// ����̺� �ð����� 
		Find_ChildUI(L"Dirve_RightDot")->SetVisible(m_bOnDrive);
	}
}

void CCanvas_DriveMove::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_DriveMove::Render()
{
	if (FAILED(CCanvas::Render()))
		return E_FAIL;

	return S_OK;
}

void CCanvas_DriveMove::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	static _float fGauge;
	ImGui::InputFloat("Drive Gauge", &fGauge);
	CUI_Manager::GetInstance()->Set_DriveGaugePlus(fGauge);

	static _float fDriveFullTime;
	ImGui::InputFloat("Drive Full Time", &fDriveFullTime);
	CUI_Manager::GetInstance()->Set_DriveFullTime(fDriveFullTime);

	ImGui::Checkbox("OnDrive", &m_bOnDrive);
	ImGui::SameLine();
	ImGui::Checkbox("bOnCircle", &m_bOnCircle);

}

void CCanvas_DriveMove::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);

}

void CCanvas_DriveMove::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);

}

void CCanvas_DriveMove::Set_OnDrive(const _float & fDriveFullTime)
{	
	// �ܺο��� Drive �� �����Ѵ�.! �� �������� ���� á��� �Ѵ�.
	if (1.0f <= CUI_Manager::GetInstance()->Get_DriveGauge())
	{
		m_bOnDrive = true;
		CUI_Manager::GetInstance()->Set_DriveFullTime(fDriveFullTime);
	}
}

void CCanvas_DriveMove::DriveGaugeFull_Tick()
{
	// ����̺� �������� ���� á�� �� ������ �� ��ü�� ä������ �����ð� �� �ٽ� ������� ���ư���.
	_bool GaugeFull = dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_Gauge"))->Get_GaugeFull();
	if (true == GaugeFull)
	{
		Find_ChildUI(L"Dirve_RightDotFull")->SetVisible(true);
	}
	else
	{
		Find_ChildUI(L"Dirve_RightDotFull")->SetVisible(false);
	}
}

CCanvas_DriveMove * CCanvas_DriveMove::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_DriveMove*		pInstance = new CCanvas_DriveMove(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_DriveMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_DriveMove::Clone(void * pArg)
{
	CCanvas_DriveMove*		pInstance = new CCanvas_DriveMove(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_DriveMove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_DriveMove::Free()
{
	CCanvas::Free();

}
