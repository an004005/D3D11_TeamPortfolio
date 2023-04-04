#include "stdafx.h"
#include "Imgui_Cheat.h"
#include "GameInstance.h"
#include "Layer.h"
#include "ClientUtils.h"
#include "Enemy.h"

CImgui_Cheat::CImgui_Cheat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_Cheat::Initialize(void* pArg)
{
	return CImguiObject::Initialize(pArg);
}

void CImgui_Cheat::Imgui_RenderMenu()
{
	if (ImGui::BeginMenu("Cheats"))
	{
		if (ImGui::MenuItem("Kill All Enemy in Layer"))
		{
			if (auto pLayer = CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, PLAYERTEST_LAYER_MONSTER))
			{
				for (auto pObj : pLayer->GetGameObjects())
				{
					if (auto pEnemy = dynamic_cast<CEnemy*>(pObj))
						pEnemy->SetDelete();
				}
			}
			
		}
		ImGui::EndMenu();
	}
}

CImgui_Cheat* CImgui_Cheat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	auto inst = new CImgui_Cheat(pDevice, pContext);
	if (FAILED(inst->Initialize(pArg)))
		return nullptr;

	return inst;
}

void CImgui_Cheat::Free()
{
	CImguiObject::Free();
}
