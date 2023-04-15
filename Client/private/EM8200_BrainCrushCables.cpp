#include "stdafx.h"
#include "EM8200_BrainCrushCables.h"

#include <Animation.h>

#include "PxModel.h"
#include <GameInstance.h>
#include "Material.h"
#include "JsonStorage.h"
#include "FSMComponent.h"
#include "Layer.h"
#include "SkyBox.h"
#include "CurveFloatMapImpl.h"
#include "ImguiUtils.h"

CEM8200_BrainCrushCables::CEM8200_BrainCrushCables(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEM8200_BrainCrushCables::CEM8200_BrainCrushCables(const CEM8200_BrainCrushCables& rhs)
	: CGameObject(rhs)
	, m_CablePivots(rhs.m_CablePivots)
	, m_CableBones(rhs.m_CableBones)
{
}

HRESULT CEM8200_BrainCrushCables::Initialize_Prototype()
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
			model->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Cables/Animation_co0100/");
			CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Prototype_PxModel_co0100", model);
		}
	}

	m_CableBones[HEAD_1] = "Neck";
	m_CableBones[HEAD_2] = "Neck";
	m_CableBones[HEAD_3] = "Neck";
	for (int i = BODY_1; i < CABLE_END; ++i)
		m_CableBones[i] = "Spine1";

	m_CablePivots[HEAD_1] = CImguiUtils::CreateMatrixFromImGuizmoData({0.135f, -0.036f, 0.006f}, {14.003f, -21.859f, -28.199f}, _float3::One);
	m_CablePivots[HEAD_2] = CImguiUtils::CreateMatrixFromImGuizmoData({0.160f, 0.001f, -0.058f}, {105.701f, -5.449f,-122.234f}, _float3::One);
	m_CablePivots[HEAD_3] = CImguiUtils::CreateMatrixFromImGuizmoData({0.216f, -0.088f, -0.096f}, {48.460f, -8.328f, -9.263f}, _float3::One);
	m_CablePivots[BODY_1] = CImguiUtils::CreateMatrixFromImGuizmoData({-0.001f, 0.001f, -0.018f}, {-56.058f, 14.490f, -2.213f}, _float3::One);
	m_CablePivots[BODY_2] = CImguiUtils::CreateMatrixFromImGuizmoData({0.075f, -0.025f, 0.032f}, {-112.824f, 20.395f, -15.451f}, _float3::One);
	m_CablePivots[BODY_3] = CImguiUtils::CreateMatrixFromImGuizmoData({0.167f, 0.004f, 0.097f}, {-147.102f, 17.386f, -23.905f}, _float3::One);
	m_CablePivots[BODY_4] = CImguiUtils::CreateMatrixFromImGuizmoData({-0.118f, 0.000f, 0.045f}, {167.413f, 7.099f, -29.859}, _float3::One);
	m_CablePivots[BODY_5] = CImguiUtils::CreateMatrixFromImGuizmoData({-0.251f, 0.050f, 0.022f}, {74.962f, -10.279f, -15.694f}, _float3::One);
	m_CablePivots[BODY_6] = CImguiUtils::CreateMatrixFromImGuizmoData({0.080f, 0.021f, -0.048f}, {37.301f, -7.836f, -6.134f}, _float3::One);
	m_CablePivots[BODY_7] = CImguiUtils::CreateMatrixFromImGuizmoData({0.157f, 0.069f, -0.128f,}, {-70.573f, 14.990f, 11.258f}, _float3::One);
	m_CablePivots[BODY_8] = CImguiUtils::CreateMatrixFromImGuizmoData({0.164f, 0.045f, -0.063f}, {-94.954f, 16.851f, 11.747f}, _float3::One);
	m_CablePivots[BODY_9] = CImguiUtils::CreateMatrixFromImGuizmoData({-0.054f, 0.035f, 0.011f}, {90.380f, -9.435f, -19.702}, _float3::One);
	m_CablePivots[BODY_10] = CImguiUtils::CreateMatrixFromImGuizmoData({-0.206f, -0.082f, -0.001f}, {}, _float3::One);
	m_CablePivots[BODY_11] = CImguiUtils::CreateMatrixFromImGuizmoData({0.124f, 0.038f, 0.111f}, {61.748f, -10.143f, -12.188}, _float3::One);
	m_CablePivots[BODY_12] = CImguiUtils::CreateMatrixFromImGuizmoData({}, {}, _float3::One);

	return S_OK;
}

HRESULT CEM8200_BrainCrushCables::Initialize(void* pArg)
{
	FAILED_CHECK(CGameObject::Initialize(pArg));
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));
	
	for (int i = HEAD_1; i < BODY_1; ++i)
	{
		CPxModel* pModel = nullptr;
		wstring wstrModelTag = L"Cable" + to_wstring(i + 1);
		FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_PxModel_co0100", wstrModelTag.c_str(), (CComponent**)&pModel));
		m_CableModels[i] = pModel;
	}
	for (int i = BODY_1; i < CABLE_END; ++i)
	{
		CPxModel* pModel = nullptr;
		wstring wstrModelTag = L"Cable" + to_wstring(i + 1);
		FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_PxModel_co0101", wstrModelTag.c_str(), (CComponent**)&pModel));
		m_CableModels[i] = pModel;
	}

	SetUpFSM();

	return S_OK;
}

void CEM8200_BrainCrushCables::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);
	m_pFSM->Tick(TimeDelta);
}

void CEM8200_BrainCrushCables::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);
}

HRESULT CEM8200_BrainCrushCables::Render()
{
	return CGameObject::Render();
}

void CEM8200_BrainCrushCables::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();

	if (ImGui::CollapsingHeader("Pivot Bone"))
	{
		 static int iIdx = 0;
		
		 ImGui::InputInt("Idx", &iIdx);
		 if (iIdx < 0 || iIdx >= CABLE_END)
	 		iIdx = 0;

		 static char szBoneName[MAX_PATH]{};
		 ImGui::InputText("BoneName", szBoneName, MAX_PATH);
		 if (ImGui::Button("change BoneName") && m_pTargetModel->Get_BonePtr(string(szBoneName)) != nullptr)
		 {
			 m_CableBones[iIdx] = string(szBoneName);
		 }

		ImGui::Text("CurBone : %s", m_CableBones[iIdx].c_str());
		 static GUIZMO_INFO tinfo;

		const _matrix TargetWorldMatrix = m_pTargetTransform->Get_WorldMatrix();
		_matrix tmp = XMLoadFloat4x4(&m_CablePivots[iIdx]) * m_pTargetModel->GetBoneMatrix(m_CableBones[iIdx]) * TargetWorldMatrix;
		_float4x4 tmp2 = tmp;
		 CImguiUtils::Render_Guizmo(&tmp2, tinfo, false, true);
		 m_CablePivots[iIdx] = tmp2 * m_pTargetTransform->Get_WorldMatrix_Inverse() * XMMatrixInverse(nullptr, m_pTargetModel->GetBoneMatrix(m_CableBones[iIdx]));

		CImguiUtils::Render_Guizmo(&m_CablePivots[iIdx], tinfo, true, false);
	}
}

void CEM8200_BrainCrushCables::Activate(_bool bActive)
{
	m_bActive = bActive;
}

void CEM8200_BrainCrushCables::SetTargetInfo(CTransform* pTargetTransform, CModel* pTargetModel)
{
	Safe_Release(m_pTargetModel);
	Safe_Release(m_pTargetTransform);
	m_pTargetModel = pTargetModel;
	m_pTargetTransform = pTargetTransform;
	Safe_AddRef(m_pTargetModel);
	Safe_AddRef(m_pTargetTransform);
}

_float CEM8200_BrainCrushCables::GetHeadPlayRatio()
{
	return m_CableModels[HEAD_1]->Find_Animation("AS_BC_em8300_c04_1_co0100_1")->GetPlayRatio();
}

void CEM8200_BrainCrushCables::SetUpFSM()
{
	Safe_Release(m_pFSM);
	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.AddTransition("Idle to BrainCrushStart", "BrainCrushStart")
				.Predicator([this]
				{
					return m_bActive;
				})

		.AddState("BrainCrushStart")
			.OnStart([this]
			{
				m_PhysxHeadOnce.Reset();
				m_PhysxBodyOnce.Reset();
				m_CableModels[HEAD_1]->SetPlayAnimation("AS_BC_em8300_c04_1_co0100_1");
				m_CableModels[HEAD_2]->SetPlayAnimation("AS_BC_em8300_c04_1_co0100_2");
				m_CableModels[HEAD_3]->SetPlayAnimation("AS_BC_em8300_c04_1_co0100_3");
			})
			.Tick([this](_double TimeDelta)
			{
				const _matrix TargetWorldMatrix = m_pTargetTransform->Get_WorldMatrix();
				for (int i = 0; i < BODY_1; ++i)
				{
					_matrix Socket = XMLoadFloat4x4(&m_CablePivots[i]) * m_pTargetModel->GetBoneMatrix(m_CableBones[i]) * TargetWorldMatrix;
					m_CableModels[i]->Update_Tick(TimeDelta, Socket);

					_float4x4 _Socket = Socket;

					CGameInstance::GetInstance()->LambdaRenderRequest(_Socket, [this, _Socket, i]
					{
						m_CableModels[i]->Render_PxModel(_Socket);
					}, CRenderer::RENDER_NONALPHABLEND_TOON);
				}
			})
			.AddTransition("BrainCrushStart to BrainCrushSmallCable", "BrainCrushSmallCable")
				.Predicator([this]
				{
					return GetHeadPlayRatio() > 0.5f;
				})

		.AddState("BrainCrushSmallCable")
			.OnStart([this]
			{
				for (int i = BODY_1; i < CABLE_END; ++i)
				{
					m_CableModels[i]->SetPlayAnimation("AS_co0101_001_start03");
				}
			})
			.Tick([this](_double TimeDelta)
			{
				if (m_CableModels[HEAD_1]->Find_Animation("AS_BC_em8300_c04_1_co0100_1")->IsFinished() 
					&& m_PhysxHeadOnce.IsNotDo())
				{
					for (int i = 0; i < BODY_1; ++i)
						m_CableModels[i]->ActivatePhysX(true);
				}

				if (m_CableModels[BODY_1]->Find_Animation("AS_co0101_001_start03")->IsFinished() 
					&& m_PhysxBodyOnce.IsNotDo())
				{
					for (int i = BODY_1; i < CABLE_END; ++i)
						m_CableModels[i]->ActivatePhysX(true);
				}


				const _matrix TargetWorldMatrix = m_pTargetTransform->Get_WorldMatrix();
				for (int i = 0; i < CABLE_END; ++i)
				{
					_matrix Socket = XMLoadFloat4x4(&m_CablePivots[i]) * m_pTargetModel->GetBoneMatrix(m_CableBones[i]) * TargetWorldMatrix;
					m_CableModels[i]->Update_Tick(TimeDelta, Socket);

					_float4x4 _Socket = Socket;

					CGameInstance::GetInstance()->LambdaRenderRequest(_Socket, [this, _Socket, i]
					{
						m_CableModels[i]->Render_PxModel(_Socket);
					}, CRenderer::RENDER_NONALPHABLEND_TOON);
				}	
			})
		.AddTransition("BrainCrushSmallCable to Dissolve", "Dissolve")
			.Predicator([this]
			{
				return !m_bActive;
			})


		.AddState("Dissolve")
			.Tick([this](_double TimeDelta)
			{
				const _matrix TargetWorldMatrix = m_pTargetTransform->Get_WorldMatrix();
				for (int i = 0; i < CABLE_END; ++i)
				{
					for (auto pMtrl : m_CableModels[i]->GetMaterials())
					{
						if (pMtrl->GetParam().Floats[0] <= 1.f)
							pMtrl->GetParam().Floats[0] += TimeDelta;
					}

					_matrix Socket = XMLoadFloat4x4(&m_CablePivots[i]) * m_pTargetModel->GetBoneMatrix(m_CableBones[i]) * TargetWorldMatrix;
					m_CableModels[i]->Update_Tick(TimeDelta, Socket);

					_float4x4 _Socket = Socket;
					CGameInstance::GetInstance()->LambdaRenderRequest(_Socket, [this, _Socket, i]
					{
						m_CableModels[i]->Render_PxModel(_Socket);
					}, CRenderer::RENDER_NONALPHABLEND_TOON);
				}	
			})
			.AddTransition("Dissolve to Idle", "Idle")
				.Predicator([this]
				{
					return m_CableModels[0]->GetMaterials()[0]->GetParam().Floats[0] > 0.95f;
				})



	.Build();
}

void CEM8200_BrainCrushCables::Free()
{
	CGameObject::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTargetModel);
	Safe_Release(m_pTargetTransform);
	Safe_Release(m_pFSM);
	for (int i = 0; i < CABLE_END; ++i)
	{
		Safe_Release(m_CableModels[i]);
		m_CableModels[i] = nullptr;
	}
}

CEM8200_BrainCrushCables* CEM8200_BrainCrushCables::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEM8200_BrainCrushCables* pInstance = new CEM8200_BrainCrushCables(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM8200_BrainCrushCables");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CEM8200_BrainCrushCables::Clone(void* pArg)
{
	CEM8200_BrainCrushCables* pInstance = new CEM8200_BrainCrushCables(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM8200_BrainCrushCables");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}
