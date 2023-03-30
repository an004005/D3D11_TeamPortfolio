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

	// todo: ���콺 ��ŷ���� ������Ʈ �����ϴ� ��� �߰�
	// todo: ���� �纻 object�� ������ �̸��� ���� ������ �ϴ� class�̸����� �س���. ���̵�� �ִ»���� �߰��ϱ�~

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
