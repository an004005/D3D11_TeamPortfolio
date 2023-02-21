#pragma once

#include "GameObject.h"

/* 렌더링에 있어 필수적으로 필요한 요소인 뷰스페이스 변환과 투영변환. */

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc
	{
		_float4		vEye, vAt, vUp;		
	}CAMERADESC;

protected:
	CCamera(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void Imgui_RenderProperty() override;
	virtual void SaveToJson(Json& json) override;
	virtual void LoadFromJson(const Json& json) override;

	void SetMainCamera() { s_pMainCamera = this; }
	static void SetMainCamera(CCamera* pCamera) { s_pMainCamera = pCamera; }
	static CCamera* GetMainCamera() { return s_pMainCamera; }
	_bool IsMainCamera() const { return s_pMainCamera == this; }


	void SetFOV(_float fDegree) { m_fFOV = fDegree; }
	_float4x4 GetProjMatrix() const;
	_float4x4 GetViewMatrix() const { return m_pTransformCom->Get_WorldMatrix_Inverse(); }

private:
	static CCamera*				s_pMainCamera;

private:
	class CPipeLine*			m_pPipeLine = nullptr;
	CAMERADESC					m_CameraDesc;
	_float4x4					m_ProjMatrix;

	_float m_fFOV = 75.f;
	_float m_fWidth = 1280.f;
	_float m_fHeight = 720.f;
	_float m_fNear = 0.01f;
	_float m_fFar = 300.f;
	_bool m_bUseViewPortSize = true;

public:		
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END