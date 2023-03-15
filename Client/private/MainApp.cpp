#include "stdafx.h"
#include "..\public\MainApp.h"
#include "GameInstance.h"
#include "Camera_Dynamic.h"
#include "ClientUtils.h"
#include "Level_Loading_Simple.h"
#include "Level_Logo.h"
#include "VIBuffer_PosRect.h"
#include "VIBuffer_Sphere.h"
#include "VIBuffer_CircleRect.h"
#include "VIBuffer_CircleRectLookOut.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Terrain.h"
#include "ScarletMap.h"
#include "Psychokinesis.h"
#include "MapNonAnim_Object.h"
#include "Camera_Player.h"
#include "ControlledRigidBody.h"
#include "RigidBody.h"
#include "UI_Manager.h"
#include "VIBuffer_Mesh_Instance.h"
#include "MapInstance_Object.h"
#include "MapKinetic_Object.h"
#include "Material.h"
#include "VFX_Manager.h"
#include "PostVFX_Penetrate.h"

#include "PostVFX_Scifi.h"
#include "EffectGroup.h"
#include "ParticleSystem.h"
#include "ParticleGroup.h"
#include "PostVFX_Distortion.h"
#include "SAS_Portrait.h"
#include "EffectSystem.h"
#include "TrailSystem.h"


CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;
	srand((unsigned)time(NULL));

	/* 게임엔진 초기화 */
	GRAPHIC_DESC			GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(GRAPHIC_DESC));

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iViewportSizeX = g_iWinSizeX;
	GraphicDesc.iViewportSizeY = g_iWinSizeY;
	GraphicDesc.eWindowMode = GRAPHIC_DESC::WINMODE_WIN;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL_LOGO)))
		return E_FAIL;

// #ifndef _DEBUG
	// CGameInstance::GetInstance()->Imgui_OnOff(false);
// #endif

	return S_OK;
}

void CMainApp::Tick(_double TimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	m_pGameInstance->Tick_Engine(TimeDelta);
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	CONTEXT_LOCK

	m_pGameInstance->Render_ImGui();

	m_pGameInstance->Clear_Graphic_Device(&_float4(0.5f, 0.25f, 0.25f, 1.f));	

	m_pGameInstance->Draw_RenderGroup();

	m_pGameInstance->Render_Update_ImGui();

	m_pGameInstance->Render_Level();

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Resize_BackBuffer()
{
	GRAPHIC_DESC	tGraphicDesc;
	ZeroMemory(&tGraphicDesc, sizeof(GRAPHIC_DESC));

	tGraphicDesc.hWnd = g_hWnd;
	if (!g_bFullScreen)
	{
		tGraphicDesc.iViewportSizeX = g_iWinSizeX;
		tGraphicDesc.iViewportSizeY = g_iWinSizeY;
	}
	else
	{
		tGraphicDesc.iViewportSizeX = GetSystemMetrics(SM_CXSCREEN);
		tGraphicDesc.iViewportSizeY = GetSystemMetrics(SM_CYSCREEN);
	}

	if (FAILED(m_pGameInstance->Update_SwapChain(tGraphicDesc.hWnd, tGraphicDesc.iViewportSizeX, tGraphicDesc.iViewportSizeY, g_bFullScreen, g_bNeedResizeSwapChain)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Open_Loading(LEVEL_LOGO, CLevel_Loading_Simple::Create<CLevel_Logo>(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("ProtoPostVFX_Scifi"),
		CPostVFX_Scifi::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("ProtoPostVFX_Distortion"),
		CPostVFX_Distortion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"ProtoVFX_EffectSystem", CEffectSystem::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"ProtoVFX_EffectGroup", CEffectGroup::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"ProtoVFX_TrailSystem", CTrailSystem::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"ProtoVFX_ParticleSystem", CParticleSystem::Create(m_pDevice, m_pContext)));
	FAILED_CHECK(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"ProtoVFX_ParticleGroup", CParticleGroup::Create(m_pDevice, m_pContext)));

	/* For.Prototype_Component_Shader_VtxPosRect */	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosRect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosRect.hlsl"), VTXPOS_DECLARATION::Elements, VTXPOS_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_PosRect */	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_VIBuffer_PosRect", CVIBuffer_PosRect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_MeshInstance */
	// _matrix TransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	_float4x4 PivotMatrix = XMMatrixIdentity();
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_VIBuffer_Mesh_Instance", CVIBuffer_Mesh_Instancing::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/VFX/Player_Default_Attack/Air_Attack_1.static_model", PivotMatrix, 500))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Sakura_Mesh_Instance", CVIBuffer_Mesh_Instancing::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/VFX_Ver2/SM_ef_Sakura_001.static_model", PivotMatrix, 100))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_LongLeaf_Mesh_Instance", CVIBuffer_Mesh_Instancing::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/VFX/Effect_Plane/VFX_Mesh_Plane_0.static_model", PivotMatrix, 100))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_ShortLeaf_Mesh_Instance", CVIBuffer_Mesh_Instancing::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/VFX/Effect_Plane/VFX_Mesh_ShorLeaf_10.static_model", PivotMatrix, 100))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_FlowerBullet_Mesh_Instance", CVIBuffer_Mesh_Instancing::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/VFX/Effect_Plane/VFX_Mesh_Plane_2.static_model", PivotMatrix, 100))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Em0650BulletDeadParticle_Mesh_Instance", CVIBuffer_Mesh_Instancing::Create(m_pDevice, m_pContext, "../Bin/Resources/Meshes/VFX/Effect_Twist/VFX_Mesh_Twist_06.static_model", PivotMatrix, 100))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh_Instance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh_Instance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_MeshInstance.hlsl"), VTXMESH_INSTANCE_DECLARATION::Elements, VTXMESH_INSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;


	/* For.Prototype_Component_Shader_VtxTex */	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel_Instancing*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel_Instancing"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel_Instancing.hlsl"), VTXMODEL_INSTANCE_DECLARATION::Elements, VTXMODEL_INSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;

	{
		/* For.Prototype_Component_Shader_VtxAnimModel*/
		auto pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements);
		pShader->SetCommonTexture("g_Weak01", "../Bin/Resources/Model/AnimModel/Monster/Common/Texture/T_em0000_weak01.png");
		pShader->SetCommonTexture("g_Vanish_Noise", "../Bin/Resources/Model/AnimModel/Monster/Common/Texture/T_em0000_vanish_noise.png");
		pShader->SetCommonTexture("g_Weak_Noise", "../Bin/Resources/Model/AnimModel/Monster/Common/Texture/T_em0000_weak_noise.png");
		pShader->SetCommonTexture("g_WaveTile", "../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Texture/T_Wave_Tile_00.dds");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), pShader)))
			return E_FAIL;
	}

	{
		/* For.Prototype_Component_Shader_VtxToonAnimModel*/
		auto pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxToonAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements);
		pShader->SetCommonTexture("g_WaveTile", "../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Texture/T_Wave_Tile_00.dds");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxToonAnimModel"), pShader)))
			return E_FAIL;
	}

	{
		/* For.Prototype_Component_Shader_VtxToonModel*/
		auto pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxToonModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements);
		pShader->SetCommonTexture("g_WaveTile", "../Bin/Resources/Meshes/Scarlet_Nexus/AnimModels/Player/Texture/T_Wave_Tile_00.dds");
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxToonModel"), pShader)))
				return E_FAIL;
	}


	/*For.Prototype_VIBuffer_Sphere*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_VIBuffer_Sphere",
		CVIBuffer_Sphere::Create(m_pDevice, m_pContext, 0.5f))))
		return E_FAIL;



	/* For.Prototype_Component_Shader_VtxNorSkyBox*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorSkyBox"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorSkyBox.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNor*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNor"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNor.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNor*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;


	/* For.Prototype_Component_Shader_VtxPointInstance */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointInstance _Particle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance_Particle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance_ParticleSystem.hlsl"), VTXPOINT_INSTANCE_PARTICLE_DECLARATION::Elements, VTXPOINT_INSTANCE_PARTICLE_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPointInstance _Particle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance_Particle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance_ParticleSystem.hlsl"), VTXPOINT_INSTANCE_PARTICLE_DECLARATION::Elements, VTXPOINT_INSTANCE_PARTICLE_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTex_UI */	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTexUI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex_UI.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTex_VFX */	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_VFX"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex_VFX.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTex_VFX_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_VFX_UI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex_VFX_UI.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel_VFX */	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel_VFX"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel_VFX.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Trail_VFX */	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail_VFX"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstanceTrail_VFX.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNor_VFX*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNor_VFX"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex_VFX.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_CircleRect */	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_VIBuffer_CircleRect", CVIBuffer_CircleRect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_Component_VIBuffer_CircleRectLookOut */	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_VIBuffer_CircleRectLookOut", CVIBuffer_CircleRectLookOut::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_VIBuffer_Terrain", CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube_SSDecal */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube_SSDecal"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube_SSDecal.hlsl"), VTXCUBETEX_DECLARATION::Elements, VTXCUBETEX_DECLARATION::iNumElements))))
		return E_FAIL;
	/* For.Prototype_Component_VIBuffer_Cube */	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_VIBuffer_Cube", CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// /* For.Prototype_Component_Shader_ParticleSystem */
	// if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ParticleSystem"),
	// 	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance_ParticleSystem.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
	// 	return E_FAIL;

	/* For. Prototype_Component_Psycokinesis */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Psycokinesis"),
		CPsychokinesis::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_ControlledRigidBody"), CControlledRigidBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"), CRigidBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Regular32"), TEXT("../Bin/Resources/Fonts/kim_regular32.spritefont"))))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Bold32"), TEXT("../Bin/Resources/Fonts/kim_bold32.spritefont"))))
	//	return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Pretendard32"), TEXT("../Bin/Resources/Fonts/Pretendard.spritefont"))))
		return E_FAIL;

	CMaterial::LoadMaterialFilePathes("../Bin/Resources/Materials/");
	
	
	m_pGameInstance->AddSoundQueue("AnnouncerVO");
	m_pGameInstance->AddSoundQueue("CharacterVO");

	// Engine::LoadEmptyMaterials();

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Dynamic */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Dynamic"),
		CCamera_Dynamic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ScarletMap */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_ScarletMap"),
		CScarletMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_Camera_Player"),
		CCamera_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. MapNonAnimObject */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_MapNonAnim_Object"),
		CMapNonAnim_Object::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. MapInstanceObject */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_MapInstance_Object"),
		CMapInstance_Object::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. MapKineticObject */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_GameObject_MapKinetic_Object"),
		CMapKinetic_Object::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For. ProtoPostVFX_Penetrate */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("ProtoPostVFX_Penetrate"),
		CPostVFX_Penetrate::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainApp::Free()
{
	CVFX_Manager::GetInstance()->DestroyInstance();
	CUI_Manager::GetInstance()->DestroyInstance();

	m_pGameInstance->Clear_ImguiObjects();
	m_pGameInstance->Clear();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	CGameInstance::Release_Engine();
}
