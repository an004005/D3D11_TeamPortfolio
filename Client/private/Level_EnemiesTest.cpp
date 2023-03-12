#include "stdafx.h"
#include "Level_EnemiesTest.h"

#include "GameInstance.h"
#include "Material.h" 
#include "GameUtils.h"
#include "Imgui_PropertyEditor.h"
#include "Imgui_PostProcess.h"
#include "Imgui_LevelSwitcher.h"
#include "Imgui_AnimModifier.h"
#include "Imgui_MapEditor.h"
#include "Imgui_PhysX.h"
#include "Imgui_CameraManager.h"
#include "Model.h"
#include "JsonLib.h"
#include "AnimationInstance.h"
#include "Texture.h"
#include "MapNonAnim_Object.h"
#include "JsonStorage.h"
#include "RigidBody.h"
#include "Model_Instancing.h"
#include "SkyBox.h"
#include "MapKinetic_Object.h"
#include "FactoryMethod.h"


CLevel_EnemiesTest::CLevel_EnemiesTest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_EnemiesTest::Initialize()
{
	CGameInstance::GetInstance()->Clear_ImguiObjects();
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_LevelSwitcher::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PropertyEditor::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AppLog::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_AnimModifier::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PostProcess::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_PhysX::Create(m_pDevice, m_pContext));
	CGameInstance::GetInstance()->Add_ImguiObject(CImgui_CameraManager::Create(m_pDevice, m_pContext));

	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Prototypes()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Bullet(TEXT("Layer_Bullet"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Kinetic(TEXT("Layer_Kinetic"))))
		return E_FAIL;
		
	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_Effect(TEXT("Layer_PostVFX"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_EnemiesTest::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);
}

void CLevel_EnemiesTest::Late_Tick(_double TimeDelta)
{
	CLevel::Late_Tick(TimeDelta);
	if (CGameInstance::GetInstance()->KeyDown(DIK_9))
	{
		CGameInstance::GetInstance()->Clone_GameObject(L"test", L"Indicator");
	}
}

HRESULT CLevel_EnemiesTest::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : EnemiesTest"));

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof LightDesc);

	/*LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);*/

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.isEnable = true;
	// LightDesc.vDirection = _float4(1.f, -1.f, 1.0f, 0.f);
	LightDesc.vDirection = _float4(-cosf(XMConvertToRadians(60.f)), -sinf(XMConvertToRadians(60.f)), 0.0f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	NULL_CHECK(pGameInstance->Add_Light("DirectionalLight", m_pDevice, m_pContext, LightDesc));

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Prototypes()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	// CGameUtils::ListFilesRecursive("../Bin/Resources/Materials/", [this](const string& fileName)
	// {
	// 	char szFileName[MAX_PATH]{};
	// 	_splitpath_s(fileName.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);
	// 	CGameInstance::GetInstance()->Add_Prototype(CGameUtils::s2ws(szFileName).c_str(), CMaterial::Create(m_pDevice, m_pContext, fileName.c_str()));
	// });

	
	// 키네틱 오브젝트 모델
	CGameUtils::ListFilesRecursive("../Bin/Resources/Model/StaticModel/MapStaicModels/Kinetic/",
		[this](const string& fileName)
	{
		char szFileExt[MAX_PATH]{};
		_splitpath_s(fileName.c_str(), nullptr, 0, nullptr, 0, nullptr, 0, szFileExt, MAX_PATH);

		if (0 == strcmp(szFileExt, ".static_model"))
		{
			FAILED_CHECK(Create_Model(s2ws(fileName), fileName.c_str()));
		}
	});
	FAILED_CHECK(pGameInstance->Add_Prototype(L"Proto_KineticObject_Table", CMapKinetic_Object::Create(m_pDevice, m_pContext)));



	FAILED_CHECK(pGameInstance->Add_Prototype(LEVEL_NOW, L"Prototype_GameObject_SkyBox", CSkyBox::Create(m_pDevice, m_pContext)));

	FAILED_CHECK(CFactoryMethod::MakePlayerPrototypes(m_pDevice, m_pContext));
	FAILED_CHECK(CFactoryMethod::MakeEffectPrototypes(m_pDevice, m_pContext));
	FAILED_CHECK(CFactoryMethod::MakeEnermyPrototypes(m_pDevice, m_pContext));

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// if (FAILED(pGameInstance->Clone_GameObject(LEVEL_NOW, pLayerTag, TEXT("Prototype_GameObject_Camera_Dynamic"))))
	// 	return E_FAIL;
	CGameInstance::GetInstance()->Add_Camera("DynamicCamera", LEVEL_NOW, pLayerTag, L"Prototype_GameObject_Camera_Dynamic");

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	/*if (FAILED(pGameInstance->Clone_GameObject(LEVEL_NOW, pLayerTag, TEXT("TestMonster"))))
		return E_FAIL;*/


	pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("BronJon"))
		->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, _float4(120.f, 3.f, 15.f, 1.f));

	// Test 하지 않는 중인 Monster 넣어두기
	/*
	pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("BuddyLumi"))
		->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, _float4(40.f, 3.f, 3.f, 1.f));
	
	pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("FlowerLeg"))
		->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, _float4(20.f, 3.f, 6.f, 1.f));
		
	pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("SkummyPool"))
		->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, _float4(-20.f, 3.f, -3.f, 1.f));

	pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("SkummyPandou"))
		->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, _float4(100.f, 3.f, 10.f, 1.f));

		
	pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("Prototype_MonsterBoss1"))
		->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, _float4(200.f, 3.f, 10.f, 1.f));
	*/


	/*auto pObj = pGameInstance->Clone_GameObject_Get(pLayerTag, L"Prototype_MonsterBoss1");
	_float4 pos = pObj->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	pos.y += 1.f;
	pObj->GetTransform()->Set_State(CTransform::STATE_TRANSLATION, pos);*/
	
	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_Bullet(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();	
	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json PreviewData;
	PreviewData["Model"] = "Model_Player";

	CGameObject* pPlayer = nullptr;
	NULL_CHECK(pPlayer = pGameInstance->Clone_GameObject_Get(pLayerTag, TEXT("Player"), &PreviewData));

	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("CamSpot"), pPlayer));

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_Kinetic(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json Test;
	Test["ModelTag"] = "../Bin/Resources/Model/StaticModel/MapStaicModels/Kinetic/Table/Table.static_model";
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Proto_KineticObject_Table"), &Test));

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Layer_Map(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Objects/Map/Map_DownTown.json");
	
	FAILED_CHECK(pGameInstance->Clone_GameObject(pLayerTag, TEXT("Prototype_GameObject_ScarletMap"), &json));
	
	return S_OK;
}

HRESULT CLevel_EnemiesTest::Ready_Effect(const _tchar * pLayerTag)
{
	 CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	//Json ScifiEffect = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Scifi/Scifi_DefaultAttack_1.json");
	//pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoVFX_EffectSystem", &ScifiEffect);

	Json Test = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Scifi/Scifi_PostVFX.json");
	pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoPostVFX_Scifi", &Test);

	Json Distortion = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/PostVFX/Distortion/Distortion_PostVFX.json");
	pGameInstance->Clone_GameObject(L"Layer_PostVFX", L"ProtoPostVFX_Distortion", &Distortion);

	return S_OK;
}

HRESULT CLevel_EnemiesTest::Create_Model(const wstring & pProtoTag, const char * pModelPath)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	CComponent* pComponent = nullptr;

	pComponent = CModel::Create(m_pDevice, m_pContext, pModelPath);
	assert(pComponent != nullptr);

	FAILED_CHECK(pGameInstance->Add_Prototype(
		pProtoTag.c_str(),
		pComponent));

	//m_pProtosTags.emplace_back(pProtoTag,  NON_INSTANCE);

	return S_OK;
}

CLevel_EnemiesTest * CLevel_EnemiesTest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_EnemiesTest*		pInstance = new CLevel_EnemiesTest(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_EnemiesTest");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_EnemiesTest::Free()
{
	__super::Free();
}
