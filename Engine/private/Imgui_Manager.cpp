#include "stdafx.h"
#include "..\public\Imgui_Manager.h"
#include "Graphic_Device.h"
#include "ImGuiFileDialog.h"
#include "ImguiObject.h"
#include "ImGuizmo.h"

IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager()
{
}

void CImgui_Manager::Ready_Imgui(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContextOut)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;		// Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	io.ConfigViewportsNoTaskBarIcon = true;

	ImGui::StyleColorsDark();
	
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContextOut;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pDeviceContext);
}


void CImgui_Manager::Tick_Imgui()
{
	if (!m_bOn)
		return;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
}

void CImgui_Manager::Render_Imgui()
{
	if (!m_bOn)
		return;

	if (m_vecObj.empty() == false)
	{
		// 마음대로 구현하기
		for (const auto& imObj : m_vecObj)
			imObj->Imgui_FreeRender();

		// 매니저 탭 윈도우 렌더 시작
		ImGui::Begin("TabWindow", nullptr, m_iWindowFlags);
		// 메뉴바 렌더
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Windows"))
			{
				for (const auto& imObj : m_vecObj)
					imObj->Imgui_Menu_OpenWindow();
				ImGui::EndMenu();
			}

			for (const auto& imObj : m_vecObj)
				imObj->Imgui_RenderMenu();

			ImGui::EndMenuBar();
		}

		// 탭 렌더
		if (ImGui::BeginTabBar("Manager_Tab", m_iTabBarFlags))
		{
			for (const auto& imObj : m_vecObj)
			{
				if (strcmp(imObj->GetTabName(), "##") == 0) // 이름 초기화 안하면 tab 생성 안함
					continue;

		        if (ImGui::BeginTabItem(imObj->GetTabName()))
		        {
					imObj->Imgui_RenderTab();
		            ImGui::EndTabItem();
		        }
			}
		    ImGui::EndTabBar();
		}
		ImGui::End();

		// 추가 윈도우 렌더
		for (const auto& imObj : m_vecObj)
			imObj->Imgui_StartWindow();
	}

	ImGui::Render();
}

void CImgui_Manager::Render_Update_ImGui()
{
	if (!m_bOn)
		return;

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void CImgui_Manager::Add_ImguiObject(CImguiObject* pImguiObject)
{
	if (pImguiObject)
		m_vecObj.push_back(pImguiObject);
}

void CImgui_Manager::Clear_ImguiObjects()
{
	for (auto& e : m_vecObj)
		Safe_Release(e);
	m_vecObj.clear();
}

void CImgui_Manager::Free()
{
	Clear_ImguiObjects();

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
