#include "stdafx.h"
#include "Model_Instancing.h"
#include "Texture.h"
#include "Shader.h"
#include "GameInstance.h"
#include "GameUtils.h"
#include "JsonLib.h"
#include "Material.h"
#include "ImguiUtils.h"
#include "Light_Manager.h"
#include "JsonStorage.h"
#include "VIBuffer_ModelInstancing.h"

const _float4x4 CModel_Instancing::s_DefaultPivot = _float4x4::CreateScale({ 0.01f, 0.01f, 0.01f }) *_float4x4::CreateRotationY(XMConvertToRadians(-180.f));
const string CModel_Instancing::s_ModifyFilePath = "../Bin/Resources/Meshes/Scarlet_Nexus/AnimationModifier.json";

CModel_Instancing::CModel_Instancing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel_Instancing::CModel_Instancing(const CModel_Instancing & rhs)
	: CComponent(rhs)
	, m_strName(rhs.m_strName)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_pShadowShader(rhs.m_pShadowShader)
{
	
	// 메쉬 클론
	m_Meshes.reserve(rhs.m_Meshes.size());
	for (auto& pMesh : rhs.m_Meshes)
		m_Meshes.push_back(dynamic_cast<CVIBuffer_ModelInstancing*>(pMesh->Clone()));

	// for (auto& pMtrl : m_Materials)
	// 	Safe_AddRef(pMtrl);
	//  머터리얼 복사
	m_Materials.reserve(rhs.m_Materials.size());
	for (auto pMtrl : rhs.m_Materials)
		m_Materials.push_back((CMaterial*)pMtrl->Clone(nullptr));


	Safe_AddRef(m_pShadowShader);
}

void CModel_Instancing::Add_Instance(_float4x4 WorldMatirxf4x4)
{
	for_each(m_Meshes.begin(), m_Meshes.end(), [&](CVIBuffer_ModelInstancing* pMesh) {
		pMesh->Add_Instance(WorldMatirxf4x4);
	});
}

void CModel_Instancing::Modify_Matrix(_uint iIndex, _float4x4 WorldMatirxf4x4)
{
	for_each(m_Meshes.begin(), m_Meshes.end(), [&](CVIBuffer_ModelInstancing* pMesh) {
		pMesh->Modify_Matrix(iIndex, WorldMatirxf4x4);
	});
}

void CModel_Instancing::Delete_Instance(_uint iIndex)
{
	for_each(m_Meshes.begin(), m_Meshes.end(), [&](CVIBuffer_ModelInstancing* pMesh) {
		pMesh->Delete_Instance(iIndex);
	});
}

void CModel_Instancing::Map_Meshs()
{
	for_each(m_Meshes.begin(), m_Meshes.end(), [&](CVIBuffer_ModelInstancing* pMesh) {
		pMesh->Map_Mesh();
	});
}


HRESULT CModel_Instancing::Initialize_Prototype(const char * pModelFilePath,  _uint iNumInstance)
{
	
	_tchar szFilePath[MAX_PATH];
	CGameUtils::c2wc(pModelFilePath, szFilePath);
	HANDLE hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		IM_ERROR("fail to open file %s", pModelFilePath);
		return E_FAIL;
	}

	DWORD dwByte = 0;

	m_strName = CGameUtils::ReadStrFile(hFile); /* Read */

	_uint iNumMeshes = 0;
	ReadFile(hFile, &iNumMeshes, sizeof(_uint), &dwByte, nullptr); /* Read */

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		CVIBuffer_ModelInstancing*	pMesh = CVIBuffer_ModelInstancing::Create(m_pDevice, m_pContext, hFile, this,  iNumInstance);
		Assert(pMesh != nullptr);
		m_Meshes.push_back(pMesh);
	}

	Ready_Materials(hFile);
	
	m_pShadowShader = dynamic_cast<CShader*>(CGameInstance::GetInstance()
		->Clone_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxModel_Shadow"));


	CloseHandle(hFile);
	return S_OK;
}

HRESULT CModel_Instancing::Initialize(void* pArg)
{
	return S_OK;
}

void CModel_Instancing::Imgui_RenderProperty()
{
	ImGui::Text("%s", m_strName.c_str());

	if (ImGui::CollapsingHeader("Material Viewer"))
	{
		static char szSeachMtrl[MAX_PATH] = "";
		ImGui::InputText("Mtrl Seach", szSeachMtrl, MAX_PATH);

		const string strSearch = szSeachMtrl;
		const _bool bSearch = strSearch.empty() == false;

		static int iSelected = 0;

		if (!m_Materials.empty() && ImGui::BeginListBox("Material List"))
		{
			for (int i = 0; i < int(m_Materials.size()); ++i)
			{
				char protoTag[MAX_PATH];
				CGameUtils::wc2c(m_Materials[i]->GetPrototypeTag(), protoTag);
				if (bSearch)
				{
					string strProtoTag = protoTag;
					if (strProtoTag.find(strSearch) == string::npos)
						continue;
				}

				const bool bSelected = iSelected == i;
				if (bSelected)
					ImGui::SetItemDefaultFocus();

				if (ImGui::Selectable(protoTag, bSelected))
					iSelected = i;
			}
			ImGui::EndListBox();

			m_Materials[iSelected]->Imgui_RenderProperty();
		}
	}
}


HRESULT CModel_Instancing::Render(CTransform* pTransform)
{
	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		RenderMesh(pTransform, static_cast<_uint>(i));
	}

	return S_OK;
}

HRESULT CModel_Instancing::Render(const _float4x4& WorldMatrix)
{
	for (const auto& mesh : m_Meshes)
	{
		const _uint iMtrlIdx = mesh->Get_MaterialIndex();
		if (m_Materials[iMtrlIdx]->IsActive() == false)
			continue;

		m_Materials[iMtrlIdx]->BindMatrices_Instancing(WorldMatrix);
		m_Materials[iMtrlIdx]->Begin_Instancing();
		mesh->Render();
	}

	return S_OK;
}

HRESULT CModel_Instancing::RenderMesh(CTransform* pTransform, _uint iMeshIdx)
{
	const _uint iMtrlIdx = m_Meshes[iMeshIdx]->Get_MaterialIndex();
	if (m_Materials[iMtrlIdx]->IsActive() == false)
		return S_OK;

	m_Materials[iMtrlIdx]->BindMatrices_Instancing(pTransform);
	m_Materials[iMtrlIdx]->Begin_Instancing();
	return m_Meshes[iMeshIdx]->Render();
}

HRESULT CModel_Instancing::RenderMeshOnly(_uint iMeshIdx) const
{
	return m_Meshes[iMeshIdx]->Render();
}

HRESULT CModel_Instancing::Render_ShadowDepth(CTransform* pTransform)
{
	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		const _uint iMtrlIdx = m_Meshes[i]->Get_MaterialIndex();
		if (m_Materials[iMtrlIdx]->IsActive() == false)
			continue;

		FAILED_CHECK(pTransform->Bind_ShaderResource(m_pShadowShader, "g_WorldMatrix"));

		FAILED_CHECK(m_pShadowShader->Set_Matrix("g_ViewMatrix", CLight_Manager::GetInstance()->GetShadowLightView()));
		FAILED_CHECK(m_pShadowShader->Set_Matrix("g_ProjMatrix", CLight_Manager::GetInstance()->GetShadowLightProj()));

		m_pShadowShader->Begin(0);
		m_Meshes[i]->Render();
	}

	return S_OK;
}


CMaterial* CModel_Instancing::FindMaterial(const _tchar* pMtrlProtoTag)
{
	if (pMtrlProtoTag == nullptr)
		return nullptr;

	for (auto pMtrl : m_Materials)
	{
		if (lstrcmp(pMtrl->GetPrototypeTag(), pMtrlProtoTag) == 0)
			return pMtrl;
	}

	return nullptr;
}



HRESULT CModel_Instancing::Ready_Materials(HANDLE hFile)
{
	DWORD dwByte = 0;

	_uint iNumMaterials = 0;
	ReadFile(hFile, &iNumMaterials, sizeof(_uint), &dwByte, nullptr); /* Read */

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		const string mtrlName = CGameUtils::ReadStrFile(hFile); /* Read */

		wstring wMtrlName = CGameUtils::s2ws(mtrlName);
		CMaterial* pMtrl = dynamic_cast<CMaterial*>(CGameInstance::GetInstance()->Clone_Component(wMtrlName.c_str()));

		if (pMtrl == nullptr)
		{
			
			pMtrl = dynamic_cast<CMaterial*>(CGameInstance::GetInstance()->Clone_Component(L"Proto_Mtrl_Empty_Instancing"));
			IM_WARN("Fail to Find Mtrl : %s", mtrlName.c_str());
		}


		pMtrl->SetAnimType(CModel::TYPE_NONANIM);
		m_Materials.push_back(pMtrl);
	}

	return S_OK;
}


CModel_Instancing * CModel_Instancing::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char * pModelFilePath, _uint iNumInstance, _float4x4 PivotMatrix)
{
	CModel_Instancing*		pInstance = new CModel_Instancing(pDevice, pContext);
	pInstance->m_PivotMatrix = PivotMatrix;

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, iNumInstance)))
	{
		MSG_BOX("Failed to Created : CModel_Instancing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel_Instancing::Clone(void * pArg)
{
	CModel_Instancing*		pInstance = new CModel_Instancing(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel_Instancing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel_Instancing::Free()
{
	__super::Free();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();


	for (auto& pMtrl : m_Materials)
		Safe_Release(pMtrl);
	m_Materials.clear();

	Safe_Release(m_pShadowShader);
}
