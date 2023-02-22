#include "stdafx.h"
#include "..\public\Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Bone.h"
#include "Animation.h"
#include "GameInstance.h"
#include "GameUtils.h"
#include "JsonLib.h"
#include "Material.h"
#include "ImguiUtils.h"
#include "Light_Manager.h"

const _float4x4 CModel::s_DefaultPivot = _float4x4::CreateScale({ 0.01f, 0.01f, 0.01f }) *_float4x4::CreateRotationY(XMConvertToRadians(-180.f));

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_strName(rhs.m_strName)
	, m_eType(rhs.m_eType)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_pShadowShader(rhs.m_pShadowShader)
{
	if (m_eType == TYPE_ANIM)
	{
		// bone 생성
		const Json jRootBone = Json::parse(rhs.m_BoneObjectDump);
		Ready_Bones(jRootBone, nullptr);
		SetBoneChildren(jRootBone);
	}

	// 메쉬 클론
	m_Meshes.reserve(rhs.m_Meshes.size());
	for (auto& pMesh : rhs.m_Meshes)
		m_Meshes.push_back(dynamic_cast<CMesh*>(pMesh->Clone()));

	if (m_eType == TYPE_ANIM)
	{
		// 메쉬 본 연결
		for (const auto pMesh : m_Meshes)
			pMesh->SetUp_MeshBones(this);

		// 애니메이션 복사 및 링크
		for (const auto& pAnimPair : rhs.m_mapAnimation)
		{
			m_mapAnimation.emplace(pAnimPair.first, pAnimPair.second->Clone());
		}
		for (const auto& pAnimPair : m_mapAnimation)
		{
			pAnimPair.second->Link_Model(this);
		}
	}


	// for (auto& pMtrl : m_Materials)
	// 	Safe_AddRef(pMtrl);
	//  머터리얼 복사
	m_Materials.reserve(rhs.m_Materials.size());
	for (auto pMtrl : rhs.m_Materials)
		m_Materials.push_back((CMaterial*)pMtrl->Clone(nullptr));


	Safe_AddRef(m_pShadowShader);
}

CBone * CModel::Get_BonePtr(const char * pBoneName)
{
	const auto itr = m_mapBones.find(pBoneName);
	if (itr == m_mapBones.end())
		return nullptr;

	return itr->second;
}

CBone* CModel::Get_BonePtr(const string& strBoneName)
{
	const auto itr = m_mapBones.find(strBoneName);
	if (itr == m_mapBones.end())
		return nullptr;

	return itr->second;
}

_matrix CModel::GetBoneMatrix(const string& strBoneName)
{
	CBone* pBone = Get_BonePtr(strBoneName);
	Assert(pBone != nullptr);
	return pBone->Get_CombindMatrix() * XMLoadFloat4x4(&m_PivotMatrix);
}

CAnimation* CModel::Find_Animation(const string& strAnimaName)
{
	const auto itr = m_mapAnimation.find(strAnimaName);
	if (itr != m_mapAnimation.end())
		return itr->second;

	return nullptr;
}

_vector & CModel::GetLocalMove(_fmatrix WorldMatrix)
{
	_vector vMovePos;
	ZeroMemory(&vMovePos, sizeof(_vector));

	m_vBefLocalMove = m_vLocalMove;
	m_vLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	if (m_CurAnimName != "")
	{
		m_vLocalMove = m_mapAnimation[m_CurAnimName]->GetLocalMove();
		if (XMVector3Equal(m_vLocalMove, XMVectorSet(0.f, 0.f, 0.f, 0.f)))
		{
			m_vLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			m_vBefLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			return XMVectorSet(0.f, 0.f, 0.f, 0.f);
		}
		if (1.f <= m_mapAnimation[m_CurAnimName]->GetPlayRatio())
		{
			m_vLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			m_vBefLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			return XMVectorSet(0.f, 0.f, 0.f, 0.f);
		}
	}

	_vector vScale, vRotation, vTrans;
	XMMatrixDecompose(&vScale, &vRotation, &vTrans, WorldMatrix);
	_matrix WorldRotation = XMMatrixRotationQuaternion(vRotation);

	vMovePos = m_vLocalMove - m_vBefLocalMove;
	XMVectorSetW(vMovePos, 0.f);
	_float	fLength = XMVectorGetX(XMVector3Length(vMovePos));

	XMMatrixDecompose(&vScale, &vRotation, &vTrans, m_PivotMatrix);
	_matrix PivotRotation = XMMatrixRotationQuaternion(vRotation);
	vMovePos = XMVector3TransformNormal(vMovePos, PivotRotation);

	// Pivot을 적용시켜도 Y축과 Z축 이동이 뒤틀리는 현상으로 인해
	// 현재 X축에 대해 회전을 해둔 상태, 원인 파악 시 다시 원상복구 할 것
	/*********************************************************************/
	_matrix ModifyRotation = XMMatrixRotationX(XMConvertToRadians(-90.f));
	vMovePos = XMVector3TransformNormal(vMovePos, ModifyRotation);
	/*********************************************************************/

	vMovePos = XMVector3TransformNormal(vMovePos, WorldRotation);

	XMVectorSetW(vMovePos, 0.f);

	//vMovePos *= fLength;

	return vMovePos;
}

HRESULT CModel::Initialize_Prototype(const char * pModelFilePath)
{
	char szExt[MAX_PATH];
	_splitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
	if (strcmp(szExt, ".anim_model") == 0)
	{
		m_eType = TYPE_ANIM;
	}
	else if (strcmp(szExt, ".static_model") == 0)
	{
		m_eType = TYPE_NONANIM;
	}
	else
	{
		IM_ERROR("Model Type is not defined : %s", pModelFilePath);
		NODEFAULT
	}

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
		CMesh*		pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, hFile, this);
		Assert(pMesh != nullptr);
		m_Meshes.push_back(pMesh);
	}

	Ready_Materials(hFile);

	
	if (m_eType == TYPE_ANIM)
	{
		m_BoneObjectDump = CGameUtils::ReadStrFile(hFile); /* Read */
	}

	if (m_eType == TYPE_ANIM)
	{
		m_pShadowShader = dynamic_cast<CShader*>(CGameInstance::GetInstance()
			->Clone_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxAnimModel_Shadow"));
	}
	else if (m_eType == TYPE_NONANIM)
	{
		m_pShadowShader = dynamic_cast<CShader*>(CGameInstance::GetInstance()
			->Clone_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxModel_Shadow"));
	}

	CloseHandle(hFile);
	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

void CModel::Imgui_RenderProperty()
{
	ImGui::Text("%s", m_strName.c_str());

	if (m_eType == TYPE_ANIM)
	{
		ImGui::Text("Pivot");
		static GUIZMO_INFO tInfo;
		CImguiUtils::Render_Guizmo(&m_PivotMatrix, tInfo, true, true);

		ImGui::Separator();
		if (ImGui::CollapsingHeader("Anim Viewer"))
		{
			static char szSeachAnim[MAX_PATH] = "";
			ImGui::InputText("Anim Search", szSeachAnim, MAX_PATH);

			const string strSearch = szSeachAnim;
			const _bool bSearch = strSearch.empty() == false;

			if (ImGui::BeginListBox("Anim list"))
			{
				for (auto& Pair : m_mapAnimation)
				{
					if (bSearch)
					{
						if (Pair.first.find(strSearch) == string::npos)
							continue;
					}

					const bool bSelected = m_CurAnimName == Pair.first;
					if (bSelected)
						ImGui::SetItemDefaultFocus();

					if (ImGui::Selectable(Pair.first.c_str(), bSelected))
						m_CurAnimName = Pair.first;
				}
				ImGui::EndListBox();
			}
		}
	}



	// if (ImGui::BeginListBox("Animations Additive"))
	// {
	// 	for (size_t i = 0; i < m_Animations.size(); ++i)
	// 	{
	// 		const bool bSelected = m_iAdditiveAnimIdx == (_uint)i;
	// 		if (bSelected)
	// 			ImGui::SetItemDefaultFocus();
	//
	// 		if (ImGui::Selectable(m_Animations[i]->GetName().c_str(), bSelected))
	// 		{
	// 			strcpy_s(animBuff, m_Animations[i]->GetName().c_str());
	// 			m_iAdditiveAnimIdx = (_uint)i;
	// 		}
	// 	}
	//
	// 	ImGui::EndListBox();
	// }

	// ImGui::SliderFloat("additiveRatio", &m_fAdditiveRatio, 0.f, 3.f);


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

	if (m_eType == TYPE_ANIM)
	{
		static string strSelected = m_pRootBone->Get_Name();
		static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
		if (ImGui::CollapsingHeader("Skeletal Viewer"))
		{
			m_pRootBone->Imgui_SkeletalViewRecursive(strSelected, baseFlags);

			if (strSelected.empty() == false)
			{
				CBone* pBone = Get_BonePtr(strSelected);
				_matrix tmp =  pBone->Get_CombindMatrix() * XMLoadFloat4x4(&m_PivotMatrix);
				_float4x4 tmp2;
				XMStoreFloat4x4(&tmp2, tmp);

				static GUIZMO_INFO tInfo;
				CImguiUtils::Render_Guizmo(&tmp2, tInfo, false, true);
			}
		}
	}

	if (m_eType == TYPE_ANIM)
	{
		if (ImGui::CollapsingHeader("Event Viewer"))
		{
			if (ImGui::BeginListBox("Event View"))
			{
				for (auto& iter : m_EventFunc)
				{
					if (ImGui::Selectable(iter.first.c_str()))
						iter.second();
				}
				ImGui::EndListBox();
			}
		}
	}
}

void CModel::SetPlayAnimation(const string& strAnimName)
{
	const auto itr = m_mapAnimation.find(strAnimName);
	if (itr != m_mapAnimation.end())
	{
		m_CurAnimName = strAnimName;
		itr->second->Reset();
	}
}

CAnimation* CModel::GetPlayAnimation()
{
	const auto itr = m_mapAnimation.find(m_CurAnimName);
	if (itr != m_mapAnimation.end())
		return itr->second;
	return nullptr;
}

void CModel::SetCurAnimName(const string & strAnimName)
{
	m_CurAnimName = strAnimName;
}

void CModel::Play_Animation(_double TimeDelta)
{
	if (m_eType == TYPE_NONANIM)
	{
		// IM_WARN("STATIC Model Cannot player animation.");
		return;
	}
	if (auto pAnim = Find_Animation(m_CurAnimName))
	{
		pAnim->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);

		KEYFRAME tempKeyFrame = *pAnim->GetCurKeyFrame();
		if (tempKeyFrame.Time != m_CurKeyFrame.Time)
		{
			m_BefKeyFrame = m_CurKeyFrame;
			m_CurKeyFrame = tempKeyFrame;
		}
	}

	Compute_CombindTransformationMatrix();
}

void CModel::Play_Animation_Test(_double TimeDelta)
{
	if (m_eType == TYPE_NONANIM)
	{
		IM_WARN("STATIC Model Cannot player animation.");
		return;
	}
	// m_Animations[m_iAnimIdx]->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
	//
	// SetBoneMask(EBoneMask::UPPER_BLOCK, "R_Shoulder");
	// SetBoneMask(EBoneMask::UPPER_BLOCK, "L_Shoulder");
	// m_Animations[m_iAdditiveAnimIdx]->Update_Bones(TimeDelta, EAnimUpdateType::ADDITIVE, m_fAdditiveRatio);
	// SetBoneMask(EBoneMask::MASK_OFF);
	//
	// Compute_CombindTransformationMatrix();
	//
	//
	// m_Animations[m_iHeadAnim]->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
	//
	// SetBoneMask(EBoneMask::OFF_CHILD_EQ, "IK_RootTarget");
	// SetBoneMask(EBoneMask::OFF_CHILD_EQ, "Neck");
	// m_Animations[m_iUB]->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
	//
	// SetBoneMask(EBoneMask::OFF_CHILD_EQ, "Spine1");
	// m_Animations[m_iLB]->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
	//
	//
	// SetBoneMask(EBoneMask::ON_ALL);
	// SetBoneMask(EBoneMask::OFF_PARENT, "Pelvis");
	// SetBoneMask(EBoneMask::OFF_CHILD_EQ, "Neck");
	// m_Animations[m_iAdditiveAnimIdx]->Update_Bones(TimeDelta, EAnimUpdateType::ADDITIVE, m_fAdditiveRatio);
	// SetBoneMask(EBoneMask::ON_ALL);


	Compute_CombindTransformationMatrix();
}

// HRESULT CModel::RenderCustomShader(_uint iPass, CShader* pShader)
// {
// 	for (const auto& mesh : m_Meshes)
// 	{
// 		if (m_eType == TYPE_ANIM)
// 		{
// 			_float4x4 BoneMatrices[128];
// 			mesh->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));
// 			pShader->Set_MatrixArray("g_BoneMatrices", BoneMatrices, 128);
// 		}
//
// 		pShader->Begin(iPass);
// 		mesh->Render();
// 	}
//
// 	return S_OK;
// }

HRESULT CModel::Render(CTransform* pTransform)
{
	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		RenderMesh(pTransform, static_cast<_uint>(i));
	}

	return S_OK;
}

HRESULT CModel::Render(const _float4x4& WorldMatrix)
{
	for (const auto& mesh : m_Meshes)
	{
		const _uint iMtrlIdx = mesh->Get_MaterialIndex();
		if (m_Materials[iMtrlIdx]->IsActive() == false)
			continue;

		if (m_eType == TYPE_ANIM)
		{
			_float4x4 BoneMatrices[512];
			mesh->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));
			m_Materials[iMtrlIdx]->GetShader()->Set_MatrixArray("g_BoneMatrices", BoneMatrices, 512);
		}

		m_Materials[iMtrlIdx]->BindMatrices(WorldMatrix);
		m_Materials[iMtrlIdx]->Begin();
		mesh->Render();
	}

	return S_OK;
}

HRESULT CModel::RenderMesh(CTransform* pTransform, _uint iMeshIdx)
{
	const _uint iMtrlIdx = m_Meshes[iMeshIdx]->Get_MaterialIndex();
	if (m_Materials[iMtrlIdx]->IsActive() == false)
		return S_OK;

	if (m_eType == TYPE_ANIM)
	{
		_float4x4 BoneMatrices[512];
		m_Meshes[iMeshIdx]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));
		m_Materials[iMtrlIdx]->GetShader()->Set_MatrixArray("g_BoneMatrices", BoneMatrices, 512);
	}

	m_Materials[iMtrlIdx]->BindMatrices(pTransform);
	m_Materials[iMtrlIdx]->Begin();
	return m_Meshes[iMeshIdx]->Render();
}

HRESULT CModel::RenderMeshOnly(_uint iMeshIdx) const
{
	return m_Meshes[iMeshIdx]->Render();
}

HRESULT CModel::Render_ShadowDepth(CTransform* pTransform)
{
	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		const _uint iMtrlIdx = m_Meshes[i]->Get_MaterialIndex();
		if (m_Materials[iMtrlIdx]->IsActive() == false)
			continue;

		if (m_eType == TYPE_ANIM)
		{
			_float4x4 BoneMatrices[512];
			m_Meshes[i]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));
			m_pShadowShader->Set_MatrixArray("g_BoneMatrices", BoneMatrices, 512);
		}
		
		FAILED_CHECK(pTransform->Bind_ShaderResource(m_pShadowShader, "g_WorldMatrix"));

		FAILED_CHECK(m_pShadowShader->Set_Matrix("g_ViewMatrix", CLight_Manager::GetInstance()->GetDirectionalLightView()));
		FAILED_CHECK(m_pShadowShader->Set_Matrix("g_ProjMatrix", CLight_Manager::GetInstance()->GetDirectionalLightProj()));

		m_pShadowShader->Begin(0);
		m_Meshes[i]->Render();
	}

	return S_OK;
}

void CModel::LoadAnimations(const char* pAnimDir)
{
	Assert(m_eType == TYPE_ANIM);

	CGameUtils::ListFiles(pAnimDir, [this](const string& pAnimPath)
	{
		auto pAnim = CAnimation::Create(pAnimPath.c_str());
		m_mapAnimation.emplace(pAnim->GetName(), pAnim);
	});

	int iA = 0;
}

void CModel::AddBoneRotation(const string& strBoneName, Quaternion qQuat)
{
	CBone* pBone = Get_BonePtr(strBoneName);
	Assert(pBone != nullptr);

	
	// pBone->Set_TransformMatrix(XMMatrixRotationQuaternion(qQuat) * XMLoadFloat4x4(&pBone->Get_TransformMatrix()));
	pBone->Set_TransformMatrix(XMMatrixRotationQuaternion(qQuat) * XMLoadFloat4x4(&pBone->Get_TransformMatrix()));
}

void CModel::SetBoneMask(EBoneMask eMask, const string& strBoneName)
{
	CBone* pStartBone = Get_BonePtr(strBoneName);

	switch (eMask)
	{
	case EBoneMask::OFF_PARENT_EQ:
		Assert(pStartBone != nullptr);
		pStartBone->DisableToParentRecursive(true);
		break;
	case EBoneMask::OFF_PARENT:
		Assert(pStartBone != nullptr);
		pStartBone->DisableToParentRecursive(true);
		pStartBone->SetDisableAnimation(false);
		break;
	case EBoneMask::OFF_CHILD_EQ:
		Assert(pStartBone != nullptr);
		pStartBone->DisableToChildRecursive(true);
		break;
	case EBoneMask::OFF_CHILD:
		Assert(pStartBone != nullptr);
		pStartBone->DisableToChildRecursive(true);
		pStartBone->SetDisableAnimation(false);
		break;
	case EBoneMask::OFF_EQ:
		Assert(pStartBone != nullptr);
		pStartBone->SetDisableAnimation(true);
		break;
	case EBoneMask::OFF_ALL:
		for (const auto& pBone : m_mapBones)
			pBone.second->SetDisableAnimation(true);
		break;
	case EBoneMask::ON_PARENT_EQ:
		Assert(pStartBone != nullptr);
		pStartBone->DisableToParentRecursive(false);
		break;
	case EBoneMask::ON_PARENT:
		Assert(pStartBone != nullptr);
		pStartBone->DisableToParentRecursive(false);
		pStartBone->SetDisableAnimation(true);
		break;
	case EBoneMask::ON_CHILD_EQ:
		Assert(pStartBone != nullptr);
		pStartBone->DisableToChildRecursive(false);
		break;
	case EBoneMask::ON_CHILD:
		Assert(pStartBone != nullptr);
		pStartBone->DisableToChildRecursive(false);
		pStartBone->SetDisableAnimation(true);
		break;
	case EBoneMask::ON_EQ:
		Assert(pStartBone != nullptr);
		pStartBone->SetDisableAnimation(false);
		break;
	case EBoneMask::ON_ALL:
		for (const auto& pBone : m_mapBones)
			pBone.second->SetDisableAnimation(false);
		break;
	case EBoneMask::BONE_MASK_END:
		FALLTHROUGH
	default: ;
		NODEFAULT
	}
}

void CModel::Compute_CombindTransformationMatrix()
{
	m_pRootBone->Compute_CombindTransformationMatrix_StartRoot();
}		

CMaterial* CModel::FindMaterial(const _tchar* pMtrlProtoTag)
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

void CModel::Ready_Bones(const Json& jBone, CBone* pParent)
{
	CBone*		pBone = CBone::Create(jBone, pParent);
	Assert(pBone != nullptr);

	if (pParent == nullptr)
	{
		m_pRootBone = pBone;
		Safe_AddRef(m_pRootBone);
	}

	// 원본 가져기 있는곳, 삭제를 위해 addref하지 않는다.
	m_mapBones.emplace(jBone["Name"].get<string>(), pBone);

	if (jBone.contains("Children"))
	{
		for (auto& jBoneChild : jBone["Children"])
		{
			Ready_Bones(jBoneChild, pBone);
		}
	}
}

void CModel::SetBoneChildren(const Json& jBone)
{
	const string boneName = jBone["Name"].get<string>();
	CBone* pBone = Get_BonePtr(boneName.c_str());

	if (jBone.contains("Children"))
	{
		for (auto& jBoneChild : jBone["Children"])
		{
			const string childBoneName = jBoneChild["Name"].get<string>();
			CBone* pChildBone = Get_BonePtr(childBoneName.c_str());
			pBone->AddChildBone(pChildBone);
		}

		for (auto& jBoneChild : jBone["Children"])
		{
			SetBoneChildren(jBoneChild);
		}
	}
}

HRESULT CModel::Ready_Materials(HANDLE hFile)
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
			if (m_eType == TYPE_NONANIM)
			{
				pMtrl = dynamic_cast<CMaterial*>(CGameInstance::GetInstance()->Clone_Component(L"Proto_Mtrl_Empty"));
			}
			else if (m_eType == TYPE_ANIM)
			{
				pMtrl = dynamic_cast<CMaterial*>(CGameInstance::GetInstance()->Clone_Component(L"Proto_MtrlAnim_Empty"));
			}
			IM_WARN("Fail to Find Mtrl : %s", mtrlName.c_str());
		}
	
		pMtrl->SetAnimType(m_eType);
		m_Materials.push_back(pMtrl);
	}

	return S_OK;
}

void CModel::EventCaller(const string& EventName)
{
	for (auto& iter : m_EventFunc)
	{
		if (EventName == iter.first) iter.second();
	}
}

void CModel::Add_EventCaller(const string & EventName, std::function<void(void)> Func)
{
	m_EventFunc.emplace(EventName, Func);
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char * pModelFilePath, _float4x4 PivotMatrix)
{
	CModel*		pInstance = new CModel(pDevice, pContext);
	pInstance->m_PivotMatrix = PivotMatrix;

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*		pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();

	for (auto& BonePair : m_mapBones)
		Safe_Release(BonePair.second);
	m_mapBones.clear();

	for (auto& pMtrl : m_Materials)
		Safe_Release(pMtrl);
	m_Materials.clear();

	for (auto pAnimPair : m_mapAnimation)
		Safe_Release(pAnimPair.second);
	m_mapAnimation.clear();

	Safe_Release(m_pRootBone);

	Safe_Release(m_pShadowShader);
}
