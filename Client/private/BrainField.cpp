#include "stdafx.h"
#include "BrainField.h"
#include <Animation.h>
#include <GameInstance.h>
#include "ImguiUtils.h"
#include "PxModel.h"
#include "FSMComponent.h"


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
	, m_InitialOpenFieldC1_Pivot(rhs.m_InitialOpenFieldC1_Pivot)
	, m_InitialOpenFieldC2_Pivot(rhs.m_InitialOpenFieldC2_Pivot)
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
			model->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Cables/Animation_co0100/");
			CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Prototype_PxModel_co0100", model);
		}
	}

	// 플레이어용(ch0100)
	m_CablePivots[HEAD_1] = CImguiUtils::CreateMatrixFromImGuizmoData({ 0.140f,  0.076f, 0.045f }, {84.540f,  56.801f, 62.620f}, _float3::One);
	m_CablePivots[HEAD_2] = CImguiUtils::CreateMatrixFromImGuizmoData({ 0.134f,  0.057f,  -0.025f }, {-24.605f, 6.286f, -22.580f}, _float3::One);
	m_CablePivots[HEAD_3] = CImguiUtils::CreateMatrixFromImGuizmoData({ 0.109f, 0.071f, -0.078f }, {36.491f,  48.483f, 22.076f}, _float3::One);
	m_CablePivots[BACK_1] = CImguiUtils::CreateMatrixFromImGuizmoData({ -0.088f, 0.048f, 0.085f }, {-122.911f, 12.644f, -161.314f}, _float3::One);
	m_CablePivots[BACK_2] = CImguiUtils::CreateMatrixFromImGuizmoData({ -0.100f, -0.027f, -0.112f }, {-105.225f, -5.311f, 161.213f}, _float3::One);
	m_CablePivots[BACK_3] = CImguiUtils::CreateMatrixFromImGuizmoData({ -0.045f, 0.073f, -0.061f }, {-180.000f, -3.719f, -180.000f }, _float3::One);
	m_CablePivots[BACK_4] = CImguiUtils::CreateMatrixFromImGuizmoData({ 0.000f, 0.000f, -0.135f }, {-180.000f,  13.680f, -180.000f}, _float3::One);
	m_CablePivots[BACK_5] = CImguiUtils::CreateMatrixFromImGuizmoData({ -0.130f, 0.000f, -0.127f }, {-180.000f, 4.923f, -180.000f}, _float3::One);

	m_InitialOpenFieldC1_Pivot[0] = XMMatrixTranslation(0.913f, 2.708f, -1.079f);
	m_InitialOpenFieldC1_Pivot[1] = XMMatrixTranslation(-0.873f, 2.662f, -0.901);
	m_InitialOpenFieldC1_Pivot[2] = XMMatrixTranslation(0.216f, 2.483f, -1.806f);

	m_InitialOpenFieldC2_Pivot[HEAD_1] = XMMatrixTranslation(-0.014f,  2.405f, -0.028f);
	m_InitialOpenFieldC2_Pivot[HEAD_2] = XMMatrixTranslation(-0.085f, 2.405f, -0.028f);
	m_InitialOpenFieldC2_Pivot[HEAD_3] = XMMatrixTranslation(-0.054f, 2.326f, -0.058f);
	m_InitialOpenFieldC2_Pivot[BACK_1] = XMMatrixTranslation(- 0.042177f, 2.09242f,  -0.026341f);
	m_InitialOpenFieldC2_Pivot[BACK_2] = XMMatrixTranslation(0.067513f, 2.09242f, -0.095833f);
	m_InitialOpenFieldC2_Pivot[BACK_3] = XMMatrixTranslation(-0.179586f, 1.88714f, -0.343242f);
	m_InitialOpenFieldC2_Pivot[BACK_4] = XMMatrixTranslation(0.004612f, 1.735f, 0.077705f);
	m_InitialOpenFieldC2_Pivot[BACK_5] = XMMatrixTranslation(-0.129024f, 1.735f, -0.183933f);

	m_CableBones[HEAD_1] = "Neck";
	m_CableBones[HEAD_2] = "Neck";
	m_CableBones[HEAD_3] = "Neck";
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

	for (int i = 0; i < CABLE_END; ++i)
	{

	}

	SetUpFSM();

	return S_OK;
}

void CBrainFieldCables::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	m_pFSM->Tick(TimeDelta);

	if (m_bActive && m_bPhysXOn)
	{
		const _matrix TargetWorldMatrix = m_pTargetTransform->Get_WorldMatrix();
		for (int i = 0; i < CABLE_END; ++i)
		{
			const _matrix SocketMatrix = XMLoadFloat4x4(&m_CablePivots[i]) * m_pTargetModel->GetBoneMatrix(m_CableBones[i]) * TargetWorldMatrix;
			m_CableModels[i]->Update_Tick(TimeDelta, SocketMatrix);
		}
	}
}

void CBrainFieldCables::Late_Tick(_double TimeDelta)
{
	if (m_bActive && m_bPhysXOn)
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


	if (ImGui::CollapsingHeader("UsingOpenPivot"))
	{
		static int iIdx = 0;
		static GUIZMO_INFO tinfo;
		 ImGui::InputInt("Idx", &iIdx);
		 if (iIdx < 0 || iIdx >= 3)
	 		iIdx = 0;

		 CImguiUtils::Render_Guizmo(&m_CableOpenPivot[iIdx], tinfo, true, true);
	}

	if (ImGui::Button("ReFSM"))
		SetUpFSM();
	
	 // static char test[MAX_PATH]{};
	 // ImGui::InputText("test", test, MAX_PATH);
	 // socket = test;


}

void CBrainFieldCables::Activate(_bool bActive)
{
	m_bActive = bActive;
}

void CBrainFieldCables::BrainFieldOpen_CH0100()
{
	m_bActive = true;
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

void CBrainFieldCables::SetUpFSM()
{
	Safe_Release(m_pFSM);
	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.AddTransition("Idle to BrainFieldOpen_c01", "BrainFieldOpen_c01")
				.Predicator([this]
				{
					return m_bActive;
				})

		.AddState("BrainFieldOpen_c01")
			.OnStart([this]
			{
				m_CableModels[HEAD_1]->SetPlayAnimation("AS_BrainFieldOpen_c01_co0100_1");
				m_CableModels[HEAD_2]->SetPlayAnimation("AS_BrainFieldOpen_c01_co0100_2");
				m_CableModels[HEAD_3]->SetPlayAnimation("AS_BrainFieldOpen_c01_co0100_3");

				const _matrix TargetWorldMatrix = m_pTargetTransform->Get_WorldMatrix();
				for (int i = 0; i < 3; ++i)
				{
					m_CableOpenPivot[i] = m_InitialOpenFieldC1_Pivot[i] * TargetWorldMatrix;
				}
			})
			.Tick([this](_double TimeDelta)
			{
				for (int i = 0; i < 3; ++i)
				{
					m_CableModels[i]->Update_Tick(TimeDelta, m_CableOpenPivot[i]);

					const _matrix WorldMatrix = m_CableOpenPivot[i];
					const _float4 vLocalMove = m_CableModels[i]->GetLocalMove(WorldMatrix);
					m_CableOpenPivot[i].m[3][0] += vLocalMove.x;
					m_CableOpenPivot[i].m[3][1] += vLocalMove.y;
					m_CableOpenPivot[i].m[3][2] += vLocalMove.z;

					CGameInstance::GetInstance()->LambdaRenderRequest(m_CableOpenPivot[i], [this, i]
					{
						m_CableModels[i]->Render_PxModel(m_CableOpenPivot[i]);
					}, CRenderer::RENDER_NONALPHABLEND_TOON);
				}
			})
			.AddTransition("BrainFieldOpen_c01 to BrainFieldOpen_c02", "BrainFieldOpen_c02")
				.Predicator([this]
				{
					return m_CableModels[HEAD_1]->GetPlayAnimation()->GetPlayRatio() > 0.99f;
				})



		.AddState("BrainFieldOpen_c02")
			.OnStart([this]
			{
				m_CableModels[HEAD_1]->SetPlayAnimation("AS_BrainFieldOpen_c02_co0100_1");
				m_CableModels[HEAD_2]->SetPlayAnimation("AS_BrainFieldOpen_c02_co0100_2");
				m_CableModels[HEAD_3]->SetPlayAnimation("AS_BrainFieldOpen_c02_co0100_3");
				m_CableModels[BACK_1]->SetPlayAnimation("AS_BrainFieldOpen_c02_co0101_1");
				m_CableModels[BACK_2]->SetPlayAnimation("AS_BrainFieldOpen_c02_co0101_2");
				m_CableModels[BACK_3]->SetPlayAnimation("AS_BrainFieldOpen_c02_co0101_3");
				m_CableModels[BACK_4]->SetPlayAnimation("AS_BrainFieldOpen_c02_co0101_4");
				m_CableModels[BACK_5]->SetPlayAnimation("AS_BrainFieldOpen_c02_co0101_5");

				const _matrix TargetWorldMatrix = m_pTargetTransform->Get_WorldMatrix();
				for (int i = 0; i < CABLE_END; ++i)
				{
					m_CableOpenPivot[i] = m_InitialOpenFieldC2_Pivot[i] * TargetWorldMatrix;
				}
			})
			.Tick([this](_double TimeDelta)
			{
				for (int i = 0; i < CABLE_END; ++i)
				{
					m_CableModels[i]->Update_Tick(TimeDelta, m_CableOpenPivot[i]);

					const _matrix WorldMatrix = m_CableOpenPivot[i];
					const _float4 vLocalMove = m_CableModels[i]->GetLocalMove(WorldMatrix);
					m_CableOpenPivot[i].m[3][0] += vLocalMove.x;
					m_CableOpenPivot[i].m[3][1] += vLocalMove.y;
					m_CableOpenPivot[i].m[3][2] += vLocalMove.z;

					CGameInstance::GetInstance()->LambdaRenderRequest(m_CableOpenPivot[i], [this, i]
					{
						m_CableModels[i]->Render_PxModel(m_CableOpenPivot[i]);
					}, CRenderer::RENDER_NONALPHABLEND_TOON);
				}
			})
			.AddTransition("BrainFieldOpen_c02 to OnPhysX", "OnPhysX")
				.Predicator([this]
				{
					return m_CableModels[HEAD_1]->GetPlayAnimation()->GetPlayRatio() > 0.99f;
				})



		.AddState("OnPhysX")
			.OnStart([this]
			{
				m_bPhysXOn = true;

				// 등 케이블 1직선으로 만들기
				m_CableModels[BACK_1]->SetPlayAnimation("AS_co0101_001_start01");
				m_CableModels[BACK_2]->SetPlayAnimation("AS_co0101_001_start02");
				m_CableModels[BACK_3]->SetPlayAnimation("AS_co0101_001_start03");
				m_CableModels[BACK_4]->SetPlayAnimation("AS_co0101_001_start04");
				m_CableModels[BACK_5]->SetPlayAnimation("AS_co0101_001_start05");
				for (int i = BACK_1; i < CABLE_END; ++i)
				{
					m_CableModels[i]->GetPlayAnimation()->SetPlayRatio(1.0);
					m_CableModels[i]->Play_Animation(0.01);
					m_CableModels[i]->ActivatePhysX(true);
				}

				for (int i = 0; i < CABLE_END; ++i)
				{
					m_CableModels[i]->ActivatePhysX(true);
				}
			})
			.OnExit([this]
			{
				m_bPhysXOn = false;
				for (int i = 0; i < CABLE_END; ++i)
				{
					m_CableModels[i]->ActivatePhysX(false);
				}
			})
			.AddTransition("OnPhysX to Idle", "Idle")
				.Predicator([this]
				{
					return !m_bActive;
				})
		
		.Build();
}

void CBrainFieldCables::Free()
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

CBrainFieldCables* CBrainFieldCables::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBrainFieldCables* pInstance = new CBrainFieldCables(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBrainFieldCables");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CBrainFieldCables::Clone(void* pArg)
{
	CBrainFieldCables* pInstance = new CBrainFieldCables(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBrainFieldCables");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}
