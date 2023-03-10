#include "stdafx.h"
#include "..\public\Imgui_CurveManager.h"
#include "CurveManager.h"

CImgui_CurveManager::CImgui_CurveManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_CurveManager::Initialize(void* pArg)
{
	m_szWindowName = "CurveManager";
	return S_OK;
}

void CImgui_CurveManager::Imgui_RenderWindow()
{
	CCurveManager::GetInstance()->Imgui_Render();
}

CImgui_CurveManager* CImgui_CurveManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	auto inst = new CImgui_CurveManager(pDevice, pContext);
	if (FAILED(inst->Initialize(pArg)))
	{
		return nullptr;
	}

	return inst;
}

void CImgui_CurveManager::Free()
{
	CImguiObject::Free();
}
