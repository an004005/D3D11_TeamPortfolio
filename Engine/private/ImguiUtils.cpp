#include "stdafx.h"
#include "..\public\ImguiUtils.h"
#include "GameInstance.h"
#include "Imgui/ImGuiFileDialog.h"
#include "Input_Device.h"

void CImguiUtils::InputFloatWheel(const char* label, float* v, float step, float step_fast, const char* format,
	ImGuiInputTextFlags flags)
{
	ImGui::InputFloat(label, v, step, step_fast, format, flags);
	ImGui::SetItemUsingMouseWheel();
	if (ImGui::IsItemHovered())
	{
	    if (const float fWheel = ImGui::GetIO().MouseWheel)
	    {
	        if (ImGui::IsItemActive())
	            ImGui::ClearActiveID();
	        else
	            *v += fWheel * 5.f;
	    }
	}
}

void CImguiUtils::Render_Guizmo(_float4x4* pWorldMatrix, GUIZMO_INFO& tInfo, _bool bMatrixEditor, _bool bManipulator)
{
	if (bMatrixEditor)
	{
		if (ImGui::IsKeyPressed(90))
			tInfo.mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(69))
			tInfo.mCurrentGizmoOperation = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(82)) // r Key
			tInfo.mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", tInfo.mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			tInfo.mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", tInfo.mCurrentGizmoOperation == ImGuizmo::ROTATE))
			tInfo.mCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", tInfo.mCurrentGizmoOperation == ImGuizmo::SCALE))
			tInfo.mCurrentGizmoOperation = ImGuizmo::SCALE;

		float matrixTranslation[3], matrixRotation[3], matrixScale[3];

		ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float*>(pWorldMatrix), matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("Translate", matrixTranslation);
		ImGui::InputFloat3("Rotate", matrixRotation);
		ImGui::InputFloat3("Scale", matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, reinterpret_cast<float*>(pWorldMatrix));

		if (tInfo.mCurrentGizmoOperation != ImGuizmo::SCALE)
		{
			if (ImGui::RadioButton("Local", tInfo.mCurrentGizmoMode == ImGuizmo::LOCAL))
				tInfo.mCurrentGizmoMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", tInfo.mCurrentGizmoMode == ImGuizmo::WORLD))
				tInfo.mCurrentGizmoMode = ImGuizmo::WORLD;
		}
	}

	if (bManipulator)
	{
		ImGui::Checkbox("##something", &tInfo.useSnap);
		ImGui::SameLine();
		switch (tInfo.mCurrentGizmoOperation)
		{
		case ImGuizmo::TRANSLATE:
			ImGui::InputFloat3("Snap", &tInfo.snap[0]);
			break;
		case ImGuizmo::ROTATE:
			ImGui::InputFloat("Angle Snap", &tInfo.snap[0]);
			break;
		case ImGuizmo::SCALE:
			ImGui::InputFloat("Scale Snap", &tInfo.snap[0]);
			break;
		}

		ImGuiIO& io = ImGui::GetIO();
		RECT rt;
		GetClientRect(CGameInstance::GetInstance()->GetHWND(), &rt);
		POINT lt{rt.left, rt.top};
		ClientToScreen(CGameInstance::GetInstance()->GetHWND(), &lt);
		ImGuizmo::SetRect((_float)lt.x, (_float)lt.y, io.DisplaySize.x, io.DisplaySize.y);

		_float4x4 matView, matProj;
		XMStoreFloat4x4(&matView, CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
		XMStoreFloat4x4(&matProj, CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
		
		ImGuizmo::Manipulate(
			reinterpret_cast<float*>(&matView), 
			reinterpret_cast<float*>(&matProj),
			tInfo.mCurrentGizmoOperation, 
			tInfo.mCurrentGizmoMode, 
			reinterpret_cast<float*>(pWorldMatrix),
			nullptr, tInfo.useSnap ? &tInfo.snap[0] : nullptr);
	}
}

void CImguiUtils::Render_Guizmo_Translate(_float3& pPosOut, GUIZMO_INFO& tInfo, _bool bMatrixEditor, _bool bManipulator)
{
	tInfo.mCurrentGizmoOperation = ImGuizmo::TRANSLATE;

	_float4x4 WorldMatrix = _float4x4::Identity;

	if (bMatrixEditor)
	{
		ImGui::InputFloat3("Translate", reinterpret_cast<_float*>(&pPosOut));

		if (tInfo.mCurrentGizmoOperation != ImGuizmo::SCALE)
		{
			if (ImGui::RadioButton("Local", tInfo.mCurrentGizmoMode == ImGuizmo::LOCAL))
				tInfo.mCurrentGizmoMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", tInfo.mCurrentGizmoMode == ImGuizmo::WORLD))
				tInfo.mCurrentGizmoMode = ImGuizmo::WORLD;
		}
	}

	if (bManipulator)
	{
		ImGui::Checkbox("##something", &tInfo.useSnap);
		ImGui::SameLine();
		switch (tInfo.mCurrentGizmoOperation)
		{
		case ImGuizmo::TRANSLATE:
			ImGui::InputFloat3("Snap", &tInfo.snap[0]);
			break;
		case ImGuizmo::ROTATE:
			break;
		case ImGuizmo::SCALE:
			break;
		}

		ImGuiIO& io = ImGui::GetIO();
		RECT rt;
		GetClientRect(CGameInstance::GetInstance()->GetHWND(), &rt);
		POINT lt{rt.left, rt.top};
		ClientToScreen(CGameInstance::GetInstance()->GetHWND(), &lt);
		ImGuizmo::SetRect((_float)lt.x, (_float)lt.y, io.DisplaySize.x, io.DisplaySize.y);

		_float4x4 matView, matProj;
		XMStoreFloat4x4(&matView, CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
		XMStoreFloat4x4(&matProj, CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

		_float4x4 WorldMatrix = _float4x4::CreateTranslation(pPosOut);

		ImGuizmo::Manipulate(
			reinterpret_cast<float*>(&matView), 
			reinterpret_cast<float*>(&matProj),
			tInfo.mCurrentGizmoOperation, 
			tInfo.mCurrentGizmoMode, 
			reinterpret_cast<float*>(&WorldMatrix),
			nullptr, tInfo.useSnap ? &tInfo.snap[0] : nullptr);

		pPosOut = WorldMatrix.Translation();
	}
}

void CImguiUtils::FileDialog_FileSelector(const char* pBtnName, const char* pFilter, const char* pFilePath, std::function<void(const string&)> FileCallback)
{
	if (ImGui::Button(pBtnName))
	{
		ImGuiFileDialog::Instance()->OpenDialog(pBtnName, "Choose File", pFilter, pFilePath);
	}
	if (ImGuiFileDialog::Instance()->Display(pBtnName))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
			// 이상하게 filePath 중간에 \0이 들어있어서 경로를 읽지 못하는 경우가 있다.(많이) 걍 \0문자 제거해버린다.
			filePath.erase(std::remove_if(filePath.begin(), filePath.end(), [](char c)
			{
				return c == '\0';
			}), filePath.end());

			FileCallback(filePath);

			// 확인 누르면 화면 피킹되는거 방지
			CInput_Device::GetInstance()->Invalidate();
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

