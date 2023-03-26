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
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;		// Enable Gamepad Controls
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

	ImGuiStyle * pstyle = &ImGui::GetStyle();

	pstyle->WindowMinSize = ImVec2(160, 20);
	pstyle->FramePadding = ImVec2(10, 6);
	pstyle->WindowPadding = ImVec2(20, 20);

	pstyle->ItemSpacing = ImVec2(10, 6);
	pstyle->ItemInnerSpacing = ImVec2(6, 4);
	pstyle->Alpha = 0.95f;
	pstyle->WindowRounding = 4.0f;
	pstyle->FrameRounding = 2.0f;
	pstyle->IndentSpacing = 6.0f;
	pstyle->ItemInnerSpacing = ImVec2(10, 4);
	pstyle->ColumnsMinSpacing = 50.0f;
	pstyle->GrabMinSize = 14.0f;
	pstyle->GrabRounding = 16.0f;
	pstyle->ScrollbarSize = 12.0f;
	pstyle->ScrollbarRounding = 16.0f;
	pstyle->TabBorderSize = 1.f;
	pstyle->FrameBorderSize = 1.f;

	// pstyle->WindowPadding = ImVec2(15, 15);
	// pstyle->WindowRounding = 5.0f;
	// pstyle->FramePadding = ImVec2(5, 5);
	// pstyle->FrameRounding = 4.0f;
	// pstyle->ItemSpacing = ImVec2(12, 8);
	// pstyle->ItemInnerSpacing = ImVec2(8, 6);
	// pstyle->IndentSpacing = 25.0f;
	// pstyle->ScrollbarSize = 15.0f;
	// pstyle->ScrollbarRounding = 9.0f;
	// pstyle->GrabMinSize = 5.0f;
	// pstyle->GrabRounding = 3.0f;
	// pstyle->WindowRounding = 4.0f;
	// pstyle->FrameRounding = 2.0f;

	pstyle->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	pstyle->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	pstyle->Colors[ImGuiCol_WindowBg] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
	pstyle->Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	pstyle->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	pstyle->Colors[ImGuiCol_Border] = ImVec4(0.41f, 0.41f, 0.41f, 0.88f);
	pstyle->Colors[ImGuiCol_Tab] = ImVec4(0.30f, 0.30f, 0.33f, 1.f);
	pstyle->Colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	pstyle->Colors[ImGuiCol_TabActive] = ImVec4(0.36f, 0.36f, 0.38f, 1.00f);
	pstyle->Colors[ImGuiCol_TabUnfocused] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	pstyle->Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.46f, 0.46f, 0.48f, 1.00f);
	// pstyle->Alpha = 0.4f;

	pstyle->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	pstyle->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	pstyle->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	pstyle->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	pstyle->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	pstyle->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	pstyle->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	pstyle->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	pstyle->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	pstyle->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	pstyle->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	pstyle->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	pstyle->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	pstyle->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	pstyle->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	pstyle->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	pstyle->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	pstyle->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	pstyle->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	// pstyle->Colors[ImGuiCol_Hover] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);

	// pstyle->Colors[ImGuiCol_BorderShadow] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);

	pstyle->Colors[ImGuiCol_Header] = ImVec4(0.25f, 0.24f, 0.27f, 1.00f);
	pstyle->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	pstyle->Colors[ImGuiCol_HeaderActive] = ImVec4(0.36f, 0.35f, 0.37f, 1.00f);
	// pstyle->Colors[ImGuiCol_Header] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);

	pstyle->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	pstyle->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	pstyle->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	pstyle->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	pstyle->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	pstyle->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	pstyle->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	pstyle->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	pstyle->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
	m_pAlpha = &ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w;
	*m_pAlpha = 0.5f;
}


void CImgui_Manager::Tick_Imgui()
{
	if (!m_bOn)
		return;
	lock_guard<mutex> lockGuard(m_mtx);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	//ImGui::ShowStyleEditor();
}

void CImgui_Manager::Render_Imgui()
{
	if (!m_bOn)
		return;
	lock_guard<mutex> lockGuard(m_mtx);

	ImGui_DockSpace();

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
		////Alpha 
		ImGui::Separator();
		ImGui::SliderFloat("Alpha Slider", m_pAlpha, 0.f, 1.f);
		ImGui::Separator();
		////
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
	lock_guard<mutex> lockGuard(m_mtx);

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void CImgui_Manager::ImGui_DockSpace()
{
	ImGuiWindowFlags			WindowFlag = ImGuiWindowFlags_NoDocking;

	const ImGuiViewport*	Viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(Viewport->WorkPos);
	ImGui::SetNextWindowSize(Viewport->WorkSize);
	ImGui::SetNextWindowViewport(Viewport->ID);
	ImGui::SetNextWindowBgAlpha(0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	WindowFlag |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	WindowFlag |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	WindowFlag |= ImGuiDockNodeFlags_PassthruCentralNode;
	WindowFlag |= ImGuiWindowFlags_NoBackground;

	_bool	bIsShow = true;

	ImGui::Begin("DockSpace", &bIsShow, WindowFlag);
	ImGui::PopStyleVar(1);
	ImGui::PopStyleVar(2);

	ImGuiIO&	io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID	DockSpaceID = ImGui::GetID("DockSpace");
		ImGuiDockNodeFlags Flag = ImGuiDockNodeFlags_PassthruCentralNode;
		ImGui::DockSpace(DockSpaceID, ImVec2(0.f, 0.f), Flag);
	}

	ImGui::End();
}

void CImgui_Manager::Add_ImguiObject(CImguiObject* pImguiObject)
{
	lock_guard<mutex> lockGuard(m_mtx);
	if (pImguiObject)
		m_vecObj.push_back(pImguiObject);
}

void CImgui_Manager::Clear_ImguiObjects()
{
	lock_guard<mutex> lockGuard(m_mtx);
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
