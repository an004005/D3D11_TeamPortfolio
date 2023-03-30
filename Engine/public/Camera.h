#pragma once

#include "GameObject.h"

/* �������� �־� �ʼ������� �ʿ��� ����� �佺���̽� ��ȯ�� ������ȯ. */

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

	_float4x4 GetWorldMatrix();
	_matrix GetXMWorldMatrix();
	_float4x4 GetProjMatrix();
	_float4x4 GetViewMatrix();
	_matrix GetXMProjMatrix();
	_matrix GetXMViewMatrix();

protected:
	CTransform* m_pPivotTransform = nullptr;

	_float m_fFOV = 60.f;
	_float m_fWidth = 1280.f;
	_float m_fHeight = 720.f;
	_float m_fNear = 0.1f;
	_float m_fFar = 1000.f;
	_bool m_bUseViewPortSize = true;

public:		
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END