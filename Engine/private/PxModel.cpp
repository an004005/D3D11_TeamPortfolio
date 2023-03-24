#include "stdafx.h"
#include "..\public\PxModel.h"

#include "Animation.h"
#include "PxBone.h"
#include "JsonLib.h"
#include "Mesh.h"
#include "Material.h"
#include "ImguiUtils.h"

using namespace physx;

CPxModel::CPxModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CModel(pDevice, pContext)
{
	m_bPx = true;
}

CPxModel::CPxModel(const CPxModel& rhs)
	: CModel(rhs)
	, m_ScalePivot(rhs.m_ScalePivot)
{
}

HRESULT CPxModel::Initialize_Prototype(const char* pModelFilePath)
{
	char szExt[MAX_PATH];
	_splitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
	Assert(strcmp(szExt, ".anim_model") == 0); // anim¸ðµ¨¸¸ °¡´É

	return CModel::Initialize_Prototype(pModelFilePath);
}

HRESULT CPxModel::Initialize(void* pArg)
{
	FAILED_CHECK(CModel::Initialize(pArg));

	m_pStartBone = dynamic_cast<CPxBone*>(Get_BonePtr("cable_02_a"));

	return S_OK;
}

void CPxModel::Imgui_RenderProperty()
{
	CModel::Imgui_RenderProperty();

	if (ImGui::Button("Mode"))
	{
		ActivatePhysX(!m_bActivatePhysX);
	}
	if (ImGui::Button("Reset Anim"))
	{
		if (auto pAnim = Find_Animation(m_CurAnimName))
		{
			pAnim->Reset();
		}
	}

	static char test[MAX_PATH]{};
	ImGui::InputText("bone", test, MAX_PATH);
	if (Get_BonePtr(test))
	{
		_float4x4 test2 = GetBoneMatrix(test, true) * XMLoadFloat4x4(&m_WorldMatrix);

		static GUIZMO_INFO tInfo;
		CImguiUtils::Render_Guizmo(&test2, tInfo, true, true);
	}
}

void CPxModel::Update_Tick(_double TimeDelta, const _float4x4& WorldMatrix)
{
	_matrix tmp = WorldMatrix;
	tmp.r[0] = XMVector3Normalize(tmp.r[0]);
	tmp.r[1] = XMVector3Normalize(tmp.r[1]);
	tmp.r[2] = XMVector3Normalize(tmp.r[2]);
	m_WorldMatrix = WorldMatrix;


	if (m_bActivatePhysX)
	{
		m_pStartBone->SetWorldMatrix(m_WorldMatrix, m_PivotMatrix);
		// auto pNotControlledBone = dynamic_cast<CPxBone*>(Get_BonePtr("Reference"));
		// pNotControlledBone->SetNotControlledWorld(pNotControlledBone->Get_CombindMatrix() * XMLoadFloat4x4(&m_PivotMatrix) * XMLoadFloat4x4(&m_WorldMatrix));

	}
	else
	{
		Play_Animation(TimeDelta);
	}
}

void CPxModel::Render_PxModel(_fmatrix WorldMatrix)
{
	_matrix TempWorldMatrix = WorldMatrix;
	TempWorldMatrix.r[0] = XMVector3Normalize(TempWorldMatrix.r[0]);
	TempWorldMatrix.r[1] = XMVector3Normalize(TempWorldMatrix.r[1]);
	TempWorldMatrix.r[2] = XMVector3Normalize(TempWorldMatrix.r[2]);
	m_WorldMatrix = TempWorldMatrix;

	if (m_bActivatePhysX)
	{
		m_pStartBone->SetWorldMatrix(m_WorldMatrix, m_PivotMatrix);
		auto pNotControlledBone = dynamic_cast<CPxBone*>(Get_BonePtr("Reference"));
		pNotControlledBone->SetNotControlledWorld(pNotControlledBone->Get_CombindMatrix() * XMLoadFloat4x4(&m_PivotMatrix) * TempWorldMatrix);

		_matrix WorldMatrixInv = XMMatrixInverse(nullptr, TempWorldMatrix);
		for (size_t i = 0; i < m_Meshes.size(); ++i)
		{
			const _uint iMtrlIdx = m_Meshes[i]->Get_MaterialIndex();
			if (m_Materials[iMtrlIdx]->IsActive() == false)
				continue;
	
			_float4x4 BoneMatrices[512];
			m_Meshes[i]->SetUp_PxBoneMatrices(BoneMatrices, WorldMatrixInv);
			m_Materials[iMtrlIdx]->GetShader()->Set_MatrixArray("g_BoneMatrices", BoneMatrices, 512);
	
			m_Materials[iMtrlIdx]->BindMatrices(m_WorldMatrix);
			m_Materials[iMtrlIdx]->Begin();
			FAILED_CHECK(m_Meshes[i]->Render());
		}
	}
	else
	{
		Render(m_WorldMatrix);
	}
}

void CPxModel::ActivatePhysX(_bool bActive)
{
	m_bActivatePhysX = bActive;
	if (m_bActivatePhysX)
	{
		if (m_pStartBone->GetActor() == nullptr)
			CreateJoints();
	}
	else
	{
		if (m_pStartBone->GetActor() != nullptr)
			ReleaseJoints();
	}
}

void CPxModel::CreateJoints()
{
	m_pStartBone->CreateJointsRecur(nullptr, m_PivotMatrix, m_WorldMatrix);
}

void CPxModel::ReleaseJoints()
{
	m_pStartBone->ReleaseJointsRecur();
}

CPxModel* CPxModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pModelFilePath,
                           _float4x4 PivotMatrix)
{
	CPxModel*		pInstance = new CPxModel(pDevice, pContext);
	pInstance->m_PivotMatrix = PivotMatrix;

	_matrix tmp = pInstance->m_PivotMatrix;
	pInstance->m_ScalePivot = _float4x4::CreateScale(
		XMVectorGetX(XMVector3Length(tmp.r[0])), XMVectorGetX(XMVector3Length(tmp.r[1])), XMVectorGetX(XMVector3Length(tmp.r[2])));

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath)))
	{
		MSG_BOX("Failed to Created : CPxModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CPxModel::Clone(void* pArg)
{
	CPxModel*		pInstance = new CPxModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPxModel::Free()
{
	CModel::Free();

}
