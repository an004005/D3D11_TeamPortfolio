#include "stdafx.h"
#include "..\public\Imgui_PhysX.h"
#include "PhysX_Manager.h"

CImgui_PhysX::CImgui_PhysX(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_PhysX::Initialize(void* pArg)
{
	m_szWindowName = "PhysX Params";
	return S_OK;
}

void CImgui_PhysX::Imgui_RenderWindow()
{
	CPhysX_Manager::GetInstance()->Imgui_RenderProperty();
}

CImgui_PhysX* CImgui_PhysX::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	auto inst = new CImgui_PhysX(pDevice, pContext);
	if (FAILED(inst->Initialize(pArg)))
	{
		return nullptr;
	}

	return inst;
}

void CImgui_PhysX::Free()
{
	CImguiObject::Free();
}
