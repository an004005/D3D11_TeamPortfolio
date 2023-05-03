#include "stdafx.h"
#include "..\public\Canvas_DriveMove.h"
#include "GameInstance.h"
#include "FSMComponent.h"
#include "UI_Manager.h"
#include "PlayerInfoManager.h"

#include "Drive_GaugeUI.h"
#include "ShaderUI.h"
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

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
	{
		if ((*iter).first == L"Drive_Gauge" || 
			(*iter).first == L"Drive_GaugeBack")
			continue;

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
	CCanvas::UIHit(TimeDelta);
	
	if (true == CPlayerInfoManager::GetInstance()->Get_DriveGauge())
	{
		CPlayerInfoManager::GetInstance()->Set_DriveGauge(false);		

		Find_ChildUI(L"Drive_LeftDot")->SetVisible(true);
		Find_ChildUI(L"Dirve_RightDot")->SetVisible(true);
		Find_ChildUI(L"Drive_OnCircle")->SetVisible(true);
	}

	DriveGauge_Tikc();
	DriveGaugeFull_Tick();
	Drive_Tick();
}

void CCanvas_DriveMove::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

	static _float fGauge[2];
	ImGui::InputFloat2("Drive Gauge", fGauge);

	if (ImGui::Button("Set Drive"))
	{
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().fDriveEnergy = fGauge[0];
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().fMaxDriveEnergy = fGauge[1];
		//m_fDriveGauge = fGauge[0] / fGauge[1];
	}
	ImGui::SameLine();
	if (ImGui::Button("DriveGauge 100")) {
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().fDriveEnergy = 100.0f;
	}
	ImGui::SameLine();
	if (ImGui::Button("OnDrive")) {
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().bDriveMode = true;
	}
}

void CCanvas_DriveMove::Set_OnDrive(const _float & fDriveFullTime) 
{				
	// ����̺긦 ����� �ð� (Gauge �� ���� 0.05 �� õõ�� �پ���.)
	m_bOnDrive = true;

	// �׳� Drive �ð����� ����ϸ� �ȴ�.
	dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_Gauge"))->Set_DriveGauge_Use(fDriveFullTime);
	dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_GaugeBack"))->Set_DriveGauge_Use(fDriveFullTime);
}

void CCanvas_DriveMove::DriveGauge_Tikc()
{
	_float fDrive = CPlayerInfoManager::GetInstance()->Get_PlayerStat().fDriveEnergy;
	_float fMaxDrive = CPlayerInfoManager::GetInstance()->Get_PlayerStat().fMaxDriveEnergy;
	m_fDriveGauge = fDrive / fMaxDrive;

	dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_Gauge"))->Set_DriveGauge(m_fDriveGauge);
	dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_GaugeBack"))->Set_DriveGauge(m_fDriveGauge);

	if (0.999f <= m_fDriveGauge)
	{
		dynamic_cast<CCanvas_Drive*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Drive"))->Set_DriveB(true);
	}
}

void CCanvas_DriveMove::DriveGaugeFull_Tick()
{
	if (true == m_bRightFullDot) return;
	m_bRightFullDot = true;

	// ����̺� �������� ���� á�� �� ������ �� ��ü�� ä������ �����ð� �� �ٽ� ������� ���ư���.
	_bool GaugeFull = dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_Gauge"))->Get_GaugeFull();
	Find_ChildUI(L"Dirve_RightDotFull")->SetVisible(GaugeFull);
}

void CCanvas_DriveMove::Drive_Tick()
{
	if (false == m_bOnDrive)
		return;

	// ����̺긦 ����ϱ� �����ϸ�
	_float fDriveGauge = dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_Gauge"))->Get_CurrentDriveGauge();

	if (0.0f == fDriveGauge)	// ����̺� �ð��� ������.
	{
		m_bOnDrive = false;	// �Ʒ��� �������鼭 ��ü���� ��δ� ����.
		m_bRightFullDot = false;
		m_fDriveGauge = 0.0f;
		m_fRightDotCount = 1.0f;
		m_fShaderRightDot = 0.0f;
		Find_ChildUI(L"Drive_OnCircle")->SetVisible(false);
		Find_ChildUI(L"Drive_LeftDot")->SetVisible(false);
		Find_ChildUI(L"Dirve_RightDot")->SetVisible(false);
		dynamic_cast<CCanvas_Drive*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Drive"))->Set_DriveB(false);
	}

	//if (1.0f > fDriveGauge)
	//{
	//	// Ư�� �ð��� ����ؾ��Ѵ�. Gauge �� 0.5f �̸����� ���� �� ����� �ִ�. ( UITODO : ���� ����� �ϴ��� �� �𸣰ڴ� �׷��� �ϴ� ����̺갡 ������ ������.)
	//	Find_ChildUI(L"Drive_OnCircle")->SetVisible(m_bOnDrive);
	//	Find_ChildUI(L"Drive_LeftDot")->SetVisible(m_bOnDrive);
	//	Find_ChildUI(L"Dirve_RightDot")->SetVisible(m_bOnDrive);
	//}

	if (m_fRightDotCount > fDriveGauge)
	{
		// ����̺� 0.5f �� �� �� ���� �þ��.
		m_fRightDotCount -= 0.05f;
		dynamic_cast<CShaderUI*>(Find_ChildUI(L"Dirve_RightDot"))->Set_Floats0(m_fShaderRightDot);
		m_fShaderRightDot += 0.1f;
		//dynamic_cast<CDrive_RightDotUI*>(Find_ChildUI(L"Dirve_RightDot"))->Set_DriveCount();
	}
}

//void CCanvas_DriveMove::StartBrainField_Tick()
//{
//	if (false == m_bStartBrainField) return;
//	
//	if (false == m_bSetDriveGauge)
//	{
//		m_bSetDriveGauge = true;
//		dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_Gauge"))->Set_CurrentDriveGauge();
//		dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_GaugeBack"))->Set_CurrentDriveGauge();
//	}
//	
//	_float fDriveGauge = dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_Gauge"))->Get_CurrentDriveGauge();
//
//	dynamic_cast<CCanvas_Drive*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Drive"))->Set_DriveB(true);
//
//	if (0.0f == fDriveGauge)	// ����̺� �ð��� ������.
//	{
//		m_bSetDriveGauge = false;
//		m_bStartBrainField = false;
//		m_fDriveGauge = 0.0f;
//		m_fRightDotCount = 1.0f;
//		m_fShaderRightDot = 0.0f;
//		//Find_ChildUI(L"Drive_LeftDot")->SetVisible(m_bStartBrainField);
//		//Find_ChildUI(L"Dirve_RightDot")->SetVisible(m_bStartBrainField);
//		//Find_ChildUI(L"Drive_OnCircle")->SetVisible(m_bStartBrainField);
//
//		dynamic_cast<CCanvas_Drive*>(CUI_Manager::GetInstance()->Find_Canvas(L"Canvas_Drive"))->Set_DriveB(false);
//	}
//
//	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
//		iter->second->SetVisible(m_bStartBrainField);
//
//	_float fMaxBrainFieldMaintain = CPlayerInfoManager::GetInstance()->Get_PlayerStat().fMaxBrainFieldMaintain;
//	dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_Gauge"))->Set_DriveGauge_Use(fMaxBrainFieldMaintain);
//	dynamic_cast<CDrive_GaugeUI*>(Find_ChildUI(L"Drive_GaugeBack"))->Set_DriveGauge_Use(fMaxBrainFieldMaintain);
//
//	if (m_fRightDotCount > fDriveGauge)
//	{
//		// ����̺� 0.5f �� �� �� ���� �þ��.
//		m_fRightDotCount -= 0.08f;
//		m_fShaderRightDot += 0.1f;
//		dynamic_cast<CShaderUI*>(Find_ChildUI(L"Dirve_RightDot"))->Set_Floats0(m_fShaderRightDot);
//	}
//}

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
