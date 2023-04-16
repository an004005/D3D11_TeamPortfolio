#include "stdafx.h"
#include "..\public\Level_Loading_Simple.h"
#include <Animation.h>
#include <Camera.h>
#include <JsonStorage.h>
#include "GameInstance.h"
#include "Material.h"
#include "EffectSystem.h"
#include "ImguiUtils.h"
#include "Camera_Dynamic.h"
#include "Material.h"

CLoadingModel::CLoadingModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CLoadingModel::CLoadingModel(const CLoadingModel& rhs)
	: CGameObject(rhs)
{
}

HRESULT CLoadingModel::Initialize_Prototype()
{
	return CGameObject::Initialize_Prototype();
}

HRESULT CLoadingModel::Initialize(void* pArg)
{
	CGameObject::Initialize(pArg);
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

	m_fDuration = 0.f;
	return S_OK;
}

void CLoadingModel::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	if (m_pModel == nullptr) return;

	Assert(m_pModel->GetPlayAnimation() != nullptr);

    if (m_pModel->GetPlayAnimation()->IsFinished())
    {
        m_pModel->GetPlayAnimation()->Reset();
    }

    m_pModel->GetPlayAnimation()->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
    m_pModel->Compute_CombindTransformationMatrix();

    // _vector vLocal = m_pModel->GetLocalMove(m_pTransformCom->Get_WorldMatrix());
    // m_pTransformCom->LocalMove(vLocal);
}

void CLoadingModel::Late_Tick(_double TimeDelta)
{
	if (m_bVisible)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND_TOON, this);
	if (CGameInstance::GetInstance()->IsLoading() == false)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);
}

HRESULT CLoadingModel::Render()
{
	if (m_pModel == nullptr) return S_OK;

	m_pModel->Render(m_pTransformCom);
	return CGameObject::Render();
}

HRESULT CLoadingModel::Render_ShadowDepth()
{
	if (m_pModel == nullptr) return S_OK;

	m_pModel->Render_ShadowDepth(m_pTransformCom);
	return S_OK;
}


void CLoadingModel::SaveToJson(OUT Json& json)
{
	__super::SaveToJson(json);

	json["ModelTag"] = ws2s(m_wstrModelTag);
	json["AnimName"] = m_strAnimName;
}

void CLoadingModel::LoadFromJson(const Json& json)
{
	__super::LoadFromJson(json);

	if (json.contains("ModelTag") && json.contains("AnimName"))
	{
		const wstring wstrModelTag = s2ws(json["ModelTag"]);
		Add_Component(LEVEL_STATIC, wstrModelTag.c_str(), L"Model", (CComponent**)&m_pModel);

		const string strAnimName = json["AnimName"];
		m_pModel->Find_Animation(strAnimName)->SetLooping(true);
		m_pModel->SetPlayAnimation(strAnimName);

		if (auto pMtrl  = m_pModel->FindMaterial(L"Proto_MtrlAnim_Empty"))
		{
			pMtrl->SetActive(false);
		}
	}

}

void CLoadingModel::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	const char* ModelTags[] = {
		"Model_Ch300_Portrait", 
		"Model_Ch400_Portrait", 
		"Model_Ch500_Portrait",
		"Model_Ch600_Portrait",
		"Model_Ch800_Portrait",
		"Model_Ch900_Portrait",
		"Model_Ch1000_Portrait"
	};

	ImGui::Combo("Select_Model", &m_iModelIndex, ModelTags, IM_ARRAYSIZE(ModelTags));

	switch (m_iModelIndex)
	{
	case 0:
		m_strAnimName = "AS_ch0300_001_AL_wait01";
		m_wstrModelTag = L"Model_Ch300_Portrait";
		break;
	case 1:
		m_strAnimName = "AS_ch0400_102_AL_wait02";
		m_wstrModelTag = L"Model_Ch400_Portrait";
		break;
	case 2:
		m_strAnimName = "AS_ch0500_001_AL_wait01";
		m_wstrModelTag = L"Model_Ch500_Portrait";
		break;
	case 3:
		m_strAnimName = "AS_ch0600_101_AL_wait01";
		m_wstrModelTag = L"Model_Ch600_Portrait";
		break;
	case 4:
		m_strAnimName = "AS_ch0800_101_AL_wait01";
		m_wstrModelTag = L"Model_Ch800_Portrait";
		break;
	case 5:
		m_strAnimName = "AS_ch0900_101_AL_wait01";
		m_wstrModelTag = L"Model_Ch900_Portrait";
		break;
	case 6:
		m_strAnimName = "AS_ch1000_101_AL_wait01";
		m_wstrModelTag = L"Model_Ch1000_Portrait";
		break;
	default:
		break;
	}


	if (ImGui::Button("Create_Model"))
	{
		const wstring wstrModelTag = m_wstrModelTag;
		Add_Component(LEVEL_STATIC, wstrModelTag.c_str(), L"Model", (CComponent**)&m_pModel);

		const string strAnimName = m_strAnimName;
		m_pModel->Find_Animation(strAnimName)->SetLooping(true);
		m_pModel->SetPlayAnimation(strAnimName);
	}
}

void CLoadingModel::SetModel(const wstring& strTag)
{
	Add_Component(LEVEL_STATIC, strTag.c_str(), L"Model", (CComponent**)&m_pModel);
}

void CLoadingModel::SetPlayAnim(const string& strAnim)
{
	m_pModel->Find_Animation(strAnim)->SetLooping(true);
	m_pModel->SetPlayAnimation(strAnim);
}

CLoadingModel* CLoadingModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
 	CLoadingModel*		pInstance = new CLoadingModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLoadingModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CLoadingModel::Clone(void* pArg)
{
	CLoadingModel*		pInstance = new CLoadingModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLoadingModel");
		Safe_Release(pInstance); 
	}
	return pInstance;
}

void CLoadingModel::Free()
{
	CGameObject::Free();
	Safe_Release(m_pModel);
	Safe_Release(m_pRendererCom);
}

CLevel_Loading_Simple::CLevel_Loading_Simple(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
                                             std::function<CLevel*()>&& LevelCreator)
	: CLoadingLevel(pDevice, pContext, std::move(LevelCreator))
{
}


HRESULT CLevel_Loading_Simple::Initialize()
{
	CLoadingLevel::Initialize();
	static _bool bOnce = true;
	if (bOnce)
	{
		if (CGameInstance::GetInstance()->Find_Prototype(LEVEL_STATIC, L"ProtoType_LoadingModel") == nullptr)
		{
			CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"ProtoType_LoadingModel", CLoadingModel::Create(m_pDevice, m_pContext));
		}


		auto pModel_ch100 = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Player.anim_model");
		pModel_ch100->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Portrait/");
		FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_Ch100_Loading", pModel_ch100));

		auto pModel_ch300 = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch300/ch300.anim_model");
		pModel_ch300->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch300/SAS_Anim/");
		FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_Ch300_Loading", pModel_ch300));

		auto pModel_ch500 = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch500/ch500.anim_model");
		pModel_ch500->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch500/SAS_Anim/");
		FAILED_CHECK(CGameInstance::GetInstance()->Add_Prototype(LEVEL_STATIC, L"Model_Ch500_Loading", pModel_ch500));
	}
	bOnce = false;

	{
		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Loading/ch0100.json");
		auto pCH0100 = dynamic_cast<CLoadingModel*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_LOADING, L"Layer_Loading", L"ProtoType_LoadingModel", &json));
		pCH0100->SetModel(L"Model_Ch100_Loading");
		pCH0100->SetPlayAnim("AS_ch0100_116_AL_walk");
		pCH0100->GetModel()->FindMaterial(L"MI_ch0100_HOOD_0")->SetActive(false);
	}

	{
		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Loading/ch0300.json");
		auto pCH0300 = dynamic_cast<CLoadingModel*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_LOADING, L"Layer_Loading", L"ProtoType_LoadingModel", &json));
		pCH0300->SetModel(L"Model_Ch300_Loading");
		pCH0300->SetPlayAnim("AS_ch0300_005_AL_walk");
	}

	{
		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Loading/ch0500.json");
		auto pCH0500 = dynamic_cast<CLoadingModel*>(CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_LOADING, L"Layer_Loading", L"ProtoType_LoadingModel", &json));
		pCH0500->SetModel(L"Model_Ch500_Loading");
		pCH0500->SetPlayAnim("AS_ch0500_005_AL_walk");
	}

	Json jsonVFX = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Loading/VFX.json");
	CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_LOADING, L"Layer_Loading", L"ProtoPostVFX_Loading", &jsonVFX);

	Json RectRound = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Loading/RedRect.json");
	pRect = (CEffectSystem*)CGameInstance::GetInstance()->Clone_GameObject_Get(LEVEL_LOADING, L"Layer_Loading", L"ProtoVFX_EffectSystem", &RectRound);
	pRect->GetTransform()->Set_WorldMatrix(CImguiUtils::CreateMatrixFromImGuizmoData({0.968f, -0.088f, 1.142f}, {180.000f, -77.595f, 180.000}, _float3::One));

	return S_OK;
}

void CLevel_Loading_Simple::Tick(_double TimeDelta)
{
	CLoadingLevel::Tick(TimeDelta);
	auto pCam = CGameInstance::GetInstance()->FindCamera("LoadingCam");
	if (pCam == nullptr)
	{
		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Loading/Cam.json");
		pCam = CGameInstance::GetInstance()->Add_Camera("LoadingCam", LEVEL_LOADING, L"Layer_Loading", L"Prototype_GameObject_Camera_Dynamic", &json);
		pCam->SetMainCamera();
		dynamic_cast<CCamera_Dynamic*>(pCam)->SetActive(false);
	}

	pCam->SetMainCamera();

	if (CGameInstance::GetInstance()->Check_ObjectAlive(pRect))
	{
		pRect->GetShader()->Tick(TimeDelta);
	}
}

void CLevel_Loading_Simple::Late_Tick(_double TimeDelta)
{
	SetFinish();
}

HRESULT CLevel_Loading_Simple::Render()
{
	if (IsFinished())
	{
		SetWindowText(g_hWnd, TEXT("Level : Loading Finish"));
	}
	else
	{
		SetWindowText(g_hWnd, TEXT("Level : Loading"));
	}

	return CLoadingLevel::Render();
}

void CLevel_Loading_Simple::Free()
{
	CLoadingLevel::Free();
}


