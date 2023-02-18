#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CDebugRay : public CGameObject
{
private:
	CDebugRay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDebugRay(const CDebugRay& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	_float3 GetOrigin() const { return m_vOrigin; }
	_float3 GetDir() const
	{
		_float3 vDir = m_vDir;
		vDir.Normalize();
		return vDir;
	}

private:
	_float3 m_vOrigin;
	_float3 m_vDir;
	_float m_fLife = 1.f;

	class CRenderer* m_pRendererCom = nullptr;

	_bool						m_isColl = false;
	PrimitiveBatch<VertexPositionColor>*				m_pBatch = nullptr;
	BasicEffect*										m_pEffect = nullptr;
	ID3D11InputLayout*									m_pInputLayout = nullptr;	
	_float4												m_vColor;

public:
	static CDebugRay* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END