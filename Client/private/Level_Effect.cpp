#include "stdafx.h"
#include "..\public\Level_Effect.h"
#include "EffectGroup.h"
#include "EffectSystem.h"
#include "GameInstance.h"
#include "Imgui_MapEditor.h"
#include "Imgui_LevelSwitcher.h"
#include "JsonLib.h"
#include "Imgui_PropertyEditor.h"
#include "JsonStorage.h"
#include "MaterialPreview.h"
#include "GameUtils.h"
#include "Material.h"
#include "Imgui_PostProcess.h"
#include "ParticleSystem.h"
#include "SkyBox.h"
#include "TrailSystem.h"
#include "CamSpot.h"
#include "Weapon_wp0190.h"
#include "Model.h"
#include "AnimationInstance.h"
#include "Player.h"
#include "Controller.h"
#include "PostVFX_Distortion.h"
#include "PostVFX_Scifi.h"
#include "PostVFX_WhiteOut.h"
#include "Imgui_EffectBrowser.h"
#include "ParticleGroup.h"
#include "PostVFX_ColorGrading.h"
#include "PostVFX_HitDecal.h"
#include "VFX_Manager.h"
CLevel_Effect::CLevel_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Effect::Initialize()
{
	CGameInstance::GetInstance()->Clear_ImguiObjects();

	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_EffectBrowser::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_LevelSwitcher::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PostProcess::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AppLog::Create(m_pDevice, m_pContext));
	CVFX_Manager::GetInstance()->Initialize(LEVEL_EFFECT);

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer(TEXT("Layer"))))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Effect::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);

	if(CGameInstance::GetInstance()->KeyDown(DIK_SPACE))
	{
		CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_DEFAULT_ATTACK, L"Default_Attack_1")->Start_EffectWork();
	}
}

void CLevel_Effect::Late_Tick(_double TimeDelta)
{
	CLevel::Late_Tick(TimeDelta);
}

HRESULT CLevel_Effect::Render()
{
	FAILED_CHECK(__super::Render());
	SetWindowText(g_hWnd, TEXT("Level : Effecttool"));

	return S_OK;
}

void CLevel_Effect::LoadEffects(const char* pEffectDir)
{
	m_mapEffectGroup.clear();

	CGameUtils::ListFiles(pEffectDir, [this](const string& pEffectPath)
	{
		Json jsonEffect = CJsonStorage::GetInstance()->FindOrLoadJson(pEffectPath);
		auto pEffectGroup = dynamic_cast<CEffectGroup*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_EffectFolder", L"ProtoVFX_EffectGroup", &jsonEffect));
		m_mapEffectGroup.emplace(pEffectGroup->GetObjectTag(), pEffectPath);
		pEffectGroup->SetDelete();
	});
}

HRESULT CLevel_Effect::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(-cosf(XMConvertToRadians(60.f)), -sinf(XMConvertToRadians(60.f)), 0.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	NULL_CHECK(pGameInstance->Add_Light("DirectionalLight", m_pDevice, m_pContext, LightDesc));

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Prototypes()
{
	auto pGameInstance = CGameInstance::GetInstance();

	// ForSky

	if (FAILED(pGameInstance->Add_Prototype(LEVEL_NOW, TEXT("Prototype_Component_Model_SkySphere"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/Model/StaticModel/Sky/SkySphere.static_model"))))
		return E_FAIL;

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"Prototype_GameObject_SkyBox", CSkyBox::Create(m_pDevice, m_pContext)));

	// ~ForSky

	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_EffectSystem", CEffectSystem::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_EffectGroup", CEffectGroup::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_TrailSystem", CTrailSystem::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_ParticleSystem", CParticleSystem::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_ParticleGroup", CParticleGroup::Create(m_pDevice, m_pContext)));


	// 김기범의 흔적
	pGameInstance->Add_Prototype(L"Player", CPlayer::Create(m_pDevice, m_pContext));
	pGameInstance->Add_Prototype(L"CamSpot", CCamSpot::Create(m_pDevice, m_pContext));

	auto pModel_Player = CModel::Create(m_pDevice, m_pContext,
		"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Player.anim_model");

	pModel_Player->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Animation/");
	FAILED_CHECK(pGameInstance->Add_Prototype(L"Model_Player", pModel_Player));

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Component_LocalController"),
		CController::Create())))
		return E_FAIL;

	_matrix WeaponPivot = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationZ(XMConvertToRadians(180.f));
	pGameInstance->Add_Prototype(L"PlayerWeapon", CWeapon_wp0190::Create(m_pDevice, m_pContext));
	auto pModel_Weapon = CModel::Create(m_pDevice, m_pContext,
		"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model", WeaponPivot);
	FAILED_CHECK(pGameInstance->Add_Prototype(L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model", pModel_Weapon));
	// ~ 김기범의 흔적



	// For PostVFX Distortion
	if (FAILED(pGameInstance->Add_Prototype(TEXT("ProtoPostVFX_Distortion"),
		CPostVFX_Distortion::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	// For PostVFX Scifi
	if (FAILED(pGameInstance->Add_Prototype(TEXT("ProtoPostVFX_Scifi"),
		CPostVFX_Scifi::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//
	if (FAILED(pGameInstance->Add_Prototype(TEXT("ProtoPostVFX_Hit_Decal"),
		CPostVFX_HitDecal::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_PostVFX_ColorGrading"),
		CPostVFX_ColorGrading::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// 모델 추가하는 방법
	// auto pModel_VFX = CModel::Create(m_pDevice, m_pContext,
	// 	"../Bin/Resources/Meshes/Scarlet_Nexus/VFX/Player_Default_Attack/Default_Attack_1.static_model");
	// // 프로토타입 추가 방법
	// FAILED_CHECK(pGameInstance->Add_Prototype(L"VFX_Model_Default_Attack_01", pModel_VFX));
	//
	// pModel_VFX = CModel::Create(m_pDevice, m_pContext,
	// 	"../Bin/Resources/Meshes/Scarlet_Nexus/VFX/Player_Default_Attack/Default_Attack_2.static_model");
	// FAILED_CHECK(pGameInstance->Add_Prototype(L"VFX_Model_Default_Attack_02", pModel_VFX));
	//
	// pModel_VFX = CModel::Create(m_pDevice, m_pContext,
	// 	"../Bin/Resources/Meshes/Scarlet_Nexus/VFX/Player_Default_Attack/Default_Attack_3.static_model");
	// FAILED_CHECK(pGameInstance->Add_Prototype(L"VFX_Model_Default_Attack_03", pModel_VFX));
	//
	// pModel_VFX = CModel::Create(m_pDevice, m_pContext,
	// 	"../Bin/Resources/Meshes/Scarlet_Nexus/VFX/Player_Default_Attack/Default_Attack_4.static_model");
	// FAILED_CHECK(pGameInstance->Add_Prototype(L"VFX_Model_Default_Attack_04", pModel_VFX));

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer(const _tchar* pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	// For_SkySphere
	FAILED_CHECK(pGameInstance->Clone_GameObject(LEVEL_NOW, L"Layer_Env", TEXT("Prototype_GameObject_SkyBox")));
	Json ColorGrading = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/ColorGrading.json");
	pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"Prototype_PostVFX_ColorGrading", &ColorGrading);
	Json Test = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Scifi/Scifi_PostVFX.json");
	pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoPostVFX_Scifi", &Test);
	Json Distortion = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Distortion/Distortion_PostVFX.json");
	pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoPostVFX_Distortion", &Distortion);



	// FAILED_CHECK(pGameInstance->Clone_GameObject(LEVEL_NOW, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic")));
	CGameInstance::GetInstance()->Add_Camera("DynamicCamera", LEVEL_NOW, pLayerTag, L"Prototype_GameObject_Camera_Dynamic");

	// FAILED_CHECK(pGameInstance->Clone_GameObject(LEVEL_NOW, L"Layer_EffectSys", TEXT("ProtoVFX_EffectSystem")));

	//For PostVFX
	// FAILED_CHECK(pGameInstance->Clone_GameObject(LEVEL_NOW, L"Layer_EffectSys", TEXT("ProtoVFX_EffectSystem")));
	// Json ScifiEffect = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Scifi/Scifi_DefaultAttack_1.json");
	// pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoVFX_EffectSystem", &ScifiEffect);


	// Json Attack = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Curve/Default_Attack/Default_Attack_1.json");
	// FAILED_CHECK(pGameInstance->Clone_GameObject(L"Layer_EffectSys", TEXT("ProtoVFX_EffectGroup"), &Attack));

	FAILED_CHECK(pGameInstance->Clone_GameObject(LEVEL_NOW, L"Layer_ParticleWorks", TEXT("ProtoVFX_ParticleGroup")));

	

	//~For PostVFX

	// For Distortion
	// Json AttackMesh = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Distortion/Distortion_Effect.json");
	// pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoVFX_EffectSystem", &AttackMesh);
	//

	//~ For Distortion


	// Json ScifiEffect = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Scifi/Scifi_DefaultAttack_4_3.json");
	// pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoVFX_EffectSystem", &ScifiEffect);



	// FAILED_CHECK(pGameInstance->Clone_GameObject(LEVEL_NOW, L"Layer_EffectSys", TEXT("ProtoVFX_EffectGroup")));


	// Json AttackMesh = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Curve/Color_Scale_Test.json");
	// pGameInstance->Clone_GameObject(L"Layer_EffectSys", L"ProtoVFX_EffectGroup", &AttackMesh);

	// Json AttackMesh = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/Trail/PlayerSwordTrail.json");
	// FAILED_CHECK(pGameInstance->Clone_GameObject(L"Layer_EffectSys", TEXT("ProtoVFX_TrailSystem"), &AttackMesh));
	//
	//
	// Json Particle = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/ParticleSystem/Default_Particle.json");
	// FAILED_CHECK(pGameInstance->Clone_GameObject(L"Layer_EffectSys", TEXT("ProtoVFX_ParticleSystem"), &Particle));

	// FAILED_CHECK(pGameInstance->Clone_GameObject(L"Preview", L"MaterialPreview"));

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	// if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
	// 	return E_FAIL;

	// if (FAILED(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_Camera_Player"))))
		// return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json PreviewData;
	PreviewData["Model"] = "Model_Player";

	CGameObject* pPlayer = nullptr;
	NULL_CHECK(pPlayer = pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("Player"), &PreviewData));

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("CamSpot"), pPlayer));

	return S_OK;
}

HRESULT CLevel_Effect::Ready_Layer_Map(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/TestMap.json");

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_ScarletMap"), &json));
	return S_OK;
}

CLevel_Effect* CLevel_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Effect*	pInstance = new CLevel_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Effect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Effect::Free()
{
	CLevel::Free();
}
