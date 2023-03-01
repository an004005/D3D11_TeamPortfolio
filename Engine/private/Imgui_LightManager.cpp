#include "stdafx.h"
#include "..\public\Imgui_LightManager.h"
#include "Light_Manager.h"

CImgui_LightManager::CImgui_LightManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_LightManager::Initialize(void* pArg)
{
	m_szWindowName = "LightManager";
	return S_OK;
}

void CImgui_LightManager::Imgui_RenderWindow()
{
	CLight_Manager::GetInstance()->Imgui_Render();
}

CImgui_LightManager* CImgui_LightManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	auto inst = new CImgui_LightManager(pDevice, pContext);
	if (FAILED(inst->Initialize(pArg)))
	{
		return nullptr;
	}

	return inst;
}

void CImgui_LightManager::Free()
{
	CImguiObject::Free();
}
