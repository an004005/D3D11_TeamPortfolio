#include "stdafx.h"
#include "EM8200_BrainField.h"
#include "PxModel.h"
#include <GameInstance.h>
#include <Animation.h>
#include <CurveManager.h>

#include "ImguiUtils.h"
#include "Material.h"
#include "JsonStorage.h"
#include "FSMComponent.h"
#include "Layer.h"
#include "SkyBox.h"
#include "ScarletMap.h"
#include "MapInstance_Object.h"
#include "RedString.h"
#include "CurveFloatMapImpl.h"
#include "Player.h"
#include "PostVFX_WhiteOut.h"


CEM8200_BrainField::CEM8200_BrainField(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEM8200_BrainField::CEM8200_BrainField(const CEM8200_BrainField& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEM8200_BrainField::Initialize_Prototype()
{
	FAILED_CHECK(CGameObject::Initialize_Prototype());

	static _bool bOnce = true;
	if (bOnce)
	{
		FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, TEXT("Monster_em8200_BrainFieldCables"), CEM8200_BrainFieldCables::Create(m_pDevice, m_pContext)));
		FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, TEXT("Monster_em8200_MapPostProcess"), CPostVFX_EM8200MapPostProcess::Create(m_pDevice, m_pContext)));
		bOnce = false;
	}

	return S_OK;
}

HRESULT CEM8200_BrainField::Initialize(void* pArg)
{
	FAILED_CHECK(CGameObject::Initialize(pArg));

	m_pCables = dynamic_cast<CEM8200_BrainFieldCables*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayerNoBegin(LEVEL_NOW, L"Monster_em8200_BrainFieldCables"));
	m_pPostProcess = dynamic_cast<CPostVFX_EM8200MapPostProcess*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayerNoBegin(LEVEL_NOW, L"Monster_em8200_MapPostProcess"));
	m_pWhiteOut = dynamic_cast<CPostVFX_WhiteOut*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayerNoBegin(LEVEL_NOW, L"ProtoPostVFX_WhiteOut"));

	m_pCableNoise_Open = CCurveManager::GetInstance()->GetCurve("EM8200_BrainFieldOpen_CableNoise");
	m_pCableYellow_Open = CCurveManager::GetInstance()->GetCurve("EM8200_BrainFieldOpen_CableYellow");
	m_pBlackWhite_Open = CCurveManager::GetInstance()->GetCurve("EM8200_BrainFieldOpen_BlackWhite");

	m_pBlackWhite_Trans = CCurveManager::GetInstance()->GetCurve("EM8200_BrainFieldTrans_BlackWhite");
	m_pWhiteOut_Trans = CCurveManager::GetInstance()->GetCurve("EM8200_BrainFieldTrans_WhiteOut");

	{
		CMapInstance_Object::s_bPhysX = false;
		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Map/Map_BrainField.json");
		m_pBrainFieldMap = dynamic_cast<CScarletMap*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_MAP, TEXT("Prototype_GameObject_ScarletMap"), &json));
		m_pBrainFieldMap->SetVisible_MapObjects(false);
		CMapInstance_Object::s_bPhysX = true;

		for (auto pMapObj : m_pBrainFieldMap->GetMapObjects())
		{
			if (auto pInstanceMapObj = dynamic_cast<CMapInstance_Object*>(pMapObj))
			{
				for (auto pMtrl : pInstanceMapObj->Get_Model_Instancing()->GetMaterials())
				{
					if (pMtrl->GetParam().iPass == 6 || pMtrl->GetParam().iPass == 9)
					{
						pMtrl->GetParam().Float4s[0] = _float4(0.f, 0.f, 1.f, 0.f);
					}
				}
			}
		}
	}
	{
		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Restrings/BranFieldStrings/FloorCombined.json");
		m_pBrainFieldRedString = dynamic_cast<CCombinedRedString*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LAYER_MAP_DECO, L"Prototype_CombinedRedString", &json));
		m_pBrainFieldRedString->SetVisible(false);
		m_pBrainFieldRedString->SetPass(2); // blue pass
		
	}

	m_CloseTimeline.SetCurve("SimpleIncrease_2xSlow");
	m_CloseTimeline.SetFinishFunction([this]
	{
		m_pPostProcess->SetVisible(false);
	});

	return S_OK;
}

void CEM8200_BrainField::BeginTick()
{
	CGameObject::BeginTick();
	m_pCables->BeginTick();
	m_pPostProcess->BeginTick();
	m_pWhiteOut->BeginTick();

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

void CEM8200_BrainField::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);
	m_pCables->Tick(TimeDelta);
	m_pPostProcess->Tick(TimeDelta);
	m_pWhiteOut->Tick(TimeDelta);

	if (m_bOpen)
	{
		if (strcmp(m_pCables->GetStateName(), "BrainFieldOpen") == 0)
		{
			_float fPlayRatio = m_pCables->GetPlayRatio();

			m_pCables->SetCableNoise(m_pCableNoise_Open->GetValue(fPlayRatio));
			m_pCables->SetCableYellow(m_pCableYellow_Open->GetValue(fPlayRatio));

			m_pPostProcess->GetParam().Floats[0] = m_pBlackWhite_Open->GetValue(fPlayRatio);
		}
		else if (strcmp(m_pCables->GetStateName(), "BrainFieldTrans") == 0)
		{
			if (m_VisibleBrainFieldOnce.IsNotDo())
			{
				m_pDefaultMap->SetVisible_MapObjects(false);
				m_pBrainFieldMap->SetVisible_MapObjects(true);
				m_pSkyBox->GetParams().iPass = 1;
				m_pBrainFieldRedString->SetVisible(true);
			}


			_float fPlayRatio = m_pCables->GetPlayRatio();

			m_pPostProcess->GetParam().Floats[0] = m_pBlackWhite_Trans->GetValue(fPlayRatio);
			m_pWhiteOut->GetParam().Floats[0] = m_pWhiteOut_Trans->GetValue(fPlayRatio);
		}
		else
		{
			if (m_pWhiteOut->GetParam().Floats[0] > 0.f)
			{
				m_pWhiteOut->GetParam().Floats[0] -= TimeDelta * 3.f;
			}
			else
			{
				m_pWhiteOut->GetParam().Floats[0] = 0.f;
			}
		}
	}
	else
	{
		m_CloseTimeline.Tick(TimeDelta, m_pPostProcess->GetParam().Floats[0]);
	}

	if (bBlackInOut)
	{
		_float fRatio = m_pWhiteOut->GetParam().Floats[0];
		if (fRatio > 0.95f && blackOnce.IsNotDo())
		{
			m_pDefaultMap->SetVisible_MapObjects(false);
			m_pBrainFieldMap->SetVisible_MapObjects(true);
			m_pSkyBox->GetParams().iPass = 1;
			m_pBrainFieldRedString->SetVisible(false);

			for (auto pMapObj : m_pBrainFieldMap->GetMapObjects())
			{
				if (auto pInstanceMapObj = dynamic_cast<CMapInstance_Object*>(pMapObj))
				{
					for (auto pMtrl : pInstanceMapObj->Get_Model_Instancing()->GetMaterials())
					{
						if (pMtrl->GetParam().iPass == 6 || pMtrl->GetParam().iPass == 9)
						{
							pInstanceMapObj->SetVisible(false);
						}
					}
				}
			}

			for (auto pObj : CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, LAYER_MAP_DECO)->GetGameObjects())
			{
				pObj->SetVisible(false);
			}
		}
	}
}

void CEM8200_BrainField::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);
	m_pCables->Late_Tick(TimeDelta);
	m_pPostProcess->Late_Tick(TimeDelta);
	m_pWhiteOut->Late_Tick(TimeDelta);
}

HRESULT CEM8200_BrainField::Render()
{
	return CGameObject::Render();
}

void CEM8200_BrainField::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();
	m_pCables->Imgui_RenderProperty();
	m_pCables->Imgui_RenderComponentProperties();
}

void CEM8200_BrainField::SetTargetInfo(CGameObject* pTarget, CTransform* pTargetTransform, CModel* pTargetModel)
{
	m_pCables->SetTargetInfo(pTargetTransform, pTargetModel);
}

void CEM8200_BrainField::OpenBrainField()
{
	m_bOpen = true;
	m_pCables->Activate(true);
	// m_pSkyBox->GetParams().iPass = 2;
	m_pPostProcess->SetVisible(true);
	m_pPostProcess->GetParam().Ints[0] = 0;
	m_pPostProcess->GetParam().Floats[0] = 0.f;
	m_VisibleBrainFieldOnce.Reset();
}

void CEM8200_BrainField::CloseBrainField()
{
	m_bOpen = false;
	m_pCables->Activate(false);
	m_CloseTimeline.PlayFromStart();

	m_pPostProcess->GetParam().Ints[0] = 1;


	m_pSkyBox->GetParams().iPass = 0;
	m_pDefaultMap->SetVisible_MapObjects(true);
	m_pBrainFieldMap->SetVisible_MapObjects(false);
	m_pBrainFieldRedString->SetVisible(false);
}

void CEM8200_BrainField::SetCableTP(_float fValue)
{
	m_pCables->SetCableTP(fValue);
}

void CEM8200_BrainField::BlackInOut()
{
	m_pWhiteOut->BlackInOut();
	bBlackInOut = true;
}

void CEM8200_BrainField::Free()
{
	CGameObject::Free();
	Safe_Release(m_pCables);
	Safe_Release(m_pPostProcess);
	Safe_Release(m_pWhiteOut);
}

CEM8200_BrainField* CEM8200_BrainField::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEM8200_BrainField* pInstance = new CEM8200_BrainField(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM8200_BrainField");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CEM8200_BrainField::Clone(void* pArg)
{
	CEM8200_BrainField* pInstance = new CEM8200_BrainField(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM8200_BrainField");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

/***********************************
 *CPostVFX_EM8200MapPostProcess
 ***********************************/
CPostVFX_EM8200MapPostProcess::CPostVFX_EM8200MapPostProcess(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPostProcess(pDevice, pContext)
{
}

CPostVFX_EM8200MapPostProcess::CPostVFX_EM8200MapPostProcess(const CPostVFX_EM8200MapPostProcess& rhs)
	: CPostProcess(rhs)
{
}

HRESULT CPostVFX_EM8200MapPostProcess::Initialize(void* pArg)
{
	FAILED_CHECK(CPostProcess::Initialize(pArg));
	m_iPriority = 0;
	m_tParam.iPass = 15;
	m_tParam.Floats.push_back(0.f);
	m_tParam.Floats.push_back(0.f);
	m_tParam.Ints.push_back(0);

	m_tParam.Floats[1] = 30.f;

	m_bVisible = false;
	return S_OK;
}

void CPostVFX_EM8200MapPostProcess::Tick(_double TimeDelta)
{
	CPostProcess::Tick(TimeDelta);
}

CPostVFX_EM8200MapPostProcess* CPostVFX_EM8200MapPostProcess::Create(ID3D11Device* pDevice,
	ID3D11DeviceContext* pContext)
{
	CPostVFX_EM8200MapPostProcess* pInstance = new CPostVFX_EM8200MapPostProcess(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPostVFX_EM8200MapPostProcess");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CPostVFX_EM8200MapPostProcess::Clone(void* pArg)
{
	CPostVFX_EM8200MapPostProcess* pInstance = new CPostVFX_EM8200MapPostProcess(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPostVFX_EM8200MapPostProcess");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CEM8200_BrainFieldCables::CEM8200_BrainFieldCables(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEM8200_BrainFieldCables::CEM8200_BrainFieldCables(const CEM8200_BrainFieldCables& rhs)
	: CGameObject(rhs)
	, m_CablePivots(rhs.m_CablePivots)
	, m_CableBones(rhs.m_CableBones)
	, m_InitialOpenFieldPivot(rhs.m_InitialOpenFieldPivot)
	, m_InitialTransPivot(rhs.m_InitialTransPivot)
{
}

HRESULT CEM8200_BrainFieldCables::Initialize_Prototype()
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
		auto pPrototype = CGameInstance::GetInstance()->Find_Prototype_Component(LEVEL_STATIC, L"Prototype_PxModel_co0102");
		if (pPrototype == nullptr)
		{
			auto model = CPxModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Cables/co0102.anim_model");
			model->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Cables/Animation_co0102/");
			CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Prototype_PxModel_co0102", model);
		}
	}

	m_CableBones[HEAD_1] = "Neck";
	m_CableBones[HEAD_2] = "Neck";
	m_CableBones[HEAD_3] = "Neck";
	m_CableBones[BACK_1] = "Neck";
	m_CableBones[BACK_2] = "Neck";
	m_CableBones[BACK_3] = "Spine1";
	m_CableBones[BACK_4] = "Spine1";
	m_CableBones[BACK_5] = "Spine1";

	m_CablePivots[HEAD_1] = CImguiUtils::CreateMatrixFromImGuizmoData({0.120f, 0.328f, 0.343f}, {67.764f, 7.363f, 19.755f }, _float3::One);
	m_CablePivots[HEAD_2] = CImguiUtils::CreateMatrixFromImGuizmoData({0.066f,  0.216f, -0.083f}, {-0.585f, -2.784f, 41.803f}, _float3::One);
	m_CablePivots[HEAD_3] = CImguiUtils::CreateMatrixFromImGuizmoData({0.317f, 0.248f, 0.150f}, {65.300f, 8.188f, 5.247f }, _float3::One);

	m_InitialOpenFieldPivot[HEAD_1] = CImguiUtils::CreateMatrixFromImGuizmoData({0.f, 2.040f, -0.175f}, _float3::Zero, _float3::One);
	m_InitialOpenFieldPivot[HEAD_2] = CImguiUtils::CreateMatrixFromImGuizmoData({0.055f, 1.965f, -0.101f}, _float3::Zero, _float3::One);
	m_InitialOpenFieldPivot[HEAD_3] = CImguiUtils::CreateMatrixFromImGuizmoData({0.055f, 1.965f, -0.101f}, _float3::Zero, _float3::One);

	m_InitialTransPivot[HEAD_1] = CImguiUtils::CreateMatrixFromImGuizmoData({0.163683f, 0.173333f, -1.27111f}, _float3::Zero, _float3::One);
	m_InitialTransPivot[HEAD_2] = CImguiUtils::CreateMatrixFromImGuizmoData({0.163683f, 0.173333f, -1.27111f}, _float3::Zero, _float3::One);
	m_InitialTransPivot[HEAD_3] = CImguiUtils::CreateMatrixFromImGuizmoData({0.163683f, 0.173333f, -1.27111f}, _float3::Zero, _float3::One);

	return S_OK;
}

HRESULT CEM8200_BrainFieldCables::Initialize(void* pArg)
{
	FAILED_CHECK(CGameObject::Initialize(pArg));
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));
	
	for (int i = HEAD_1; i < BACK_1; ++i)
	{
		CPxModel* pModel = nullptr;
		wstring wstrModelTag = L"Cable" + to_wstring(i + 1);
		FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_PxModel_co0102", wstrModelTag.c_str(), (CComponent**)&pModel));
		m_CableModels[i] = pModel;
	}
	for (int i = BACK_1; i < CABLE_END; ++i)
	{
		CPxModel* pModel = nullptr;
		wstring wstrModelTag = L"Cable" + to_wstring(i + 1);
		FAILED_CHECK(Add_Component(LEVEL_NOW, L"Prototype_PxModel_co0101", wstrModelTag.c_str(), (CComponent**)&pModel));
		m_CableModels[i] = pModel;
	}

	SetUpFSM();

	return S_OK;
}

void CEM8200_BrainFieldCables::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	m_pFSM->Tick(TimeDelta);

	if (m_bActive && m_bPhysXOn)
	{
		const _matrix TargetWorldMatrix = m_pTargetTransform->Get_WorldMatrix();
		for (int i = 0; i < CABLE_END; ++i)
		{
			_matrix BoneMatrix = m_pTargetModel->GetBoneMatrix(m_CableBones[i]);
			if (i >= BACK_1)
			{
				BoneMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(BoneMatrix.r[0]));
				BoneMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(BoneMatrix.r[1]));
				BoneMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(BoneMatrix.r[2]));
			}

			const _matrix SocketMatrix = XMLoadFloat4x4(&m_CablePivots[i]) * BoneMatrix * TargetWorldMatrix;
			m_CableModels[i]->Update_Tick(TimeDelta, SocketMatrix);
		}
	}
}

void CEM8200_BrainFieldCables::Late_Tick(_double TimeDelta)
{
	if (m_bActive && m_bPhysXOn)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND_TOON, this);
}

HRESULT CEM8200_BrainFieldCables::Render()
{
	const _matrix TargetWorldMatrix = m_pTargetTransform->Get_WorldMatrix();
	for (int i = 0; i < CABLE_END; ++i)
	{
		_matrix BoneMatrix = m_pTargetModel->GetBoneMatrix(m_CableBones[i]);
		if (i >= BACK_1)
		{
			BoneMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(BoneMatrix.r[0]));
			BoneMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(BoneMatrix.r[1]));
			BoneMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(BoneMatrix.r[2]));
		}

		const _matrix SocketMatrix = XMLoadFloat4x4(&m_CablePivots[i]) * BoneMatrix * TargetWorldMatrix;
		m_CableModels[i]->Render_PxModel(SocketMatrix);
	}
	return S_OK;
}

void CEM8200_BrainFieldCables::Imgui_RenderProperty()
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

		_matrix tmp = XMLoadFloat4x4(&m_InitialTransPivot[iIdx]) * m_pTargetTransform->Get_WorldMatrix();
		_float4x4 tmp2 = tmp;
		 CImguiUtils::Render_Guizmo(&tmp2, tinfo, false, true);
		 m_InitialTransPivot[iIdx] = tmp2 * m_pTargetTransform->Get_WorldMatrix_Inverse();

		CImguiUtils::Render_Guizmo(&m_InitialTransPivot[iIdx], tinfo, true, false);
	}

	if (ImGui::Button("ReFSM"))
		SetUpFSM();
}

void CEM8200_BrainFieldCables::Activate(_bool bActive)
{
	m_bActive = bActive;
}

void CEM8200_BrainFieldCables::SetTargetInfo(CTransform* pTargetTransform, CModel* pTargetModel)
{
	Safe_Release(m_pTargetModel);
	Safe_Release(m_pTargetTransform);
	m_pTargetModel = pTargetModel;
	m_pTargetTransform = pTargetTransform;
	Safe_AddRef(m_pTargetModel);
	Safe_AddRef(m_pTargetTransform);
}

_float CEM8200_BrainFieldCables::GetPlayRatio()
{
	return m_CableModels[HEAD_1]->GetPlayAnimation()->GetPlayRatio();
}

const char* CEM8200_BrainFieldCables::GetStateName()
{
	return m_pFSM->GetCurStateName();
}

void CEM8200_BrainFieldCables::SetCableNoise(_float fValue)
{
	for (int i = 0; i < CABLE_END; ++i)
	{
		for (auto pMtrl : m_CableModels[i]->GetMaterials())
		{
			pMtrl->GetParam().Floats[3] = fValue;
		}
	}
}

void CEM8200_BrainFieldCables::SetCableYellow(_float fValue)
{
	for (int i = 0; i < CABLE_END; ++i)
	{
		for (auto pMtrl : m_CableModels[i]->GetMaterials())
		{
			pMtrl->GetParam().Ints[0] = (_int)fValue;
		}
	}
}


void CEM8200_BrainFieldCables::SetCableTP(_float fValue)
{
	for (int i = 0; i < CABLE_END; ++i)
	{
		for (auto pMtrl : m_CableModels[i]->GetMaterials())
		{
			pMtrl->GetParam().Floats[2] = fValue;
		}
	}
}

void CEM8200_BrainFieldCables::SetUpFSM()
{
	Safe_Release(m_pFSM);
	m_pFSM = CFSMComponentBuilder()
		.InitState("Idle")
		.AddState("Idle")
			.AddTransition("Idle to BrainFieldOpen", "BrainFieldOpen")
				.Predicator([this]
				{
					return m_bActive;
				})

		.AddState("BrainFieldOpen")
			.OnStart([this]
			{
				m_CableModels[HEAD_1]->SetPlayAnimation("AS_co0102_001_brain_start01");
				m_CableModels[HEAD_2]->SetPlayAnimation("AS_co0102_001_brain_start02");
				m_CableModels[HEAD_3]->SetPlayAnimation("AS_co0102_001_brain_start03");

				const _matrix TargetWorldMatrix = m_pTargetTransform->Get_WorldMatrix();
				for (int i = 0; i < 3; ++i)
				{
					m_CableOpenPivot[i] = m_InitialOpenFieldPivot[i] * TargetWorldMatrix;
				}
			})
			.Tick([this](_double TimeDelta)
			{
				for (int i = 0; i < 3; ++i)
				{
					m_CableModels[i]->Update_Tick(TimeDelta, m_CableOpenPivot[i]);

					CGameInstance::GetInstance()->LambdaRenderRequest(m_CableOpenPivot[i], [this, i]
					{
						m_CableModels[i]->Render_PxModel(m_CableOpenPivot[i]);
					}, CRenderer::RENDER_NONALPHABLEND_TOON);
				}
			})
			.AddTransition("BrainFieldOpen to BrainFieldTrans", "BrainFieldTrans")
				.Predicator([this]
				{
					return m_CableModels[HEAD_1]->GetPlayAnimation()->GetPlayRatio() > 0.49f; // 절반만 사용 맞음
				})

		.AddState("BrainFieldTrans")
			.OnStart([this]
			{
				m_CableModels[HEAD_1]->SetPlayAnimation("AS_co0102_001_brain_trans01");
				m_CableModels[HEAD_2]->SetPlayAnimation("AS_co0102_001_brain_trans02");
				m_CableModels[HEAD_3]->SetPlayAnimation("AS_co0102_001_brain_trans03");

				const _matrix TargetWorldMatrix = m_pTargetTransform->Get_WorldMatrix();
				for (int i = 0; i < 3; ++i)
				{
					m_CableOpenPivot[i] = m_InitialTransPivot[i] * TargetWorldMatrix;
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
			.AddTransition("BrainFieldTrans to OnPhysX", "OnPhysX")
				.Predicator([this]
				{
					return m_CableModels[HEAD_1]->GetPlayAnimation()->GetPlayRatio() > 0.99f;
				})

		.AddState("OnPhysX")
			.OnStart([this]
			{
				m_bPhysXOn = true;

				// 등 케이블 1직선으로 만들기
				m_CableModels[HEAD_1]->SetPlayAnimation("AS_co0102_001_brain_start01");
				m_CableModels[HEAD_2]->SetPlayAnimation("AS_co0102_001_brain_start02");
				m_CableModels[HEAD_3]->SetPlayAnimation("AS_co0102_001_brain_start03");
				m_CableModels[BACK_1]->SetPlayAnimation("AS_co0101_001_start01");
				m_CableModels[BACK_2]->SetPlayAnimation("AS_co0101_001_start02");
				m_CableModels[BACK_3]->SetPlayAnimation("AS_co0101_001_start03");
				m_CableModels[BACK_4]->SetPlayAnimation("AS_co0101_001_start04");
				m_CableModels[BACK_5]->SetPlayAnimation("AS_co0101_001_start05");
				for (int i = 0; i < CABLE_END; ++i)
				{
					m_CableModels[i]->GetPlayAnimation()->SetPlayRatio(1.0);
					m_CableModels[i]->Play_Animation(0.01);
					m_CableModels[i]->ActivatePhysX(true);
				}
				// for (int i = 0; i < 3; ++i)
				// {
				// 	m_CableModels[i]->ActivatePhysX(true);
				// }

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

void CEM8200_BrainFieldCables::Free()
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

CEM8200_BrainFieldCables* CEM8200_BrainFieldCables::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEM8200_BrainFieldCables* pInstance = new CEM8200_BrainFieldCables(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEM8200_BrainFieldCables");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CEM8200_BrainFieldCables::Clone(void* pArg)
{
	CEM8200_BrainFieldCables* pInstance = new CEM8200_BrainFieldCables(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEM8200_BrainFieldCables");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}
