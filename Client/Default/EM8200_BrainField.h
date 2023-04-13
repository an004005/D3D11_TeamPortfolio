#pragma once
#include <Timeline.h>
#include "PostProcess.h"
#include "GameObject.h"
#include "Client_Defines.h"
#include "HelperClasses.h"

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

class CEM8200_BrainFieldCables;
class CPostVFX_EM8200MapPostProcess;

class CEM8200_BrainField : public CGameObject
{
private:
	CEM8200_BrainField(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM8200_BrainField(const CEM8200_BrainField& rhs);
	virtual ~CEM8200_BrainField() = default;

public:
	HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void BeginTick() override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

public:
	void SetTargetInfo(CGameObject* pTarget, CTransform* pTargetTransform, CModel* pTargetModel);
	void OpenBrainField();
	void CloseBrainField();

private:
	_bool m_bOpen = false;

	CEM8200_BrainFieldCables* m_pCables = nullptr;
	CPostVFX_EM8200MapPostProcess* m_pPostProcess = nullptr;
	class CPostVFX_WhiteOut* m_pWhiteOut = nullptr;

	CCurveFloatImpl* m_pCableNoise_Open = nullptr;
	CCurveFloatImpl* m_pCableYellow_Open = nullptr;
	CCurveFloatImpl* m_pBlackWhite_Open = nullptr;

	CCurveFloatImpl* m_pBlackWhite_Trans = nullptr;
	CCurveFloatImpl* m_pWhiteOut_Trans = nullptr;


	class CSkyBox* m_pSkyBox = nullptr;
	class CScarletMap* m_pDefaultMap = nullptr;
	class CScarletMap* m_pBrainFieldMap = nullptr;
	class CCombinedRedString* m_pBrainFieldRedString = nullptr;

	CSimpleTimeline m_CloseTimeline;
	CDoOnce m_VisibleBrainFieldOnce;


public:
	virtual void Free() override;
	static CEM8200_BrainField* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
};

/***********************************
 *CPostVFX_EM8200MapPostProcess
 ***********************************/
class CPostVFX_EM8200MapPostProcess : public CPostProcess
{
protected:
	CPostVFX_EM8200MapPostProcess(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPostVFX_EM8200MapPostProcess(const CPostVFX_EM8200MapPostProcess& rhs);

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;

public:
	static CPostVFX_EM8200MapPostProcess* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
};

/**********************
 * CBrainFieldCables
 **********************/
class CEM8200_BrainFieldCables : public CGameObject
{
private:
	enum EBrainFieldCable
	{
		HEAD_1, // 머리 아래
		HEAD_2, // 머리 오른쪽 위
		HEAD_3, // 머리 왼쪽 위
		BACK_1, // 등 왼쪽 위
		BACK_2, // 등 오른쪽 위
		BACK_3, // 등 가운데
		BACK_4, // 등 왼쪽 아래
		BACK_5, // 등 오른쪽 아래
		CABLE_END
	};

private:
	CEM8200_BrainFieldCables(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEM8200_BrainFieldCables(const CEM8200_BrainFieldCables& rhs);
	virtual ~CEM8200_BrainFieldCables() = default;

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

	array<_float4x4, 3> m_InitialOpenFieldPivot{};
	array<_float4x4, 3> m_InitialTransPivot{};
	array<_float4x4, 3> m_CableOpenPivot{};


	CFSMComponent* m_pFSM = nullptr;

	_bool m_bActive = false;
	_bool m_bPhysXOn = false;


	CTransform* m_pTargetTransform = nullptr;
	CModel*		m_pTargetModel = nullptr;
	CRenderer* m_pRendererCom = nullptr;


public:
	virtual void Free() override;
	static CEM8200_BrainFieldCables* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr) override;
};

END