#include "stdafx.h"
#include "..\public\Bone.h"
#include "JsonLib.h"


CBone::CBone()
{
}

void CBone::AddChildBone(CBone* pBone)
{
	Assert(pBone != nullptr);
	m_Children.push_back(pBone);

	// 상호참조 막기
	// Safe_AddRef(pBone);
}

void CBone::DisableToParentRecursive(_bool bDisable)
{
	m_bDisableAnim = bDisable;
	if (m_pParent)
	{
		m_pParent->DisableToParentRecursive(bDisable);
	}
}

void CBone::DisableToChildRecursive(_bool bDisable)
{
	m_bDisableAnim = bDisable;
	for (auto pChild : m_Children)
	{
		pChild->DisableToChildRecursive(bDisable);
	}
}

HRESULT CBone::Initialize(const Json& jBone, CBone* pParent)
{
	jBone["Name"].get_to(m_strName);
	jBone["OffsetMatrix"].get_to(m_OffsetMatrix);
	jBone["TransformMatrix"].get_to(m_TransformMatrix);

	XMStoreFloat4x4(&m_CombindTransformMatrix, XMMatrixIdentity());

	m_pParent = pParent;

	// 상호참조 
	// Safe_AddRef(m_pParent);

	return S_OK;
}

void CBone::Compute_CombindTransformationMatrix_StartRoot()
{
	// 루트본에서 이 함수를 실행한다.
	Assert(m_pParent == nullptr);
	m_CombindTransformMatrix = m_TransformMatrix;
	for (const auto pChild : m_Children)
	{
		pChild->Compute_CombindTransformationMatrix();
	}
}

void CBone::Compute_CombindTransformationMatrix()
{
	Assert(m_pParent != nullptr);
	XMStoreFloat4x4(&m_CombindTransformMatrix, XMLoadFloat4x4(&m_TransformMatrix) * XMLoadFloat4x4(&m_pParent->m_CombindTransformMatrix));
	for (const auto pChild : m_Children)
	{
		pChild->Compute_CombindTransformationMatrix();
	}
}

void CBone::Imgui_SkeletalViewRecursive(string& strSelected, ImGuiTreeNodeFlags baseFlags)
{
	ImGuiTreeNodeFlags nodeFlags = baseFlags;

	if (strSelected == m_strName)
		nodeFlags |= ImGuiTreeNodeFlags_Selected;

	if (ImGui::TreeNodeEx(m_strName.c_str(), nodeFlags, "%s", m_strName.c_str()))
	{
		if (ImGui::IsItemClicked())
			strSelected = m_strName;

		for (const auto& child : m_Children)
			child->Imgui_SkeletalViewRecursive(strSelected, baseFlags);

		ImGui::TreePop();
	}
}

void CBone::MakeDFSTravelVector(vector<CBone*>& vectorOut)
{
	vectorOut.push_back(this);
	for (const auto pChild : m_Children)
	{
		pChild->MakeDFSTravelVector(vectorOut);
	}
}

CBone * CBone::Create(const Json& jBone, CBone* pParent)
{
	CBone*		pInstance = new CBone();

	if (FAILED(pInstance->Initialize(jBone, pParent)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBone::Free()
{
	// Safe_Release(m_pParent);
	// for (auto pChild : m_Children)
	// 	Safe_Release(pChild);
	m_Children.clear();
}
