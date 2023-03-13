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

	// Drive UI 의 경우 처음부터 모든 UI 를 보이지 않고 시작한다.	
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

	m_pUIMoveFSM->Tick(TimeDelta);	// UI 의 움직임

	// 드라이브 게이지를 계산 시작
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
		Set_OnDrive(fDriveFullTime);	// 드라이브 사용
	}

	ImGui::Checkbox("bOnCircle", &m_bOnCircle);		// 드라이브 원 사용
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
	if (0.999f > m_bDriveGauge)	// 조건! 드라이브 게이지를 다 채워야 한다.
		return;
			
	// 드라이브를 사용할 시간 (Gauge 안 에서 0.05 로 천천히 줄어든다.)
	m_bOnDrive = true;

	// 그냥 Drive 시간동안 출력하면 된다.
	Find_ChildUI(L"Drive_LeftDot")->SetVisible(true);

	dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_Gauge"))->Set_DriveGauge_Use(fDriveFullTime);
	dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_GaugeBack"))->Set_DriveGauge_Use(fDriveFullTime);
}

void CCanvas_DriveMove::DriveGaugeFull_Tick()
{
	// 드라이브 게이지가 가득 찼을 때 오른쪽 점 전체가 채워지고 일정시간 뒤 다시 원래대로 돌아간다.
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

	// 드라이브를 사용하기 시작하면
	_float fDriveGauge = dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_Gauge"))->Get_CurrentDriveGauge();
	
	dynamic_cast<CCanvas_Drive*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Drive"))->Set_Drive(false);
	dynamic_cast<CCanvas_Drive*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Drive"))->Set_DriveB(true);
	dynamic_cast<CCanvas_Drive*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Drive"))->Set_DriveB(true);

	if (0.0f == fDriveGauge)	// 드라이브 시간이 끝났다.
	{
		m_bOnDrive = false;	// 아래로 내려가면서 객체를들 모두다 끈다.
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
		// 특정 시간에 출력해야한다. Gauge 를 0.5f 미만으로 있을 때 사용할 있다. ( UITODO : 언제 꺼줘야 하는지 잘 모르겠다 그래서 일단 드라이브가 꺼지면 꺼진다.)
		Find_ChildUI(L"Drive_OnCircle")->SetVisible(m_bOnDrive);
	}

	if (m_fRightDotCount > fDriveGauge)
	{
		// 드라이브 0.5f 가 될 때 까지 늘어난다.
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
