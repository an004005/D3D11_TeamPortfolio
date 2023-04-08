#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "PostProcess.h"
#include "HelperClasses.h"
#include "Timeline.h"


BEGIN(Engine)
class CModel;
class CPxModel;
class CAnimation;
class CFSMComponent;
class CRenderer;
class CCurveFloatImpl;
class CMaterial;
END

BEGIN(Client)

class CBrainFieldCables;
class CPostVFX_ChromaticAberration;
class CPostVFX_MapPostProcess;


/**********************
 * CBrainField
 **********************/
class CBrainField : public CGameObject
{
private:
	CBrainField(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBrainField(const CBrainField& rhs);
	virtual ~CBrainField() = default;

public:
	HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

public:
	void SetTargetInfo(CTransform* pTargetTransform, CModel* pTargetModel);
	void OpenBrainField();
	void CloseBrainField();

public:
	CBrainFieldCables* m_pBrainFieldCables = nullptr;
	CPostVFX_ChromaticAberration* m_pChromaticAberration = nullptr;
	CPostVFX_MapPostProcess* m_pMapPostProcess = nullptr;

	CModel* m_pTargetModel = nullptr;
	CTransform* m_pTargetTransform = nullptr;

	_bool m_bOpen = false;

	CCurveFloatImpl* m_pCableNoise_c1 = nullptr;
	CCurveFloatImpl* m_pCableYellow_c1 = nullptr;
	CCurveFloatImpl* m_pChromaticAberration_c1 = nullptr;
	CCurveFloatImpl* m_pMaskEmissive_c1 = nullptr;
	CCurveFloatImpl* m_pMaskNoise_c1 = nullptr;
	CCurveFloatImpl* m_pMapDisappear_c1 = nullptr;

	CDoOnce m_MapPostProcessFloat0Reset;
	CCurveFloatImpl* m_pMaskEmissive_c2 = nullptr;
	CCurveFloatImpl* m_pMapAppear_c2 = nullptr;

	CMaterial* m_pMask = nullptr;


	class CSkyBox* m_pSkyBox = nullptr;
	class CScarletMap* m_pDefaultMap = nullptr;
	class CScarletMap* m_pBrainFieldMap = nullptr;
	class CCombinedRedString* m_pBrainFieldRedString = nullptr;

	CSimpleTimeline m_CloseTimeline;

public:
	virtual void Free() override;
	static CBrainField* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
};


// 색수차
/***********************************
 *CPostVFX_ChromaticAberration
 ***********************************/
class CPostVFX_ChromaticAberration : public CPostProcess
{
protected:
	CPostVFX_ChromaticAberration(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPostVFX_ChromaticAberration(const CPostVFX_ChromaticAberration& rhs);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;

public:
	static CPostVFX_ChromaticAberration* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
};

/***********************************
 *CPostVFX_MapPostProcess
 ***********************************/
class CPostVFX_MapPostProcess : public CPostProcess
{
protected:
	CPostVFX_MapPostProcess(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPostVFX_MapPostProcess(const CPostVFX_MapPostProcess& rhs);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;


public:
	static CPostVFX_MapPostProcess* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
};


/**********************
 * CBrainFieldCables
 **********************/
class CBrainFieldCables : public CGameObject
{
private:
	enum EBrainFieldCable
	{
		HEAD_1, // 머리 왼쪽위
		HEAD_2, // 머리 오른쪽 위
		HEAD_3, // 머리 아라
		BACK_1, // 등 왼쪽 위
		BACK_2, // 등 오른쪽 위
		BACK_3, // 등 가운데
		BACK_4, // 등 왼쪽 아래
		BACK_5, // 등 오른쪽 아래
		CABLE_END
	};

private:
	CBrainFieldCables(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBrainFieldCables(const CBrainFieldCables& rhs);
	virtual ~CBrainFieldCables() = default;

public:
	HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

public:
	void Activate(_bool bActive); // 장착 후 바로 루프
public:
	void SetTargetInfo(CTransform* pTargetTransform, CModel* pTargetModel);
	_float GetPlayRatio();
	const char* GetStateName();

	void SetCableNoise(_float fValue);
	void SetCableYellow(_float fValue);

private:
	void SetUpFSM();

private:
	array<CPxModel*, CABLE_END> m_CableModels{};
	array<_float4x4, CABLE_END> m_CablePivots{};
	array<string, CABLE_END> m_CableBones{};

	array<_float4x4, 3> m_InitialOpenFieldC1_Pivot;
	array<_float4x4, CABLE_END> m_InitialOpenFieldC2_Pivot;
	array<_float4x4, CABLE_END> m_CableOpenPivot;

	CFSMComponent* m_pFSM = nullptr;

	_bool m_bActive = false;
	_bool m_bPhysXOn = false;


	CTransform* m_pTargetTransform = nullptr;
	CModel*		m_pTargetModel = nullptr;
	CRenderer* m_pRendererCom = nullptr;


public:
	virtual void Free() override;
	static CBrainFieldCables* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr) override;
};

END