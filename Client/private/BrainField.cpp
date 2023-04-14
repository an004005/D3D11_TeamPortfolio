#include "stdafx.h"
#include "BrainField.h"
#include <Animation.h>
#include <GameInstance.h>
#include "ImguiUtils.h"
#include "PxModel.h"
#include "FSMComponent.h"
#include "CurveManager.h"
#include "CurveFloatMapImpl.h"
#include "EffectSystem.h"
#include "Material.h"
#include "Layer.h"
#include "SkyBox.h"
#include "ScarletMap.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "RedString.h"
#include "VFX_Manager.h"
#include "MapInstance_Object.h"

/**********************
 * CBrainField
 **********************/
CBrainField::CBrainField(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CBrainField::CBrainField(const CBrainField& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBrainField::Initialize_Prototype()
{
	FAILED_CHECK(CGameObject::Initialize_Prototype());

	static _bool bOnce = true;
	if (bOnce)
	{
		FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Prototype_GameObject_BrainFieldCable", CBrainFieldCables::Create(m_pDevice, m_pContext)));
		FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Prototype_PostVFX_ChromaticAberration", CPostVFX_ChromaticAberration::Create(m_pDevice, m_pContext)));
		FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Prototype_PostVFX_MapPostProcess", CPostVFX_MapPostProcess::Create(m_pDevice, m_pContext)));
		bOnce = false;
	}

	return S_OK;
}


HRESULT CBrainField::Initialize(void* pArg)
{
	FAILED_CHECK(CGameObject::Initialize(pArg));

	m_pBrainFieldCables = dynamic_cast<CBrainFieldCables*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayerNoBegin(LEVEL_STATIC, L"Prototype_GameObject_BrainFieldCable"));
	m_pChromaticAberration = dynamic_cast<CPostVFX_ChromaticAberration*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayerNoBegin(LEVEL_STATIC, L"Prototype_PostVFX_ChromaticAberration"));
	m_pMapPostProcess = dynamic_cast<CPostVFX_MapPostProcess*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayerNoBegin(LEVEL_STATIC, L"Prototype_PostVFX_MapPostProcess"));
	
	m_pCableNoise_c1 = CCurveManager::GetInstance()->GetCurve("BrainField_c1_CableNoise");
	m_pCableYellow_c1 = CCurveManager::GetInstance()->GetCurve("BrainField_c1_CableYellow");
	m_pChromaticAberration_c1 = CCurveManager::GetInstance()->GetCurve("BrainField_c1_ChromaticAberration");
	m_pMaskEmissive_c1 = CCurveManager::GetInstance()->GetCurve("BrainField_c1_MaskEmissive");
	m_pMaskNoise_c1 = CCurveManager::GetInstance()->GetCurve("BrainField_c1_MaskNoise");
	m_pMapDisappear_c1 = CCurveManager::GetInstance()->GetCurve("BrainField_c1_MapDisappear");
	m_pMapBlink_c1 = CCurveManager::GetInstance()->GetCurve("BrainField_c1_BrainFieldBuildBlink");

	m_pMaskEmissive_c2 = CCurveManager::GetInstance()->GetCurve("BrainField_c2_MaskEmissive");
	m_pMapAppear_c2 = CCurveManager::GetInstance()->GetCurve("BrainField_c2_MapAppear");

	{
		CMapInstance_Object::s_bPhysX = false;
		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Map/Map_BrainField.json");
		m_pBrainFieldMap = dynamic_cast<CScarletMap*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_MAP, TEXT("Prototype_GameObject_ScarletMap"), &json));
		m_pBrainFieldMap->SetVisible_MapObjects(false);
		CMapInstance_Object::s_bPhysX = true;
	}
	{
		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Restrings/BranFieldStrings/FloorCombined.json");
		m_pBrainFieldRedString = dynamic_cast<CCombinedRedString*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LAYER_MAP_DECO, L"Prototype_CombinedRedString", &json));
		m_pBrainFieldRedString->SetVisible(false);
		m_pBrainFieldRedString->SetPass(1);
	}

	if (LEVEL_NOW == LEVEL_HOSPITAL_1F)
	{
		m_pBrainFieldRedString->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, _float4{74.4f, 0.f, 13.06f, 1.f});
		m_pBrainFieldMap->SetPos_MapObjects(_float4{74.4f, 0.f, 13.06f, 1.f});
	}

	m_CloseTimeline.SetCurve("SimpleIncrease_2xSlow");
	m_CloseTimeline.SetFinishFunction([this]
	{
		m_pMapPostProcess->SetVisible(false);
	});

	return S_OK;
}

void CBrainField::BeginTick()
{
	CGameObject::BeginTick();
	m_pBrainFieldCables->BeginTick();
	m_pChromaticAberration->BeginTick();
	m_pMapPostProcess->BeginTick();


	for (auto pObj : CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, PLAYERTEST_LAYER_MAP)->GetGameObjects())
	{
		if (auto pSkyBox = dynamic_cast<CSkyBox*>(pObj))
			m_pSkyBox = pSkyBox;
		else if (auto pScarletMap = dynamic_cast<CScarletMap*>(pObj))
		{
			if (pScarletMap != m_pBrainFieldMap)
				m_pDefaultMap = pScarletMap;
		}
	}


}

void CBrainField::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);
	m_pBrainFieldCables->Tick(TimeDelta);
	m_pChromaticAberration->Tick(TimeDelta);
	m_pMapPostProcess->Tick(TimeDelta);

	if (m_bOpen)
	{
		if (strcmp(m_pBrainFieldCables->GetStateName(), "BrainFieldOpen_c01") == 0)
		{
			m_pMapPostProcess->GetParam().Ints[0] = 0;
			_float fPlayRatio = m_pBrainFieldCables->GetPlayRatio();

			m_pBrainFieldCables->SetCableNoise(m_pCableNoise_c1->GetValue(fPlayRatio));
			m_pBrainFieldCables->SetCableYellow(m_pCableYellow_c1->GetValue(fPlayRatio));

			m_pMask->GetParam().Floats[4] = m_pMaskEmissive_c1->GetValue(fPlayRatio);
			m_pMask->GetParam().Ints[1] = (_int)m_pMaskNoise_c1->GetValue(fPlayRatio);
			m_pChromaticAberration->GetParam().Floats[0] = m_pChromaticAberration_c1->GetValue(fPlayRatio);

			m_pMapPostProcess->GetParam().Floats[0] = m_pMapDisappear_c1->GetValue(fPlayRatio);
			m_pMapPostProcess->GetParam().Floats[2] = m_pMapBlink_c1->GetValue(fPlayRatio);
			if (m_pMapPostProcess->GetParam().Floats[0] > 0.99 && m_DefaultMapDeActiveOnce.IsNotDo())
			{
				m_pDefaultMap->SetVisible_MapObjects(false);
				m_pBrainFieldMap->SetVisible_MapObjects(true);
			}

		}
		else if (strcmp(m_pBrainFieldCables->GetStateName(), "BrainFieldOpen_c02") == 0)
		{
			if (m_MapPostProcessFloat0Reset.IsNotDo())
			{
				m_pMapPostProcess->GetParam().Floats[0] = 0.f;
			}

			m_pMapPostProcess->GetParam().Ints[0] = 1;
			m_pBrainFieldRedString->SetVisible(true);

			m_pSkyBox->GetParams().iPass = 1;

			_float fPlayRatio = m_pBrainFieldCables->GetPlayRatio();
			m_pMask->GetParam().Floats[4] = m_pMaskEmissive_c2->GetValue(fPlayRatio);
			m_pMapPostProcess->GetParam().Floats[0] = m_pMapAppear_c2->GetValue(fPlayRatio);
		}
		else if (strcmp(m_pBrainFieldCables->GetStateName(), "OnPhysX") == 0 && m_FloorEffectOnce.IsNotDo())
		{
			m_pBrainFieldFloor = CVFX_Manager::GetInstance()->GetEffect(EF_UI, L"BrainFieldFloorGroup");
			m_pBrainFieldFloor->Start_AttachOnlyPos(_float4(0.f, -0.15f, 0.f, 1.f), false);
			Safe_AddRef(m_pBrainFieldFloor);
			m_pBrainFieldFloor->GetFirstEffect()->SetRenderGroup(CRenderer::RENDER_MESH_ALPHABLEND);
		}

		if (m_pBrainFieldFloor)
		{
			const _vector vPos = m_pTargetTransform->Get_State(CTransform::STATE_TRANSLATION);
			m_pBrainFieldFloor->GetFirstEffect()->GetParams().Float4s[0] = vPos;
			m_pBrainFieldRedString->SetPos(vPos);
		}
	}
	else
	{
		m_CloseTimeline.Tick(TimeDelta, m_pMapPostProcess->GetParam().Floats[0]);
	}
}

void CBrainField::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);
	m_pBrainFieldCables->Late_Tick(TimeDelta);
	m_pChromaticAberration->Late_Tick(TimeDelta);
	m_pMapPostProcess->Late_Tick(TimeDelta);
}

HRESULT CBrainField::Render()
{
	return S_OK;
}

void CBrainField::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();
	ImGui::Indent(20.f);
	if (ImGui::CollapsingHeader("BP Cables"))
	{
		m_pBrainFieldCables->Imgui_RenderProperty();
		m_pBrainFieldCables->Imgui_RenderComponentProperties();
	}
	if (ImGui::CollapsingHeader("BP ChromaticAberration"))
	{
		m_pChromaticAberration->Imgui_RenderProperty();
		m_pChromaticAberration->Imgui_RenderComponentProperties();
	}
	if (ImGui::CollapsingHeader("BP MapPost"))
	{
		m_pMapPostProcess->Imgui_RenderProperty();
		m_pMapPostProcess->Imgui_RenderComponentProperties();
	}
	ImGui::Unindent(20.f);
}

void CBrainField::SetTargetInfo(CGameObject* pOwner, CTransform* pTargetTransform, CModel* pTargetModel)
{
	m_pTarget = pOwner;
	m_pBrainFieldCables->SetTargetInfo(pTargetTransform, pTargetModel);
	m_pTargetModel = pTargetModel;
	m_pTargetTransform = pTargetTransform;
	m_pMask = m_pTargetModel->FindMaterial(L"MI_ch0100_MASK_0");
}

void CBrainField::OpenBrainField()
{
	m_MapPostProcessFloat0Reset.Reset();
	m_FloorEffectOnce.Reset();
	m_DefaultMapDeActiveOnce.Reset();
	m_pBrainFieldCables->Activate(true);
	m_bOpen = true;
	m_pSkyBox->GetParams().iPass = 2;
	m_pMapPostProcess->SetVisible(true);
}

void CBrainField::CloseBrainField()
{
	m_pBrainFieldCables->Activate(false);
	m_bOpen = false;

	// 임시
	m_CloseTimeline.PlayFromStart();

	m_pSkyBox->GetParams().iPass = 0;
	m_pDefaultMap->SetVisible_MapObjects(true);
	m_pBrainFieldMap->SetVisible_MapObjects(false);
	m_pBrainFieldRedString->SetVisible(false);

	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pBrainFieldFloor))
	{
		m_pBrainFieldFloor->SetDelete();
		Safe_Release(m_pBrainFieldFloor);
		m_pBrainFieldFloor = nullptr;
	}
}

void CBrainField::Free()
{
	CGameObject::Free();
	Safe_Release(m_pBrainFieldCables);
	Safe_Release(m_pChromaticAberration);
	Safe_Release(m_pMapPostProcess);
}

CBrainField* CBrainField::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBrainField* pInstance = new CBrainField(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBrainField");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CBrainField::Clone(void* pArg)
{
	CBrainField* pInstance = new CBrainField(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBrainField");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}


/***********************************
 *CPostVFX_ChromaticAberration
 ***********************************/
CPostVFX_ChromaticAberration::CPostVFX_ChromaticAberration(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPostProcess(pDevice, pContext)
{
}

CPostVFX_ChromaticAberration::CPostVFX_ChromaticAberration(const CPostVFX_ChromaticAberration& rhs)
	: CPostProcess(rhs)
{
}

HRESULT CPostVFX_ChromaticAberration::Initialize(void* pArg)
{
	FAILED_CHECK(CPostProcess::Initialize(pArg));

	m_iPriority = 1;
	m_tParam.iPass = 13;
	if (m_tParam.Floats.empty())
		m_tParam.Floats.push_back(0.f);

	m_tParam.Floats[0] = 0.f;
	m_tParam.Float4s.push_back(_float4::Zero);

	return S_OK;
}

void CPostVFX_ChromaticAberration::Tick(_double TimeDelta)
{
	CPostProcess::Tick(TimeDelta);
	if (m_tParam.Floats[0] <= 0.f)
		m_bVisible = false;
	else
		m_bVisible = true;
}

CPostVFX_ChromaticAberration* CPostVFX_ChromaticAberration::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPostVFX_ChromaticAberration* pInstance = new CPostVFX_ChromaticAberration(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPostVFX_ChromaticAberration");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CPostVFX_ChromaticAberration::Clone(void* pArg)
{
	CPostVFX_ChromaticAberration* pInstance = new CPostVFX_ChromaticAberration(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPostVFX_ChromaticAberration");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

/***********************************
 *CPostVFX_MapPostProcess
 ***********************************/
CPostVFX_MapPostProcess::CPostVFX_MapPostProcess(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPostProcess(pDevice, pContext)
{
}

CPostVFX_MapPostProcess::CPostVFX_MapPostProcess(const CPostVFX_MapPostProcess& rhs)
	: CPostProcess(rhs)
{
}

HRESULT CPostVFX_MapPostProcess::Initialize(void* pArg)
{
	FAILED_CHECK(CPostProcess::Initialize(pArg));
	m_iPriority = 0;
	m_tParam.iPass = 14;
	m_tParam.Floats.push_back(0.f);
	m_tParam.Floats.push_back(0.f);
	m_tParam.Floats.push_back(0.f);
	m_tParam.Ints.push_back(0);

	m_tParam.Floats[1] = 30.f;

	m_bVisible = false;
	return S_OK;
}

void CPostVFX_MapPostProcess::Tick(_double TimeDelta)
{
	CPostProcess::Tick(TimeDelta);
	// if (m_tParam.Floats[0] <= 0.f)
	// 	m_bVisible = false;
	// else
	// 	m_bVisible = true;
}

CPostVFX_MapPostProcess* CPostVFX_MapPostProcess::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPostVFX_MapPostProcess* pInstance = new CPostVFX_MapPostProcess(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPostVFX_MapPostProcess");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CPostVFX_MapPostProcess::Clone(void* pArg)
{
	CPostVFX_MapPostProcess* pInstance = new CPostVFX_MapPostProcess(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPostVFX_MapPostProcess");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

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

void CBrainFieldCables::SetTargetInfo(CTransform* pTargetTransform, CModel* pTargetModel)
{
	Safe_Release(m_pTargetModel);
	Safe_Release(m_pTargetTransform);
	m_pTargetModel = pTargetModel;
	m_pTargetTransform = pTargetTransform;
	Safe_AddRef(m_pTargetModel);
	Safe_AddRef(m_pTargetTransform);
}

_float CBrainFieldCables::GetPlayRatio()
{
	return m_CableModels[HEAD_1]->GetPlayAnimation()->GetPlayRatio();
}

const char* CBrainFieldCables::GetStateName()
{
	return m_pFSM->GetCurStateName();
}

void CBrainFieldCables::SetCableNoise(_float fValue)
{
	for (int i = 0; i < CABLE_END; ++i)
	{
		for (auto pMtrl : m_CableModels[i]->GetMaterials())
		{
			pMtrl->GetParam().Floats[3] = fValue;
		}
	}
}

void CBrainFieldCables::SetCableYellow(_float fValue)
{
	for (int i = 0; i < CABLE_END; ++i)
	{
		for (auto pMtrl : m_CableModels[i]->GetMaterials())
		{
			pMtrl->GetParam().Ints[0] = (_int)fValue;
		}
	}
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
