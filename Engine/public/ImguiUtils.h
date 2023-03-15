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

	static void InputFloat2(_float2* pVector, const char* str);
	static void InputFloat3(_float3* pVector, const char* str);
	static void InputColor(_float4* pVector, const char* str);

	// imgui로 본 월드매트릭스 정보 "그대로" 넣으면 월드매트릭스 나오는 함수
	static _float4x4 CreateMatrixFromImGuizmoData(_float3 vPos, _float3 vRot, _float3 vScale);
};

END