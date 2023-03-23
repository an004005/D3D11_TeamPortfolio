#include "stdafx.h"
#include "..\public\SAS_Cable.h"
#include "GameInstance.h"
#include "PxModel.h"
#include "Animation.h"
#include "ImguiUtils.h"

CSAS_Cable::CSAS_Cable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CSAS_Cable::CSAS_Cable(const CSAS_Cable& rhs)
	: CGameObject(rhs)
	, m_CablePivots(rhs.m_CablePivots)
{
}

HRESULT CSAS_Cable::Initialize_Prototype()
{
	FAILED_CHECK(CGameObject::Initialize_Prototype());

	auto model = CPxModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Cables/co0101.anim_model");
	model->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Cables/Animation/");
	CGameInstance::GetInstance()->Add_Prototype(LEVEL_NOW, L"Prototype_PxModel_co0101", model);

	m_CablePivots[0] = CImguiUtils::CreateMatrixFromImGuizmoData({ -0.088f, 0.048f, 0.085f }, {-122.911f, 12.644f, -161.314f}, _float3::One);
	m_CablePivots[1] = CImguiUtils::CreateMatrixFromImGuizmoData({ -0.100f, -0.027f, -0.112f }, {-105.225f, -5.311f, 161.213f}, _float3::One);
	m_CablePivots[2] = CImguiUtils::CreateMatrixFromImGuizmoData({ -0.045f, 0.073f, -0.061f }, {-180.000f, -3.719f, -180.000f }, _float3::One);
	m_CablePivots[3] = CImguiUtils::CreateMatrixFromImGuizmoData({ 0.000f, 0.000f, -0.135f }, {-180.000f,  13.680f, -180.000f}, _float3::One);
	m_CablePivots[4] = CImguiUtils::CreateMatrixFromImGuizmoData({ -0.130f, 0.000f, -0.127f }, {-180.000f, 4.923f, -180.000f}, _float3::One);

	return S_OK;
}

HRESULT CSAS_Cable::Initialize(void* pArg)
{
	FAILED_CHECK(CGameObject::Initialize(pArg));

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	for (int i = 0; i < CABLE_CNT; ++i)
	{
		CPxModel* pModel = nullptr;
		wstring wstrModelTag = L"Cable" + to_wstring(i);
		FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_PxModel_co0101", wstrModelTag.c_str(), (CComponent**)&pModel));
		m_CableModels[i] = pModel;
	}

	m_pWatchAnim = m_CableModels[0]->Find_Animation("AS_co0101_001_start01");
	m_pWatchAnim->SetLooping(false);

	return S_OK;
}

void CSAS_Cable::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);
	// if (CGameInstance::GetInstance()->KeyDown(DIK_9))
	// {
	// 	EquipCable(ESASType::SAS_FIRE);
	// }
	// if (CGameInstance::GetInstance()->KeyDown(DIK_8))
	// {
	// 	UnEquipCable();
	// }

	if (m_bActive && m_bVisible)
	{
		_matrix TargetWorldMatrix = m_pTargetTransform->Get_WorldMatrix();
		for (int i = 0; i < CABLE_CNT; ++i)
		{
			m_CableModels[i]->Update_Tick(TimeDelta, CalcSocketMatrix(i, TargetWorldMatrix));
		}
	}


	if (m_bActive && m_pWatchAnim->IsFinished() && m_CableModels[0]->IsActivePhysX() == false)
	{
		for (int i = 0; i < CABLE_CNT; ++i)
		{
			m_CableModels[i]->ActivatePhysX(true);
		}
	}
}

void CSAS_Cable::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	if (m_bActive && m_bVisible)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND_TOON, this);
}

HRESULT CSAS_Cable::Render()
{
	_matrix TargetWorldMatrix = m_pTargetTransform->Get_WorldMatrix();
	for (int i = 0; i < CABLE_CNT; ++i)
	{
		m_CableModels[i]->Render_PxModel(CalcSocketMatrix(i, TargetWorldMatrix));
	}
	return S_OK;
}

void CSAS_Cable::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();
	// static int iIdx = 0;
	//
	// ImGui::InputInt("Idx", &iIdx);
	// if (iIdx < 0 || iIdx >= CABLE_CNT)
	// 	iIdx = 0;
	//
	// static GUIZMO_INFO tinfo;
	// CImguiUtils::Render_Guizmo(&m_CablePivots[iIdx], tinfo, true, true);
	//
	// static char test[MAX_PATH]{};
	// ImGui::InputText("test", test, MAX_PATH);
	// socket = test;
}

void CSAS_Cable::SetTargetInfo(CTransform* pTargetTransform, CModel* pTargetModel)
{
	Safe_Release(m_pTargetModel);
	Safe_Release(m_pTargetTransform);
	m_pTargetModel = pTargetModel;
	m_pTargetTransform = pTargetTransform;
	Safe_AddRef(m_pTargetModel);
	Safe_AddRef(m_pTargetTransform);
}

void CSAS_Cable::EquipCable(ESASType eType)
{
	m_CableModels[0]->ActivatePhysX(false);
	m_CableModels[1]->ActivatePhysX(false);
	m_CableModels[2]->ActivatePhysX(false);
	m_CableModels[3]->ActivatePhysX(false);
	m_CableModels[4]->ActivatePhysX(false);

	m_CableModels[0]->SetPlayAnimation("AS_co0101_001_start01");
	m_CableModels[1]->SetPlayAnimation("AS_co0101_001_start02");
	m_CableModels[2]->SetPlayAnimation("AS_co0101_001_start03");
	m_CableModels[3]->SetPlayAnimation("AS_co0101_001_start04");
	m_CableModels[4]->SetPlayAnimation("AS_co0101_001_start05");
	m_bActive = true;
}

void CSAS_Cable::UnEquipCable()
{
	for (int i = 0; i < CABLE_CNT; ++i)
	{
		m_CableModels[i]->ActivatePhysX(false);
	}
	m_bActive = false;
}

_matrix CSAS_Cable::CalcSocketMatrix(_int iIdx, _fmatrix TargetWorldMatrix)
{
	_matrix SocketMatrix;
	// string boneName = "String";
	// if (m_pTargetModel->Get_BonePtr(socket) != nullptr)
	// {
	// 	boneName = socket;
	// }
	switch (iIdx)
	{
	case 0:
		SocketMatrix = XMLoadFloat4x4(&m_CablePivots[iIdx]) * m_pTargetModel->GetBoneMatrix("Neck") * TargetWorldMatrix;
		break;
	case 1:
		SocketMatrix = XMLoadFloat4x4(&m_CablePivots[iIdx]) * m_pTargetModel->GetBoneMatrix("Neck") * TargetWorldMatrix;
		break;
	case 2:
		SocketMatrix = XMLoadFloat4x4(&m_CablePivots[iIdx]) * m_pTargetModel->GetBoneMatrix("String") * TargetWorldMatrix;
		break;
	case 3:
		SocketMatrix = XMLoadFloat4x4(&m_CablePivots[iIdx]) * m_pTargetModel->GetBoneMatrix("String") * TargetWorldMatrix;
		break;
	case 4:
		SocketMatrix = XMLoadFloat4x4(&m_CablePivots[iIdx]) * m_pTargetModel->GetBoneMatrix("String") * TargetWorldMatrix;
		break;
	default:
		NODEFAULT;
	}
	return SocketMatrix;
}

CSAS_Cable* CSAS_Cable::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSAS_Cable* pInstance = new CSAS_Cable(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSAS_Cable");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CSAS_Cable::Clone(void* pArg)
{
	CSAS_Cable* pInstance = new CSAS_Cable(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSAS_Cable");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSAS_Cable::Free()
{
	CGameObject::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTargetModel);
	Safe_Release(m_pTargetTransform);

	for (int i = 0; i < CABLE_CNT; ++i)
	{
		Safe_Release(m_CableModels[i]);
		m_CableModels[i] = nullptr;
	}
}
