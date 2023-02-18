#include "stdafx.h"
#include "..\public\MaterialPreview.h"
#include "GameInstance.h"
#include "Material.h"
#include "ImguiUtils.h"
#include "GameUtils.h"

CMaterialPreview::CMaterialPreview(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CMaterialPreview::CMaterialPreview(const CMaterialPreview& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMaterialPreview::Initialize(void* pArg)
{
	CGameObject::Initialize(pArg);

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_Component_VIBuffer_Rect", TEXT("Buffer"),
	// 	(CComponent**)&m_pBuffer));



	return S_OK;
}

void CMaterialPreview::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);
}

void CMaterialPreview::Late_Tick(_double TimeDelta)
{
	if (m_pModel)
	{
		m_pModel->Play_Animation(TimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
	else
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
}

HRESULT CMaterialPreview::Render()
{
	if (m_pModel)
	{
		m_pModel->Render(m_pTransformCom);
	}
	else if (m_pBuffer && m_pMaterial)
	{
		m_pMaterial->BindMatrices(m_pTransformCom);
		m_pMaterial->Begin();
		m_pBuffer->Render();
	}

	return S_OK;
}

void CMaterialPreview::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();

	ImGui::Separator();
	CImguiUtils::FileDialog_FileSelector("Change Material", ".json", "../Bin/Resources/Meshes/Valorant/Materials/",
		[this](const string& filePath)
		{
			Safe_Release(m_pMaterial);
			Delete_Component(L"Material");

			wstring fileName = s2ws(CGameUtils::GetFileName(filePath));
			CMaterial* pMaterial = CMaterial::Create(m_pDevice, m_pContext, filePath.c_str());

			if (FAILED(CGameInstance::GetInstance()->Add_Prototype(fileName.c_str(), pMaterial)))
				Safe_Release(pMaterial);

			Add_Component(LEVEL_NOW, fileName.c_str(), L"Material", (CComponent**)&m_pMaterial);
		});

	ImGui::Separator();
	static char BufferProtoTag[MAX_PATH]{};
	ImGui::InputText("Buffer ProtoTag", BufferProtoTag, MAX_PATH);
	ImGui::SameLine();
	if (ImGui::Button("Set Buffer"))
	{
		Safe_Release(m_pBuffer);
		Delete_Component(TEXT("Buffer"));
		Add_Component(LEVEL_NOW, s2ws(BufferProtoTag).c_str(), TEXT("Buffer"), (CComponent**)&m_pBuffer);
	}

	ImGui::Separator();
	static char ModelProtoTag[MAX_PATH]{};
	ImGui::InputText("Model ProtoTag", ModelProtoTag, MAX_PATH);
	ImGui::SameLine();
	if (ImGui::Button("Set Model"))
	{
		Safe_Release(m_pModel);
		Delete_Component(TEXT("Model"));
		Add_Component(LEVEL_NOW, s2ws(ModelProtoTag).c_str(), TEXT("Model"), (CComponent**)&m_pModel);
	}
}

void CMaterialPreview::Free()
{
	CGameObject::Free();
	Safe_Release(m_pBuffer);
	Safe_Release(m_pMaterial);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModel);
}

CMaterialPreview* CMaterialPreview::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
 	CMaterialPreview*		pInstance = new CMaterialPreview(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMaterialPreview");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMaterialPreview::Clone(void* pArg)
{
	CMaterialPreview*		pInstance = new CMaterialPreview(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMaterialPreview");
		Safe_Release(pInstance);
	}
	return pInstance;
}