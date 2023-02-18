#pragma once
#include "Base.h"

BEGIN(Engine)

// imgui�ڵ带 �ۼ��ϰ� CImgui_Manager�� �����ϱ� ���� �θ�Ŭ����
class ENGINE_DLL CImguiObject abstract : public CBase
{
protected:
	CImguiObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg = nullptr);

	// imgui �����Լ� ���ϴ� ���·� ����, ImGui::Begin ���� �����ؾ��մϴ�.
	virtual void Imgui_FreeRender(){}

	// imgui manager�� tab�� �߰��� ��� ����
	virtual void Imgui_RenderTab(){}

	// imgui manager�� window�� �߰��� ��� ����
	virtual void Imgui_RenderWindow(){}

	/*
	 *  imgui manager�� menu bar�� �ְ� ���� ��� ����
	 * 	if (ImGui::BeginMenu("Windows"))
	 *	{
	 *		for (const auto& imObj : m_vecObj)
	 *			imObj->Imgui_Menu_OpenWindow();
	 *		ImGui::EndMenu();
	 *	}
	 *	ImGui::EndMenuBar();
	 *	���·� �����ϱ�.
	 */
	virtual void Imgui_RenderMenu(){}

	void Imgui_StartWindow();
	void Imgui_Menu_OpenWindow();

	const char* GetTabName() const { return m_szTabname; }
	const char* GetWindowName() const { return m_szWindowName; }
	_bool IsWindowOpened() const { return m_bOpenWindow; }

	virtual void Free() override;

protected:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

	const char* m_szTabname = "##";
	const char* m_szWindowName = "##";

	_bool m_bOpenWindow = false;
	ImGuiWindowFlags m_ImguiWindowFlags = 0;
};

END