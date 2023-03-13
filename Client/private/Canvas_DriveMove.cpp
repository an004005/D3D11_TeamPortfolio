#include "stdafx.h"
#include "..\public\Canvas_DriveMove.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "UI_Manager.h"

#include "Drive_GaugeUI.h"
#include "Drive_RightDotUI.h"
#include "Canvas_Drive.h"

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

	CUI_Manager::GetInstance()->Add_MoveCanvas(L"Canvas_DriveMove", this);
	m_vMaxDestination = { 2.0f, -7.0f };
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
	CCanvas::BeginTick();
}

void CCanvas_DriveMove::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	m_pUIMoveFSM->Tick(TimeDelta);	// UI �� ������

	// ����̺� �������� ��� ����
	if (0.0f == m_bDriveGauge)
	{
		m_bOnDriveStart_OneCheck = false;
		return;
	}

	if (false == m_bOnDriveStart_OneCheck)
	{
		m_bOnDriveStart_OneCheck = true;
		
		Find_ChildUI(L"Drive_Gauge")->SetVisible(true);
		Find_ChildUI(L"Drive_GaugeBack")->SetVisible(true);
		dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_Gauge"))->Set_DriveGauge(m_bDriveGauge);
		dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_GaugeBack"))->Set_DriveGauge(m_bDriveGauge);

		dynamic_cast<CCanvas_Drive*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Drive"))->Set_Drive(true);
	}

	DriveGaugeFull_Tick();
	Drive_Tick();
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

	static _float fGauge[2];
	ImGui::InputFloat2("Drive Gauge", fGauge);

	static _float fDriveFullTime;
	ImGui::InputFloat("Drive Full Time", &fDriveFullTime);

	if (ImGui::Button("Set Drive"))
	{
		m_bDriveGauge = fGauge[0] / fGauge[1];
	}
	ImGui::SameLine();
	if (ImGui::Button("OnDrive")) {
		Set_OnDrive(fDriveFullTime);	// ����̺� ���
	}

	ImGui::Checkbox("bOnCircle", &m_bOnCircle);		// ����̺� �� ���
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
	if (0.999f > m_bDriveGauge)	// ����! ����̺� �������� �� ä���� �Ѵ�.
		return;
			
	// ����̺긦 ����� �ð� (Gauge �� ���� 0.05 �� õõ�� �پ���.)
	m_bOnDrive = true;

	// �׳� Drive �ð����� ����ϸ� �ȴ�.
	Find_ChildUI(L"Drive_LeftDot")->SetVisible(true);

	dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_Gauge"))->Set_DriveGauge_Use(fDriveFullTime);
	dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_GaugeBack"))->Set_DriveGauge_Use(fDriveFullTime);
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

void CCanvas_DriveMove::Drive_Tick()
{
	if (false == m_bOnDrive)
		return;

	// ����̺긦 ����ϱ� �����ϸ�
	_float fDriveGauge = dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_Gauge"))->Get_CurrentDriveGauge();
	
	dynamic_cast<CCanvas_Drive*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Drive"))->Set_Drive(false);
	dynamic_cast<CCanvas_Drive*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Drive"))->Set_DriveB(true);
	dynamic_cast<CCanvas_Drive*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Drive"))->Set_DriveB(true);

	if (0.0f == fDriveGauge)	// ����̺� �ð��� ������.
	{
		m_bOnDrive = false;	// �Ʒ��� �������鼭 ��ü���� ��δ� ����.
		m_bDriveGauge = 0.0f;
		m_fRightDotCount = 1.0f;
		Find_ChildUI(L"Drive_LeftDot")->SetVisible(m_bOnDrive);
		dynamic_cast<CDrive_RightDotUI*>(Find_ChildUI(L"Dirve_RightDot"))->Reset_DriveRightDot();

		dynamic_cast<CCanvas_Drive*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Drive"))->Set_DriveB(false);
		dynamic_cast<CCanvas_Drive*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Drive"))->Set_DriveB(false);
	}

	Find_ChildUI(L"Dirve_RightDot")->SetVisible(m_bOnDrive);

	if (0.5f > fDriveGauge)
	{
		// Ư�� �ð��� ����ؾ��Ѵ�. Gauge �� 0.5f �̸����� ���� �� ����� �ִ�. ( UITODO : ���� ����� �ϴ��� �� �𸣰ڴ� �׷��� �ϴ� ����̺갡 ������ ������.)
		Find_ChildUI(L"Drive_OnCircle")->SetVisible(m_bOnDrive);
	}

	if (m_fRightDotCount > fDriveGauge)
	{
		// ����̺� 0.5f �� �� �� ���� �þ��.
		m_fRightDotCount -= 0.05f;
		dynamic_cast<CDrive_RightDotUI*>(Find_ChildUI(L"Dirve_RightDot"))->Set_DriveCount();
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
