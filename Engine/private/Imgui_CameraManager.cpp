#include "stdafx.h"
#include "..\public\Imgui_CameraManager.h"
#include "Camera_Manager.h"

CImgui_CameraManager::CImgui_CameraManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_CameraManager::Initialize(void* pArg)
{
	m_szWindowName = "CameraManager";

	return S_OK;
}

void CImgui_CameraManager::Imgui_RenderWindow()
{
	CCamera_Manager::GetInstance()->Imgui_Render();
}

CImgui_CameraManager* CImgui_CameraManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	auto inst = new CImgui_CameraManager(pDevice, pContext);
	if (FAILED(inst->Initialize(pArg)))
	{
		return nullptr;
	}

	return inst;
}

void CImgui_CameraManager::Free()
{
	CImguiObject::Free();
}
