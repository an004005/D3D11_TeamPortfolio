#include "stdafx.h"
#include "..\public\Imgui_PropertyEditor.h"
#include "GameObject.h"
#include "GameInstance.h"

CImgui_PropertyEditor::CImgui_PropertyEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_PropertyEditor::Initialize(void* pArg)
{
	m_szTabname = "PropertyEditor";
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	return S_OK;
}

void CImgui_PropertyEditor::Imgui_RenderTab()
{
	m_pGameInstance->Imgui_ObjectViewer(LEVEL_NOW, m_pSelectedObject);

	// todo: 마우스 피킹으로 오브젝트 선택하는 기능 추가
	// todo: 현재 사본 object를 구분할 이름이 없기 때문에 일단 class이름으로 해놓음. 아이디어 있는사람이 추가하기~

	if (m_pSelectedObject)
	{
		ImGui::Separator();
		ImGui::Text("%s", typeid(*m_pSelectedObject).name());
		m_pSelectedObject->Imgui_RenderProperty();
		m_pSelectedObject->Imgui_RenderComponentProperties();
	}

	if (m_pGameInstance->KeyDown(DIK_ESCAPE))
		m_pSelectedObject = nullptr;
}

CImgui_PropertyEditor* CImgui_PropertyEditor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	auto inst = new CImgui_PropertyEditor(pDevice, pContext);
	if (FAILED(inst->Initialize(pArg)))
	{
		return nullptr;
	}

	return inst;
}

void CImgui_PropertyEditor::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
