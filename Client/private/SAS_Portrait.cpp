#include "stdafx.h"
#include "..\public\SAS_Portrait.h"
#include "GameInstance.h"
#include "Material.h"
#include "Animation.h"
#include "Camera.h"
#include "JsonStorage.h"
#include "CurveManager.h"
#include "CurveFloatMapImpl.h"

#define USE_PORTRAIT

/*********************
 *CPostVFX_SAS_Portrait
 *********************/
CPostVFX_SAS_Portrait::CPostVFX_SAS_Portrait(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPostProcess(pDevice, pContext)
{
}

CPostVFX_SAS_Portrait::CPostVFX_SAS_Portrait(const CPostVFX_SAS_Portrait& rhs)
	: CPostProcess(rhs)
{
}

HRESULT CPostVFX_SAS_Portrait::Initialize(void* pArg)
{
	FAILED_CHECK(CPostProcess::Initialize(pArg));
	m_tParam.Floats.push_back(1.f);
	m_tParam.iPass = 9;
	m_iPriority = 1100;
	return S_OK;
}

void CPostVFX_SAS_Portrait::Tick(_double TimeDelta)
{
	CPostProcess::Tick(TimeDelta);
	if (m_tParam.Floats[0] <= 0.f)
		m_bVisible = false;
	else
		m_bVisible = true;
}

CPostVFX_SAS_Portrait* CPostVFX_SAS_Portrait::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPostVFX_SAS_Portrait* pInstance = new CPostVFX_SAS_Portrait(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPostVFX_SAS_Portrait");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPostVFX_SAS_Portrait::Clone(void* pArg)
{
	CPostVFX_SAS_Portrait*		pInstance = new CPostVFX_SAS_Portrait(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPostVFX_SAS_Portrait");
		Safe_Release(pInstance);
	}
	return pInstance;
}

/*********************
 *CSAS_Portrait
 *********************/
CSAS_Portrait::CSAS_Portrait(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CSAS_Portrait::CSAS_Portrait(const CSAS_Portrait& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSAS_Portrait::Initialize_Prototype()
{
	FAILED_CHECK(CGameObject::Initialize_Prototype());

#ifdef USE_PORTRAIT
	auto pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->Find_Prototype(LEVEL_STATIC, L"Model_Ch100_Portrait") == nullptr)
	{
		auto pModel_ch100 = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Player.anim_model");
		pModel_ch100->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Portrait/");
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Model_Ch100_Portrait", pModel_ch100));
	}

	if (pGameInstance->Find_Prototype(LEVEL_STATIC, L"Model_Ch300_Portrait") == nullptr)
	{
		auto pModel_ch300 = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch300/ch300.anim_model");
		pModel_ch300->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch300/SAS_Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Model_Ch300_Portrait", pModel_ch300));
	}

	if (pGameInstance->Find_Prototype(LEVEL_STATIC, L"Model_Ch400_Portrait") == nullptr)
	{
		auto pModel_ch400 = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch400/ch400.anim_model");
		pModel_ch400->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch400/SAS_Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Model_Ch400_Portrait", pModel_ch400));
	}

	if (pGameInstance->Find_Prototype(LEVEL_STATIC, L"Model_Ch500_Portrait") == nullptr)
	{
		auto pModel_ch500 = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch500/ch500.anim_model");
		pModel_ch500->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch500/SAS_Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Model_Ch500_Portrait", pModel_ch500));
	}

	if (pGameInstance->Find_Prototype(LEVEL_STATIC, L"Model_Ch600_Portrait") == nullptr)
	{
		auto pModel_ch600 = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch600/ch600.anim_model");
		pModel_ch600->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch600/SAS_Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Model_Ch600_Portrait", pModel_ch600));
	}

	if (pGameInstance->Find_Prototype(LEVEL_STATIC, L"Model_Ch800_Portrait") == nullptr)
	{
		auto pModel_ch800 = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch800/ch800.anim_model");
		pModel_ch800->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch800/SAS_Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Model_Ch800_Portrait", pModel_ch800));
	}

	if (pGameInstance->Find_Prototype(LEVEL_STATIC, L"Model_Ch900_Portrait") == nullptr)
	{
		auto pModel_ch900 = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch900/ch0900.anim_model");
		pModel_ch900->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch900/SAS_Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Model_Ch900_Portrait", pModel_ch900));
	}

	if (pGameInstance->Find_Prototype(LEVEL_STATIC, L"Model_Ch800_Portrait") == nullptr)
	{
		auto pModel_ch1000 = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch1000/ch1000.anim_model");
		pModel_ch1000->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/ch1000/SAS_Anim/");
		FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Model_Ch1000_Portrait", pModel_ch1000));
	}
#endif

	return S_OK;
}

HRESULT CSAS_Portrait::Initialize(void* pArg)
{
	FAILED_CHECK(CGameObject::Initialize(pArg));

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom));

#ifdef USE_PORTRAIT
	{
		CModel* pModel = nullptr;

		FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_Ch100_Portrait", L"ch100",
			(CComponent**)&pModel));

		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_NOT)] = pModel;

		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_NOT)]->FindMaterial(L"MI_ch0100_HOOD_0")->SetActive(false);
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_NOT)]->FindMaterial(L"MI_ch0100_MASK_0")->SetActive(false);

		for (auto pMtrl : m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_NOT)]->GetMaterials())
		{
			pMtrl->GetParam().iPass = 6;
		}
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_NOT)]->FindMaterial(L"MI_ch0100_WIRE_0")->GetParam().iPass = 7;
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_NOT)]->FindMaterial(L"MI_ch0100_WIRE_1")->GetParam().iPass = 7;


		const Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/PortraitCams/ch100_cam.json");
		m_SAS_PortraitCams[static_cast<_uint>(ESASType::SAS_NOT)] = CGameInstance::GetInstance()->Add_Camera("ch100_PortraitCam", LEVEL_NOW, L"Layer_Camera", L"Prototype_GameObject_Camera_Dynamic", &json);
		Safe_AddRef(m_SAS_PortraitCams[static_cast<_uint>(ESASType::SAS_NOT)]);
	}

	{
		CModel* pModel = nullptr;

		FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_Ch300_Portrait", L"ch300",
		   (CComponent**)&pModel));

		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_FIRE)] = pModel;

		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_FIRE)]->FindMaterial(L"MI_ch0300_HOOD_0")->SetActive(false);
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_FIRE)]->FindMaterial(L"MI_ch0300_MASK_0")->SetActive(false);

		for (auto pMtrl : m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_FIRE)]->GetMaterials())
		{
			pMtrl->GetParam().iPass = 6;
		}
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_FIRE)]->FindMaterial(L"MI_ch0300_WIRE_0")->GetParam().iPass = 7;
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_FIRE)]->FindMaterial(L"MI_ch0300_WIRE_1")->GetParam().iPass = 7;


		const Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/PortraitCams/ch300_cam.json");
		m_SAS_PortraitCams[static_cast<_uint>(ESASType::SAS_FIRE)] = CGameInstance::GetInstance()->Add_Camera("ch300_PortraitCam", LEVEL_NOW, L"Layer_Camera", L"Prototype_GameObject_Camera_Dynamic", &json);
		Safe_AddRef(m_SAS_PortraitCams[static_cast<_uint>(ESASType::SAS_FIRE)]);
	}

	{
		CModel* pModel = nullptr;
		
		FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_Ch400_Portrait", L"ch400",
		   (CComponent**)&pModel));
		
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_HARDBODY)] = pModel;
		
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_HARDBODY)]->FindMaterial(L"MI_ch0400_HOOD_0")->SetActive(false);
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_HARDBODY)]->FindMaterial(L"MI_ch0400_MASK_0")->SetActive(false);
		
		for (auto pMtrl : m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_HARDBODY)]->GetMaterials())
		{
			pMtrl->GetParam().iPass = 6;
		}
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_HARDBODY)]->FindMaterial(L"MI_ch0400_WIRE_0")->GetParam().iPass = 7;
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_HARDBODY)]->FindMaterial(L"MI_ch0400_WIRE_1")->GetParam().iPass = 7;
		
		
		const Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/PortraitCams/ch400_cam.json");
		m_SAS_PortraitCams[static_cast<_uint>(ESASType::SAS_HARDBODY)] = CGameInstance::GetInstance()->Add_Camera("ch400_PortraitCam", LEVEL_NOW, L"Layer_Camera", L"Prototype_GameObject_Camera_Dynamic", &json);
		Safe_AddRef(m_SAS_PortraitCams[static_cast<_uint>(ESASType::SAS_HARDBODY)]);
	}

	{
		CModel* pModel = nullptr;

		FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_Ch500_Portrait", L"ch500",
		   (CComponent**)&pModel));

		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_PENETRATE)] = pModel;

		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_PENETRATE)]->FindMaterial(L"MI_ch0500_HOOD_0")->SetActive(false);
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_PENETRATE)]->FindMaterial(L"MI_ch0500_MASK_0")->SetActive(false);

		for (auto pMtrl : m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_PENETRATE)]->GetMaterials())
		{
			pMtrl->GetParam().iPass = 6;
		}
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_PENETRATE)]->FindMaterial(L"MI_ch0500_WIRE_0")->GetParam().iPass = 7;
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_PENETRATE)]->FindMaterial(L"MI_ch0500_WIRE_1")->GetParam().iPass = 7;

		const Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/PortraitCams/ch500_cam.json");
		m_SAS_PortraitCams[static_cast<_uint>(ESASType::SAS_PENETRATE)] = CGameInstance::GetInstance()->Add_Camera("ch500_PortraitCam", LEVEL_NOW, L"Layer_Camera", L"Prototype_GameObject_Camera_Dynamic", &json);
		Safe_AddRef(m_SAS_PortraitCams[static_cast<_uint>(ESASType::SAS_PENETRATE)]);
	}

	{
		CModel* pModel = nullptr;

		FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_Ch600_Portrait", L"ch600",
		   (CComponent**)&pModel));

		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_TELEPORT)] = pModel;

		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_TELEPORT)]->FindMaterial(L"MI_ch0600_HOOD_0")->SetActive(false);
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_TELEPORT)]->FindMaterial(L"MI_ch0600_MASK_0")->SetActive(false);

		for (auto pMtrl : m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_TELEPORT)]->GetMaterials())
		{
			pMtrl->GetParam().iPass = 6;
		}
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_TELEPORT)]->FindMaterial(L"MI_ch0600_WIRE_0")->GetParam().iPass = 7;
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_TELEPORT)]->FindMaterial(L"MI_ch0600_WIRE_1")->GetParam().iPass = 7;


		const Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/PortraitCams/ch600_cam.json");
		m_SAS_PortraitCams[static_cast<_uint>(ESASType::SAS_TELEPORT)] = CGameInstance::GetInstance()->Add_Camera("ch600_PortraitCam", LEVEL_NOW, L"Layer_Camera", L"Prototype_GameObject_Camera_Dynamic", &json);
		Safe_AddRef(m_SAS_PortraitCams[static_cast<_uint>(ESASType::SAS_TELEPORT)]);
	}

	{
		CModel* pModel = nullptr;

		FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_Ch800_Portrait", L"ch800",
		   (CComponent**)&pModel));

		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_ELETRIC)] = pModel;

		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_ELETRIC)]->FindMaterial(L"MI_ch0800_HOOD_0")->SetActive(false);
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_ELETRIC)]->FindMaterial(L"MI_ch0800_MASK_0")->SetActive(false);

		for (auto pMtrl : m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_ELETRIC)]->GetMaterials())
		{
			pMtrl->GetParam().iPass = 6;
		}
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_ELETRIC)]->FindMaterial(L"MI_ch0800_WIRE_0")->GetParam().iPass = 7;
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_ELETRIC)]->FindMaterial(L"MI_ch0800_WIRE_1")->GetParam().iPass = 7;


		const Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/PortraitCams/ch800_cam.json");
		m_SAS_PortraitCams[static_cast<_uint>(ESASType::SAS_ELETRIC)] = CGameInstance::GetInstance()->Add_Camera("ch800_PortraitCam", LEVEL_NOW, L"Layer_Camera", L"Prototype_GameObject_Camera_Dynamic", &json);
		Safe_AddRef(m_SAS_PortraitCams[static_cast<_uint>(ESASType::SAS_ELETRIC)]);
	}

	{
		CModel* pModel = nullptr;
		
		FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_Ch900_Portrait", L"ch900",
		   (CComponent**)&pModel));
		
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_COPY)] = pModel;
		
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_COPY)]->FindMaterial(L"MI_ch0900_HOOD_0")->SetActive(false);
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_COPY)]->FindMaterial(L"MI_ch0900_MASK_0")->SetActive(false);
		
		for (auto pMtrl : m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_COPY)]->GetMaterials())
		{
			pMtrl->GetParam().iPass = 6;
		}
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_COPY)]->FindMaterial(L"MI_ch0900_WIRE_0")->GetParam().iPass = 7;
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_COPY)]->FindMaterial(L"MI_ch0900_WIRE_1")->GetParam().iPass = 7;
		
		
		const Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/PortraitCams/ch900_cam.json");
		m_SAS_PortraitCams[static_cast<_uint>(ESASType::SAS_COPY)] = CGameInstance::GetInstance()->Add_Camera("ch900_PortraitCam", LEVEL_NOW, L"Layer_Camera", L"Prototype_GameObject_Camera_Dynamic", &json);
		Safe_AddRef(m_SAS_PortraitCams[static_cast<_uint>(ESASType::SAS_COPY)]);
	}

	{
		CModel* pModel = nullptr;

		FAILED_CHECK(__super::Add_Component(LEVEL_NOW, L"Model_Ch1000_Portrait", L"ch1000",
		   (CComponent**)&pModel));

		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_SUPERSPEED)] = pModel;

		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_SUPERSPEED)]->FindMaterial(L"MI_ch1000_HOOD_0")->SetActive(false);
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_SUPERSPEED)]->FindMaterial(L"MI_ch1000_MASK_0")->SetActive(false);

		for (auto pMtrl : m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_SUPERSPEED)]->GetMaterials())
		{
			pMtrl->GetParam().iPass = 6;
		}
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_SUPERSPEED)]->FindMaterial(L"MI_ch1000_WIRE_0")->GetParam().iPass = 7;
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_SUPERSPEED)]->FindMaterial(L"MI_ch1000_WIRE_1")->GetParam().iPass = 7;


		const Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/PortraitCams/ch1000_cam.json");
		m_SAS_PortraitCams[static_cast<_uint>(ESASType::SAS_SUPERSPEED)] = CGameInstance::GetInstance()->Add_Camera("ch1000_PortraitCam", LEVEL_NOW, L"Layer_Camera", L"Prototype_GameObject_Camera_Dynamic", &json);
		Safe_AddRef(m_SAS_PortraitCams[static_cast<_uint>(ESASType::SAS_SUPERSPEED)]);
	}
#endif


	{
		const Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Portrait.json");
		m_pPostVFX = dynamic_cast<CPostVFX_SAS_Portrait*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_PostVFX", L"ProtoPostVFX_SASPortrait", (void*)&json));
		Safe_AddRef(m_pPostVFX);
	}


	m_pDissolveCurve = CCurveManager::GetInstance()->GetCurve("PortraitDissolveCurve");
	m_pColorLerpCurve = CCurveManager::GetInstance()->GetCurve("PortraitColorLerpRatio");
	m_pOutlineLerpCurve = CCurveManager::GetInstance()->GetCurve("PortraitOutlineLerpRatio");
	
	return S_OK;
}

void CSAS_Portrait::BeginTick()
{
	CGameObject::BeginTick();
}

void CSAS_Portrait::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	// if (CGameInstance::GetInstance()->KeyDown(DIK_8))
	// {
	// 	Start_SAS(ESASType::SAS_FIRE);
	// }
	// if (CGameInstance::GetInstance()->KeyDown(DIK_9))
	// {
	// 	// Start_SAS(ESASType::SAS_HARDBODY);
	// 	// Start_SAS(ESASType::SAS_TELEPORT);
	// 	// Start_SAS(ESASType::SAS_ELETRIC);
	// 	// Start_SAS(ESASType::SAS_FIRE);
	// 	// Start_SAS(ESASType::SAS_PENETRATE);
	// 	// Start_SAS(ESASType::SAS_SUPERSPEED);
	// }
	 // if (CGameInstance::GetInstance()->KeyDown(DIK_9))
	 // {
	 // 	Start_SAS(ESASType::SAS_COPY);
	 // }

	if (m_eCurType != ESASType::SAS_END)
	{
		m_SAS_PortraitModels[static_cast<_uint>(m_eCurType)]->Play_Animation(TimeDelta);

		const _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		const _vector vLocalMove = m_SAS_PortraitModels[static_cast<_uint>(m_eCurType)]->GetLocalMove(WorldMatrix);
		m_pTransformCom->LocalMove(vLocalMove);

		const _float fPlayRatio = m_SAS_PortraitModels[static_cast<_uint>(m_eCurType)]->GetPlayAnimation()->GetPlayRatio();

		m_pPostVFX->GetParam().Floats[1] = m_pColorLerpCurve->GetValue((_double)fPlayRatio);
		m_pPostVFX->GetParam().Floats[2] = m_pDissolveCurve->GetValue((_double)fPlayRatio);
		m_pPostVFX->GetParam().Floats[3] = m_pOutlineLerpCurve->GetValue((_double)fPlayRatio);
		if (fPlayRatio >= 0.99f)
		{
			m_eCurType = ESASType::SAS_END;
			ResetParams();
			m_bFinish = true;
		}

		if (fPlayRatio > 0.95f)
		{
			CGameInstance::GetInstance()->ResetTimeRatio();
		}
	}

	TickParams();
}

void CSAS_Portrait::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);
	if (m_eCurType != ESASType::SAS_END)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PORTRAIT, this);
	}
}

HRESULT CSAS_Portrait::Render()
{
	if (m_eCurType != ESASType::SAS_END)
	{
		m_SAS_PortraitModels[static_cast<_uint>(m_eCurType)]->Render_Cam(m_pTransformCom, m_SAS_PortraitCams[static_cast<_uint>(m_eCurType)]);
	}
	return S_OK;
}

void CSAS_Portrait::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();
}

void CSAS_Portrait::Start_SAS(ESASType eType)
{
#ifndef USE_PORTRAIT
	m_bFinish = true;
	return;
#endif
	m_eCurType = eType;
	// m_pPostVFX->GetParam().Float4s[0] = _float4(1.f, 0.231f, 0.f, 1.f);
	// m_pPostVFX->GetParam().Float4s[1] = _float4(0.957f, 0.459f, 0.048f, 1.f);
	const vector<wstring> except = { LAYER_SAS };
	CGameInstance::GetInstance()->SetTimeRatioCurve("SAS_StopTimer", false, &except);

	switch (m_eCurType)
	{
	case ESASType::SAS_FIRE: 
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_FIRE)]->SetPlayAnimation("AS_ch0300_SAS01");
		break;
	case ESASType::SAS_PENETRATE:
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_PENETRATE)]->SetPlayAnimation("AS_ch0500_SAS01");
		 break;
	case ESASType::SAS_HARDBODY: 
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_HARDBODY)]->SetPlayAnimation("AS_ch0400_SAS01");
		break;
	case ESASType::SAS_TELEPORT: 
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_TELEPORT)]->SetPlayAnimation("AS_ch0600_SAS01");
		break;
	case ESASType::SAS_ELETRIC: 
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_ELETRIC)]->SetPlayAnimation("AS_ch0800_SAS01");
		break;
	case ESASType::SAS_SUPERSPEED: 
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_SUPERSPEED)]->SetPlayAnimation("AS_ch1000_SAS01");
		break;
	case ESASType::SAS_COPY: 
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_COPY)]->SetPlayAnimation("AS_ch0900_SAS01");
		break;

	case ESASType::SAS_INVISIBLE: break;
	case ESASType::SAS_GRAVIKENISIS: break;
	case ESASType::SAS_NOT: 
		m_SAS_PortraitModels[static_cast<_uint>(ESASType::SAS_NOT)]->SetPlayAnimation("AS_ch0100_BrainCrash_Cutin");
		break;
	case ESASType::SAS_END: break;
	default:
		NODEFAULT;

	}

	m_pPostVFX->GetParam().Floats[0] = 1.f;
	m_pTransformCom->Set_WorldMatrix(_float4x4::Identity);
}

void CSAS_Portrait::ResetParams()
{
	m_pPostVFX->GetParam().Floats[0] = 0.f;
	m_pPostVFX->GetParam().Floats[2] = 0.f;
}

void CSAS_Portrait::TickParams()
{
	switch (m_eCurType)
	{
	case ESASType::SAS_FIRE: 
		break;
	case ESASType::SAS_PENETRATE: break;
	case ESASType::SAS_HARDBODY: break;
	case ESASType::SAS_TELEPORT: break;
	case ESASType::SAS_ELETRIC: break;
	case ESASType::SAS_SUPERSPEED: break;
	case ESASType::SAS_COPY: break;
	case ESASType::SAS_INVISIBLE: break;
	case ESASType::SAS_GRAVIKENISIS: break;
	case ESASType::SAS_NOT: break;
	case ESASType::SAS_END: break;
	default:
		NODEFAULT;
	}
}

_bool CSAS_Portrait::isFinish()
{
	if (m_bFinish)
	{
		m_bFinish = false;
		return true;
	}

	return false;
}

CSAS_Portrait* CSAS_Portrait::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSAS_Portrait* pInstance = new CSAS_Portrait(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSAS_Portrait");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSAS_Portrait::Clone(void* pArg)
{
	CSAS_Portrait*		pInstance = new CSAS_Portrait(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSAS_Portrait");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSAS_Portrait::Free()
{
	CGameObject::Free();
	for (auto& pModel : m_SAS_PortraitModels)
		Safe_Release(pModel);
	for (auto& pCam : m_SAS_PortraitCams)
		Safe_Release(pCam);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pPostVFX);
}

