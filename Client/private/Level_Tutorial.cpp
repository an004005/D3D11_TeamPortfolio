#include "stdafx.h"
#include "..\public\Level_Tutorial.h"
#include "GameInstance.h"
#include "Material.h"
#include "JsonStorage.h"
#include "Imgui_PropertyEditor.h"
#include "Imgui_LevelSwitcher.h"
#include "Controller.h"
#include "CamSpot.h"
#include "EffectSystem.h"
#include "Player.h"
#include "Weapon_wp0190.h"
#include "PostVFX_Scifi.h"
#include "EffectGroup.h"
#include "ParticleSystem.h"
#include "PostVFX_Distortion.h"
#include "TrailSystem.h"

CLevel_Tutorial::CLevel_Tutorial(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tutorial::Initialize()
{

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_LevelSwitcher::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AppLog::Create(m_pDevice, m_pContext));

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(L"Layer_UI")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(L"Layer_Effect")))
		return E_FAIL;

	return S_OK;
}

void CLevel_Tutorial::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CLevel_Tutorial::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CLevel_Tutorial::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : TUTORIAL"));

	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Prototypes()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

		// player
	{
		pGameInstance->Add_Prototype(L"Player", CPlayer::Create(m_pDevice, m_pContext));
		pGameInstance->Add_Prototype(L"CamSpot", CCamSpot::Create(m_pDevice, m_pContext));

		{
			auto pModel_Player = CModel::Create(m_pDevice, m_pContext,
				"../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Player.anim_model");

			pModel_Player->LoadAnimations("../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Animation/");
			FAILED_CHECK(pGameInstance->Add_Prototype(L"Model_Player", pModel_Player));
		}

		_matrix WeaponPivot = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationZ(XMConvertToRadians(180.f));
		pGameInstance->Add_Prototype(L"PlayerWeapon", CWeapon_wp0190::Create(m_pDevice, m_pContext));
		auto pModel_Weapon = CModel::Create(m_pDevice, m_pContext,
			"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model", WeaponPivot);
		FAILED_CHECK(pGameInstance->Add_Prototype(L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_190/wp0190.static_model", pModel_Weapon));



		{	// 이펙트 프로토타입
			if (FAILED(pGameInstance->Add_Prototype(TEXT("ProtoPostVFX_Scifi"),
				CPostVFX_Scifi::Create(m_pDevice, m_pContext))))
				return E_FAIL;

			if (FAILED(pGameInstance->Add_Prototype(TEXT("ProtoPostVFX_Distortion"),
				CPostVFX_Distortion::Create(m_pDevice, m_pContext))))
				return E_FAIL;

			FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_EffectSystem", CEffectSystem::Create(m_pDevice, m_pContext)));
			FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_EffectGroup", CEffectGroup::Create(m_pDevice, m_pContext)));
			FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_TrailSystem", CTrailSystem::Create(m_pDevice, m_pContext)));
			FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"ProtoVFX_ParticleSystem", CParticleSystem::Create(m_pDevice, m_pContext)));
		}


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_Component_LocalController"), CController::Create())))
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Lights()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	NULL_CHECK(pGameInstance->Add_Light("DirectionalLight", m_pDevice, m_pContext, LightDesc));


	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CGameInstance::GetInstance()->Add_Camera("DynamicCamera", LEVEL_NOW, pLayerTag, L"Prototype_GameObject_Camera_Dynamic");


	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json PreviewData;
	PreviewData["Model"] = "Model_Player";

	CGameObject* pPlayer = nullptr;
	NULL_CHECK(pPlayer = pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("Player"), &PreviewData));

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("CamSpot"), pPlayer));

	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_UI(const _tchar * pLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_Map(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Tutorial.json");

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_ScarletMap"), &json));

	return S_OK;
}

HRESULT CLevel_Tutorial::Ready_Layer_Effect(const _tchar * pLayerTag)
{
	return S_OK;
}

CLevel_Tutorial * CLevel_Tutorial::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Tutorial*		pInstance = new CLevel_Tutorial(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Tutorial");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Tutorial::Free()
{
	__super::Free();
}
