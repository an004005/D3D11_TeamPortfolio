#include "stdafx.h"
#include "Material.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Transform.h"
#include "GameUtils.h"
#include "JsonStorage.h"
#include "ImguiUtils.h"

CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
}

CMaterial::CMaterial(const CMaterial& rhs)
	: CComponent(rhs)
	, m_pShader(rhs.m_pShader)
	, m_tParams(rhs.m_tParams)
#ifdef _DEBUG
	, m_strFilePath(rhs.m_strFilePath)
#endif
{
	Safe_AddRef(m_pShader);
	for (auto e : m_tParams.Textures)
		Safe_AddRef(e.first);
}

HRESULT CMaterial::Initialize_Prototype(const char* pMtrlFilePath)
{
	Assert(pMtrlFilePath != nullptr);

	const Json& json = CJsonStorage::GetInstance()->FindOrLoadJson(pMtrlFilePath);

	CShader::LoadShaderParam(m_tParams, json);
	string shaderProtoTag = json["ShaderProtoTag"];
	m_pShader = dynamic_cast<CShader*>(CGameInstance::GetInstance()->Clone_Component(CGameUtils::s2ws(shaderProtoTag).c_str()));

#ifdef _DEBUG
	m_strFilePath = pMtrlFilePath;
#endif

	return S_OK;
}

void CMaterial::SaveToJson(Json& json)
{
	CShader::SaveShaderParam(m_tParams, json);

	char szProtoTag[MAX_PATH];
	CGameUtils::wc2c(m_pShader->GetPrototypeTag(), szProtoTag);
	json["ShaderProtoTag"] = szProtoTag;
}

void CMaterial::Imgui_RenderProperty()
{
	static char protoTag[MAX_PATH];
	CGameUtils::wc2c(m_pPrototypeTag, protoTag);
	ImGui::Text("Material ProtoTag : %s", protoTag);
	ImGui::Checkbox("Active", &m_bActive);

	ImGui::Separator();

	m_pShader->Imgui_RenderProperty();
	ImGui::Separator();

	CShader::Imgui_RenderShaderParams(m_tParams);


	{
		char CurShaderProtoTag[MAX_PATH];
		CGameUtils::wc2c(m_pShader->GetPrototypeTag(), CurShaderProtoTag);
		ImGui::Text("Current Shader : %s", CurShaderProtoTag);

		static char ShaderProtoTag[MAX_PATH];
		ImGui::InputText("New Shader ProtoTag", ShaderProtoTag, MAX_PATH);
		ImGui::SameLine();
		if (ImGui::Button("Change Shader"))
		{
			auto pNewShaderCom = CGameInstance::GetInstance()->Clone_Component(CGameUtils::s2ws(ShaderProtoTag).c_str());

			if (auto pNewShader = dynamic_cast<CShader*>(pNewShaderCom))
			{
				Safe_Release(m_pShader);
				m_pShader = pNewShader;
			}
			else
			{
				Safe_Release(pNewShaderCom);
			}
		}
	}

	ImGui::Separator();

	CImguiUtils::FileDialog_FileSelector("Save Material to", ".json", "../Bin/Resources/Meshes/Valorant/Materials/", [this](const string& filePath)
	{
		Json json;
		SaveToJson(json);
		std::ofstream file(filePath);
		file << json;
#ifdef _DEBUG
		m_strFilePath = filePath;
#endif
	});

#ifdef _DEBUG
	if (ImGui::Button("QuickSave") && m_strFilePath.empty() == false)
	{
		wstring msg = L"Save to " + s2ws(m_strFilePath);
		if (MessageBox(NULL, msg.c_str(), L"System Message", MB_YESNO) == IDYES)
		{
			Json json;
			SaveToJson(json);
			std::ofstream file(m_strFilePath);
			file << json;
		}
	}
#endif

}

void CMaterial::BindMatrices(CTransform* pTransform)
{
	FAILED_CHECK(pTransform->Bind_ShaderResource(m_pShader, "g_WorldMatrix"));
	FAILED_CHECK(m_pShader->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShader->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));
	FAILED_CHECK(m_pShader->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)));

	// const _float4x4 ProjInv = CGameInstance::GetInstance()->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
	// const _float4x4 ViewInv = CGameInstance::GetInstance()->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);
	// FAILED_CHECK(m_pShader->Set_Matrix("g_ProjMatrixInv", &ProjInv));
	// FAILED_CHECK(m_pShader->Set_Matrix("g_ViewMatrixInv", &ViewInv));
}

void CMaterial::BindMatrices(const _float4x4& WorldMatrix)
{
	FAILED_CHECK(m_pShader->Set_Matrix("g_WorldMatrix", &WorldMatrix));
	FAILED_CHECK(m_pShader->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShader->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));
	FAILED_CHECK(m_pShader->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)));

	// const _float4x4 ProjInv = CGameInstance::GetInstance()->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
	// const _float4x4 ViewInv = CGameInstance::GetInstance()->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);
	// FAILED_CHECK(m_pShader->Set_Matrix("g_ProjMatrixInv", &ProjInv));
	// FAILED_CHECK(m_pShader->Set_Matrix("g_ViewMatrixInv", &ViewInv));
}

void CMaterial::Begin()
{
	m_pShader->Begin_Params(m_tParams);
}

void CMaterial::Begin(_uint iPass)
{
	m_pShader->Set_Params(m_tParams);
	m_pShader->Begin(iPass);
}

void CMaterial::Free()
{
	CComponent::Free();
	Safe_Release(m_pShader);
	for (auto e : m_tParams.Textures)
		Safe_Release(e.first);
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pMtrlFilePath)
{
	CMaterial*		pInstance = new CMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pMtrlFilePath)))
	{
		MSG_BOX("Failed to Created : CMaterial");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CMaterial::Clone(void* pArg)
{
	CMaterial*		pInstance = new CMaterial(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMaterial");
		Safe_Release(pInstance);
	}
	return pInstance;
}

