#include "stdafx.h"
#include "MapInstance_Object.h"
#include "GameInstance.h"
#include "PhysXStaticModel.h"
#include "JsonStorage.h"
#include "ImguiUtils.h"

CMapInstance_Object::CMapInstance_Object(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMapObject(pDevice, pContext)
{
}

CMapInstance_Object::CMapInstance_Object(const CMapInstance_Object & rhs)
	: CMapObject(rhs)
{
}

HRESULT CMapInstance_Object::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CMapInstance_Object::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components());

	//m_pPxModel->SetPxWorldMatrix(m_pTransformCom->Get_WorldMatrix());

	Json json = *reinterpret_cast<Json*>(pArg);
	if (json.contains("InstanceInfo"))
	{
		m_iComNum = 0;

		for (auto WorldMatrix : json["InstanceInfo"]["WorldMatrix"])
		{
			_float4x4 WorldMat = WorldMatrix;
			m_pModel_InstancingCom->Add_Instance(WorldMat);

			CPhysXStaticModel* pPxModel = nullptr;

			//todo: 임시로 모든 CMapNonAnim_Object 에 PxModel을 가지도록 설정 추후 수정 바람
			
			const wstring ComponentTag = L"Com_PxModel" + to_wstring(m_iComNum++);

			FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_PxModelTag.c_str(), ComponentTag.c_str(),
				(CComponent**)&pPxModel));

			m_pPxModels.emplace_back(pPxModel);
			m_pPxModels.back()->SetPxWorldMatrix(WorldMatrix);

		}
		m_pModel_InstancingCom->Map_Meshs();
	}

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	return S_OK;
}

void CMapInstance_Object::BeginTick()
{
}

void CMapInstance_Object::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

#ifdef _DEBUG
	if (m_iIndex > -1)
		m_pPxModels[m_iIndex]->SetPxWorldMatrix(m_WorldMatrix);
#endif
}

void CMapInstance_Object::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CMapInstance_Object::Render()
{
	FAILED_CHECK(__super::Render());

	FAILED_CHECK(m_pModel_InstancingCom->Render(m_pTransformCom));

	return S_OK;
}

void CMapInstance_Object::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);
	m_strModelTag = s2ws(json["ModelTag"]);
}

void CMapInstance_Object::SaveToJson(Json & json)
{
	__super::SaveToJson(json);
	json["ModelTag"] = ws2s(m_strModelTag);
	auto Meshes = m_pModel_InstancingCom->GetMeshes();

	json["InstanceInfo"]["WorldMatrix"] = Json::array();


	vector<_float4x4> WorldMatrixs = Meshes.front()->GetWorldMatirxs();

	for (auto WorldMatrix : WorldMatrixs)
	{
		json["InstanceInfo"]["WorldMatrix"].push_back(WorldMatrix);
	}

}

void CMapInstance_Object::Imgui_RenderProperty()
{
	if (LEVEL_NOW != LEVEL_MAPTOOL) return;

	CVIBuffer_ModelInstancing* Mesh = m_pModel_InstancingCom->GetMeshes().front();
	vector<_float4x4> WorldMatrixs = Mesh->GetWorldMatirxs();

	if (ImGui::BeginListBox("WorldMatrix List"))
	{
		for (_uint i = 0; i<WorldMatrixs.size(); ++i)
		{
			const bool bSelected = (i == m_iIndex);

			if (bSelected)
				ImGui::SetItemDefaultFocus();

			char pStr[MAX_PATH] = "WorldMatrix";
			sprintf_s(pStr, sizeof(pStr), "%s %d", pStr, i);

			if (ImGui::Selectable(pStr, bSelected))
			{
				m_WorldMatrix = WorldMatrixs[i];
				m_iIndex = i;
			}
		}
		ImGui::EndListBox();
	}

	ImGui::Separator();

	if (ImGui::Button("Delete_MapInstance_Object"))
	{
		if (m_pModel_InstancingCom->GetMeshes().front()->GetWorldMatirxs().size() > 1)
		{
			m_pPxModels.erase(m_pPxModels.begin() + m_iIndex);
			m_pModel_InstancingCom->Delete_Instance(m_iIndex--);
			m_WorldMatrix = WorldMatrixs[max(0, m_iIndex)];
		}
			
	}

	ImGui::BeginChild("Instance Object", { 500.f, 200.f });

	static GUIZMO_INFO tInfo;
	CImguiUtils::Render_Guizmo(&m_WorldMatrix, tInfo, true, true);

	ImGui::EndChild();

	if(m_iIndex >= 0)
		m_pModel_InstancingCom->Modify_Matrix(m_iIndex, m_WorldMatrix);
}

const CPhysXStaticModel * CMapInstance_Object::Find_PhysXStaticModel(CPhysXStaticModel * pPhsXSM)
{
	_uint iIndex = 0;

	for (auto it : m_pPxModels)
	{
		if (it == pPhsXSM)
		{
			CVIBuffer_ModelInstancing* Mesh = m_pModel_InstancingCom->GetMeshes().front();
			vector<_float4x4> WorldMatrixs = Mesh->GetWorldMatirxs();
			m_iIndex = iIndex;
			m_WorldMatrix = WorldMatrixs[m_iIndex];

			return it;
		}

		iIndex++;
	}

	return nullptr;
}

_float4 CMapInstance_Object::Get_FocusPosition()
{
	return (_float4)m_WorldMatrix.m[3];
}



void CMapInstance_Object::Set_Focus()
{
	CVIBuffer_ModelInstancing* Mesh = m_pModel_InstancingCom->GetMeshes().front();
	vector<_float4x4> WorldMatrixs = Mesh->GetWorldMatirxs();
	m_iIndex = WorldMatrixs.size() - 1;
	m_WorldMatrix = WorldMatrixs.back();
}

void CMapInstance_Object::Create_PhsyX(_fmatrix WorldMatrix)
{
	CPhysXStaticModel* pPxModel = nullptr;

	const wstring ComponentTag = L"Com_PxModel" + to_wstring(m_iComNum++);

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_PxModelTag.c_str(), ComponentTag.c_str(),
		(CComponent**)&pPxModel));

	pPxModel->SetPxWorldMatrix(WorldMatrix);
	m_pPxModels.emplace_back(pPxModel);
}

wstring CMapInstance_Object::MakePxModelProtoTag()
{
	_tchar szDriveName[MAX_PATH]{};
	_tchar szDirName[MAX_PATH]{};
	_tchar szFileName[MAX_PATH]{};
	_tchar szExtName[MAX_PATH]{};
	_wsplitpath_s(m_strModelTag.c_str(), szDriveName, MAX_PATH, szDirName, MAX_PATH, szFileName, MAX_PATH, szExtName, MAX_PATH);

	wstring PxModelTag = szDriveName;
	PxModelTag += szDirName;

	const wstring strFileName = wstring(L"PhysX_") + szFileName;
	PxModelTag += strFileName;
	PxModelTag += szExtName;

	return PxModelTag;
}

HRESULT CMapInstance_Object::SetUp_Components()
{
	/* For.Com_Model */

	//여기서 원본이 없으면 생성
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (nullptr == pGameInstance->Find_Prototype_Component(LEVEL_NOW, m_strModelTag.c_str()))
	{
		_uint iNumInstance = 500;
		FAILED_CHECK(pGameInstance->Add_Prototype(m_strModelTag.c_str(), CModel_Instancing::Create(m_pDevice, m_pContext, ws2s(m_strModelTag).c_str(), iNumInstance)));
	}

	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_strModelTag.c_str(), TEXT("Com_Model"),
		(CComponent**)&m_pModel_InstancingCom));


	m_PxModelTag = MakePxModelProtoTag();
	if (nullptr == CGameInstance::GetInstance()->Find_Prototype_Component(LEVEL_NOW, m_PxModelTag.c_str()))
	{
		FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_NOW,
			m_PxModelTag.c_str(), CPhysXStaticModel::Create(m_pDevice, m_pContext, ws2s(m_strModelTag).c_str())));
	}
	return S_OK;
}

CMapInstance_Object * CMapInstance_Object::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMapInstance_Object*		pInstance = new CMapInstance_Object(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapInstance_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapInstance_Object::Clone(void * pArg)
{
	CMapInstance_Object*		pInstance = new CMapInstance_Object(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapInstance_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapInstance_Object::Free()
{
	__super::Free();

	for(auto pxModel : m_pPxModels)
		Safe_Release(pxModel);

	Safe_Release(m_pModel_InstancingCom);
}
