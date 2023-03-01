#pragma once

#include "GameObject.h"

/* 렌더링에 있어 필수적으로 필요한 요소인 뷰스페이스 변환과 투영변환. */

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
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

	void SetMainCamera();
	static void SetMainCamera(CCamera* pCamera);
	static CCamera* GetMainCamera();
	_bool IsMainCamera() const;

	_float GetFOV() const { return m_fFOV; }
	_float GetAspect() const { return m_fWidth / m_fHeight; }
	_float GetNear() const { return m_fNear; }
	_float GetFar() const { return m_fFar; }

	void SetFOV(_float fDegree) { m_fFOV = fDegree; }
	void SetWidth(_float fWidth) { m_fWidth = fWidth; }
	void SetHeight(_float fHeight) { m_fHeight = fHeight; }
	void SetNear(_float fNear) { m_fNear = fNear; }
	void SetFar(_float fFar) { m_fFar = fFar; }

	_float4x4 GetProjMatrix() const;
	_float4x4 GetViewMatrix() const { return m_pTransformCom->Get_WorldMatrix_Inverse(); }


private:
	_float4x4					m_ProjMatrix;

	_float m_fFOV = 75.f;
	_float m_fWidth = 1280.f;
	_float m_fHeight = 720.f;
	_float m_fNear = 0.1f;
	_float m_fFar = 300.f;

public:		
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END