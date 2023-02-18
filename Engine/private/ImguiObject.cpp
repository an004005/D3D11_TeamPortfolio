#include "stdafx.h"
#include "..\public\ImguiObject.h"

CImguiObject::CImguiObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CImguiObject::Initialize(void* pArg)
{
	return S_OK;
}

void CImguiObject::Imgui_StartWindow()
{
	if (m_bOpenWindow)
	{
		if (!ImGui::Begin(m_szWindowName, &m_bOpenWindow, m_ImguiWindowFlags))
		{
			ImGui::End();
		}
		else
		{
			Imgui_RenderWindow();
			ImGui::End();
		}
	}
}

void CImguiObject::Imgui_Menu_OpenWindow()
{
	if (strcmp(m_szWindowName, "##") != 0)
		ImGui::MenuItem(m_szWindowName, nullptr, &m_bOpenWindow);
}

void CImguiObject::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
