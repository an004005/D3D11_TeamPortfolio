#include "stdafx.h"
#include "Imgui_CamAnimEditor.h"
#include "GameInstance.h"
#include "AnimCam.h"
#include "Camera_Manager.h"

CImgui_CamAnimEditor::CImgui_CamAnimEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_CamAnimEditor::Initialize(void* pArg)
{
	m_szWindowName = "Cam Anim Editor Window";
	m_szTabname = "Cam Anim Editor Tab";

	m_pAnimCam = dynamic_cast<CAnimCam*>(CCamera_Manager::GetInstance()->Add_Camera("EditorCam", LEVEL_NOW, L"Layer_CamAnimEditor", L"Prototype_AnimCam", nullptr));
	Assert(m_pAnimCam != nullptr);

	return S_OK;
}

void CImgui_CamAnimEditor::Imgui_RenderWindow()
{
	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pAnimCam) == false)
		return;

	if (m_pCurAnim)
	{
		if (ImGui::Button("Start Anim"))
		{
			m_pAnimCam->StartCamAnim(m_pCurAnim,
				_float4x4::Identity,
				m_pAnimCam->GetTransform()->Get_WorldMatrix_f4x4());
		}

		m_pCurAnim->Imgui_Render();
		m_pCurAnim->Imgui_RenderFrameData();
	}
}

void CImgui_CamAnimEditor::Imgui_RenderTab()
{
	if (ImGui::CollapsingHeader("Cam Properties"))
	{
		m_pAnimCam->Imgui_RenderProperty();
		m_pAnimCam->Imgui_RenderComponentProperties();
	}
	ImGui::Separator();


	ImGui::Separator();
	static char szCamAnimName[MAX_PATH]{};
	ImGui::InputText("CamAnimName", szCamAnimName, MAX_PATH);
	if (ImGui::Button("Create New CamAnim"))
	{
		auto pCamAnim = CCamera_Manager::GetInstance()->GetCamAnim(szCamAnimName);
		if (pCamAnim == nullptr)
		{
			pCamAnim = CCamAnimation::Create();
			pCamAnim->SetName(szCamAnimName);
			CCamera_Manager::GetInstance()->AddCamAnim(pCamAnim);
		}

	}
	if (ImGui::Button("Load CamAnim"))
	{
		auto pCamAnim = CCamera_Manager::GetInstance()->GetCamAnim(szCamAnimName);
		if (pCamAnim)
		{
			m_pCurAnim = pCamAnim;
		}
	}

	if (ImGui::Button("Save All CamAnim"))
	{
		CCamera_Manager::GetInstance()->SaveCamAnims();
	}
}

CImgui_CamAnimEditor* CImgui_CamAnimEditor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	auto inst = new CImgui_CamAnimEditor(pDevice, pContext);
	if (FAILED(inst->Initialize(pArg)))
	{
		return nullptr;
	}

	return inst;
}

void CImgui_CamAnimEditor::Free()
{
	CImguiObject::Free();
}
