#include "stdafx.h"
#include "..\public\Imgui_PostProcess.h"
#include "HDR.h"
#include "GameInstance.h"


CImgui_PostProcess::CImgui_PostProcess(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_PostProcess::Initialize(void* pArg)
{
	m_szWindowName = "PostProcess Params";
	return S_OK;
}

void CImgui_PostProcess::Imgui_RenderWindow()
{
	CHDR::GetInstance()->Imgui_Render();
	ImGui::Separator();
	CGameInstance::GetInstance()->GetRenderer()->Imgui_RenderOtherWindow();
}

CImgui_PostProcess* CImgui_PostProcess::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	auto inst = new CImgui_PostProcess(pDevice, pContext);
	if (FAILED(inst->Initialize(pArg)))
	{
		return nullptr;
	}

	return inst;
}

void CImgui_PostProcess::Free()
{
	CImguiObject::Free();
}
