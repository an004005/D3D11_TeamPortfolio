#pragma once
#include "ImGuizmo.h"

BEGIN(Engine)
typedef struct tagGuizmoInfo
{
	float snap[3] = {1.f, 1.f, 1.f};
	bool useSnap = false;
	ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE mCurrentGizmoMode = ImGuizmo::WORLD;
} GUIZMO_INFO;

class ENGINE_DLL CImguiUtils
{
private:
	CImguiUtils() = default;
	~CImguiUtils() = default;

public:
	static void InputFloatWheel(const char* label, float* v, float step = 0, float step_fast = 0, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
	static void Render_Guizmo(_float4x4* pWorldMatrix, GUIZMO_INFO& tInfo, _bool bMatrixEditor, _bool bManipulator);
	static void Render_Guizmo_Translate(OUT _float3& pPosOut, GUIZMO_INFO& tInfo, _bool bMatrixEditor, _bool bManipulator);
	// pBtnName을 파일다이얼로그의 키로 사용하므로 중복되면 안된다.
	static void FileDialog_FileSelector(const char* pBtnName, const char* pFilter, const char* pFilePath, std::function<void(const string&)> FileCallback);
};

END