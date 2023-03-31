#pragma once

/* 싱글톤.  */
/* 클라이언트와 엔진의 소통을 위해 만들어진 클래스. */

#include "Base.h"
#include "Input_Device.h"
#include "Component_Manager.h"
#include "PipeLine.h"
#include "Graphic_Device.h"
#include "Layer.h"
#include "Sound.h"

#define LEVEL_STATIC CGameInstance::Get_StaticLevelIndex()
#define LEVEL_LOADING CGameInstance::GetLoadingLevelIndex()
#define LEVEL_NOW CGameInstance::GetInstance()->GetCurLevelIdx()

#define	WINCX CGameInstance::GetInstance()->GetWinCX()
#define	WINCY CGameInstance::GetInstance()->GetWinCY()

#define CLIENT_HWND CGameInstance::GetInstance()->GetHWND()

#define CONTEXT_LOCK CContext_LockGuard _CtxLock_(CGameInstance::GetInstance()->GetContextMtx(), CGameInstance::GetInstance()->IsLoading());

#define TIME_DELTA CGameInstance::GetInstance()->GetTimeDelta()

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance);
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	static _uint Get_StaticLevelIndex() { return m_iStaticLevelIndex; }
	static _uint GetLoadingLevelIndex() { return m_iLoadingLevelIndex; }

	HWND GetHWND() { return m_hWnd; }
	_double GetTimeDelta() const { return m_TimeDelta; }
	CRenderer* GetRenderer() { return m_pRenderer; }

public: /* For.GameInstance */	
	static const _tchar*			m_pPrototypeTransformTag;
	static const _tchar*			m_pPrototypeVIRectTag;
private:
	_double m_TimeDelta = 0.0;
	std::function<void()> m_LevelLoadingAsync = nullptr;

public: /* For.GameInstance */
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut);
	void Tick_Engine(_double TimeDelta);
	void Clear();
	void Clear_Level(_uint iLevelIndex);
	void Draw_RenderGroup();

public: /* For.Graphic_Device */
	HRESULT Clear_Graphic_Device(const _float4* pColor);
	HRESULT Present();
	HRESULT Update_SwapChain(HWND hWnd, _uint iWinCX, _uint iWinCY, _bool bIsFullScreen, _bool bNeedUpdate);
	_uint GetWinCX() const;
	_uint GetWinCY() const;
	mutex& GetContextMtx() const;
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();


public: /* For.Input_Device */
	_byte		Get_DIKeyState(_ubyte byKeyID);
	_byte		Get_DIMouseState(CInput_Device::MOUSEKEYSTATE byMouseID);
	_long		Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMoveState);

	bool KeyDown(_ubyte byKeyID);
	bool KeyUp(_ubyte byKeyID);
	bool KeyPressing(_ubyte byKeyID);
	bool KeyDown(CInput_Device::MOUSEKEYSTATE byMouseID);
	bool KeyUp(CInput_Device::MOUSEKEYSTATE byMouseID);
	bool KeyPressing(CInput_Device::MOUSEKEYSTATE byMouseID);

public: /* For.Level_Manager */
	HRESULT Open_Loading(_uint iNewLevelIdx, class CLoadingLevel* pLoadingLevel);
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);	
	HRESULT Render_Level();
	_uint GetCurLevelIdx();
	_bool IsLoading();
	class CLevel* GetLevel();


public: /* For.Object_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Clone_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg = nullptr);
	HRESULT Clone_GameObject(const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg = nullptr);
	// Clone_GameObject_Get 및 Clone_GameObject_NoLayer는 AddRef 가 안되므로 사용시 주의 요망
	CGameObject* Clone_GameObject_Get(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg = nullptr);
	CGameObject* Clone_GameObject_Get(const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg = nullptr);
	CGameObject* Clone_GameObject_NoLayer(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);
	CGameObject* Clone_GameObject_NoLayerNoBegin(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);
	wcmap<class CLayer*>& GetLayers(_uint iLevelIndex);
	CLayer* GetLayer(_uint iLevelIndex, const _tchar* pLayerTag);
	CGameObject* Find_ObjectByPredicator(_uint iLevelIndex, std::function<_bool(CGameObject*)> Pred, const _tchar* pLayerTag = nullptr);
	list<CGameObject*> Find_AllObjectByPredicator(_uint iLevelIndex, std::function<_bool(CGameObject*)> Pred);
	template<typename T>
	T* Find_OneObjectByType(_uint iLevelIndex, const _tchar* pLayerTag);
	class CGameObject* Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag);
	void Add_InLayer(const _tchar* pLayerTag, CGameObject* pGameObject);
	void Add_EmptyLayer(_uint iLevelIndex, const _tchar* pLayerTag);

	void Imgui_ProtoViewer(const _tchar*& szSelectedProto);
	void Imgui_ObjectViewer(_uint iLevel, CGameObject*& pSelectedObject);
	void Imgui_LayerCombo(_uint iLevel, OUT const _tchar*& pLayerName);
	_bool Check_ObjectAlive(CGameObject* pObject);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);
	class CComponent* Clone_Component(const _tchar* pPrototypeTag, void* pArg = nullptr);
	wcmap<class CComponent*>* GetProtoTypes(_uint iLevelIndex);
	class CComponent* Find_Prototype_Component(_uint iLevelIndex, const _tchar* pPrototypeTag);

public: /* For.PipeLine */
	_matrix Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState) ;
	_float4x4 Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eState) ;
	_matrix Get_TransformMatrix_Inverse(CPipeLine::TRANSFORMSTATE eState) ;
	void Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_float4 Get_CamPosition();
	_float4 Get_CamLook();	// Cam Look 가져오는거

public: /* For.Timer_Manager */ 
	_double		Get_TimeDelta(const _tchar* pTimerTag);
	HRESULT		Ready_Timer(const _tchar* pTimerTag);
	void		Update_Timer(const _tchar* pTimerTag);

public: /* For.Light_Manager */ 
	class CLight* Find_Light(const string& strLightTag);
	class CLight* Add_Light(const string& strLightTag, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	void Delete_Light(const string& strLightTag);
	void ClearLight();
	void SetShadowCam(class CCamera* pShadowCam);
	void AddLifePointLight(_float fLife, _float4 vPos, _float fRange, _float4 vColor);
	void AddLifeCapsuleLight(_float fLife, _float4 vStart, _float4 vEnd, _float fRange, _float4 vColor);

public: /* For.Font_Manager */
	HRESULT Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Font(const _tchar* pFontTag, const _tchar* pText, const _float2& vPos, _float fRadian, _float2 vScale, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f));

public: /* For.Frustum */
	_bool isInFrustum_WorldSpace(_fvector vWorldPos, _float fRange = 0.f);
	_bool isInFrustum_LocalSpace(_fvector vLocalPos, _float fRange = 0.f);

public: /* for sound manager*/
	class CSound* CloneSound(const string& soundName);
	void Stop_All_Sound();
	void AddSoundQueue(const string& QName);

public: /* for targetManager*/
	ID3D11ShaderResourceView* Get_SRV(const _tchar* pTargetTag);

public:/* for PhysX Manager*/
	// 주의사항 CPhysX_Manager 에서 읽고 사용하기, 예제도 있음
	_bool RayCast(const RayCastParams& params);
	_bool OverlapSphere(const SphereOverlapParams& params);
	_bool OverlapCapsule(const CapsuleOverlapParams& params);
	_bool SweepSphere(const SphereSweepParams& params);
	_bool SweepCapsule(const CapsuleSweepParams& params);
	_bool PxSweepCapsule(const PxCapsuleSweepParams& params);

public: /* for CameraManager*/
	CCamera* Add_Camera(const string& strCamTag, _uint iLevelIdx, const wstring& pLayerTag, const wstring& pPrototypeTag, const Json* camJson = nullptr);
	void SetMainCamera(const string& strCamTag);
	void SetMainCamera(CCamera* pCamera);
	CCamera* GetMainCam();
	_matrix GetCamViewMatrix(const string& strCamTag);
	_matrix GetCamProjMatrix(const string& strCamTag);
	_float4 Get_CamPosition(const string& strCamTag);
	CCamera* FindCamera(const string& strCamTag);
	void SetCameraFovCurve(const string& strCurveTag);
	void ActionCamTickByPlayTime(_float fRatio);
	void ReleaseCameraFovCurve();
	void SetCameraFov(_float fFov);

public:/*for GameTimeManager */
	void ResetTimeRatio();
	void SetTimeRatioCurve(const string& strCurveTag, _bool bStay = false, const vector<wstring>* ExceptLayers = nullptr);
	void SetTimeRatio(_float fTimeRatio, const vector<wstring>* ExceptLayers = nullptr);
	void SetLayerTimeRatio(_float fLayerTimeRatio, const wstring& strLayerTag);
	void ResetDefaultTimeRatio();
	void ClearAllTimeRatio();

public:
	void LambdaRenderRequest(const _float4x4& WorldMatrix, const std::function<void()>& RenderFunction, CRenderer::RENDERGROUP eRenderGroup);

public: // for CImgui_Manager
	void Render_ImGui();
	void Render_Update_ImGui();
	void Add_ImguiObject(class CImguiObject* pImguiObject);
	void Clear_ImguiObjects();
	void Imgui_OnOff(_bool bOn);

public: // for Curve_Manager
	class CCurveFloatImpl* GetCurve(const string& strCurveTag);

public: // for RootAnimation
	_vector&	GetPeekingPos();
	void		SetPeekingPos(_fvector vPeekingPos);
private: // for RootAnimation
	_vector		m_vecPeekingPos;

private:
	static _uint					m_iStaticLevelIndex;
	static _uint					m_iLoadingLevelIndex;

private:
	class CGraphic_Device*			m_pGraphic_Device = nullptr;
	class CInput_Device*			m_pInput_Device = nullptr;
	class CLevel_Manager*			m_pLevel_Manager = nullptr;
	class CObject_Manager*			m_pObject_Manager = nullptr;
	class CComponent_Manager*		m_pComponent_Manager = nullptr;
	class CPipeLine*				m_pPipeLine = nullptr;
	class CTimer_Manager*			m_pTimer_Manager = nullptr;
	class CLight_Manager*			m_pLight_Manager = nullptr;
	class CFont_Manager*			m_pFont_Manager = nullptr;
	class CFrustum*					m_pFrustum = nullptr;
	class CTarget_Manager*			m_pTarget_Manager = nullptr;
	class CHDR*						m_pHDR = nullptr;
	class CSound_Manager*			m_pSound_Manager = nullptr;
	class CPhysX_Manager*			m_pPhysX_Manager = nullptr;
	class CCamera_Manager*			m_pCamera_Manager = nullptr;
	class CCurveManager*			m_pCurve_Manager = nullptr;
	class CGameTime_Manager*		m_pGameTime_Manager = nullptr;
	class CImgui_Manager*			m_pImgui_Manager = nullptr;
	class CSSAOManager*				m_pSSAO_Manager = nullptr;

public:
	static void Release_Engine();

private:
	HWND m_hWnd;
	CRenderer* m_pRenderer = nullptr;
	_uint m_iTotalLevel = 0;

	list<class CLambdaRenderObject*> m_LambdaRenderQ;
	list<class CLambdaRenderObject*> m_UsedLambdaRenderQ;

public:
	virtual void Free() override;
};

template <typename T>
T* CGameInstance::Find_OneObjectByType(_uint iLevelIndex, const _tchar* pLayerTag)
{
	const auto pLayer = GetLayer(iLevelIndex, pLayerTag);
	if (pLayer == nullptr)
		return nullptr;

	for (auto pObject : pLayer->GetGameObjects())
	{
		if (auto pTarget = dynamic_cast<T*>(pObject))
			return pTarget;
	}

	return nullptr;
}

END
