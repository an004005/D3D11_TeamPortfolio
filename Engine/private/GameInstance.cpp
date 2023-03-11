#include "stdafx.h"
#include "..\public\GameInstance.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Imgui_Manager.h"
#include "Timer_Manager.h"
#include "Light_Manager.h"
#include "JsonStorage.h"
#include "Font_Manager.h"
#include "Frustum.h"
#include "Animation.h"
#include "Camera.h"
#include "Target_Manager.h"
#include "HDR.h"
#include "Material.h"
#include "Sound_Manager.h"
#include "PhysX_Manager.h"
#include "Camera_Manager.h"
#include "CurveManager.h"
#include "GameTime_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance)

_uint CGameInstance::m_iStaticLevelIndex = 0;
_uint CGameInstance::m_iLoadingLevelIndex = 0;
const _tchar* CGameInstance::m_pPrototypeTransformTag = TEXT("Prototype_Component_Transform");
const _tchar* CGameInstance::m_pPrototypeVIRectTag = TEXT("Prototype_Component_VIBuffer_Rect");

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::GetInstance())
	, m_pInput_Device(CInput_Device::GetInstance())
	, m_pLevel_Manager(CLevel_Manager::GetInstance())
	, m_pObject_Manager(CObject_Manager::GetInstance())
	, m_pComponent_Manager(CComponent_Manager::GetInstance())
	, m_pPipeLine(CPipeLine::GetInstance())
	, m_pTimer_Manager(CTimer_Manager::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
	, m_pFont_Manager(CFont_Manager::GetInstance())
	, m_pFrustum(CFrustum::GetInstance())
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pImgui_Manager(CImgui_Manager::GetInstance())
	, m_pHDR(CHDR::GetInstance())
	, m_pSound_Manager(CSound_Manager::GetInstance())
	, m_pPhysX_Manager(CPhysX_Manager::GetInstance())
	, m_pCamera_Manager(CCamera_Manager::GetInstance())
	, m_pCurve_Manager(CCurveManager::GetInstance())
	, m_pGameTime_Manager(CGameTime_Manager::GetInstance())
{
	Safe_AddRef(m_pGraphic_Device);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pImgui_Manager);
	Safe_AddRef(m_pHDR);
	Safe_AddRef(m_pSound_Manager);
	Safe_AddRef(m_pPhysX_Manager);
	Safe_AddRef(m_pCamera_Manager);
	Safe_AddRef(m_pCurve_Manager);
	Safe_AddRef(m_pGameTime_Manager);
}

/*************************
 *	GameInstance
 *************************/
HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut)
{
	if (nullptr == m_pGraphic_Device || 
		nullptr == m_pInput_Device || 
		nullptr == m_pObject_Manager || 
		nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_hWnd = GraphicDesc.hWnd;
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_ORTHO, XMMatrixOrthographicLH((_float)GraphicDesc.iViewportSizeX, (_float)GraphicDesc.iViewportSizeY, 0.f, 1.f));

	/* 그래픽 디바이스 초기화. */
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWindowMode, GraphicDesc.iViewportSizeX, GraphicDesc.iViewportSizeY, ppDeviceOut, ppContextOut)))
		return E_FAIL;

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	/* imgui 초기화 */
	m_pImgui_Manager->Ready_Imgui(GraphicDesc.hWnd, *ppDeviceOut, *ppContextOut);

	// HDR 초기화
	if (FAILED(m_pHDR->Initialize(*ppDeviceOut, *ppContextOut)))
		return E_FAIL;

	/* 입력 디바이스 초기화. */
	if (FAILED(m_pInput_Device->Ready_Input_Device(hInst, GraphicDesc.hWnd)))
		return E_FAIL;
	
	m_iTotalLevel = iNumLevels + 2; // static, loading 포함

	if (FAILED(m_pObject_Manager->Reserve_Manager(m_iTotalLevel)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Manager(m_iTotalLevel)))
		return E_FAIL;

	/* 엔진에서 제공하는 스태틱레벨의 인덱스를 저장해준다. */
	/* 클라이언트 개발자가 스태틱 레벨에 컴포넌트 원형을 추가하고싶은 경우에 스태틱레벨인덱스를 
	클랑리언트에 보여주기 위해서. */
	m_iStaticLevelIndex = m_iTotalLevel - 1;
	m_iLoadingLevelIndex = m_iTotalLevel - 2;

	{
		/* 엔진에서 제공하는 CGameObject를 상속받는 객체들이 기본적으로 CTransform컴포넌트를 기본으로 가지고 있게 만들어주기위해 
		복제할 수 있는 CTransform의 원형객체를 생성한다. */
		/* 실제 이 원형을 복제하는 루틴 CGameObject의 Initialize함수에서 복제를 담당한다. */
		if (FAILED(m_pComponent_Manager->Add_Prototype(
			m_iStaticLevelIndex, m_pPrototypeTransformTag, 
			CTransform::Create(*ppDeviceOut, *ppContextOut))))
			return E_FAIL;

		if (FAILED(m_pComponent_Manager->Add_Prototype(
			m_iStaticLevelIndex, TEXT("Prototype_Component_Renderer"), 
			m_pRenderer = CRenderer::Create(*ppDeviceOut, *ppContextOut))))
			return E_FAIL;
		Safe_AddRef(m_pRenderer);


		if (FAILED(m_pComponent_Manager->Add_Prototype(
			m_iStaticLevelIndex, m_pPrototypeVIRectTag,
			CVIBuffer_Rect::Create(*ppDeviceOut, *ppContextOut))))
			return E_FAIL;

		if (FAILED(m_pComponent_Manager->Add_Prototype(
			m_iStaticLevelIndex, TEXT("Prototype_Component_Shader_VtxAnimModel_Shadow"), 
			CShader::Create(*ppDeviceOut, *ppContextOut, L"../Bin/ShaderFiles/Shader_VtxAnimModel_Shadow.hlsl", VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
			return E_FAIL;
		if (FAILED(m_pComponent_Manager->Add_Prototype(
			m_iStaticLevelIndex, TEXT("Prototype_Component_Shader_VtxModel_Shadow"), 
			CShader::Create(*ppDeviceOut, *ppContextOut, L"../Bin/ShaderFiles/Shader_VtxModel_Shadow.hlsl", VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
			return E_FAIL;
	}

	// null animation 셋팅
	CAnimation::s_NullAnimation.SetFinish();
	CAnimation::s_NullAnimation.SetLooping(false);

	// coll mgr 초기화
	// m_pCollision_Manager->Initialize({ -50.f, 130.f }, {95.f, -32.f});

	if (FAILED(m_pTarget_Manager->Initialize(*ppDeviceOut, *ppContextOut)))
		return E_FAIL;

	if (FAILED(m_pFrustum->Initialize()))
		return E_FAIL;

	m_pPhysX_Manager->Initialize();

	FAILED_CHECK(m_pSound_Manager->Initialize("../Bin/Resources/Sound/SoundDesc.json"));

	m_pCurve_Manager->LoadCurves("../Bin/Resources/Curve/CurveManagerData.json");

	return S_OK;
}

void CGameInstance::Tick_Engine(_double TimeDelta)
{
	if (nullptr == m_pInput_Device || 
		nullptr == m_pLevel_Manager || 
		nullptr == m_pObject_Manager)
		return;

	m_pGameTime_Manager->Tick(TimeDelta);

	const _double TimeDeltaModified = TimeDelta * m_pGameTime_Manager->GetTimeRatio();
	m_TimeDelta = TimeDeltaModified;

	/* 입력장치의 상태를 갱신받아온다. */
	m_pInput_Device->Invalidate_Input_Device();
	m_pObject_Manager->SetUpdatedLevel(m_pLevel_Manager->GetUpdatedLevel());

	m_pImgui_Manager->Tick_Imgui();

	m_pObject_Manager->Tick(TimeDelta);
	m_pLevel_Manager->Tick(TimeDeltaModified);

	m_pObject_Manager->Late_Tick(TimeDelta);
	m_pLevel_Manager->Late_Tick(TimeDeltaModified);

	if (m_pLevel_Manager->GetUpdatedLevel() != LEVEL_LOADING)
	{
		m_pPhysX_Manager->Simulate(TimeDeltaModified);
		m_pPhysX_Manager->WaitSimulate();
	}

	m_pObject_Manager->AfterPhysX();

	m_pCamera_Manager->Tick();
	m_pPipeLine->Tick();

	if (m_pCamera_Manager->GetMainCam())
	{
		const _float4x4 ListenerWorldMatrix = m_pCamera_Manager->GetMainCam()->GetTransform()->Get_WorldMatrix_f4x4();
		m_pSound_Manager->Tick((_float)TimeDelta, &ListenerWorldMatrix);
	}
	else
	{
		m_pSound_Manager->Tick((_float)TimeDelta);
	}
}

void CGameInstance::Clear()
{
	for (_uint i = 0; i < m_iTotalLevel; ++i)
	{
		m_pObject_Manager->Clear(i);
		m_pComponent_Manager->Clear(i);
	}
	m_pRenderer->Clear();
	m_pLight_Manager->Clear();
	m_pCamera_Manager->Clear();
	m_pSound_Manager->Stop_All();
}

void CGameInstance::Clear_Level(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)		
		return;

	m_pObject_Manager->Clear(iLevelIndex);
	m_pComponent_Manager->Clear(iLevelIndex);
	m_pRenderer->Clear();
}

void CGameInstance::Draw_RenderGroup()
{
	if (m_pRenderer)
		m_pRenderer->Draw_RenderGroup();
}

/*************************
 *	Graphic_Device
 *************************/
HRESULT CGameInstance::Clear_Graphic_Device(const _float4 * pColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	HRESULT			hr = 0;

	hr = m_pGraphic_Device->Clear_BackBuffer_View(*pColor);	
	hr = m_pGraphic_Device->Clear_DepthStencil_View();

	return hr;
}


HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Update_SwapChain(HWND hWnd, _uint iWinCX, _uint iWinCY, _bool bIsFullScreen, _bool bNeedUpdate)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_hWnd = hWnd;

	CONTEXT_LOCK
	if (FAILED(m_pGraphic_Device->Update_SwapChain(hWnd, iWinCX, iWinCY, bIsFullScreen, bNeedUpdate)))
		return E_FAIL;

	if (bNeedUpdate)
	{
		m_pPipeLine->Set_Transform(CPipeLine::D3DTS_ORTHO, XMMatrixOrthographicLH((_float)iWinCX, (_float)iWinCY, 0.f, 1.f));
		if (FAILED(m_pTarget_Manager->Resize(GetContext())))
			return E_FAIL;

		if (FAILED(m_pHDR->Initialize(m_pGraphic_Device->GetDevice(), m_pGraphic_Device->GetContext())))
			return E_FAIL;
	}

	return S_OK;
}

_uint CGameInstance::GetWinCX() const
{
	if (nullptr == m_pGraphic_Device)
		return 0;
	return m_pGraphic_Device->GetWinCX();
}

_uint CGameInstance::GetWinCY() const
{
	if (nullptr == m_pGraphic_Device)
		return 0;
	return m_pGraphic_Device->GetWinCY();
}

mutex& CGameInstance::GetContextMtx() const
{
	return m_pGraphic_Device->GetContextMtx();
}

ID3D11Device* CGameInstance::GetDevice()
{
	return m_pGraphic_Device->GetDevice();
}

ID3D11DeviceContext* CGameInstance::GetContext()
{
	return m_pGraphic_Device->GetContext();
}

/*************************
 	Input_Device
 *************************/
_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(CInput_Device::MOUSEKEYSTATE byMouseID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseState(byMouseID);
}

_long CGameInstance::Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMoveState)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMove(eMoveState);
}

bool CGameInstance::KeyDown(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->KeyDown(byKeyID);
}

bool CGameInstance::KeyPressing(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->KeyPressing(byKeyID);
}

bool CGameInstance::KeyUp(_ubyte byKeyID)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->KeyUp(byKeyID);
}

bool CGameInstance::KeyDown(CInput_Device::MOUSEKEYSTATE byMouseID)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->KeyDown(byMouseID);
}

bool CGameInstance::KeyUp(CInput_Device::MOUSEKEYSTATE byMouseID)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->KeyUp(byMouseID);
}

bool CGameInstance::KeyPressing(CInput_Device::MOUSEKEYSTATE byMouseID)
{
	if (nullptr == m_pInput_Device)
		return false;
	return m_pInput_Device->KeyPressing(byMouseID);
}

/*************************
 *	Level_Manager
 *************************/
HRESULT CGameInstance::Open_Loading(_uint iNewLevelIdx, CLoadingLevel* pLoadingLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pLight_Manager->Clear();
	m_pCamera_Manager->Clear();

	return m_pLevel_Manager->Open_Loading(iNewLevelIdx, pLoadingLevel);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

HRESULT CGameInstance::Render_Level()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Render();
}

_uint CGameInstance::GetCurLevelIdx()
{
	if (nullptr == m_pLevel_Manager)
		return 0;

	return m_pLevel_Manager->GetCurLevelIdx();
}

_bool CGameInstance::IsLoading()
{
	return m_pLevel_Manager->IsLoading();
}

CLevel* CGameInstance::GetLevel()
{
	return m_pLevel_Manager->GetLevel();
}

/*************************
 *	Object_Manager
 *************************/
HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);	
}

HRESULT CGameInstance::Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pPrototype)
{
	return Add_Prototype(LEVEL_NOW, pPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Clone_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
{
	return m_pObject_Manager->Clone_GameObject(iLevelIndex, pLayerTag, pPrototypeTag, pArg);
}

HRESULT CGameInstance::Clone_GameObject(const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg)
{
	return Clone_GameObject(LEVEL_NOW, pLayerTag, pPrototypeTag, pArg);
}

CGameObject* CGameInstance::Clone_GameObject_Get(_uint iLevelIndex, const _tchar* pLayerTag,
	const _tchar* pPrototypeTag, void* pArg)
{
	return m_pObject_Manager->Clone_GameObject_Get(iLevelIndex, pLayerTag, pPrototypeTag, pArg);
}

CGameObject* CGameInstance::Clone_GameObject_Get(const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg)
{
	return Clone_GameObject_Get(LEVEL_NOW, pLayerTag, pPrototypeTag, pArg);
}

CGameObject* CGameInstance::Clone_GameObject_NoLayer(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg)
{

	return m_pObject_Manager->Clone_GameObject_NoLayer(iLevelIndex, pPrototypeTag, pArg);
}

wcmap<class CLayer*>& CGameInstance::GetLayers(_uint iLevelIndex)
{
	return m_pObject_Manager->GetLayers(iLevelIndex);
}

CLayer* CGameInstance::GetLayer(_uint iLevelIndex, const _tchar* pLayerTag)
{
	return m_pObject_Manager->GetLayer(iLevelIndex, pLayerTag);
}

CGameObject* CGameInstance::Find_ObjectByPredicator(_uint iLevelIndex, std::function<_bool(CGameObject*)> Pred, const _tchar* pLayerTag)
{
	return m_pObject_Manager->Find_ObjectByPredicator(iLevelIndex, Pred, pLayerTag);
}

list<CGameObject*> CGameInstance::Find_AllObjectByPredicator(_uint iLevelIndex, std::function<_bool(CGameObject*)> Pred)
{
	return m_pObject_Manager->Find_AllObjectByPredicator(iLevelIndex, Pred);
}

CGameObject * CGameInstance::Find_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag)
{
	return m_pObject_Manager->Find_Prototype(iLevelIndex, pPrototypeTag);
}

void CGameInstance::Imgui_ProtoViewer(const _tchar*& szSelectedProto)
{
	m_pObject_Manager->Imgui_ProtoViewer(szSelectedProto);
}

void CGameInstance::Imgui_ObjectViewer(_uint iLevel, CGameObject*& pSelectedObject)
{
	m_pObject_Manager->Imgui_ObjectViewer(iLevel, pSelectedObject);
}

void CGameInstance::Imgui_LayerCombo(_uint iLevel, const _tchar*& pLayerName)
{
	m_pObject_Manager->Imgui_LayerCombo(iLevel, pLayerName);
}

/*************************
 *	Component_Manager
 *************************/
HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);	
}

HRESULT CGameInstance::Add_Prototype(const _tchar* pPrototypeTag, CComponent* pPrototype)
{
	return Add_Prototype(LEVEL_NOW, pPrototypeTag, pPrototype);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

CComponent* CGameInstance::Clone_Component(const _tchar* pPrototypeTag, void* pArg)
{
	return Clone_Component(LEVEL_NOW, pPrototypeTag, pArg);
}

wcmap<CComponent*>* CGameInstance::GetProtoTypes(_uint iLevelIndex)
{
	return m_pComponent_Manager->GetProtoTypes(iLevelIndex);
}

CComponent * CGameInstance::Find_Prototype_Component(_uint iLevelIndex, const _tchar * pPrototypeTag)
{
	return m_pComponent_Manager->Find_Prototype(iLevelIndex, pPrototypeTag);
}

/*************************
 *	PipeLine
 *************************/
_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();
		
	return m_pPipeLine->Get_TransformMatrix(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(eState);
}

_matrix CGameInstance::Get_TransformMatrix_Inverse(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix_Inverse(eState);
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

_float4 CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return _float4(0.0f, 0.f, 0.f, 1.f);

	return m_pPipeLine->Get_CamPosition();	
}

_float4 CGameInstance::Get_CamLook()
{
	if (nullptr == m_pPipeLine)
		return _float4(0.0f, 0.f, 0.f, 0.f);

	return m_pPipeLine->Get_CamLook();
}

/*************************
 *	Timer_Manager
 *************************/
_double CGameInstance::Get_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0;

	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Ready_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Ready_Timer(pTimerTag);
}

void CGameInstance::Update_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return;

	m_pTimer_Manager->Update_Timer(pTimerTag);
}

/*************************
 *	Light_Manager
 *************************/
CLight* CGameInstance::Find_Light(const string& strLightTag)
{
	return m_pLight_Manager->Find_Light(strLightTag);
}

CLight* CGameInstance::Add_Light(const string& strLightTag, ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	const LIGHTDESC& LightDesc)
{
	return m_pLight_Manager->Add_Light(strLightTag, pDevice, pContext, LightDesc);
}

void CGameInstance::Delete_Light(const string& strLightTag)
{
	m_pLight_Manager->Delete_Light(strLightTag);
}

void CGameInstance::ClearLight()
{
	m_pLight_Manager->Clear();
}

void CGameInstance::SetShadowCam(CCamera* pShadowCam)
{
	m_pLight_Manager->SetShadowCam(pShadowCam);
}

/*************************
 *	CFont_Manager
 *************************/
HRESULT CGameInstance::Add_Font(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontTag, const _tchar * pFontFilePath)
{
	return m_pFont_Manager->Add_Font(pDevice, pContext, pFontTag, pFontFilePath);
}

HRESULT CGameInstance::Render_Font(const _tchar * pFontTag, const _tchar * pText, const _float2 & vPos, _float fRadian, _float2 vScale, _fvector vColor)
{
	return m_pFont_Manager->Render_Font(pFontTag, pText, vPos, fRadian, vScale, vColor);
}

/*************************
 *	CFrustum
 *************************/
_bool CGameInstance::isInFrustum_WorldSpace(_fvector vWorldPos, _float fRange)
{
	return m_pFrustum->isInFrustum_WorldSpace(vWorldPos, fRange);
}

_bool CGameInstance::isInFrustum_LocalSpace(_fvector vLocalPos, _float fRange)
{
	return m_pFrustum->isInFrustum_LocalSpace(vLocalPos, fRange);
}

/*************************
 *	CSound_Manager
 *************************/
CSound* CGameInstance::CloneSound(const string& soundName)
{
	return m_pSound_Manager->CloneSound(soundName);
}

void CGameInstance::Stop_All_Sound()
{
	m_pSound_Manager->Stop_All();
}

void CGameInstance::AddSoundQueue(const string& QName)
{
	m_pSound_Manager->AddSoundQ(QName);
}

/*************************
 *	CTarget_Manager
 *************************/
ID3D11ShaderResourceView* CGameInstance::Get_SRV(const _tchar* pTargetTag)
{
	return m_pTarget_Manager->Get_SRV(pTargetTag);
}

/*************************
 *	CPhysX_Manager
 *************************/
_bool CGameInstance::RayCast(const RayCastParams& params)
{
	return m_pPhysX_Manager->RayCast(params);
}

_bool CGameInstance::OverlapSphere(const SphereOverlapParams& params)
{
	return m_pPhysX_Manager->OverlapSphere(params);
}

_bool CGameInstance::OverlapCapsule(const CapsuleOverlapParams& params)
{
	return m_pPhysX_Manager->OverlapCapsule(params);
}

_bool CGameInstance::SweepSphere(const SphereSweepParams& params)
{
	return m_pPhysX_Manager->SweepSphere(params);
}

_bool CGameInstance::SweepCapsule(const CapsuleSweepParams& params)
{
	return m_pPhysX_Manager->SweepCapsule(params);
}

_bool CGameInstance::PxSweepCapsule(const PxCapsuleSweepParams & params)
{
	return m_pPhysX_Manager->PxSweepCapsule(params);
}

/*************************
 *	CCamera_Manager
 *************************/
CCamera* CGameInstance::Add_Camera(const string& strCamTag, _uint iLevelIdx, const wstring& pLayerTag, const wstring& pPrototypeTag, const Json* camJson)
{
	return m_pCamera_Manager->Add_Camera(strCamTag, iLevelIdx, pLayerTag, pPrototypeTag, camJson);
}

void CGameInstance::SetMainCamera(const string& strCamTag)
{
	m_pCamera_Manager->SetMainCamera(strCamTag);
}

void CGameInstance::SetMainCamera(CCamera* pCamera)
{
	m_pCamera_Manager->SetMainCamera(pCamera);
}

CCamera* CGameInstance::GetMainCam()
{
	return m_pCamera_Manager->GetMainCam();
}

_matrix CGameInstance::GetCamViewMatrix(const string& strCamTag)
{
	return m_pCamera_Manager->GetCamViewMatrix(strCamTag);
}

_matrix CGameInstance::GetCamProjMatrix(const string& strCamTag)
{
	return m_pCamera_Manager->GetCamProjMatrix(strCamTag);
}

_float4 CGameInstance::Get_CamPosition(const string& strCamTag)
{
	return m_pCamera_Manager->Get_CamPosition(strCamTag);
}

CCamera* CGameInstance::FindCamera(const string& strCamTag)
{
	return m_pCamera_Manager->FindCamera(strCamTag);
}

void CGameInstance::ResetTimeRatio()
{
	m_pGameTime_Manager->ResetTimeRatio();
}

void CGameInstance::SetTimeRatioCurve(const string& strCurveTag, _bool bStay, const vector<wstring>* ExceptLayers)
{
	m_pGameTime_Manager->SetTimeRatioCurve(strCurveTag, bStay, ExceptLayers);
}

/*************************
 *	CImgui_Manager
 *************************/
void CGameInstance::Render_ImGui()
{
	if (nullptr == m_pImgui_Manager)
		return;

	m_pImgui_Manager->Render_Imgui();
}

void CGameInstance::Render_Update_ImGui()
{
	if (nullptr == m_pImgui_Manager)
		return;

	m_pImgui_Manager->Render_Update_ImGui();
}

void CGameInstance::Add_ImguiObject(CImguiObject* ImguiObject)
{
	if (m_pImgui_Manager == nullptr) return;
	m_pImgui_Manager->Add_ImguiObject(ImguiObject);
}

void CGameInstance::Clear_ImguiObjects()
{
	if (m_pImgui_Manager == nullptr) return;
	m_pImgui_Manager->Clear_ImguiObjects();
}

void CGameInstance::Imgui_OnOff(_bool bOn)
{
	m_pImgui_Manager->ImguiOnOff(bOn);
}

_vector & CGameInstance::GetPeekingPos()
{
	return m_vecPeekingPos;
}

void CGameInstance::SetPeekingPos(_fvector vPeekingPos)
{
	m_vecPeekingPos = vPeekingPos;
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->Clear();
	_uint ref = CGameInstance::GetInstance()->DestroyInstance();

	ref = CCamera_Manager::GetInstance()->DestroyInstance();

	ref = CObject_Manager::GetInstance()->DestroyInstance();

	ref = CComponent_Manager::GetInstance()->DestroyInstance();

	ref = CLevel_Manager::GetInstance()->DestroyInstance();

	ref = CInput_Device::GetInstance()->DestroyInstance();

	ref = CPipeLine::GetInstance()->DestroyInstance();

	ref = CLight_Manager::GetInstance()->DestroyInstance();

	ref = CGraphic_Device::GetInstance()->DestroyInstance();

	ref = CTimer_Manager::GetInstance()->DestroyInstance();

	ref = CImgui_Manager::GetInstance()->DestroyInstance();

	ref = CTarget_Manager::GetInstance()->DestroyInstance();

	ref = CFont_Manager::GetInstance()->DestroyInstance();

	ref = CFrustum::GetInstance()->DestroyInstance();

	ref = CHDR::GetInstance()->DestroyInstance();

	CGameTime_Manager::GetInstance()->DestroyInstance();

	CSound_Manager::GetInstance()->DestroyInstance();

	// json은 동떨어진 기능이라서 gameinstace에서 포함하지 않고 파괴만 담당
	ref = CJsonStorage::GetInstance()->DestroyInstance();

	CCurveManager::GetInstance()->DestroyInstance();

	CPhysX_Manager::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pCamera_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pSound_Manager);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pHDR);
	Safe_Release(m_pImgui_Manager);
	Safe_Release(m_pGameTime_Manager);

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pCurve_Manager);
	Safe_Release(m_pPhysX_Manager);

}

