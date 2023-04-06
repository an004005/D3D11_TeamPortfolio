#include "stdafx.h"
#include "BrainField.h"

#include <Animation.h>
#include <GameInstance.h>
#include "ImguiUtils.h"
#include "PxModel.h"


/**********************
 * CBrainFieldCables
 **********************/
CBrainFieldCables::CBrainFieldCables(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBrainFieldCables::CBrainFieldCables(const CBrainFieldCables& rhs)
	: CGameObject(rhs)
	, m_CablePivots(rhs.m_CablePivots)
	, m_CableBones(rhs.m_CableBones)
{


}

HRESULT CBrainFieldCables::Initialize_Prototype()
{
	FAILED_CHECK(CGameObject::Initialize_Prototype());

	{
		auto pPrototype = CGameInstance::GetInstance()->Find_Prototype_Component(LEVEL_STATIC, L"Prototype_PxModel_co0101");
		if (pPrototype == nullptr)
		{
			auto model = CPxModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Cables/co0101.anim_model");
			model->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Cables/Animation/");
			CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Prototype_PxModel_co0101", model);
		}
	}
	{
		auto pPrototype = CGameInstance::GetInstance()->Find_Prototype_Component(LEVEL_STATIC, L"Prototype_PxModel_co0100");
		if (pPrototype == nullptr)
		{
			auto model = CPxModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Cables/co0100.anim_model");
			model->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Cables/Animation/");
			CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Prototype_PxModel_co0100", model);
		}
	}

	// 플레이어용(ch0100)
	m_CablePivots[BACK_1] = CImguiUtils::CreateMatrixFromImGuizmoData({ -0.088f, 0.048f, 0.085f }, {-122.911f, 12.644f, -161.314f}, _float3::One);
	m_CablePivots[BACK_2] = CImguiUtils::CreateMatrixFromImGuizmoData({ -0.100f, -0.027f, -0.112f }, {-105.225f, -5.311f, 161.213f}, _float3::One);
	m_CablePivots[BACK_3] = CImguiUtils::CreateMatrixFromImGuizmoData({ -0.045f, 0.073f, -0.061f }, {-180.000f, -3.719f, -180.000f }, _float3::One);
	m_CablePivots[BACK_4] = CImguiUtils::CreateMatrixFromImGuizmoData({ 0.000f, 0.000f, -0.135f }, {-180.000f,  13.680f, -180.000f}, _float3::One);
	m_CablePivots[BACK_5] = CImguiUtils::CreateMatrixFromImGuizmoData({ -0.130f, 0.000f, -0.127f }, {-180.000f, 4.923f, -180.000f}, _float3::One);

	m_CableBones[BACK_1] = "Neck";
	m_CableBones[BACK_2] = "Neck";
	m_CableBones[BACK_3] = "String";
	m_CableBones[BACK_4] = "String";
	m_CableBones[BACK_5] = "String";

	return S_OK;
}

HRESULT CBrainFieldCables::Initialize(void* pArg)
{
	FAILED_CHECK(CGameObject::Initialize(pArg));
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	for (int i = HEAD_1; i < BACK_1; ++i)
	{
		CPxModel* pModel = nullptr;
		wstring wstrModelTag = L"Cable" + to_wstring(i + 1);
		FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_PxModel_co0100", wstrModelTag.c_str(), (CComponent**)&pModel));
		m_CableModels[i] = pModel;
	}
	for (int i = BACK_1; i < CABLE_END; ++i)
	{
		CPxModel* pModel = nullptr;
		wstring wstrModelTag = L"Cable" + to_wstring(i + 1);
		FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_PxModel_co0101", wstrModelTag.c_str(), (CComponent**)&pModel));
		m_CableModels[i] = pModel;
	}

	m_fDefaultCableLookTicPerSec = m_CableModels[0]->Find_Animation("AS_co0103_001_RT_loop01")->GetTickPerSec();
	for (int i = 0; i < CABLE_END; ++i)
	{
		m_CableModels[i]->Find_Animation("AS_co0101_001_start01")->SetTickPerSec(m_CableModels[i]->Find_Animation("AS_co0101_001_start01")->GetTickPerSec() * 2.f);
		m_CableModels[i]->Find_Animation("AS_co0101_001_start02")->SetTickPerSec(m_CableModels[i]->Find_Animation("AS_co0101_001_start02")->GetTickPerSec() * 2.f);
		m_CableModels[i]->Find_Animation("AS_co0101_001_start03")->SetTickPerSec(m_CableModels[i]->Find_Animation("AS_co0101_001_start03")->GetTickPerSec() * 2.f);
		m_CableModels[i]->Find_Animation("AS_co0101_001_start04")->SetTickPerSec(m_CableModels[i]->Find_Animation("AS_co0101_001_start04")->GetTickPerSec() * 2.f);
		m_CableModels[i]->Find_Animation("AS_co0101_001_start05")->SetTickPerSec(m_CableModels[i]->Find_Animation("AS_co0101_001_start05")->GetTickPerSec() * 2.f);
	}

	m_pWatchAnim = m_CableModels[BACK_1]->Find_Animation("AS_co0101_001_start01");
	for (int i = 0; i < CABLE_END; ++i)
	{
		m_CableModels[i]->Find_Animation("AS_co0103_001_RT_loop01")->SetLooping(false);
	}

	return S_OK;
}

void CBrainFieldCables::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);



}

void CBrainFieldCables::Late_Tick(_double TimeDelta)
{
	if (m_bVisible)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND_TOON, this);
}

HRESULT CBrainFieldCables::Render()
{
	const _matrix TargetWorldMatrix = m_pTargetTransform->Get_WorldMatrix();
	for (int i = 0; i < CABLE_END; ++i)
	{
		const _matrix SocketMatrix = XMLoadFloat4x4(&m_CablePivots[i]) * m_pTargetModel->GetBoneMatrix(m_CableBones[i]) * TargetWorldMatrix;
		m_CableModels[i]->Render_PxModel(SocketMatrix);
	}
	return S_OK;
}

void CBrainFieldCables::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();
}

void CBrainFieldCables::Activate(_bool bActive)
{
}

void CBrainFieldCables::ActivatePhysX(_bool bActive)
{
}

void CBrainFieldCables::SetLoopAnimTickPerSecRatio(_float fRatio)
{
	for (int i = 0; i < CABLE_END; ++i)
	{
		m_CableModels[i]->Find_Animation("AS_co0103_001_RT_loop01")->SetTickPerSec((_double)fRatio * m_fDefaultCableLookTicPerSec);
	}
}

void CBrainFieldCables::SetTargetInfo(CTransform* pTargetTransform, CModel* pTargetModel)
{
	Safe_Release(m_pTargetModel);
	Safe_Release(m_pTargetTransform);
	m_pTargetModel = pTargetModel;
	m_pTargetTransform = pTargetTransform;
	Safe_AddRef(m_pTargetModel);
	Safe_AddRef(m_pTargetTransform);
}

void CBrainFieldCables::Free()
{
	CGameObject::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTargetModel);
	Safe_Release(m_pTargetTransform);
	for (int i = 0; i < CABLE_END; ++i)
	{
		Safe_Release(m_CableModels[i]);
		m_CableModels[i] = nullptr;
	}
}
